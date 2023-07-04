#pragma once



// CCallListView 폼 보기

class CCallListView : public CFormView
{
	DECLARE_DYNCREATE(CCallListView)

protected:
	CCallListView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CCallListView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CALL_LIST_VIEW };
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
};


