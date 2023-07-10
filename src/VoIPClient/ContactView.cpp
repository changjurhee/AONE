
#include "pch.h"
#include "framework.h"
#include "mainfrm.h"

#include "ContactView.h"
#include "Resource.h"
#include "VoIPClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

class CContactViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CContactView;

	DECLARE_SERIAL(CContactViewMenuButton)

public:
	CContactViewMenuButton(HMENU hMenu = nullptr) noexcept : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
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

IMPLEMENT_SERIAL(CContactViewMenuButton, CMFCToolBarMenuButton, 1)

/////////////////////////////////////////////////////////////////////////////
// CContactView

CContactView::CContactView() noexcept
{
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
}

CContactView::~CContactView()
{
}

BEGIN_MESSAGE_MAP(CContactView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_OPEN, OnFileOpen)
	ON_COMMAND(ID_OPEN_WITH, OnFileOpenWith)
	ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_NEW_FOLDER, OnNewUser)
	ON_COMMAND(ID_DELETE_USER, OnDeleteUser)
	ON_COMMAND(ID_JOIN_SESSION, OnJoinUser)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnSort)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnUpdateSort)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 메시지 처리기

int CContactView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 뷰를 만듭니다.
	const DWORD dwViewStyle = WS_OVERLAPPED | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndContactView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("파일 뷰를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	// 뷰 이미지를 로드합니다.
	m_ContactViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndContactView.SetImageList(&m_ContactViewImages, TVSIL_NORMAL);

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

	m_wndToolBar.ReplaceButton(ID_SORT_MENU, CContactViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	CContactViewMenuButton* pButton = DYNAMIC_DOWNCAST(CContactViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != nullptr)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}

	// 정적 트리 뷰 데이터를 더미 코드로 채웁니다.
	FillContactView();
	AdjustLayout();

	return 0;
}

BOOL CContactView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CContactView::OnSort(UINT id)
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;

	CContactViewMenuButton* pButton = DYNAMIC_DOWNCAST(CContactViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != nullptr)
	{
		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
		m_wndToolBar.Invalidate();
		m_wndToolBar.UpdateWindow();
	}
}

void CContactView::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void CContactView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CContactView::FillContactView()
{
	HTREEITEM hRoot = m_wndContactView.InsertItem(_T("Contact List"), 2, 2);
	m_wndContactView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	HTREEITEM hSrc = m_wndContactView.InsertItem(_T("홍길동"), 1, 1, hRoot);

	m_wndContactView.InsertItem(_T("길동.홍@구글.본사"), 0, 0, hSrc);
	m_wndContactView.InsertItem(_T("홍_001"), 3, 3, hSrc);

	HTREEITEM hInc = m_wndContactView.InsertItem(_T("황진이"), 1, 1, hRoot);

	m_wndContactView.InsertItem(_T("진이.황@나주.관아"), 3, 3, hInc);
	m_wndContactView.InsertItem(_T("황_002"), 0, 0, hInc);

	HTREEITEM hRes = m_wndContactView.InsertItem(_T("스티브잡스"), 1, 1, hRoot);

	m_wndContactView.InsertItem(_T("잡스.스티브@애플.회사"), 3, 3, hRes);
	m_wndContactView.InsertItem(_T("스티브_003"), 0, 0, hRes);

	m_wndContactView.Expand(hRoot, TVE_EXPAND);
	m_wndContactView.Expand(hSrc, TVE_EXPAND);
	m_wndContactView.Expand(hInc, TVE_EXPAND);
}

void CContactView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	//CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndContactView;
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
}

void CContactView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndContactView.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CContactView::OnProperties()
{
}

void CContactView::OnFileOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CContactView::OnFileOpenWith()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CContactView::OnDummyCompile()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CContactView::OnEditCut()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CContactView::OnEditCopy()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CContactView::OnEditClear()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CContactView::OnNewUser()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->AddContactList();
}

void CContactView::OnDeleteUser()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->DeleteUser();
}

void CContactView::OnJoinUser()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->JoinUser();
}

void CContactView::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	CRect rectTree;
	m_wndContactView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CContactView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndContactView.SetFocus();
}

void CContactView::OnChangeVisualStyle()
{
	m_ContactViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_PAGES_SMALL_HC : IDB_PAGES_SMALL;

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

	m_ContactViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ContactViewImages.Add(&bmp, RGB(255, 0, 0));

	m_wndContactView.SetImageList(&m_ContactViewImages, TVSIL_NORMAL);

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_24 : IDR_SORT, 0, 0, TRUE /* 잠금 */);
}
