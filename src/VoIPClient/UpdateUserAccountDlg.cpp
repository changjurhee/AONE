// CUpdateUserAccountDlg.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"

#include "MainFrm.h"
#include "VoIPClientDoc.h"

#include "afxdialogex.h"
#include "UpdateUserAccountDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "session/UiController.h"

// CUpdateUserAccountDlg 대화 상자

IMPLEMENT_DYNAMIC(CUpdateUserAccountDlg, CDialogEx)

CUpdateUserAccountDlg::CUpdateUserAccountDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_UPDATE_ACCOUNT, pParent)
{

}

CUpdateUserAccountDlg::~CUpdateUserAccountDlg()
{
}

void CUpdateUserAccountDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_PASSWORD, m_edPassword);
	DDX_Text(pDX, IDC_ED_EMAIL, m_edEmailID);
	DDX_Text(pDX, IDC_ED_LAST_NAME, m_edName);
}


BEGIN_MESSAGE_MAP(CUpdateUserAccountDlg, CDialogEx)
	ON_BN_CLICKED(IDC_MFCBTN_UPDATE_CONTACT, &CUpdateUserAccountDlg::OnBnClickedMfcbtnUpdateContact)
	ON_BN_CLICKED(IDC_MFCBTN_CANCEL, &CUpdateUserAccountDlg::OnBnClickedMfcbtnCancel)
	ON_MESSAGE(UWM_UI_CONTROLLER, &CUpdateUserAccountDlg::processUiControlNotify)
	ON_WM_CTLCOLOR()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

BOOL CUpdateUserAccountDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_RETURN  ) {
			OnBnClickedMfcbtnUpdateContact();
			return TRUE;
		} else if (pMsg->wParam == VK_ESCAPE) {
			return FALSE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CUpdateUserAccountDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE) {
		EndDialog((INT_PTR)KResponse::UPDATE_USER_CANCELED);
	}
	else {
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CUpdateUserAccountDlg::OnBnClickedMfcbtnUpdateContact()
{
	UpdateData(TRUE);
	string email = std::string(CT2CA(m_edEmailID));
	string pass = std::string(CT2CA(m_edPassword));
	string name = std::string(CT2CA(m_edName));
	UiController::getInstance()->req_updateContact(this, email, name, pass);
}


void CUpdateUserAccountDlg::OnBnClickedMfcbtnCancel()
{
	EndDialog((INT_PTR)KResponse::UPDATE_USER_CANCELED);
}

LRESULT CUpdateUserAccountDlg::processUiControlNotify(WPARAM wParam, LPARAM lParam)
{
	cout << "processUiControlNotify()/WPARAM:" << wParam << "/LPARAM:" << lParam << endl;
	switch (wParam)
	{
	case MSG_RESPONSE_UPDATE_CONTACT:
		if (lParam == 0) {
			cout << "SUCCESS" << endl;
			MessageBox(_T("Update Success"));
			EndDialog((INT_PTR)KResponse::UPDATE_USER_COMPLETE);
		}
		else if (lParam == 1) {
			cout << "FAILED" << endl;
			MessageBox(_T("Update failed : No CID exists"));
		} else if (lParam == 2) {
			cout << "FAILED" << endl;
			MessageBox(_T("Update failed : Missing email address"));
		} else {
			cout << "FAILED" << endl;
			MessageBox(_T("Update failed"));
		}
		break;
	default:
		break;
	}
	return LRESULT();
}


