#pragma once
#include "afxdialogex.h"


// CResetPasswordDlg 대화 상자

class CResetPasswordDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CResetPasswordDlg)

public:
	CResetPasswordDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CResetPasswordDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RESET_PASSWORD };
#endif

public:
	CVoIPClientDoc* GetDocument() const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

};
