
// VoIPClientView.h: CVoIPClientView 클래스의 인터페이스
//

#pragma once

#define ID_CV_CONNECTION               0x61A8
#define ID_CV_CHK_VAD						0x61A9
#define ID_CV_CHK_ECHO                   0x61AA

class CVoIPClientView : public CView
{
protected: // serialization에서만 만들어집니다.
	CVoIPClientView() noexcept;
	DECLARE_DYNCREATE(CVoIPClientView)

// 특성입니다.
public:
	CVoIPClientDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CVoIPClientView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// 특성입니다.
	CBitmapButton m_btnConnection;
	CBitmapButton m_btnVad;
	CBitmapButton m_btnEchoCancel;
	CBitmap CallViewBmp;

// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

	void OnEditCut();
	void OnConnection();
	void OnCheckedVAD();
	void OnCheckedEcho();
	
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT processUiControlNotify(WPARAM wParam, LPARAM lParam);

};

#ifndef _DEBUG  // VoIPClientView.cpp의 디버그 버전
inline CVoIPClientDoc* CVoIPClientView::GetDocument() const
   { return reinterpret_cast<CVoIPClientDoc*>(m_pDocument); }
#endif

