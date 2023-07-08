// StartDlg.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"

#include "MainFrm.h"
#include "VoIPClientDoc.h"

#include "afxdialogex.h"
#include "StartDlg.h"

// CStartDlg 대화 상자

IMPLEMENT_DYNAMIC(CStartDlg, CDialogEx)

CStartDlg::CStartDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_START, pParent)
	, m_sIpAddressServer(_T("127.0.0.1"))
{

}

CStartDlg::~CStartDlg()
{
}

void CStartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_IPADDRESS_SERVER, m_sIpAddressServer);
}


BEGIN_MESSAGE_MAP(CStartDlg, CDialogEx)
	ON_BN_CLICKED(IDC_MFCBTN_START, &CStartDlg::OnBnClickedMfcbtnStart)
END_MESSAGE_MAP()


// CStartDlg 메시지 처리기

CVoIPClientDoc* CStartDlg::GetDocument() const
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CVoIPClientDoc* m_pDocument = (CVoIPClientDoc*)pFrame->GetActiveDocument();
	return (CVoIPClientDoc*)m_pDocument;
}

void CStartDlg::OnBnClickedMfcbtnStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	// @todo 체크 결과 정상이면, 
	std::shared_ptr<userInfo> spUserInfo = GetDocument()->GetUser();
	spUserInfo->server_ip_num = tstring(m_sIpAddressServer);
	GetDocument()->SetUser(spUserInfo);

	EndDialog((INT_PTR)KResponse::START);
}
