// ContactRegisterDlg.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"

#include "MainFrm.h"
#include "VoIPClientDoc.h"

#include "afxdialogex.h"
#include "ContactRegisterDlg.h"


// CContactRegisterDlg 대화 상자

IMPLEMENT_DYNAMIC(CContactRegisterDlg, CDialogEx)

CContactRegisterDlg::CContactRegisterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_CONTACT_REGISTER, pParent)
{

}

CContactRegisterDlg::~CContactRegisterDlg()
{
}

void CContactRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CContactRegisterDlg, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CContactRegisterDlg 메시지 처리기

CVoIPClientDoc* CContactRegisterDlg::GetDocument() const
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CVoIPClientDoc* m_pDocument = (CVoIPClientDoc*)pFrame->GetActiveDocument();
	return (CVoIPClientDoc*)m_pDocument;
}

HBRUSH CContactRegisterDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return (HBRUSH)GetStockObject(WHITE_BRUSH);
}
