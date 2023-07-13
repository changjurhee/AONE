#pragma once



// CVoIPNewClient 폼 보기

class CVoIPNewClient : public CFormView
{
	DECLARE_DYNCREATE(CVoIPNewClient)

protected:
	CVoIPNewClient();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CVoIPNewClient();

	// 특성입니다.
public:
	CVoIPClientDoc* GetDocument() const;
	CStatic m_DisplayBox;

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_VIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	HWND GetBitmapHandle() { return m_DisplayBox.GetSafeHwnd(); }
	void AdjustLayout();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMfcbtnConnect();
	afx_msg void OnBnClickedMfcVad();
	afx_msg void OnBnClickedMfcEcho();

	LRESULT processUiControlNotify(WPARAM wParam, LPARAM lParam);
	virtual void OnDraw(CDC* /*pDC*/);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedCheckVad();
	afx_msg void OnBnClickedCheckEcho();
	CButton m_chkVAD;
	CButton m_chkEcho;
	afx_msg void OnBnClickedrtplog();
};


