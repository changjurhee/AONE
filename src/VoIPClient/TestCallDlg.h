#pragma once
#include "afxdialogex.h"


// CTestCallDlg 대화 상자

class CTestCallDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTestCallDlg)

public:
	CTestCallDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTestCallDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CALL_VIEW };
#endif
public:
	CVoIPClientDoc* GetDocument() const;

	CStatic mDisplayBox;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedClientStartcall();
	afx_msg void OnBnClickedClientEndcall();
	afx_msg void OnBnClickedServerStartcall();
	afx_msg void OnBnDoubleclickedServerEndcall();
	afx_msg void OnBnClickedServerAddclient();
	afx_msg void OnBnClickedServerRemoveclient();
	afx_msg void OnBnClickedSetHandler();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNcPaint();
	virtual BOOL OnInitDialog();
};
