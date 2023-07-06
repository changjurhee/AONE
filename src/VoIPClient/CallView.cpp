﻿// CallView.cpp: 구현 파일
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
    DDX_Control(pDX, IDC_DISPLAY, mDisplayBox);
}

BEGIN_MESSAGE_MAP(CCallView, CFormView)
	ON_MESSAGE(UM_VAD_STATE, &CCallView::OnVadNotify)
    ON_BN_CLICKED(IDC_Client_StartCall, &CCallView::OnBnClickedClientStartcall)
    ON_BN_CLICKED(IDC_Client_EndCall, &CCallView::OnBnClickedClientEndcall)
    ON_BN_CLICKED(IDC_Server_StartCall, &CCallView::OnBnClickedServerStartcall)
    ON_BN_DOUBLECLICKED(IDC_Server_EndCall, &CCallView::OnBnDoubleclickedServerEndcall)
    ON_BN_CLICKED(IDC_Server_AddClient, &CCallView::OnBnClickedServerAddclient)
    ON_BN_CLICKED(IDC_Server_RemoveClient, &CCallView::OnBnClickedServerRemoveclient)
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

void CCallView::OnBnClickedClientStartcall()
{
    CString client_ServerIP;
    CString client_ClientIP;
    CString client_VideoCodec;
    CString client_AudioCodec;

    // Get information from call view to operating start call by client
    GetDlgItemText(IDC_Client_ServerIP, client_ServerIP);
    GetDlgItemText(IDC_Client_ClientIP, client_ClientIP);
    GetDlgItemText(IDC_Client_VideoCodec, client_VideoCodec);
    GetDlgItemText(IDC_Client_AudioCodec, client_AudioCodec);

    // TODO : Get client_EncryptionAlgo, client_EncryptionKey

    // TODO : Start Call

    // Get DC(Device Context) from window handle
    HDC hdc = ::GetDC(mDisplayBox);

    // Get window size from handle
    RECT rect;
    ::GetClientRect(mDisplayBox, &rect);

    // Overlay as much as the size of the window in red
    HBRUSH hBrush = ::CreateSolidBrush(RGB(255, 0, 0));
    ::FillRect(hdc, &rect, hBrush);

    // Release
    ::ReleaseDC(mDisplayBox, hdc);
    ::DeleteObject(hBrush);

}


void CCallView::OnBnClickedClientEndcall()
{
    // TODO: 여기에 Client의 EndCall이 눌렸을 때 처리 코드를 추가합니다.
}


void CCallView::OnBnClickedServerStartcall()
{
    CString server_RID;
    CString server_ServerIP;
    CString server_MaxClients;
    CString server_VideoCodec;
    CString server_AudioCodec;

    // Get information from call view to operating start call by server
    GetDlgItemText(IDC_Server_RIDForStart, server_RID);
    GetDlgItemText(IDC_Server_ServerIP, server_ServerIP);
    GetDlgItemText(IDC_Server_MaxClient, server_MaxClients);
    GetDlgItemText(IDC_Server_VideoCodec, server_VideoCodec);
    GetDlgItemText(IDC_Server_AudioCodec, server_AudioCodec);

    // TODO : Get server_EncryptionAlgo, server_EncryptionKey

    // TODO : Start Call

}


void CCallView::OnBnDoubleclickedServerEndcall()
{
    // TODO: 여기에 Server의 EndCall이 눌렸을 때 처리 코드를 추가합니다.
}


void CCallView::OnBnClickedServerAddclient()
{
    CString server_ClientIPForAdd;
    CString server_RID;
    CString server_CID;

    // Get information from call view to operating add client
    GetDlgItemText(IDC_Server_RID, server_RID);
    GetDlgItemText(IDC_Server_ClientIPForAdd, server_ClientIPForAdd);
    GetDlgItemText(IDC_Server_CID, server_CID);

}


void CCallView::OnBnClickedServerRemoveclient()
{
    // TODO: 여기에 Server의 RemoveClient가 눌렸을 때 처리 코드를 추가합니다.
}
