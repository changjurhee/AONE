#pragma once
#include "afxdialogex.h"

// CAccountLoginDlg 대화 상자

class CAccountLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAccountLoginDlg)

public:
	CAccountLoginDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CAccountLoginDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_ACCOUNT_LOGIN };
#endif

public:
	CVoIPClientDoc* GetDocument() const;

private:
	std::shared_ptr<userInfo> spUserInfo;
	CString m_edEmailID;
	CString m_edPassword;
	CString m_wdServerIpAddress;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMfcbtnLogin();
	afx_msg void OnBnClickedMfcbtnSignIn();
	afx_msg void OnBnClickedMfcbtnResetPw();

public:
	std::shared_ptr<userInfo> GetUserInfo() { return spUserInfo; }
};
