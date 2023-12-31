﻿// AccountLoginDlg.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"

#include "MainFrm.h"
#include "VoIPClientDoc.h"

#include "afxdialogex.h"
#include "AccountLoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "session/UiController.h"

// CAccountLoginDlg 대화 상자

IMPLEMENT_DYNAMIC(CAccountLoginDlg, CDialogEx)

CAccountLoginDlg::CAccountLoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_ACCOUNT_LOGIN, pParent)
	, m_edEmailID(_T("gildong.hong@WeWork.com"))
	, m_edPassword(_T(""))
{
	spUserInfo = std::shared_ptr<userInfo>(new userInfo);
}

CAccountLoginDlg::~CAccountLoginDlg()
{
}

void CAccountLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_EMAIL_ID, m_edEmailID);
	DDX_Text(pDX, IDC_ED_PASSWORD, m_edPassword);
	DDX_Control(pDX, IDC_MFCBTN_LOGIN, m_btnLogIn);
	DDX_Control(pDX, IDC_MFCBTN_SIGN_IN, m_btnSignIn);
	DDX_Control(pDX, IDC_MFCBTN_RESET_PW, m_btnResetPw);
}

BEGIN_MESSAGE_MAP(CAccountLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDC_MFCBTN_LOGIN, &CAccountLoginDlg::OnBnClickedMfcbtnLogin)
	ON_BN_CLICKED(IDC_MFCBTN_SIGN_IN, &CAccountLoginDlg::OnBnClickedMfcbtnSignIn)
	ON_MESSAGE(UWM_UI_CONTROLLER, &CAccountLoginDlg::processUiControlNotify)
	ON_WM_CTLCOLOR()
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_MFCBTN_RESET_PW, &CAccountLoginDlg::OnBnClickedMfcbtnResetPw)
END_MESSAGE_MAP()

// CAccountLoginDlg 메시지 처리기
CVoIPClientDoc* CAccountLoginDlg::GetDocument() const 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CVoIPClientDoc* m_pDocument = (CVoIPClientDoc*)pFrame->GetActiveDocument();
	return (CVoIPClientDoc*)m_pDocument;
}

HBRUSH CAccountLoginDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return (HBRUSH)GetStockObject(WHITE_BRUSH);
}

BOOL CAccountLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	/*m_btnLogIn.LoadBitmaps(IDB_LOG_IN, IDB_LOG_IN_D, NULL , IDB_LOG_IN_F);
	m_btnSignIn.LoadBitmaps(IDB_SIGN_IN, IDB_SIGN_IN_D, NULL, IDB_SIGN_IN_F);
	m_btnResetPw.LoadBitmaps(IDB_RESET_PW, IDB_RESET_PW_D, NULL, IDB_RESET_PW_F);

	m_btnLogIn.SizeToContent();
	m_btnSignIn.SizeToContent();
	m_btnResetPw.SizeToContent();*/
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CAccountLoginDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_RETURN  ) {
			OnBnClickedMfcbtnLogin();
			return TRUE;
		} else if (pMsg->wParam == VK_ESCAPE) {
			return FALSE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CAccountLoginDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE) {
		EndDialog((INT_PTR)KResponse::LOGIN_FAILED);
	}
	else {
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CAccountLoginDlg::OnBnClickedMfcbtnLogin()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 입력 한 데이터를 담는다.  
	UpdateData(TRUE);

	TRACE2("%s, %s\n", m_edEmailID, m_edPassword);
	spUserInfo->email = tstring(m_edEmailID);
	spUserInfo->password = tstring(m_edPassword);

	// @todo server check 필요 
	// @todo 체크 결과 정상이면, 
	GetDocument()->SetUser(spUserInfo);
	GetDocument()->IsCurrentUser = TRUE;

	GetDlgItem(IDC_MFCBTN_LOGIN)->EnableWindow(false);
	UiController::getInstance()->req_Login(this, std::string(CT2CA(m_edEmailID)), std::string(CT2CA(m_edPassword)));
}

void CAccountLoginDlg::OnBnClickedMfcbtnSignIn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	TRACE2("%s, %s\n", m_edEmailID, m_edPassword);

	spUserInfo->email = tstring(m_edEmailID);
	spUserInfo->password = tstring(m_edPassword);

	EndDialog((INT_PTR)KResponse::CREATE_USER);
}

void CAccountLoginDlg::OnBnClickedMfcbtnResetPw()
{
	EndDialog((INT_PTR)KResponse::RESET_PASSWORD);
}

LRESULT CAccountLoginDlg::processUiControlNotify(WPARAM wParam, LPARAM lParam)
{
	cout << "processUiControlNotify()/WPARAM:" << wParam << "/LPARAM:" << lParam << endl;
	switch (wParam)
	{
	case MSG_RESPONSE_LOGIN:
		GetDlgItem(IDC_MFCBTN_LOGIN)->EnableWindow(true);
		if (lParam == 0) {
			cout << "SUCCESS" << endl;
			MessageBox(_T("Login Success"));
			EndDialog((INT_PTR)KResponse::LOGIN_COMPLETE);
		} else if (lParam == 1) {
			cout << "FAILED" << endl;
			MessageBox(_T("Login Failed : ID not registered"));
		} else if (lParam == 2) {
			cout << "FAILED" << endl;
			MessageBox(_T("Login Failed : Wrong password"));
		} else if ( lParam == 3 ) {
			cout << "FAILED" << endl;
			MessageBox(_T("Login Failed : ID disabled by Administrator"));
		}
		else if (lParam == 4) {
			cout << "FAILED" << endl;
			MessageBox(_T("Login Failed : Already Login"));
		} else {
			cout << "FAILED" << endl;
			MessageBox(_T("Login Failed"));
		}
		break;
	default:
		break;
	}
	return LRESULT();
}
