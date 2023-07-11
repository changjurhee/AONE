
// VoIPServer.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "VoIPServer.h"
#include "MainFrm.h"

#include "VoIPServerDoc.h"
#include "VoIPServerView.h"
#include "ServerMediaManager.h"
#include "common/logger.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Session
#include "session/SessionManager.h"
#include "session/TelephonyManager.h"
#include "session/AccountManager.h"
#include "session/SUiController.h"

// CVoIPServerApp

BEGIN_MESSAGE_MAP(CVoIPServerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CVoIPServerApp::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 표준 인쇄 설정 명령입니다.
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CVoIPServerApp 생성

CVoIPServerApp::CVoIPServerApp() noexcept
{
	m_bHiColorIcons = TRUE;


	m_nAppLook = 0;
	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 애플리케이션을 공용 언어 런타임 지원을 사용하여 빌드한 경우(/clr):
	//     1) 이 추가 설정은 다시 시작 관리자 지원이 제대로 작동하는 데 필요합니다.
	//     2) 프로젝트에서 빌드하려면 System.Windows.Forms에 대한 참조를 추가해야 합니다.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 아래 애플리케이션 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("VoIPServer.AppID.NoVersion"));

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 CVoIPServerApp 개체입니다.

CVoIPServerApp theApp;
// 이 식별자는 응용 프로그램에서 통계적으로 고유한 값을 가지도록 생성되었습니다.
// 특정 식별자를 선호할 경우 변경할 수 있습니다.

// {e8369551-792d-4552-880f-ed63d9e9ba40}
static const CLSID clsid =
{0xe8369551,0x792d,0x4552,{0x88,0x0f,0xed,0x63,0xd9,0xe9,0xba,0x40}};

const GUID CDECL _tlid = {0x5988b6c4,0x9318,0x47ae,{0xad,0xfc,0xf0,0x2e,0x8a,0x96,0xb1,0x55}};
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


// CVoIPServerApp 초기화

BOOL CVoIPServerApp::InitInstance()
{
	Logger::GetInstance()->SetLogLevel(LogLevel::LL_DEBUG);

	// 애플리케이션 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit 컨트롤을 사용하려면 AfxInitRichEdit2()가 있어야 합니다.
	// AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 애플리케이션 마법사에서 생성된 애플리케이션"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.


	InitContextMenuManager();
	InitShellManager();

	InitKeyboardManager();

	InitTooltipManager();

	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 애플리케이션의 문서 템플릿을 등록합니다.  문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		//IDI_HOMER_FRAME,
		RUNTIME_CLASS(CVoIPServerDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CVoIPServerView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// COleTemplateServer를 문서 템플릿에 연결합니다.
	//  COleTemplateServer는 OLE 컨테이너를 요청하는 대신 문서 템플릿에
	//  지정된 정보를 사용하여 새 문서를
	//  만듭니다.
	m_server.ConnectTemplate(clsid, pDocTemplate, TRUE);
		// 참고: SDI 애플리케이션은 명령줄에 /Embedding 또는 /Automation이
		//   있을 경우에만 서버 개체를 등록합니다.


	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// 응용 프로그램이 /Embedding 또는 /Automation 스위치로 시작되었습니다.
	// 응용 프로그램을 자동화 서버로 실행합니다.
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// 모든 OLE 서버 팩터리를 실행 중으로 등록합니다.  이렇게 하면
		//  OLE 라이브러리가 다른 애플리케이션에서 개체를 만들 수 있습니다.
		COleTemplateServer::RegisterAll();

		// 주 창을 표시하지 않습니다.
		return TRUE;
	}
	// 응용 프로그램이 /Unregserver 또는 /Unregister 스위치로 시작되었습니다. 
	// typelibrary를 등록 취소합니다.  다른 등록 취소는 ProcessShellCommand()에서 발생합니다.
	else if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister)
	{
		m_server.UpdateRegistry(OAT_DISPATCH_OBJECT, nullptr, nullptr, FALSE);
		AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor);
	}
	// 응용 프로그램이 독립 실행형으로 시작되었거나 다른 스위치로 시작되었습니다(예: /Register
	// 또는 /Regserver).  typelibrary를 포함하여 레지스트리 항목을 업데이트합니다.
	else
	{
		m_server.UpdateRegistry(OAT_DISPATCH_OBJECT);
		COleObjectFactory::UpdateRegistryAll();
		AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid);
	}

	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	m_pMainWnd->ShowWindow(SW_HIDE);
	m_pMainWnd->UpdateWindow();

	// SessionManager create and init
	SessionManager::getInstance()->init();

	media::ServerMediaManager* server = media::ServerMediaManager::getInstance();

	SUiController::getInstance()->startCommandLineInterface();

	return TRUE;
}

int CVoIPServerApp::ExitInstance()
{
	//TODO: 추가한 추가 리소스를 처리합니다.
	AfxOleTerm(FALSE);

	// Session clear
	SessionManager::releaseInstance();
	TelephonyManager::releaseInstance();
	AccountManager::releaseInstance();

	return CWinAppEx::ExitInstance();
}

// CVoIPServerApp 메시지 처리기


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CVoIPServerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CVoIPServerApp 사용자 지정 로드/저장 방법

void CVoIPServerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CVoIPServerApp::LoadCustomState()
{
}

void CVoIPServerApp::SaveCustomState()
{
}

// CVoIPServerApp 메시지 처리기



