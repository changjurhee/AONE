// SessionRegisterDlg.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"

#include "MainFrm.h"
#include "VoIPClientDoc.h"

#include "afxdialogex.h"
#include "SessionRegisterDlg.h"

#include "session/UiController.h"

// CSessionRegisterDlg 대화 상자

IMPLEMENT_DYNAMIC(CSessionRegisterDlg, CDialogEx)

CSessionRegisterDlg::CSessionRegisterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_SESSION_REGISTER, pParent)
	, m_nIndexContact(0)
	, m_nIndexParticipants(0)
	, m_Date(COleDateTime::GetCurrentTime())
	, m_StartTime(COleDateTime::GetCurrentTime())
	, m_endTime(COleDateTime::GetCurrentTime())
{

}

CSessionRegisterDlg::~CSessionRegisterDlg()
{
}

void CSessionRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LT_CONTACT, m_ltContact);
	DDX_Control(pDX, IDC_LT_PARTICIPANTS, m_ltParticipants);
	DDX_DateTimeCtrl(pDX, IDC_DP_DATE, m_Date);
	DDX_DateTimeCtrl(pDX, IDC_DP_TIME_START, m_StartTime);
	DDX_DateTimeCtrl(pDX, IDC_DP_TIME_END, m_endTime);
}


BEGIN_MESSAGE_MAP(CSessionRegisterDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_CLICK, IDC_LT_CONTACT, &CSessionRegisterDlg::OnNMClickLtContact)
	ON_NOTIFY(NM_CLICK, IDC_LT_PARTICIPANTS, &CSessionRegisterDlg::OnNMClickLtParticipants)
	ON_NOTIFY(NM_THEMECHANGED, IDC_DP_DATE, &CSessionRegisterDlg::OnNMThemeChangedDpDate)
	ON_NOTIFY(NM_THEMECHANGED, IDC_DP_TIME_START, &CSessionRegisterDlg::OnNMThemeChangedDpTimeStart)
	ON_NOTIFY(NM_THEMECHANGED, IDC_DP_TIME_END, &CSessionRegisterDlg::OnNMThemeChangedDpTimeEnd)
	ON_BN_CLICKED(IDC_MFCBTN_RIGHT, &CSessionRegisterDlg::OnBnClickedMfcbtnRight)
	ON_BN_CLICKED(IDC_MFCBTN_LEFT, &CSessionRegisterDlg::OnBnClickedMfcbtnLeft)
	ON_MESSAGE(UWM_UI_CONTROLLER, processUiControlNotify)
	ON_BN_CLICKED(IDC_MB_SESSION_CANCEL, &CSessionRegisterDlg::OnBnClickedMbSessionCancel)
	ON_BN_CLICKED(IDC_MB_SESSEION_CREATE, &CSessionRegisterDlg::OnBnClickedMbSesseionCreate)
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

void CSessionRegisterDlg::OnNMClickLtContact(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	m_nIndexContact = pNMItemActivate->iItem;
}


void CSessionRegisterDlg::OnNMClickLtParticipants(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	m_nIndexParticipants = pNMItemActivate->iItem;
}


void CSessionRegisterDlg::OnNMThemeChangedDpDate(NMHDR* pNMHDR, LRESULT* pResult)
{
	// 이 기능을 사용하려면 Windows XP 이상이 필요합니다.
	// _WIN32_WINNT 기호는 0x0501보다 크거나 같아야 합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSessionRegisterDlg::OnNMThemeChangedDpTimeStart(NMHDR* pNMHDR, LRESULT* pResult)
{
	// 이 기능을 사용하려면 Windows XP 이상이 필요합니다.
	// _WIN32_WINNT 기호는 0x0501보다 크거나 같아야 합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSessionRegisterDlg::OnNMThemeChangedDpTimeEnd(NMHDR* pNMHDR, LRESULT* pResult)
{
	// 이 기능을 사용하려면 Windows XP 이상이 필요합니다.
	// _WIN32_WINNT 기호는 0x0501보다 크거나 같아야 합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CSessionRegisterDlg::OnBnClickedMfcbtnRight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos;
	pos = m_ltContact.GetFirstSelectedItemPosition();
	std::vector<INT> vSelectedIndex;
	m_ltParticipants.LockWindowUpdate();
	m_ltContact.LockWindowUpdate();
	while (pos) {
		int nSelected = m_ltContact.GetNextSelectedItem(pos);
		tstring tmp_cid(m_ltContact.GetItemText(nSelected, 0));
		tstring tmp_email(m_ltContact.GetItemText(nSelected, 1));
		tstring tmp_name(m_ltContact.GetItemText(nSelected, 2));
		vSelectedIndex.push_back(nSelected);

		int num = m_ltParticipants.GetItemCount();
		int index_num = m_ltParticipants.InsertItem(num, tmp_cid.data());
		m_ltParticipants.SetItemText(index_num, 1, tmp_email.data());
		m_ltParticipants.SetItemText(index_num, 2, tmp_name.data());
	}
	for (auto iter = vSelectedIndex.rbegin(); iter != vSelectedIndex.rend(); iter++) {
		m_ltContact.DeleteItem(*iter);
	}
	m_ltContact.UnlockWindowUpdate();
	m_ltParticipants.UnlockWindowUpdate();
}

void CSessionRegisterDlg::OnBnClickedMfcbtnLeft()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	POSITION pos;
	pos = m_ltParticipants.GetFirstSelectedItemPosition();
	std::vector<INT> vSelectedIndex;
	m_ltParticipants.LockWindowUpdate();
	m_ltContact.LockWindowUpdate();
	while (pos) {
		int nSelected = m_ltParticipants.GetNextSelectedItem(pos);
		tstring tmp_cid(m_ltParticipants.GetItemText(nSelected, 0));
		tstring tmp_email(m_ltParticipants.GetItemText(nSelected, 1));
		tstring tmp_name(m_ltParticipants.GetItemText(nSelected, 2));
		vSelectedIndex.push_back(nSelected);

		int num = m_ltContact.GetItemCount();
		int index_num = m_ltContact.InsertItem(num, tmp_cid.data());
		m_ltContact.SetItemText(index_num, 1, tmp_email.data());
		m_ltContact.SetItemText(index_num, 2, tmp_name.data());
	}
	for (auto iter = vSelectedIndex.rbegin(); iter != vSelectedIndex.rend(); iter++) {
		m_ltParticipants.DeleteItem(*iter);
	}
	m_ltContact.UnlockWindowUpdate();
	m_ltParticipants.UnlockWindowUpdate();
}


BOOL CSessionRegisterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CRect rt;
	m_ltContact.GetWindowRect(&rt);
	m_ltContact.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	m_ltContact.InsertColumn(0, TEXT("cid"), LVCFMT_CENTER, rt.Width() * 20 / 100);
	m_ltContact.InsertColumn(1, TEXT("email"), LVCFMT_CENTER, rt.Width() * 50 / 100);
	m_ltContact.InsertColumn(2, TEXT("name"), LVCFMT_CENTER, rt.Width() * 30 / 100);

	m_ltParticipants.GetWindowRect(&rt);
	m_ltParticipants.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	m_ltParticipants.InsertColumn(0, TEXT("cid"), LVCFMT_CENTER, rt.Width() * 20 / 100);
	m_ltParticipants.InsertColumn(1, TEXT("email"), LVCFMT_CENTER, rt.Width() * 50 / 100);
	m_ltParticipants.InsertColumn(2, TEXT("name"), LVCFMT_CENTER, rt.Width() * 30 / 100);

	auto result_list = UiController::getInstance()->get_MyContacts();

	m_ltContact.DeleteAllItems();

	for (ContactData p : result_list) {
		tstring tmp_cid, tmp_email, tmp_name;

		int num = m_ltContact.GetItemCount();

		tmp_cid.assign(p.cid.begin(), p.cid.end());
		tmp_email.assign(p.email.begin(), p.email.end());
		tmp_name.assign(p.name.begin(), p.name.end());

		int index_num = m_ltContact.InsertItem(num, tmp_cid.data());
		m_ltContact.SetItemText(index_num, 1, tmp_email.data());
		m_ltContact.SetItemText(index_num, 2, tmp_name.data());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

LRESULT CSessionRegisterDlg::processUiControlNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	default:
		break;
	}

	return LRESULT();
}

void CSessionRegisterDlg::OnBnClickedMbSessionCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EndDialog(IDCANCEL);
}


void CSessionRegisterDlg::OnBnClickedMbSesseionCreate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	stOleToDate res = SetDateTime(m_Date, m_StartTime, m_endTime);
	if (res.duration < 0) {
		MessageBox(_T("Invalid Data and Time. Please Check the schedule."), _T("Error"));
		return;
	}
	int num = m_ltParticipants.GetItemCount();
	std::list<std::string> Participants;

	for (int i = 0; i < num; i++) {
		Participants.push_back(std::string(CT2CA(m_ltParticipants.GetItemText(i, 0))));
	}

	if (num != 0) UiController::getInstance()->request_CreateConference(this, res.dataAndTime, res.duration, Participants);
}
