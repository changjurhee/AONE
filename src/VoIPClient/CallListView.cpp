// CallListView.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"
#include "CallListView.h"


// CCallListView

IMPLEMENT_DYNCREATE(CCallListView, CFormView)

CCallListView::CCallListView()
	: CFormView(IDD_DLG_CALL_LIST_VIEW)
{

}

CCallListView::~CCallListView()
{
}

void CCallListView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCallListView, CFormView)
END_MESSAGE_MAP()


// CCallListView 진단

#ifdef _DEBUG
void CCallListView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CCallListView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CCallListView 메시지 처리기
