// VoIPNewClient.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "VoIPClient.h"
#include "VoIPClientDoc.h"
#include "VoIPNewClient.h"

#include "session/UiController.h"
#include "session/Constants.h"

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
}

BEGIN_MESSAGE_MAP(CVoIPNewClient, CFormView)
	ON_BN_CLICKED(IDC_MFCBTN_CONNECT, &CVoIPNewClient::OnBnClickedMfcbtnConnect)
	ON_BN_CLICKED(IDC_MFC_VAD, &CVoIPNewClient::OnBnClickedMfcVad)
	ON_BN_CLICKED(IDC_MFC_ECHO, &CVoIPNewClient::OnBnClickedMfcEcho)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_GETMINMAXINFO()
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
	CRect rectClient;
	GetClientRect(rectClient);
	CPoint StartPos = rectClient.CenterPoint();

	if (FALSE == GetDocument()->GetConnection()) {
		// Picture Control DC를 생성.
		CClientDC dc(GetDlgItem(IDC_IMAGE_VIEW));

		// Picture Control 크기를 얻는다.
		CRect rect;
		GetDlgItem(IDC_IMAGE_VIEW)->GetClientRect(&rect);

		CDC memDC;
		CBitmap* pOldBitmap, bitmap;

		// Picture Control DC에 호환되는 새로운 CDC를 생성. (임시 버퍼)
		memDC.CreateCompatibleDC(&dc);

		// Picture Control의 크기와 동일한 비트맵을 생성.
		//bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
		bitmap.LoadBitmapW(IDB_MASK);

		// 임시 버퍼에서 방금 생성한 비트맵을 선택하면서, 이전 비트맵을 보존.
		pOldBitmap = memDC.SelectObject(&bitmap);

		// 임시 버퍼에 WHITENESS으로 채움.
		memDC.PatBlt(0, 0, rect.Width(), rect.Height(), WHITENESS);

		// 임시 버퍼(memDC)에 그리는 동작을 수행.
		// 
		// ...
		// ...

	// 임시 버퍼를 Picture Control에 그린다.
		dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

		// 이전 비트맵으로 재설정.
		memDC.SelectObject(pOldBitmap);

		// 생성한 리소스 해제.
		memDC.DeleteDC();
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
