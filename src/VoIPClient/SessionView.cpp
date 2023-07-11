
#include "pch.h"
#include "framework.h"
#include "MainFrm.h"

#include "SessionView.h"
#include "Resource.h"
#include "VoIPClient.h"

#include "session/UiController.h"
#include "session/Constants.h"

class CSessionViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CSessionView;

	DECLARE_SERIAL(CSessionViewMenuButton)

public:
	CSessionViewMenuButton(HMENU hMenu = nullptr) noexcept : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CSessionViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// 생성/소멸
//////////////////////////////////////////////////////////////////////

CSessionView::CSessionView() noexcept
{
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
}

CSessionView::~CSessionView()
{
}

BEGIN_MESSAGE_MAP(CSessionView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CLASS_ADD_MEMBER_FUNCTION, OnClassAddMemberFunction)
	ON_COMMAND(ID_CLASS_ADD_MEMBER_VARIABLE, OnClassAddMemberVariable)
	ON_COMMAND(ID_CLASS_DEFINITION, OnClassDefinition)
	ON_COMMAND(ID_CLASS_PROPERTIES, OnClassProperties)
	ON_COMMAND(ID_NEW_FOLDER, OnNewSession)
	ON_COMMAND(ID_DELETE_USER, OnDeleteSession)
	ON_COMMAND(ID_JOIN_SESSION, OnJoinSession)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnSort)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnUpdateSort)
	ON_MESSAGE(UWM_UI_CONTROLLER, processUiControlNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSessionView 메시지 처리기

int CSessionView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 뷰를 만듭니다.
	const DWORD dwViewStyle = WS_OVERLAPPED | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndSessionView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("클래스 뷰를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	// 이미지를 로드합니다.
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT);
	m_wndToolBar.LoadToolBar(IDR_SORT, 0, 0, TRUE /* 잠금 */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT);

	m_wndToolBar.ReplaceButton(ID_SORT_MENU, CSessionViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	CSessionViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CSessionViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != nullptr)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}

	// 정적 트리 뷰 데이터를 더미 코드로 채웁니다.
	//FillSessionView();

	UiController::getInstance()->setCallbackWnd(this);

	return 0;
}

void CSessionView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CSessionView::FillSessionView()
{
	m_wndSessionView.DeleteAllItems();

	auto ConfDataList = UiController::getInstance()->get_MyConferences();

	HTREEITEM hRoot = m_wndSessionView.InsertItem(_T("Session List"), 2, 2);
	m_wndSessionView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
	
	for (ConferenceData p : ConfDataList) {
		tstring tmp_rid, tmp_time_s, tmp_time_e;

		tmp_rid.assign(p.rid.begin(), p.rid.end());
		HTREEITEM hRoom = m_wndSessionView.InsertItem(tmp_rid.data(), 1, 1, hRoot);

		std::pair<std::string, std::string> res = GetDateTime(p.dataAndTime, p.duration);

		tstring tmp1, tmp2;
		tmp1.assign(res.first.begin(), res.first.end());
		tmp_time_s = _T("start : ") + tmp1;
		m_wndSessionView.InsertItem(tmp_time_s.data(), 3, 3, hRoom);

		tmp2.assign(res.second.begin(), res.second.end());
		tmp_time_e = _T("end : ") + tmp2;
		m_wndSessionView.InsertItem(tmp_time_e.data(), 3, 3, hRoom);

		HTREEITEM hParticipant = m_wndSessionView.InsertItem(_T("Participant"), 0, 0, hRoom);
		for (auto participant : p.participants) {
			tstring tmp_name;
			tmp_name.assign(participant.begin(), participant.end());
			m_wndSessionView.InsertItem(tmp_name.data(), 2, 2, hParticipant);
		}
		m_wndSessionView.Expand(hRoot, TVE_EXPAND);
		m_wndSessionView.Expand(hRoom, TVE_EXPAND);
		m_wndSessionView.Expand(hParticipant, TVE_EXPAND);
	}

#ifdef DEBUG
	if (ConfDataList.size() == 0) {
		tstring tmp_rid = _T("hello"), tmp_time_s, tmp_time_e;

		HTREEITEM hRoom = m_wndSessionView.InsertItem(tmp_rid.data(), 1, 1, hRoot);

		std::pair<std::string, std::string> res = GetDateTime(1688955000, 600);

		tstring tmp1, tmp2;
		tmp1.assign(res.first.begin(), res.first.end());
		tmp_time_s = _T("start : ") + tmp1;
		m_wndSessionView.InsertItem(tmp_time_s.data(), 3, 3, hRoom);

		tmp2.assign(res.second.begin(), res.second.end());
		tmp_time_e = _T("end : ") + tmp2;
		m_wndSessionView.InsertItem(tmp_time_e.data(), 3, 3, hRoom);

		HTREEITEM hParticipant = m_wndSessionView.InsertItem(_T("Participant"), 0, 0, hRoom);
		tstring tmp_name = _T("hog.gildong"), tmp_name2 = _T("steve.jobs");
		m_wndSessionView.InsertItem(tmp_name.data(), 2, 2, hParticipant);
		m_wndSessionView.InsertItem(tmp_name2.data(), 2, 2, hParticipant);

		m_wndSessionView.Expand(hRoot, TVE_EXPAND);
		m_wndSessionView.Expand(hRoom, TVE_EXPAND);
		m_wndSessionView.Expand(hParticipant, TVE_EXPAND);
	}
#endif // DEBUG

	AdjustLayout();
}


void CSessionView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	//CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndSessionView;
	//ASSERT_VALID(pWndTree);

	//if (pWnd != pWndTree)
	//{
	//	CDockablePane::OnContextMenu(pWnd, point);
	//	return;
	//}

	//if (point != CPoint(-1, -1))
	//{
	//	// 클릭한 항목을 선택합니다.
	//	CPoint ptTree = point;
	//	pWndTree->ScreenToClient(&ptTree);

	//	UINT flags = 0;
	//	HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
	//	if (hTreeItem != nullptr)
	//	{
	//		pWndTree->SelectItem(hTreeItem);
	//	}
	//}

	//pWndTree->SetFocus();
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);

	/*CMenu menu;
	menu.LoadMenu(IDR_POPUP_SORT);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}*/
}

void CSessionView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndSessionView.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CSessionView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CSessionView::OnSort(UINT id)
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;

	CSessionViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CSessionViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != nullptr)
	{
		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
		m_wndToolBar.Invalidate();
		m_wndToolBar.UpdateWindow();
	}
}

void CSessionView::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void CSessionView::OnClassAddMemberFunction()
{

}

void CSessionView::OnClassAddMemberVariable()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CSessionView::OnClassDefinition()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CSessionView::OnClassProperties()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CSessionView::OnNewSession()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->AddSessionList();
}

void CSessionView::OnDeleteSession()
{
	HTREEITEM ht = m_wndSessionView.GetSelectedItem();
	if (ht == m_wndSessionView.GetRootItem()) return;
	if (MessageBox(_T("Do you want to delete the Session ?"), _T("DeleteSession"), MB_OKCANCEL) == IDOK)
	{
		// participants 인 경우 예외 처리 필요
		CString sRID = m_wndSessionView.GetItemText(m_wndSessionView.GetParentItem(ht));
		if (sRID == CString(_T("Participant"))) {
			ht = m_wndSessionView.GetParentItem(ht);
			sRID = m_wndSessionView.GetItemText(m_wndSessionView.GetParentItem(ht));
		}
		else {
			sRID = m_wndSessionView.GetItemText(m_wndSessionView.ItemHasChildren(ht) == 0 ? m_wndSessionView.GetParentItem(ht) : ht);
		}
		UiController::getInstance()->request_deleteConference(this, std::string(CT2CA(sRID)));
	}
}

void CSessionView::OnJoinSession()
{
	HTREEITEM ht = m_wndSessionView.GetSelectedItem();
	if (ht == m_wndSessionView.GetRootItem()) return;
	if (MessageBox(_T("Do you want to Join the Session ?"), _T("JoinSession"), MB_OKCANCEL) == IDOK)
	{
		// participants 인 경우 예외 처리 필요
		CString sRID = m_wndSessionView.GetItemText(m_wndSessionView.GetParentItem(ht));
		if (sRID == CString(_T("Participant"))) {
			ht = m_wndSessionView.GetParentItem(ht);
			sRID = m_wndSessionView.GetItemText(m_wndSessionView.GetParentItem(ht));
		}
		else {
			sRID = m_wndSessionView.GetItemText(m_wndSessionView.ItemHasChildren(ht) == 0 ? m_wndSessionView.GetParentItem(ht) : ht);
		}
		UiController::getInstance()->request_JoinConference(this, std::string(CT2CA(sRID)));
	}
}

void CSessionView::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	CRect rectTree;
	m_wndSessionView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CSessionView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndSessionView.SetFocus();
}

void CSessionView::OnChangeVisualStyle()
{
	m_SessionViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_PAGES_SMALL_HC : IDB_PAGES_SMALL;
	//UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_NAVIGATION_LARGE_HC : IDB_NAVIGATION_LARGE;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("비트맵을 로드할 수 없습니다. %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_SessionViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_SessionViewImages.Add(&bmp, RGB(255, 0, 0));

	m_wndSessionView.SetImageList(&m_SessionViewImages, TVSIL_NORMAL);

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_24 : IDR_SORT, 0, 0, TRUE /* 잠금 */);
}

LRESULT CSessionView::processUiControlNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case MSG_RESPONSE_DELETE_CONF:
		cout << "CSessionView::MSG_RESPONSE_DELETE_CONF" << endl;
		switch (lParam) {
			case 1: MessageBox(_T("No conference ID exists")); break;
			case 2: MessageBox(_T("Only HOST can delete conference")); break;
			case 3: MessageBox(_T("Invalid request")); break;
		}
		UiController::getInstance()->req_GetAllConferences(this);
		break;
	case MSG_RESPONSE_JOIN_CONF:
		switch (lParam) {
			case 1: MessageBox(_T("No Room to join")); break;
			case 2: MessageBox(_T("Conference not started")); break;
			case 3: MessageBox(_T("You are not invited")); break;
		}
		break;
	case MSG_RESPONSE_DATA:
		if (lParam == 0) {
			FillSessionView();
		}
		break;
	case MSG_RESPONSE_LOGIN:
		if (lParam == 0) {
			FillSessionView();
		}
		break;
	default:
		break;
	}

	return LRESULT();
}