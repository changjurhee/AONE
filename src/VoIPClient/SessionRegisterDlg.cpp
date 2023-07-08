// SessionRegisterDlg.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"

#include "MainFrm.h"
#include "VoIPClientDoc.h"

#include "afxdialogex.h"
#include "SessionRegisterDlg.h"


// CSessionRegisterDlg 대화 상자

IMPLEMENT_DYNAMIC(CSessionRegisterDlg, CDialogEx)

CSessionRegisterDlg::CSessionRegisterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_SESSION_REGISTER, pParent)
{

}

CSessionRegisterDlg::~CSessionRegisterDlg()
{
}

void CSessionRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSessionRegisterDlg, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CSessionRegisterDlg 메시지 처리기

CVoIPClientDoc* CSessionRegisterDlg::GetDocument() const
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CVoIPClientDoc* m_pDocument = (CVoIPClientDoc*)pFrame->GetActiveDocument();
	return (CVoIPClientDoc*)m_pDocument;
}

HBRUSH CSessionRegisterDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return (HBRUSH)GetStockObject(WHITE_BRUSH);
}
