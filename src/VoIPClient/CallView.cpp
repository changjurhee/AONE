// CallView.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"
#include "CallView.h"


// CCallView

IMPLEMENT_DYNCREATE(CCallView, CFormView)

CCallView::CCallView()
	: CFormView(IDD_DLG_CALL_VIEW)
{

}

CCallView::~CCallView()
{
}

void CCallView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCallView, CFormView)
END_MESSAGE_MAP()


// CCallView 진단

#ifdef _DEBUG
void CCallView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CCallView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CCallView 메시지 처리기
