// ContactRegisterDlg.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"

#include "MainFrm.h"
#include "VoIPClientDoc.h"

#include "afxdialogex.h"
#include "ContactRegisterDlg.h"

#include "session/UiController.h"

// CContactRegisterDlg 대화 상자

IMPLEMENT_DYNAMIC(CContactRegisterDlg, CDialogEx)

CContactRegisterDlg::CContactRegisterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_CONTACT_REGISTER, pParent)
	, m_sSearchName(_T(""))
	, m_ltIndex(0)
{

}

CContactRegisterDlg::~CContactRegisterDlg()
{
}

void CContactRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LT_CONTACT_NAMES, m_ltContactNames);
	DDX_Text(pDX, IDC_ED_SEARCH_NAME, m_sSearchName);
}


BEGIN_MESSAGE_MAP(CContactRegisterDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_MFCBTN_SEARCH, &CContactRegisterDlg::OnBnClickedMfcbtnSearch)
	ON_MESSAGE(UWM_UI_CONTROLLER, processUiControlNotify)
	ON_NOTIFY(NM_CLICK, IDC_LT_CONTACT_NAMES, &CContactRegisterDlg::OnNMClickLtContactNames)
	ON_BN_CLICKED(IDOK, &CContactRegisterDlg::OnBnClickedOk)
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


void CContactRegisterDlg::OnBnClickedMfcbtnSearch()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	auto result_list = UiController::getInstance()->get_SearchResult(std::string(CT2CA(m_sSearchName)));
	
	if (!result_list.empty()) {
		m_ltContactNames.DeleteAllItems();

		int itemIndex = 0;

		m_ltContactNames.LockWindowUpdate();
		for (ContactData p : result_list) {
			int num = m_ltContactNames.InsertItem(itemIndex, CString(p.cid.c_str()));
			m_ltContactNames.SetItemText(num, 1, CString(p.email.c_str()));
			m_ltContactNames.SetItemText(num, 2, CString(p.name.c_str()));
			itemIndex++;
		}
		m_ltContactNames.UnlockWindowUpdate();
	}

	UpdateData(FALSE);
}

LRESULT CContactRegisterDlg::processUiControlNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	default:
		break;
	}

	return LRESULT();
}

BOOL CContactRegisterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CRect rt;
	m_ltContactNames.GetWindowRect(&rt);
	m_ltContactNames.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	m_ltContactNames.InsertColumn(0, TEXT("cid"), LVCFMT_CENTER, rt.Width() * 0.2);
	m_ltContactNames.InsertColumn(1, TEXT("email"), LVCFMT_CENTER, rt.Width() * 0.5);
	m_ltContactNames.InsertColumn(2, TEXT("name"), LVCFMT_CENTER, rt.Width() * 0.3);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CContactRegisterDlg::OnNMClickLtContactNames(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	m_ltIndex = pNMItemActivate->iItem;

}

void CContactRegisterDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	CString cid = m_ltContactNames.GetItemText(m_ltIndex, 0);
	UiController::getInstance()->req_AddMyContact(this, std::string(CT2CA(cid)));
}
