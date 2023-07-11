#pragma once



// CContactListView 폼 보기

class CContactListView : public CFormView
{
	DECLARE_DYNCREATE(CContactListView)

protected:
	CContactListView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CContactListView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIST_VIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	CListCtrl m_ltContactNames;
	INT m_ltIndex;
	afx_msg void OnNMClickLtContact(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedMfcbtnSearch();
	LRESULT processUiControlNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedMfcbtnChangeState();
	afx_msg void OnBnClickedMfcbtnDelete();
	afx_msg void OnBnClickedMfcbtnChangeDisable();
	void UpdateView();
};


