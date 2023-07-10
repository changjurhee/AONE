#pragma once
#include "afxdialogex.h"


// CSessionRegisterDlg 대화 상자

class CSessionRegisterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSessionRegisterDlg)

public:
	CSessionRegisterDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CSessionRegisterDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SESSION_REGISTER };
#endif

public:
	CVoIPClientDoc* GetDocument() const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

};
