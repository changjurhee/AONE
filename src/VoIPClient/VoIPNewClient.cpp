// VoIPNewClient.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "VoIPClient.h"
#include "VoIPClientDoc.h"
#include "VoIPNewClient.h"

#include "session/UiController.h"
#include "session/Constants.h"
#include "ClientMediaManager.h"
#include <gst/gst.h>

// CVoIPNewClient

IMPLEMENT_DYNCREATE(CVoIPNewClient, CFormView)

CVoIPNewClient::CVoIPNewClient()
	: CFormView(IDD_CLIENT_VIEW)
{

}

CVoIPNewClient::~CVoIPNewClient()
{
}

void CVoIPNewClient::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE_VIEW, m_DisplayBox);
	DDX_Control(pDX, IDC_CHECK_VAD, m_chkVAD);
	DDX_Control(pDX, IDC_CHECK_ECHO, m_chkEcho);
}

BEGIN_MESSAGE_MAP(CVoIPNewClient, CFormView)
	ON_BN_CLICKED(IDC_MFCBTN_CONNECT, &CVoIPNewClient::OnBnClickedMfcbtnConnect)
	ON_BN_CLICKED(IDC_MFC_VAD, &CVoIPNewClient::OnBnClickedMfcVad)
	ON_BN_CLICKED(IDC_MFC_ECHO, &CVoIPNewClient::OnBnClickedMfcEcho)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_CHECK_VAD, &CVoIPNewClient::OnBnClickedCheckVad)
	ON_BN_CLICKED(IDC_CHECK_ECHO, &CVoIPNewClient::OnBnClickedCheckEcho)
	ON_BN_CLICKED(IDC_rtplog, &CVoIPNewClient::OnBnClickedrtplog)
END_MESSAGE_MAP()


// CVoIPNewClient 진단

#ifdef _DEBUG
void CVoIPNewClient::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CVoIPNewClient::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CVoIPNewClient 메시지 처리기

CVoIPClientDoc* CVoIPNewClient::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVoIPClientDoc)));
	return (CVoIPClientDoc*)m_pDocument;
}

LRESULT CVoIPNewClient::processUiControlNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case MSG_RESPONSE_CALLSTATE:
	{
		CallResult* res = reinterpret_cast<CallResult*>(lParam);
		int result = res->result;
		int cause = res->cause;
		if (result == CallState::STATE_DISCONNECTED) {
			TRACE("void CVoIPNewClient::processUiControlNotify()");
			GetDocument()->SetConnection(FALSE);
			OnPaint();
		}
		break;
	}
	default:
		break;
	}
	return LRESULT();
}

void CVoIPNewClient::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient, rect;
	GetClientRect(rectClient);
	
	GetDlgItem(IDC_MFCBTN_CONNECT)->GetClientRect(&rect);
	m_DisplayBox.SetWindowPos(nullptr, rect.right + 50, rect.top, 1280, 960, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CVoIPNewClient::OnBnClickedMfcbtnConnect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UiController::getInstance()->request_EndOrExitCall(this);
}


void CVoIPNewClient::OnBnClickedMfcVad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CVoIPNewClient::OnBnClickedMfcEcho()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CVoIPNewClient::OnDraw(CDC* /*pDC*/)
{
	if (FALSE == GetDocument()->GetConnection()) {
		// Picture Control DC를 생성.
		CClientDC dc(GetDlgItem(IDC_IMAGE_VIEW));

		// Picture Control 크기를 얻는다.
		CRect rect;
		GetDlgItem(IDC_IMAGE_VIEW)->GetClientRect(&rect);

		CDC memDC, memDC_bg;
		CBitmap* pOldBitmap, bitmap;
		CBitmap* pOldBitmap_bg, bitmap_bg;

		// Picture Control DC에 호환되는 새로운 CDC를 생성. (임시 버퍼)
		memDC_bg.CreateCompatibleDC(&dc);
		memDC.CreateCompatibleDC(&dc);

		// Picture Control의 크기와 동일한 비트맵을 생성.
		bitmap_bg.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
		bitmap.LoadBitmapW(IDB_MASK);

		// 임시 버퍼에서 방금 생성한 비트맵을 선택하면서, 이전 비트맵을 보존.
		pOldBitmap_bg = memDC_bg.SelectObject(&bitmap_bg);
		pOldBitmap = memDC.SelectObject(&bitmap);

		// 임시 버퍼에 WHITENESS으로 채움.
		memDC_bg.PatBlt(0, 0, rect.Width(), rect.Height(), WHITENESS);
		memDC.PatBlt(0, 0, rect.Width(), rect.Height(), SRCCOPY);

		// 임시 버퍼(memDC)에 그리는 동작을 수행.
		// 
		// ...

	// 임시 버퍼를 Picture Control에 그린다.
		dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC_bg, 0, 0, SRCCOPY);
		dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

		// 이전 비트맵으로 재설정.
		memDC_bg.SelectObject(pOldBitmap_bg);
		memDC.SelectObject(pOldBitmap);

		// 생성한 리소스 해제.
		memDC_bg.DeleteDC();
		memDC.DeleteDC();
		bitmap_bg.DeleteObject();
		bitmap.DeleteObject();
	}
}


void CVoIPNewClient::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CFormView::OnPaint()을(를) 호출하지 마십시오.
	OnDraw(&dc);
}


HBRUSH CVoIPNewClient::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return (HBRUSH)GetStockObject(WHITE_BRUSH);
}


void CVoIPNewClient::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CFormView::OnGetMinMaxInfo(lpMMI);
}


void CVoIPNewClient::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	AdjustLayout();
}


void CVoIPNewClient::OnBnClickedCheckVad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_chkVAD.GetCheck() == BST_CHECKED) {
		// @todo
		ClientMediaManager::getInstance()->setVad(true);
		TRACE0("VAD_CHECKED");

	}
	else {
		ClientMediaManager::getInstance()->setVad(false);
		// @todo
		TRACE0("VAD_UNCHECKED");

	}
}


void CVoIPNewClient::OnBnClickedCheckEcho()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_chkEcho.GetCheck() == BST_CHECKED) {
		// @todo
		TRACE0("ECHO_CHECKED");
		//GstDebugLevel logLevel = GST_LEVEL_DEBUG;
		//gst_debug_set_threshold_for_name("GST_SCHEDULING", GST_LEVEL_DEBUG);
		gst_debug_set_threshold_for_name("rtpjitterbuffer", GST_LEVEL_DEBUG);
		//gst_debug_set_default_threshold(GST_LEVEL_FIXME);

	}
	else {
		//gst_debug_set_threshold_for_name("GST_SCHEDULING", GST_LEVEL_NONE);
		gst_debug_set_threshold_for_name("rtpjitterbuffer", GST_LEVEL_NONE);
		//gst_debug_set_default_threshold(GST_LEVEL_NONE);
		// @todo
		TRACE0("ECHO_UNCHECKED");

	}
}


void CVoIPNewClient::OnBnClickedrtplog()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	static bool rtplog = false;
	rtplog = !rtplog;
	if (rtplog) {
		//GstDebugLevel logLevel = GST_LEVEL_DEBUG;
		//gst_debug_set_threshold_for_name("GST_SCHEDULING", GST_LEVEL_DEBUG);
		gst_debug_set_threshold_for_name("rtpjitterbuffer", GST_LEVEL_DEBUG);
		//gst_debug_set_default_threshold(GST_LEVEL_FIXME);
	}
	else {
		//gst_debug_set_threshold_for_name("GST_SCHEDULING", GST_LEVEL_NONE);
		gst_debug_set_threshold_for_name("rtpjitterbuffer", GST_LEVEL_NONE);
		//gst_debug_set_default_threshold(GST_LEVEL_NONE);
	}
}
