#pragma once
#include "afxdialogex.h"


// CContactRegisterDlg 대화 상자

class CContactRegisterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CContactRegisterDlg)

public:
	CContactRegisterDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CContactRegisterDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CONTACT_REGISTER };
#endif

public:
	CVoIPClientDoc* GetDocument() const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

};
