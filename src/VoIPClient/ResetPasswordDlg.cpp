// ResetPasswordDlg.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"

#include "MainFrm.h"
#include "VoIPClientDoc.h"

#include "afxdialogex.h"
#include "ResetPasswordDlg.h"

#include "session/UiController.h"

// CResetPasswordDlg 대화 상자

IMPLEMENT_DYNAMIC(CResetPasswordDlg, CDialogEx)

CResetPasswordDlg::CResetPasswordDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RESET_PASSWORD, pParent)
	, m_sNewPassword(_T(""))
	, m_edPasswordAnswer(_T(""))
{

}

CResetPasswordDlg::~CResetPasswordDlg()
{
}

void CResetPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_PASSWORD, m_sNewPassword);
	DDX_Control(pDX, IDC_CB_PASSWORD_QUESTION, m_cbPassword);
	DDX_Text(pDX, IDC_ED_PASSWORD_ANSWER, m_edPasswordAnswer);
}


BEGIN_MESSAGE_MAP(CResetPasswordDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_MESSAGE(UWM_UI_CONTROLLER, processUiControlNotify)
	ON_BN_CLICKED(IDOK, &CResetPasswordDlg::OnBnClickedOk)
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

LRESULT CResetPasswordDlg::processUiControlNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case MSG_RESPONSE_RESET_PW:
		if (lParam == 0) {
			MessageBox(_T("Valid Password Reset."));
		}
		else {
			MessageBox(_T("Invalid Password Reset."));
		}
		CDialogEx::OnOK();
		break;
	default:
		break;
	}

	return LRESULT();
}

void CResetPasswordDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	UiController::getInstance()->req_ResetPassword(this, std::string(CT2CA(m_sNewPassword)), m_cbPassword.GetCurSel(), std::string(CT2CA(m_edPasswordAnswer)));
}


BOOL CResetPasswordDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_cbPassword.AddString(_T("1: What is your favorite city?"));
	m_cbPassword.AddString(_T("2: What is your favorite food?"));
	m_cbPassword.AddString(_T("3: What is your favorite car?"));
	m_cbPassword.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
