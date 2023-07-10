#pragma once
#include "afxdialogex.h"


// CManageUserAccountDlg 대화 상자

class CManageUserAccountDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CManageUserAccountDlg)

public:
	CManageUserAccountDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CManageUserAccountDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_MANAGE_ACCOUNT };
#endif

public:
	CVoIPClientDoc* GetDocument() const;

private:
	CString m_edID;
	CString m_edEmailID;
	CString m_edPassword;
	CString m_edName;
	INT m_edPasswordQuestion;
	CString m_edPasswordAnswer;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnBnClickedMfcbtnCreate();
	afx_msg void OnBnClickedMfcbtnCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	LRESULT processUiControlNotify(WPARAM wParam, LPARAM lParam);
};
