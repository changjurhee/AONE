
// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "VoIPClient.h"

#include "MainFrm.h"
#include "VoIPClientDoc.h"
//#include "VoIPClientView.h"
#include "VoIPNewClient.h"

#include "AccountLoginDlg.h"
#include "ManageUserAccountDlg.h"
#include "ResetPasswordDlg.h"
#include "SessionRegisterDlg.h"
#include "ContactRegisterDlg.h"

// @deprecated only test
#include "TestCallDlg.h"

// Session
#include "session/SessionManager.h"
#include "session/UiController.h"
#include "session/Constants.h"
#include "MessageBoxDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "StartDlg.h"
#include "UpdateUserAccountDlg.h"

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CMainFrame::OnOptions)
	ON_COMMAND(ID_VIEW_FILEVIEW, &CMainFrame::OnViewFileView)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FILEVIEW, &CMainFrame::OnUpdateViewFileView)
	ON_COMMAND(ID_VIEW_CLASSVIEW, &CMainFrame::OnViewClassView)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CLASSVIEW, &CMainFrame::OnUpdateViewClassView)
	ON_COMMAND(ID_VIEW_OUTPUTWND, &CMainFrame::OnViewOutputWindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUTWND, &CMainFrame::OnUpdateViewOutputWindow)
	ON_COMMAND(ID_VIEW_PROPERTIESWND, &CMainFrame::OnViewPropertiesWindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIESWND, &CMainFrame::OnUpdateViewPropertiesWindow)
	ON_WM_SETTINGCHANGE()
	/*ON_COMMAND(ID_TEST_LOG_IN, &CMainFrame::OnUserLogIn)
	ON_UPDATE_COMMAND_UI(ID_TEST_LOG_IN, &CMainFrame::OnUpdateUserLogIn)*/
	ON_COMMAND(ID_TEST_LOG_OUT, &CMainFrame::OnUserLogOut)
	ON_UPDATE_COMMAND_UI(ID_TEST_LOG_OUT, &CMainFrame::OnUpdateUserLogOut)
	ON_COMMAND(ID_SETTING_TEST, &CMainFrame::OnTestDlg)
	ON_UPDATE_COMMAND_UI(ID_SETTING_TEST, &CMainFrame::OnUpdateTestDlg)
	ON_COMMAND(ID_TEST_UPDATE_USER, &CMainFrame::OnResetPassword)
	ON_UPDATE_COMMAND_UI(ID_TEST_UPDATE_USER, &CMainFrame::OnUpdateResetPassword)
	ON_WM_GETMINMAXINFO()
	ON_MESSAGE(UWM_UI_CONTROLLER, &CMainFrame::processUiControlNotify)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	LOG_DEBUG("waiting something...");

	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("메뉴 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 메뉴 모음을 활성화해도 포커스가 이동하지 않게 합니다.
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	/*CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);*/

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	/*m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);*/

	// 사용자 정의 도구 모음 작업을 허용합니다.
	InitUserToolbars(nullptr, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 도구 모음 및 메뉴 모음을 도킹할 수 없게 하려면 이 다섯 줄을 삭제하십시오.
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 탐색 창이 왼쪽에 만들어지므로 일시적으로 왼쪽에 도킹을 비활성화합니다.
	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);

	//// "Outlook" 탐색 모음을 만들고 설치합니다.
	//if (!CreateOutlookBar(m_wndNavigationBar, ID_VIEW_NAVIGATION, m_wndTree, m_wndCalendar, 250))
	//{
	//	TRACE0("탐색 창을 만들지 못했습니다.\n");
	//	return -1;      // 만들지 못했습니다.
	//}

	// 캡션 표시줄을 만듭니다.
	if (!CreateCaptionBar())
	{
		TRACE0("캡션 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	// Outlook 표시줄이 만들어지고 왼쪽에 도킹할 수 있도록 허용해야 합니다.
	EnableDocking(CBRS_ALIGN_LEFT);
	EnableAutoHidePanes(CBRS_ALIGN_RIGHT);

	// 메뉴 항목 이미지를 로드합니다(표준 도구 모음에 없음).
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 도킹 창을 만듭니다.
	if (!CreateDockingWindows())
	{
		TRACE0("도킹 창을 만들지 못했습니다.\n");
		return -1;
	}

	m_wndContactView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndSessionView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndContactView);
	CDockablePane* pTabbedBar = nullptr;
	m_wndSessionView.AttachToTabWnd(&m_wndContactView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	//m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	//DockPane(&m_wndProperties);
	
	// 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다.
	OnApplicationLook(theApp.m_nAppLook);

	// 도구 모음 및 도킹 창 메뉴 바꾸기를 활성화합니다.
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 빠른(<Alt> 키를 누른 채 끌기) 도구 모음 사용자 지정을 활성화합니다.
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == nullptr)
	{
		// 사용자 정의 도구 모음 이미지를 로드합니다.
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// 메뉴 개인 설정을 활성화합니다(가장 최근에 사용한 명령).
	// TODO: 사용자의 기본 명령을 정의하여 각 풀다운 메뉴에 하나 이상의 기본 명령을 포함시킵니다.
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);
	lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
	lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);

	// @ test menu
	//lstBasicCommands.AddTail(ID_TEST_LOG_IN);
	lstBasicCommands.AddTail(ID_TEST_LOG_OUT);
	//lstBasicCommands.AddTail(ID_TEST_CREATE_USER);
	lstBasicCommands.AddTail(ID_TEST_UPDATE_USER);
	//lstBasicCommands.AddTail(ID_SETTING_TEST);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	UiController::getInstance()->setCallbackWnd(this);

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	return m_wndSplitter.Create(this,
		2, 2,						// TODO: 행 및 열의 개수를 조정합니다.
		CSize(10, 10),      // TODO: 최소 창 크기를 조정합니다.
		pContext);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{

	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU | WS_THICKFRAME;

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// 클래스 뷰를 만듭니다.
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);
	if (!m_wndSessionView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	// 파일 뷰를 만듭니다.
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndContactView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("파일 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	// 출력 창을 만듭니다.
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("출력 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	//// 속성 창을 만듭니다.
	//CString strPropertiesWnd;
	//bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	//ASSERT(bNameValid);
	//if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	//{
	//	TRACE0("속성 창을 만들지 못했습니다.\n");
	//	return FALSE; // 만들지 못했습니다.
	//}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndContactView.SetIcon(hFileViewIcon, FALSE);

	HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndSessionView.SetIcon(hClassViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	/*HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);*/

}

BOOL CMainFrame::CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, CMFCShellTreeCtrl& tree, CCalendarBar& calendar, int nInitialWidth)
{
	bar.SetMode2003();

	BOOL bNameValid;
	CString strTemp;
	bNameValid = strTemp.LoadString(IDS_SHORTCUTS);
	ASSERT(bNameValid);
	if (!bar.Create(strTemp, this, CRect(0, 0, nInitialWidth, 32000), uiID, WS_CHILD | WS_VISIBLE | CBRS_LEFT))
	{
		return FALSE; // 만들지 못했습니다.
	}

	CMFCOutlookBarTabCtrl* pOutlookBar = (CMFCOutlookBarTabCtrl*)bar.GetUnderlyingWindow();

	if (pOutlookBar == nullptr)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	pOutlookBar->EnableInPlaceEdit(TRUE);

	static UINT uiPageID = 1;

	// 움직이거나, 자동으로 숨기거나, 크기를 조정할 수 있지만 닫을 수는 없습니다.
	DWORD dwStyle = AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_RESIZE;

	CRect rectDummy(0, 0, 0, 0);
	const DWORD dwTreeStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	tree.Create(dwTreeStyle, rectDummy, &bar, 1200);
	bNameValid = strTemp.LoadString(IDS_FOLDERS);
	ASSERT(bNameValid);
	pOutlookBar->AddControl(&tree, strTemp, 2, TRUE, dwStyle);

	calendar.Create(rectDummy, &bar, 1201);
	bNameValid = strTemp.LoadString(IDS_CALENDAR);
	ASSERT(bNameValid);
	pOutlookBar->AddControl(&calendar, strTemp, 3, TRUE, dwStyle);

	bar.SetPaneStyle(bar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	pOutlookBar->SetImageList(theApp.m_bHiColorIcons ? IDB_PAGES_HC : IDB_PAGES, 24);
	pOutlookBar->SetToolbarImageList(theApp.m_bHiColorIcons ? IDB_PAGES_SMALL_HC : IDB_PAGES_SMALL, 16);
	pOutlookBar->RecalcLayout();

	BOOL bAnimation = theApp.GetInt(_T("OutlookAnimation"), TRUE);
	CMFCOutlookBarTabCtrl::EnableAnimation(bAnimation);

	bar.SetButtonsFont(&afxGlobalData.fontBold);

	return TRUE;
}

BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("캡션 표시줄을 만들지 못했습니다.\n");
		return FALSE;
	}

	BOOL bNameValid;

	CString strTemp, strTemp2;
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);

	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnViewCustomize()
{
	//CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 메뉴를 검색합니다. */);
	//pDlgCust->EnableUserDefinedToolbars();
	//pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	/*BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);*/
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	m_wndOutput.UpdateFonts();
	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnViewCaptionBar()
{
	m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}

void CMainFrame::OnOptions()
{
}

void CMainFrame::OnViewFileView()
{
	// 현재 상태에 따라 창을 표시하거나 활성화합니다.
	// 창을 닫으려면 창 프레임의 [x] 단추를 사용해야 합니다.
	m_wndContactView.ShowPane(TRUE, FALSE, TRUE);
	m_wndContactView.SetFocus();
}

void CMainFrame::OnUpdateViewFileView(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnViewClassView()
{
	// 현재 상태에 따라 창을 표시하거나 활성화합니다.
	// 창을 닫으려면 창 프레임의 [x] 단추를 사용해야 합니다.
	m_wndSessionView.ShowPane(TRUE, FALSE, TRUE);
	m_wndSessionView.SetFocus();
}

void CMainFrame::OnUpdateViewClassView(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnViewOutputWindow()
{
	// 현재 상태에 따라 창을 표시하거나 활성화합니다.
	// 창을 닫으려면 창 프레임의 [x] 단추를 사용해야 합니다.
	m_wndOutput.ShowPane(TRUE, FALSE, TRUE);
	m_wndOutput.SetFocus();
}

void CMainFrame::OnUpdateViewOutputWindow(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnViewPropertiesWindow()
{
	// 현재 상태에 따라 창을 표시하거나 활성화합니다.
	// 창을 닫으려면 창 프레임의 [x] 단추를 사용해야 합니다.
	m_wndProperties.ShowPane(TRUE, FALSE, TRUE);
	m_wndProperties.SetFocus();
}

void CMainFrame::OnUpdateViewPropertiesWindow(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUserLogIn()
{
	ShowWindow(SW_HIDE);
	UserLogIn();
	ShowWindow(SW_SHOW);
}

void CMainFrame::OnUpdateUserLogIn(CCmdUI* pCmdUI)
{
	CVoIPClientDoc* pDoc = (CVoIPClientDoc*)this->GetActiveDocument();
	pCmdUI->Enable(!pDoc->IsCurrentUser);
}

void CMainFrame::OnUserLogOut()
{
	UiController::getInstance()->req_Logout(this);
	ShowWindow(SW_HIDE);
	UserLogIn();
	ShowWindow(SW_SHOW);
}

void CMainFrame::OnUpdateUserLogOut(CCmdUI* pCmdUI)
{
	CVoIPClientDoc* pDoc = (CVoIPClientDoc*)this->GetActiveDocument();
	pCmdUI->Enable(pDoc->IsCurrentUser);
}

void CMainFrame::OnTestDlg()
{
#if 0
	ShowWindow(SW_HIDE);
	CTestCallDlg dlg;
	dlg.DoModal();
	ShowWindow(SW_SHOW);
#endif
}

void CMainFrame::OnUpdateTestDlg(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateResetPassword(CCmdUI* pCmdUI)
{
	CVoIPClientDoc* pDoc = (CVoIPClientDoc*)this->GetActiveDocument();
	pCmdUI->Enable(pDoc->IsCurrentUser);
}

void CMainFrame::OnResetPassword()
{
	//CResetPasswordDlg resetDlg;
	CUpdateUserAccountDlg resetDlg;
	resetDlg.DoModal();
}

void CMainFrame::Connect()
{
	CStartDlg startDlg;
	INT_PTR retValue = startDlg.DoModal();
	if ((KResponse)retValue == KResponse::CONNECT_COMPLETE) {
		CVoIPClientDoc* pDoc = (CVoIPClientDoc*)GetActiveDocument();
		tstring wsip_server = pDoc->GetUser()->server_ip_num;
		std::string sip_server;
		sip_server.assign(wsip_server.begin(), wsip_server.end());
		UserLogIn();
	} else {
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0); 
		//DestroyWindow();
	}
}

void CMainFrame::UserLogIn()
{
	CAccountLoginDlg accountLoginDlg;
	INT_PTR retValue = accountLoginDlg.DoModal();
	if ((KResponse)retValue == KResponse::LOGIN_COMPLETE) {
		CVoIPClientDoc* pDoc = (CVoIPClientDoc*)GetActiveDocument();
		SetWindowText(FormatString(_T("MOOZ %s"), pDoc->GetUser()->email.c_str()).data());
		// Go to main screen (finish modal status)
	} else if ((KResponse)retValue == KResponse::CREATE_USER) {
		UserSignIn();
	} else if ((KResponse)retValue == KResponse::RESET_PASSWORD) {
		ResetPassword();
	}
	else {
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0); 
		//DestroyWindow();
	}
}

void CMainFrame::UserSignIn()
{
	CManageUserAccountDlg createNewDlg;
	INT_PTR retValue = createNewDlg.DoModal();
	if ((KResponse)retValue == KResponse::SIGNIN_COMPLETE) {
		UserLogIn();
	} else if ((KResponse)retValue == KResponse::SIGNIN_CANCELED) {
		UserLogIn();
	}else {
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
		//DestroyWindow();
	}
}

void CMainFrame::ResetPassword()
{
	CResetPasswordDlg resetPwDlg;
	INT_PTR retValue = resetPwDlg.DoModal();
	if ((KResponse)retValue == KResponse::RESET_PASSWORD_COMPLETE) {
		UserLogIn();
	} else if ((KResponse)retValue == KResponse::RESET_PASSWORD_CANCELED) {
		UserLogIn();
	} else {
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
		//DestroyWindow();
	}
}

void CMainFrame::UpdateAccount()
{
	CUpdateUserAccountDlg updateDlg;
	INT_PTR retValue = updateDlg.DoModal();
	if ((KResponse)retValue == KResponse::UPDATE_USER_COMPLETE) {
		// Do nothing (Go to mainframe screen)
	}
	else if ((KResponse)retValue == KResponse::UPDATE_USER_CANCELED) {
		// Do nothing (Go to mainframe screen)
	} else {
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
		//DestroyWindow();
	}
}

void CMainFrame::AddSessionList()
{
	CSessionRegisterDlg dlg;
	dlg.DoModal();
}

void CMainFrame::AddContactList()
{
	CContactRegisterDlg dlg;
	dlg.DoModal();
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	// 기본 클래스가 실제 작업을 수행합니다.
	
	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	// 모든 사용자 도구 모음에 사용자 지정 단추를 활성화합니다.
	/*BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != nullptr)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}*/

	return TRUE;
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	lpMMI->ptMinTrackSize.x = 500;
	lpMMI->ptMinTrackSize.y = 500;

	CFrameWndEx::OnGetMinMaxInfo(lpMMI);
}

void SetForegroundWindowForce(HWND hWnd)
{
	HWND hWndForeground = ::GetForegroundWindow();
	if (hWndForeground == hWnd) return;
	DWORD Strange = ::GetWindowThreadProcessId(hWndForeground, NULL);
	DWORD My = ::GetWindowThreadProcessId(hWnd, NULL);
	if (!::AttachThreadInput(Strange, My, TRUE))
	{
		;// ASSERT(0);
	}
	::SetForegroundWindow(hWnd);
	::BringWindowToTop(hWnd);
	::ShowWindow(hWnd, SW_RESTORE);
	if (!::AttachThreadInput(Strange, My, FALSE))
	{
		;// ASSERT(0);
	}
}

LRESULT CMainFrame::processUiControlNotify(WPARAM wParam, LPARAM lParam)
{
	CVoIPClientDoc* pDoc = (CVoIPClientDoc*)GetActiveDocument();
	CVoIPNewClient* pView = (CVoIPNewClient*)GetActiveView();

	switch (wParam)
	{
	case MSG_RESPONSE_CALLSTATE:
	{
		CallResult* res = reinterpret_cast<CallResult*>(lParam);
		int result = res->result;
		int cause = res->cause;
		if (result == CallState::STATE_RINGING) {
			std::cout << "show incoming dlg from: " << res->callerId << std::endl;
			SetForegroundWindowForce(this->m_hWnd);
			CString cid((res->callerId).c_str());
			CMessageBoxDlg dlg(this, (INT)CMessageBoxDlg::Msg::INCOMING, cid);
			dlg.DoModal();
		}
		else if (result == CallState::STATE_DIALING) {
			CString cid((res->callerId).c_str());
			CMessageBoxDlg dlg(this, (INT)CMessageBoxDlg::Msg::OUTGOING, cid);
			dlg.DoModal();
		}
		else if (result == CallState::STATE_ACTIVE) {
			pDoc->SetConnection(TRUE);
		}
		else if (result == CallState::STATE_DISCONNECTED) {
			std::string causeString;
			if (cause == 1) {
				causeString = "REJECTED";
			}
			else if (cause == 2) {
				causeString = "BUSY";
			}
			else if (cause == 3) {
				causeString = "UNREACHABLE";
			}
			std::cout << "Disconnected " << result << ", " << causeString << std::endl;
			std::string str = "Disconnected " + causeString;
			CString cstr(str.c_str());
			MessageBox(cstr);
			//MessageBox(FormatString(_T("Disconnected %s"), causeString.c_str()).data());
			pDoc->SetConnection(FALSE);
			pView->OnPaint();
		}
		break;
	}
	default:
		break;
	}
	return LRESULT();
}
