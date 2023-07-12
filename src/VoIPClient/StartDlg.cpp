// StartDlg.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"

#include "MainFrm.h"
#include "VoIPClientDoc.h"

#include "afxdialogex.h"
#include "StartDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "session/UiController.h"

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
	ON_MESSAGE(UWM_UI_CONTROLLER, &CStartDlg::processUiControlNotify)
	ON_WM_CTLCOLOR()
	ON_WM_NCPAINT()
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_MFCBTN_SHUTDOWN, &CStartDlg::OnBnClickedMfcbtnShutdown)
END_MESSAGE_MAP()

// CStartDlg 메시지 처리기
CVoIPClientDoc* CStartDlg::GetDocument() const
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CVoIPClientDoc* m_pDocument = (CVoIPClientDoc*)pFrame->GetActiveDocument();
	return (CVoIPClientDoc*)m_pDocument;
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

BOOL CStartDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_RETURN  ) {
			OnBnClickedMfcbtnStart();
			return TRUE;
		} else if (pMsg->wParam == VK_ESCAPE) {
			return FALSE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CStartDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE) {
		EndDialog((INT_PTR)KResponse::CONNECT_FAILED);
	}
	else {
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CStartDlg::OnBnClickedMfcbtnStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	// @todo 체크 결과 정상이면, 
	std::shared_ptr<userInfo> spUserInfo = GetDocument()->GetUser();
	spUserInfo->server_ip_num = tstring(m_sIpAddressServer);
	GetDocument()->SetUser(spUserInfo);

	GetDlgItem(IDC_MFCBTN_START)->EnableWindow(false);
	UiController::getInstance()->req_Connect(this, std::string(CT2CA(m_sIpAddressServer)));
}

LRESULT CStartDlg::processUiControlNotify(WPARAM wParam, LPARAM lParam)
{
	cout << "processUiControlNotify()/WPARAM:" << wParam << "/LPARAM:" << lParam << endl;
	switch (wParam)
	{
	case MSG_RESPONSE_CONNECT:
		GetDlgItem(IDC_MFCBTN_START)->EnableWindow(true);
		if (lParam == 0) {
			cout << "SUCCESS" << endl;
			MessageBox(_T("Connection Success"));
			EndDialog((INT_PTR)KResponse::CONNECT_COMPLETE);
		}
		else {
			cout << "FAILED" << endl;
			MessageBox(_T("Connection FAILED"));
		}
		break;
	default:
		break;
	}
	return LRESULT();
}


void CStartDlg::OnBnClickedMfcbtnShutdown()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EndDialog((INT_PTR)KResponse::SHUT_DOWN);
}
