#pragma once
#include "afxdialogex.h"


// CMessageBoxDlg 대화 상자

class CMessageBoxDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMessageBoxDlg)
	enum class Msg {
		INCOMING = 0,
		OUTGOING = 1,
	};

public:
	CMessageBoxDlg(CWnd* pParent = nullptr, int mode = 0, CString cid = _T(""));   // 표준 생성자입니다.
	virtual ~CMessageBoxDlg();

private:
	CString m_stMessage;
	int m_mode; // 0 : Ingoing, 1 : Outgoing

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_MSGBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	// Handle UI notification from modules
	LRESULT processUiControlNotify(WPARAM wParam, LPARAM lParam);

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
