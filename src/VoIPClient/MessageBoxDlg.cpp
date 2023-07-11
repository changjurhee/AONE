// MessageBoxDlg.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"
#include "afxdialogex.h"
#include "MessageBoxDlg.h"

#include "session/UiController.h"
#include "session/Constants.h"

// CMessageBoxDlg 대화 상자

IMPLEMENT_DYNAMIC(CMessageBoxDlg, CDialogEx)

CMessageBoxDlg::CMessageBoxDlg(CWnd* pParent /*=nullptr*/, int mode, CString cid)
	: CDialogEx(IDD_DLG_MSGBOX, pParent)
	, m_stMessage(cid)
	, m_mode(mode)
{

}

CMessageBoxDlg::~CMessageBoxDlg()
{
}

void CMessageBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ST_MSG, m_stMessage);
}

BEGIN_MESSAGE_MAP(CMessageBoxDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_MESSAGE(UWM_UI_CONTROLLER, processUiControlNotify)
	ON_BN_CLICKED(IDOK, &CMessageBoxDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMessageBoxDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

// CMessageBoxDlg 메시지 처리기

HBRUSH CMessageBoxDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return (HBRUSH)GetStockObject(WHITE_BRUSH);
}

LRESULT CMessageBoxDlg::processUiControlNotify(WPARAM wParam, LPARAM lParam)
{
	std::cout << "CMessageBoxDlg::wp: " << wParam << ", lp: " << lParam << std::endl;
	switch (wParam)
	{
	case MSG_RESPONSE_CALLSTATE:
	{
		CallResult* res = reinterpret_cast<CallResult*>(lParam);
		int result = res->result;
		if (result != CallState::STATE_DIALING && result != CallState::STATE_RINGING) {
			EndDialog(IDOK);
		}
		break;
	}
	default:
		break;
	}

	return LRESULT();
}

BOOL CMessageBoxDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	std::cout << "setCallbackWnd()" << std::endl;
	UiController::getInstance()->setCallbackWnd(this);
	SetWindowText(m_mode == (int)Msg::INCOMING ? _T("INCOMING") : _T("OUTGOING"));
	GetDlgItem(IDOK)->EnableWindow(m_mode == (int)Msg::INCOMING ? TRUE : FALSE);
	GetDlgItem(IDOK)->SetWindowText(_T("Answer"));
	GetDlgItem(IDCANCEL)->SetWindowText(m_mode == (int)Msg::INCOMING ? _T("Reject") : _T("End"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMessageBoxDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_mode == (int)Msg::INCOMING) UiController::getInstance()->request_AnswerCall(this);
}

void CMessageBoxDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_mode == (int)Msg::INCOMING) 
		UiController::getInstance()->request_RejectCall(this);
	else 
		UiController::getInstance()->request_EndCall(this);
}
