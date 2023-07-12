// ContactListView.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPServer.h"
#include "ContactListView.h"

#include "session/SUiController.h"

// CContactListView

IMPLEMENT_DYNCREATE(CContactListView, CFormView)

CContactListView::CContactListView()
	: CFormView(IDD_LIST_VIEW)
	, m_edPreSet(_T("0"))
	, m_edLatency(_T("100"))
{
	m_PreSetValue = 0;
	m_LatencyValue = 100;
}

CContactListView::~CContactListView()
{
}

void CContactListView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LT_CONTACT, m_ltContactNames);
	DDX_Control(pDX, IDC_LT_CONFERENCE, m_ltConferenceNames);
	DDX_Control(pDX, IDC_MFCBTN_CHANGE_STATE, m_btnEnable);
	DDX_Control(pDX, IDC_MFCBTN_CHANGE_DISABLE, m_btnDisable);
	DDX_Control(pDX, IDC_MFCBTN_DELETE, m_btnDelete);
	DDX_Control(pDX, IDC_MFCBTN_REFRESH_CONTACT, m_btnContactRefresh);
	DDX_Control(pDX, IDC_MFCBTN_REFRESH_CONF, m_btnConfRefresh);
	DDX_Text(pDX, IDC_ED_PRESET, m_edPreSet);
	DDX_Text(pDX, IDC_ED_LATENCY, m_edLatency);
}

BEGIN_MESSAGE_MAP(CContactListView, CFormView)
	ON_NOTIFY(NM_CLICK, IDC_LT_CONTACT, &CContactListView::OnNMClickLtContact)
	ON_MESSAGE(UWM_SUI_CONTROLLER, processUiControlNotify)
	ON_BN_CLICKED(IDC_MFCBTN_CHANGE_STATE, &CContactListView::OnBnClickedMfcbtnChangeState)
	ON_BN_CLICKED(IDC_MFCBTN_DELETE, &CContactListView::OnBnClickedMfcbtnDelete)
	ON_BN_CLICKED(IDC_MFCBTN_CHANGE_DISABLE, &CContactListView::OnBnClickedMfcbtnChangeDisable)
	ON_BN_CLICKED(IDC_MFCBTN_REFRESH_CONTACT, &CContactListView::OnBnClickedMfcbtnRefreshContact)
	ON_BN_CLICKED(IDC_MFCBTN_REFRESH_CONF, &CContactListView::OnBnClickedMfcbtnRefreshConf)
	ON_WM_CTLCOLOR()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PRESET, &CContactListView::OnDeltaposSpinPreset)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LATENCY, &CContactListView::OnDeltaposSpinLatency)
	ON_EN_CHANGE(IDC_ED_PRESET, &CContactListView::OnEnChangeEdPreset)
	ON_EN_CHANGE(IDC_ED_LATENCY, &CContactListView::OnEnChangeEdLatency)
END_MESSAGE_MAP()


// CContactListView 진단

#ifdef _DEBUG
void CContactListView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CContactListView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CContactListView 메시지 처리기


void CContactListView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CRect rt;
	m_ltContactNames.GetWindowRect(&rt);
	m_ltContactNames.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ltContactNames.InsertColumn(0, TEXT("ID"), LVCFMT_CENTER, rt.Width() * 10 / 100);
	m_ltContactNames.InsertColumn(1, TEXT("EMAIL"), LVCFMT_CENTER, rt.Width() * 40 / 100);
	m_ltContactNames.InsertColumn(2, TEXT("NAME"), LVCFMT_CENTER, rt.Width() * 30 / 100);
	m_ltContactNames.InsertColumn(3, TEXT("ENABLED"), LVCFMT_CENTER, rt.Width() * 10 / 100);
	m_ltContactNames.InsertColumn(4, TEXT("LOGIN"), LVCFMT_CENTER, rt.Width() * 10 / 100);

	m_ltConferenceNames.GetWindowRect(&rt);
	m_ltConferenceNames.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ltConferenceNames.InsertColumn(0, TEXT("ID"), LVCFMT_CENTER, rt.Width() * 10 / 100);
	m_ltConferenceNames.InsertColumn(1, TEXT("PARTICIPANTS"), LVCFMT_CENTER, rt.Width() * 40 / 100);
	m_ltConferenceNames.InsertColumn(2, TEXT("START"), LVCFMT_CENTER, rt.Width() * 20 / 100);
	m_ltConferenceNames.InsertColumn(3, TEXT("END"), LVCFMT_CENTER, rt.Width() * 20 / 100);

	SUiController::getInstance()->setCallbackWnd(this);
	UpdateContactView();
	UpdateConferenceView();
}


void CContactListView::OnNMClickLtContact(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	m_ltIndex = pNMItemActivate->iItem;
}

void CContactListView::OnBnClickedMfcbtnChangeState()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	CString cid = m_ltContactNames.GetItemText(m_ltIndex, 0);
	SUiController::getInstance()->req_enableContact(this, std::string(CT2CA(cid)), true);
	UpdateData(FALSE);
}

void CContactListView::OnBnClickedMfcbtnChangeDisable()
{
	UpdateData(TRUE);
	CString cid = m_ltContactNames.GetItemText(m_ltIndex, 0);
	SUiController::getInstance()->req_enableContact(this, std::string(CT2CA(cid)), false);
	UpdateData(FALSE);
}


void CContactListView::OnBnClickedMfcbtnDelete()
{
	UpdateData(TRUE);
	CString cid = m_ltContactNames.GetItemText(m_ltIndex, 0);
	SUiController::getInstance()->req_deleteContact(this, std::string(CT2CA(cid)));
	UpdateData(FALSE);
}

void CContactListView::OnBnClickedMfcbtnRefreshContact()
{
	UpdateContactView();
}

void CContactListView::OnBnClickedMfcbtnRefreshConf()
{
	UpdateConferenceView();
}

void CContactListView::UpdateContactView()
{
	UpdateData(TRUE);

	auto contact_list = SUiController::getInstance()->get_AllContacts();
	m_ltContactNames.DeleteAllItems();
	if (!contact_list.empty()) {
		int itemIndex = 0;
		m_ltContactNames.LockWindowUpdate();
		for (ContactData p : contact_list) {
			int num = m_ltContactNames.InsertItem(itemIndex, CString(p.cid.c_str()));
			m_ltContactNames.SetItemText(num, 1, CString(p.email.c_str()));
			m_ltContactNames.SetItemText(num, 2, CString(p.name.c_str()));
			m_ltContactNames.SetItemText(num, 3, CString(p.enabled ? "TRUE" : "FALSE"));
			m_ltContactNames.SetItemText(num, 4, CString(p.login ? "O" : ""));
			itemIndex++;
		}
		m_ltContactNames.UnlockWindowUpdate();
	}
	UpdateData(FALSE);
}

void CContactListView::UpdateConferenceView()
{
	UpdateData(TRUE);
	auto conference_list = SUiController::getInstance()->get_AllConferences();
	m_ltConferenceNames.DeleteAllItems();
	if (!conference_list.empty()) {
		int itemIndex = 0;
		m_ltConferenceNames.LockWindowUpdate();
		for (ConferenceData p : conference_list) {
			int num = m_ltConferenceNames.InsertItem(itemIndex, CString(p.rid.c_str()));
			string allParticipants;
			for (string parti : p.participants) {
				allParticipants += parti + " ";
			}
			m_ltConferenceNames.SetItemText(num, 1, CString(allParticipants.c_str()));
			std::pair<std::string, std::string> res = GetDateTime(p.dataAndTime, p.duration);
			m_ltConferenceNames.SetItemText(num, 2, CString(res.first.c_str()));
			m_ltConferenceNames.SetItemText(num, 3, CString(res.second.c_str()));
			itemIndex++;
		}
		m_ltContactNames.UnlockWindowUpdate();
	}

	UpdateData(FALSE);
}

LRESULT CContactListView::processUiControlNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case MSG_RESPONSE_UPDATE_DATA_CONFERENCE:
		UpdateConferenceView();		
		break;
	case MSG_RESPONSE_UPDATE_DATA_CONTACT:
		UpdateContactView();
		break;
	default:
		break;
	}

	return LRESULT();
}

HBRUSH CContactListView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return (HBRUSH)GetStockObject(WHITE_BRUSH);
}

void CContactListView::OnDeltaposSpinPreset(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0; // 0 ~ 5
	if (pNMUpDown->iDelta < 0) {
		if (m_PreSetValue < 5) m_PreSetValue++;
	}
	else {
		if (m_PreSetValue > 0) m_PreSetValue--;
	}
	SetDlgItemInt(IDC_ED_PRESET, m_PreSetValue);
	//m_edPreSet.Format(_T("%d"), m_PreSetValue);
}


void CContactListView::OnDeltaposSpinLatency(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0; // 100 ~ 2000
	if (pNMUpDown->iDelta < 0) {
		if (m_LatencyValue < 2000) m_LatencyValue++;
	}
	else {
		if (m_LatencyValue > 100) m_LatencyValue--;
	}

	SetDlgItemInt(IDC_ED_LATENCY, m_LatencyValue);
	//m_edLatency.Format(_T("%d"), m_LatencyValue);
}


void CContactListView::OnEnChangeEdPreset()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TRACE1("%d", m_PreSetValue);
	// 여기에 필요한 작업 하세요.
}


void CContactListView::OnEnChangeEdLatency()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TRACE1("%d", m_LatencyValue);
	// 여기에 필요한 작업 하세요.

}
