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
	ON_MESSAGE(UM_VAD_STATE, &CCallView::OnVadNotify)
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

LRESULT CCallView::OnVadNotify(WPARAM wParam, LPARAM lParam)
{
    // @todo 아래 코드는 좀 수정이 필요 함. 
   /* HWND hTempWnd;
    litevad_result_t VadState;
    hTempWnd = GetDlgItem(hWnd, IDC_VAD_STATE_STATUS);
    VadState = (litevad_result_t)wParam;
    switch (VadState)
    {
    case LITEVAD_RESULT_SPEECH_BEGIN:
        SetWindowTextA(hTempWnd, "Speech");
        break;
    case LITEVAD_RESULT_SPEECH_END:
        SetWindowTextA(hTempWnd, "Speech End");
        break;
    case LITEVAD_RESULT_SPEECH_BEGIN_AND_END:
        SetWindowTextA(hTempWnd, "Speech Begin & End");
        break;
    case LITEVAD_RESULT_FRAME_SILENCE:
        SetWindowTextA(hTempWnd, "Silence");
        break;
    case LITEVAD_RESULT_FRAME_ACTIVE:
        break;
    case LITEVAD_RESULT_ERROR:
        SetWindowTextA(hTempWnd, "VAD Error");
        break;
    default:
        SetWindowTextA(hTempWnd, "Unknown Error");
        break;
    }*/

	return TRUE;
}