
#pragma once

#include "ViewTree.h"

class CContactViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CContactView : public CDockablePane
{
// 생성입니다.
public:
	CContactView() noexcept;

	void AdjustLayout();
	void OnChangeVisualStyle();

// 특성입니다.
protected:

	CViewTree m_wndContactView;
	CImageList m_ContactViewImages;
	CContactViewToolBar m_wndToolBar;
	UINT m_nCurrSort;

protected:
	void FillContactView();

// 구현입니다.
public:
	virtual ~CContactView();

	// 재정의입니다.
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenWith();
	afx_msg void OnDummyCompile();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnNewFolder();
	afx_msg void OnSort(UINT id);
	afx_msg void OnUpdateSort(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};

