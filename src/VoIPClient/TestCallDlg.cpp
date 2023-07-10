// TestCallDlg.cpp: 구현 파일
//

#include "pch.h"
#include "VoIPClient.h"
#include "MainFrm.h"
#include "VoIPClientDoc.h"

#include "afxdialogex.h"
#include "TestCallDlg.h"

#include "ClientMediaManager.h"
#include "ServerMediaManager.h"

using namespace media;

// CTestCallDlg 대화 상자

IMPLEMENT_DYNAMIC(CTestCallDlg, CDialogEx)

CTestCallDlg::CTestCallDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_CALL_VIEW, pParent)
{

}

CTestCallDlg::~CTestCallDlg()
{
}

void CTestCallDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DISPLAY, mDisplayBox);
}


BEGIN_MESSAGE_MAP(CTestCallDlg, CDialogEx)
    ON_BN_CLICKED(IDC_Client_StartCall, &CTestCallDlg::OnBnClickedClientStartcall)
    ON_BN_CLICKED(IDC_Client_EndCall, &CTestCallDlg::OnBnClickedClientEndcall)
    ON_BN_CLICKED(IDC_Server_StartCall, &CTestCallDlg::OnBnClickedServerStartcall)
    ON_BN_DOUBLECLICKED(IDC_Server_EndCall, &CTestCallDlg::OnBnDoubleclickedServerEndcall)
    ON_BN_CLICKED(IDC_Server_AddClient, &CTestCallDlg::OnBnClickedServerAddclient)
    ON_BN_CLICKED(IDC_Server_RemoveClient, &CTestCallDlg::OnBnClickedServerRemoveclient)
    ON_BN_CLICKED(IDC_SET_HANDLER, &CTestCallDlg::OnBnClickedSetHandler)
    ON_WM_CTLCOLOR()
    ON_WM_NCPAINT()
END_MESSAGE_MAP()


// CTestCallDlg 메시지 처리기

CVoIPClientDoc* CTestCallDlg::GetDocument() const
{
    CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
    CVoIPClientDoc* m_pDocument = (CVoIPClientDoc*)pFrame->GetActiveDocument();
    return (CVoIPClientDoc*)m_pDocument;
}

HBRUSH CTestCallDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  여기서 DC의 특성을 변경합니다.
    // TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
    return (HBRUSH)GetStockObject(WHITE_BRUSH);
}

void CTestCallDlg::OnNcPaint()
{
    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
    // 그리기 메시지에 대해서는 CDialogEx::OnNcPaint()을(를) 호출하지 마십시오.
    CDC* pDC = GetWindowDC();
    CRect rect;
    GetWindowRect(&rect);
    rect.OffsetRect(-rect.left, -rect.top);

    CBrush brush(DKGRAY_BRUSH);
    pDC->FrameRect(&rect, &brush);
    ReleaseDC(pDC);
}

void CTestCallDlg::OnBnClickedClientStartcall()
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

    ClientMediaManager* test = ClientMediaManager::getInstance();

    Json::Value client_join_info;
    client_join_info["rid"] = "TEST_ID";
    client_join_info["serverIp"] = string(CT2CA(client_ServerIP));
    client_join_info["myIp"] = string(CT2CA(client_ClientIP));
    client_join_info["videoCodec"] = string(CT2CA(client_VideoCodec));
    client_join_info["audioCodec"] = string(CT2CA(client_AudioCodec));
    client_join_info["encryption_alg"] = "TEST_ID";
    client_join_info["encryption_key"] = "TEST_ID";
    test->setViewHandler((handleptr)mDisplayBox.GetSafeHwnd());

    test->startCall(client_join_info);

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


void CTestCallDlg::OnBnClickedClientEndcall()
{
    // TODO: 여기에 Client의 EndCall이 눌렸을 때 처리 코드를 추가합니다.
    CString client_ServerIP;
    CString client_ClientIP;
    CString client_VideoCodec;
    CString client_AudioCodec;

    // Get information from call view to operating start call by client
    GetDlgItemText(IDC_Client_ServerIP, client_ServerIP);
    GetDlgItemText(IDC_Client_ClientIP, client_ClientIP);
    GetDlgItemText(IDC_Client_VideoCodec, client_VideoCodec);
    GetDlgItemText(IDC_Client_AudioCodec, client_AudioCodec);


    ClientMediaManager* test = ClientMediaManager::getInstance();

    Json::Value client_join_info;
    client_join_info["rid"] = "TEST_ID";
    //test->setViewHandler((handleptr)mDisplayBox.GetSafeHwnd());

    test->endCall(client_join_info);
}


void CTestCallDlg::OnBnClickedServerStartcall()
{
    CString server_RIDForStart;
    CString server_ServerIP;
    CString server_MaxClients;
    CString server_VideoCodec;
    CString server_AudioCodec;

    // Get information from call view to operating start call by server
    GetDlgItemText(IDC_Server_RIDForStart, server_RIDForStart);
    GetDlgItemText(IDC_Server_ServerIP, server_ServerIP);
    GetDlgItemText(IDC_Server_MaxClient, server_MaxClients);
    GetDlgItemText(IDC_Server_VideoCodec, server_VideoCodec);
    GetDlgItemText(IDC_Server_AudioCodec, server_AudioCodec);

    // TODO : Get server_EncryptionAlgo, server_EncryptionKey

    // TODO : Start Call

    ServerMediaManager* test = ServerMediaManager::getInstance();

    Json::Value room_creat_info;
    room_creat_info["rid"] = string(CT2CA(server_RIDForStart));
    room_creat_info["myIp"] = string(CT2CA(server_ServerIP));
    //room_creat_info["myIp"] = string(CT2CA(server_AudioCodec));
    room_creat_info["conferenceSize"] = string(CT2CA(server_MaxClients));
    //room_creat_info["videocodec"] = string(CT2CA(server_VideoCodec));
    //room_creat_info["audiocodec"] = string(CT2CA(server_AudioCodec));

    test->startCall(room_creat_info);
}


void CTestCallDlg::OnBnDoubleclickedServerEndcall()
{
    // TODO: 여기에 Server의 EndCall이 눌렸을 때 처리 코드를 추가합니다.
    CString server_RIDForStart;
    CString server_ServerIP;
    CString server_MaxClients;
    CString server_VideoCodec;
    CString server_AudioCodec;

    // Get information from call view to operating start call by server
    GetDlgItemText(IDC_Server_RIDForStart, server_RIDForStart);
    GetDlgItemText(IDC_Server_ServerIP, server_ServerIP);
    GetDlgItemText(IDC_Server_MaxClient, server_MaxClients);
    GetDlgItemText(IDC_Server_VideoCodec, server_VideoCodec);
    GetDlgItemText(IDC_Server_AudioCodec, server_AudioCodec);

    // TODO : Get server_EncryptionAlgo, server_EncryptionKey

    // TODO : Start Call

    ServerMediaManager* test = ServerMediaManager::getInstance();

    Json::Value room_creat_info;
    room_creat_info["rid"] = string(CT2CA(server_RIDForStart));

    test->endCall(room_creat_info);

}


void CTestCallDlg::OnBnClickedServerAddclient()
{
    CString server_ClientIPForAdd;
    CString server_RID;
    CString server_CID;

    // Get information from call view to operating add client
    GetDlgItemText(IDC_Server_RIDForAdd, server_RID);
    GetDlgItemText(IDC_Server_ClientIP, server_ClientIPForAdd);
    GetDlgItemText(IDC_Server_CID, server_CID);

    ServerMediaManager* test = ServerMediaManager::getInstance();

    Json::Value room_creat_info;
    room_creat_info["rid"] = string(CT2CA(server_RID));
    room_creat_info["clientIp"] = string(CT2CA(server_ClientIPForAdd));
    room_creat_info["cid"] = string(CT2CA(server_CID));
    test->addClient(room_creat_info);
}


void CTestCallDlg::OnBnClickedServerRemoveclient()
{
    // TODO: 여기에 Server의 RemoveClient가 눌렸을 때 처리 코드를 추가합니다.

    CString server_ClientIPForAdd;
    CString server_RID;
    CString server_CID;

    // Get information from call view to operating add client
    GetDlgItemText(IDC_Server_RIDForAdd, server_RID);
    GetDlgItemText(IDC_Server_ClientIP, server_ClientIPForAdd);
    GetDlgItemText(IDC_Server_CID, server_CID);

    ServerMediaManager* test = ServerMediaManager::getInstance();
    Json::Value room_creat_info;
    room_creat_info["rid"] = string(CT2CA(server_RID));
    room_creat_info["clientIp"] = string(CT2CA(server_ClientIPForAdd));
    room_creat_info["cid"] = string(CT2CA(server_CID));
    test->removeClient(room_creat_info);
}


void CTestCallDlg::OnBnClickedSetHandler()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ClientMediaManager* test = ClientMediaManager::getInstance();
    test->setViewHandler((handleptr)mDisplayBox.GetSafeHwnd());

}

