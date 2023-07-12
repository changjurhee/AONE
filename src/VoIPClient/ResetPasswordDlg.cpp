// ResetPasswordDlg.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"

#include "MainFrm.h"
#include "VoIPClientDoc.h"

#include "afxdialogex.h"
#include "ResetPasswordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "session/UiController.h"
// CResetPasswordDlg 대화 상자

IMPLEMENT_DYNAMIC(CResetPasswordDlg, CDialogEx)

CResetPasswordDlg::CResetPasswordDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RESET_PASSWORD, pParent)
{

}

CResetPasswordDlg::~CResetPasswordDlg()
{
}

void CResetPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_ID, m_edId);
	DDX_Text(pDX, IDC_ED_PASSWORD, m_edPassword);
	DDX_Text(pDX, IDC_ED_PASSWORD_ANSWER, m_edPasswordAnswer);
}

BEGIN_MESSAGE_MAP(CResetPasswordDlg, CDialogEx)
	ON_BN_CLICKED(IDC_MFCBTN_RESET, &CResetPasswordDlg::OnBnClickedMfcbtnReset)
	ON_BN_CLICKED(IDC_RESET_CANCEL, &CResetPasswordDlg::OnBnClickedMfcbtnCancel)
	ON_MESSAGE(UWM_UI_CONTROLLER, &CResetPasswordDlg::processUiControlNotify)
	ON_WM_CTLCOLOR()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

// CResetPasswordDlg 메시지 처리기
CVoIPClientDoc* CResetPasswordDlg::GetDocument() const
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CVoIPClientDoc* m_pDocument = (CVoIPClientDoc*)pFrame->GetActiveDocument();
	return (CVoIPClientDoc*)m_pDocument;
}

HBRUSH CResetPasswordDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return (HBRUSH)GetStockObject(WHITE_BRUSH);
}

BOOL CResetPasswordDlg::OnInitDialog()
{
	CComboBox* combo = (CComboBox*)GetDlgItem(IDC_CB_PASSWORD_QUESTION);
	combo->AddString(_T("1: What is your favorite city?"));
	combo->AddString(_T("2: What is your favorite food?"));
	combo->AddString(_T("3: What is your favorite car?"));
	combo->SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CResetPasswordDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_RETURN  ) {
			OnBnClickedMfcbtnReset();
			return TRUE;
		} else if (pMsg->wParam == VK_ESCAPE) {
			return FALSE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CResetPasswordDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE) {
		EndDialog((INT_PTR)KResponse::RESET_PASSWORD_COMPLETE);
	}
	else {
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CResetPasswordDlg::OnBnClickedMfcbtnReset()
{
	UpdateData(TRUE);
	string id = std::string(CT2CA(m_edId));
	string pass = std::string(CT2CA(m_edPassword));
	CComboBox* combo = (CComboBox*)GetDlgItem(IDC_CB_PASSWORD_QUESTION);
	int questionIndex = combo->GetCurSel();
	string answer = std::string(CT2CA(m_edPasswordAnswer));

	UiController::getInstance()->req_ResetPassword(this, id, pass, questionIndex, answer);
}


void CResetPasswordDlg::OnBnClickedMfcbtnCancel()
{
	EndDialog((INT_PTR)KResponse::RESET_PASSWORD_CANCELED);
}

LRESULT CResetPasswordDlg::processUiControlNotify(WPARAM wParam, LPARAM lParam)
{
	cout << "processUiControlNotify()/WPARAM:" << wParam << "/LPARAM:" << lParam << endl;
	switch (wParam)
	{
	case MSG_RESPONSE_RESET_PW:
		if (lParam == 0) {
			cout << "SUCCESS" << endl;
			MessageBox(_T("Reset Password Success"));
			EndDialog((INT_PTR)KResponse::RESET_PASSWORD_COMPLETE);
		}
		else if (lParam == 1) {
			cout << "FAILED" << endl;
			MessageBox(_T("Reset Password failed : No ID exists"));
		} else if (lParam == 2) {
			cout << "FAILED" << endl;
			MessageBox(_T("Reset Password failed : Wrong password info"));
		} else {
			cout << "FAILED" << endl;
			MessageBox(_T("Reset Password FAILED"));
		}
		break;
	default:
		break;
	}
	return LRESULT();
}

