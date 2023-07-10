// ManageUserAccountDlg.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"

#include "MainFrm.h"
#include "VoIPClientDoc.h"

#include "afxdialogex.h"
#include "ManageUserAccountDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "session/UiController.h"

// CManageUserAccountDlg 대화 상자

IMPLEMENT_DYNAMIC(CManageUserAccountDlg, CDialogEx)

CManageUserAccountDlg::CManageUserAccountDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_MANAGE_ACCOUNT, pParent)
{
}

CManageUserAccountDlg::~CManageUserAccountDlg()
{
}

void CManageUserAccountDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_ACCOUNT_ID, m_edID);
	DDX_Text(pDX, IDC_ED_PASSWORD, m_edPassword);
	DDX_Text(pDX, IDC_ED_EMAIL, m_edEmailID);
	DDX_Text(pDX, IDC_ED_LAST_NAME, m_edName);
	DDX_Text(pDX, IDC_ED_PASSWORD_ANSWER, m_edPasswordAnswer);
}

BEGIN_MESSAGE_MAP(CManageUserAccountDlg, CDialogEx)
	ON_BN_CLICKED(IDC_MFCBTN_CREATE, &CManageUserAccountDlg::OnBnClickedMfcbtnCreate)
	ON_BN_CLICKED(IDC_MFCBTN_CANCEL, &CManageUserAccountDlg::OnBnClickedMfcbtnCancel)
	ON_MESSAGE(UWM_UI_CONTROLLER, &CManageUserAccountDlg::processUiControlNotify)
	ON_WM_CTLCOLOR()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

// CManageUserAccountDlg 메시지 처리기
CVoIPClientDoc* CManageUserAccountDlg::GetDocument() const
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CVoIPClientDoc* m_pDocument = (CVoIPClientDoc*)pFrame->GetActiveDocument();
	return (CVoIPClientDoc*)m_pDocument;
}

HBRUSH CManageUserAccountDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return (HBRUSH)GetStockObject(WHITE_BRUSH);
}

BOOL CManageUserAccountDlg::OnInitDialog()
{
	CComboBox* combo = (CComboBox*)GetDlgItem(IDC_COMBO_PASSWORD_QUESTION);
	combo->AddString(_T("1: What is your favorite city?"));
	combo->AddString(_T("2: What is your favorite food?"));
	combo->AddString(_T("3: What is your favorite car?"));
	combo->SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CManageUserAccountDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_RETURN  ) {
			OnBnClickedMfcbtnCreate();
			return TRUE;
		} else if (pMsg->wParam == VK_ESCAPE) {
			return FALSE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CManageUserAccountDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE) {
		EndDialog((INT_PTR)KResponse::SIGNIN_CANCELED);
	}
	else {
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CManageUserAccountDlg::OnBnClickedMfcbtnCreate()
{
	UpdateData(TRUE);
	string id = std::string(CT2CA(m_edID));
	string email = std::string(CT2CA(m_edEmailID));
	string pass = std::string(CT2CA(m_edPassword));
	string name = std::string(CT2CA(m_edName));
	CComboBox* combo = (CComboBox*)GetDlgItem(IDC_COMBO_PASSWORD_QUESTION);
	int questionIndex = combo->GetCurSel();
	string passAnswer = std::string(CT2CA(m_edPasswordAnswer));

	UiController::getInstance()->req_Register(this, id, email, pass, name, questionIndex, passAnswer);
}

void CManageUserAccountDlg::OnBnClickedMfcbtnCancel()
{
	EndDialog((INT_PTR)KResponse::SIGNIN_CANCELED);
}

LRESULT CManageUserAccountDlg::processUiControlNotify(WPARAM wParam, LPARAM lParam)
{
	cout << "processUiControlNotify()/WPARAM:" << wParam << "/LPARAM:" << lParam << endl;
	switch (wParam)
	{
	case MSG_RESPONSE_REGISTER:
		if (lParam == 0) {
			cout << "SUCCESS" << endl;
			MessageBox(_T("SignIn Success"));
			EndDialog((INT_PTR)KResponse::SIGNIN_COMPLETE);
		}
		else {
			cout << "FAILED" << endl;
			MessageBox(_T("SignIn FAILED"));
		}
		break;
	default:
		break;
	}
	return LRESULT();
}