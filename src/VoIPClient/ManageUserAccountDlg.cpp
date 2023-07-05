// ManageUserAccountDlg.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"
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
END_MESSAGE_MAP()


// CManageUserAccountDlg 메시지 처리기
