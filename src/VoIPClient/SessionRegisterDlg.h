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

private:
	CListCtrl m_ltContact;
	CListCtrl m_ltParticipants;
	int m_nIndexContact;
	int m_nIndexParticipants;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnNMClickLtContact(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMClickLtParticipants(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMThemeChangedDpDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMThemeChangedDpTimeStart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMThemeChangedDpTimeEnd(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedMfcbtnRight();
	afx_msg void OnBnClickedMfcbtnLeft();
	virtual BOOL OnInitDialog();

	// Handle UI notification from modules
	LRESULT processUiControlNotify(WPARAM wParam, LPARAM lParam);
	// // second
	COleDateTime m_Date;
	// // second
	COleDateTime m_StartTime;
	// // second
	COleDateTime m_endTime;
	afx_msg void OnBnClickedMbSessionCancel();
	afx_msg void OnBnClickedMbSesseionCreate();
};
