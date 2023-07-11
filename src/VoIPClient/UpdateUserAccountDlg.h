#pragma once
#include "afxdialogex.h"


// CUpdateUserAccountDlg 대화 상자

class CUpdateUserAccountDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUpdateUserAccountDlg)

public:
	CUpdateUserAccountDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CUpdateUserAccountDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_UPDATE_ACCOUNT };
#endif
	CString m_edId;
	CString m_edEmailID;
	CString m_edPassword;
	CString m_edName;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnBnClickedMfcbtnUpdateContact();
	afx_msg void OnBnClickedMfcbtnCancel();

	LRESULT processUiControlNotify(WPARAM wParam, LPARAM lParam);
};
