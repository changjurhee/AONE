// ManageUserAccountDlg.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"

#include "MainFrm.h"
#include "VoIPClientDoc.h"

#include "afxdialogex.h"
#include "ManageUserAccountDlg.h"

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
}


BEGIN_MESSAGE_MAP(CManageUserAccountDlg, CDialogEx)
	ON_BN_CLICKED(IDC_MFCBTN_CREATE, &CManageUserAccountDlg::OnBnClickedMfcbtnCreate)
	ON_BN_CLICKED(IDC_MFCBTN_UPDATE, &CManageUserAccountDlg::OnBnClickedMfcbtnUpdate)
	ON_WM_CTLCOLOR()
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


void CManageUserAccountDlg::OnBnClickedMfcbtnCreate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CManageUserAccountDlg::OnBnClickedMfcbtnUpdate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
