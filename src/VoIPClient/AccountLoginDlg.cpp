// AccountLoginDlg.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"
#include "afxdialogex.h"
#include "AccountLoginDlg.h"


// CAccountLoginDlg 대화 상자

IMPLEMENT_DYNAMIC(CAccountLoginDlg, CDialogEx)

CAccountLoginDlg::CAccountLoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_ACCOUNT_LOGIN, pParent)
	, m_edEmailID(_T("gildong.hong@WeWork.com"))
	, m_edPassword(_T(""))
	, m_wdServerIpAddress(_T("127.0.0.1"))
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
	DDX_Text(pDX, IDC_IPADDRESS_SERVER, m_wdServerIpAddress);
}


BEGIN_MESSAGE_MAP(CAccountLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDC_MFCBTN_LOGIN, &CAccountLoginDlg::OnBnClickedMfcbtnLogin)
	ON_BN_CLICKED(IDC_MFCBTN_SIGN_IN, &CAccountLoginDlg::OnBnClickedMfcbtnSignIn)
	ON_BN_CLICKED(IDC_MFCBTN_RESET_PW, &CAccountLoginDlg::OnBnClickedMfcbtnResetPw)
END_MESSAGE_MAP()


// CAccountLoginDlg 메시지 처리기

void CAccountLoginDlg::OnBnClickedMfcbtnLogin()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 입력 한 데이터를 담는다.  
	UpdateData(TRUE);

	TRACE3("%s, %s, %s\n", m_edEmailID, m_edPassword, m_wdServerIpAddress);
	
	/*CT2CA convertedEmail(m_edEmailID);
	CT2CA convertedPassword(m_edPassword);
	CT2CA convertedIpAddress(m_wdServerIpAddress);*/

	spUserInfo->email = tstring(m_edEmailID);
	spUserInfo->password = tstring(m_edPassword);
	spUserInfo->server_ip_num = tstring(m_wdServerIpAddress);

	EndDialog((INT_PTR)KResponse::LOGIN);
}

void CAccountLoginDlg::OnBnClickedMfcbtnSignIn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	TRACE3("%s, %s, %s\n", m_edEmailID, m_edPassword, m_wdServerIpAddress);

	/*CT2CA convertedEmail(m_edEmailID);
	CT2CA convertedPassword(m_edPassword);
	CT2CA convertedIpAddress(m_wdServerIpAddress);*/

	spUserInfo->email = tstring(m_edEmailID);
	spUserInfo->password = tstring(m_edPassword);
	spUserInfo->server_ip_num = tstring(m_wdServerIpAddress);

	// 설정한 정보가 등록 되어 있지 않다면, CREATE 리턴
	EndDialog((INT_PTR)KResponse::CREATE_USER);

	// 설정한 정보가 등록 되어 있다면, UPDATE 리턴
	//EndDialog((INT_PTR)KResponse::UPDATE_USER);
}


void CAccountLoginDlg::OnBnClickedMfcbtnResetPw()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EndDialog((INT_PTR)KResponse::RESET_PASSWORD);
}
