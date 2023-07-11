// MessageBoxDlg.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"
#include "afxdialogex.h"
#include "MessageBoxDlg.h"


// CMessageBoxDlg 대화 상자

IMPLEMENT_DYNAMIC(CMessageBoxDlg, CDialogEx)

CMessageBoxDlg::CMessageBoxDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_MSGBOX, pParent)
{

}

CMessageBoxDlg::~CMessageBoxDlg()
{
}

void CMessageBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMessageBoxDlg, CDialogEx)
END_MESSAGE_MAP()


// CMessageBoxDlg 메시지 처리기
