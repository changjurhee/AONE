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
{

}

CContactListView::~CContactListView()
{
}

void CContactListView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LT_CONTACT, m_ltContactNames);
}

BEGIN_MESSAGE_MAP(CContactListView, CFormView)
	ON_NOTIFY(NM_CLICK, IDC_LT_CONTACT, &CContactListView::OnNMClickLtContact)
	ON_BN_CLICKED(IDC_MFCBTN_SEARCH, &CContactListView::OnBnClickedMfcbtnSearch)
	ON_MESSAGE(UWM_SUI_CONTROLLER, processUiControlNotify)
	ON_BN_CLICKED(IDC_MFCBTN_CHANGE_STATE, &CContactListView::OnBnClickedMfcbtnChangeState)
	ON_BN_CLICKED(IDC_MFCBTN_DELETE, &CContactListView::OnBnClickedMfcbtnDelete)
	ON_BN_CLICKED(IDC_MFCBTN_CHANGE_DISABLE, &CContactListView::OnBnClickedMfcbtnChangeDisable)
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

	m_ltContactNames.InsertColumn(0, TEXT("cid"), LVCFMT_CENTER, rt.Width() * 20 / 100);
	m_ltContactNames.InsertColumn(1, TEXT("email"), LVCFMT_CENTER, rt.Width() * 40 / 100);
	m_ltContactNames.InsertColumn(2, TEXT("name"), LVCFMT_CENTER, rt.Width() * 30 / 100);

}


void CContactListView::OnNMClickLtContact(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	m_ltIndex = pNMItemActivate->iItem;
}


void CContactListView::OnBnClickedMfcbtnSearch()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateView();
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	CString cid = m_ltContactNames.GetItemText(m_ltIndex, 0);
	SUiController::getInstance()->req_enableContact(this, std::string(CT2CA(cid)), false);
	UpdateData(FALSE);
}

void CContactListView::OnBnClickedMfcbtnDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	CString cid = m_ltContactNames.GetItemText(m_ltIndex, 0);
	SUiController::getInstance()->req_deleteContact(this, std::string(CT2CA(cid)));
	UpdateData(FALSE);
}

void CContactListView::UpdateView()
{
	UpdateData(TRUE);

	auto result_list = SUiController::getInstance()->get_AllContacts();

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

LRESULT CContactListView::processUiControlNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case 	MSG_RESPONSE_UPDATE_DATA:
		UpdateView();
		break;
	default:
		break;
	}

	return LRESULT();
}
