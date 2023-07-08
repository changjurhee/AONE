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
	ON_WM_CTLCOLOR()
	ON_WM_NCPAINT()
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


HBRUSH CStartDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return (HBRUSH)GetStockObject(WHITE_BRUSH);
}

void CStartDlg::OnNcPaint()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialogEx::OnNcPaint()을(를) 호출하지 마십시오.
	CDC* pDC = GetWindowDC();
	CRect rect;
	GetWindowRect(&rect);
	rect.OffsetRect(-rect.left, -rect.top);

	CBrush brush(DKGRAY_BRUSH);
	pDC->FrameRect(&rect, &brush);
	ReleaseDC(pDC);
}
