// ResetPasswordDlg.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"

#include "MainFrm.h"
#include "VoIPClientDoc.h"

#include "afxdialogex.h"
#include "ResetPasswordDlg.h"

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
}


BEGIN_MESSAGE_MAP(CResetPasswordDlg, CDialogEx)
	ON_WM_CTLCOLOR()
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