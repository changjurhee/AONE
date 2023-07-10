#pragma once

#define WM_MY_WINDOW_CREATED (WM_USER + 1)

// CCallView 폼 보기

class CCallView : public CFormView
{
	DECLARE_DYNCREATE(CCallView)

protected:
	CCallView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CCallView();
	void OnInitialUpdate() override;
	LRESULT OnMyWindowCreated(WPARAM wParam, LPARAM lParam);

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CALL_VIEW };
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
	LRESULT OnVadNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedClientStartcall();
	afx_msg void OnBnClickedClientEndcall();
	afx_msg void OnBnClickedServerStartcall();
	afx_msg void OnBnDoubleclickedServerEndcall();
	afx_msg void OnBnClickedServerAddclient();
	afx_msg void OnBnClickedServerRemoveclient();
	CStatic mDisplayBox;
	afx_msg void OnBnClickedSetHandler();
};


