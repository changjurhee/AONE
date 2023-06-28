#include <gst/gst.h>
#include <thread>
#include <iostream>
#include <Windows.h>

#include "sendMain.h"
#include "recvMain.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

std::thread senderThread;
std::thread receiverThread;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        // 윈도우 종료
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        // 애플리케이션 종료
        PostQuitMessage(0);
        break;
    case WM_CREATE:
    {
        // 버튼 생성
        HWND button1 = CreateWindow(L"BUTTON", L"송신활성화", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            200, 200, 200, 50, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);
        // 버튼 생성
        HWND button2 = CreateWindow(L"BUTTON", L"수신활성화", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            200, 300, 200, 50, hwnd, (HMENU)2, GetModuleHandle(NULL), NULL);
        // 버튼 생성
        HWND button3 = CreateWindow(L"BUTTON", L"320 x 240으로 해상도 변경", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            200, 400, 200, 50, hwnd, (HMENU)3, GetModuleHandle(NULL), NULL);
        // 버튼 생성
        HWND button4 = CreateWindow(L"BUTTON", L"640 x 480으로 해상도 변경", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            200, 500, 200, 50, hwnd, (HMENU)4, GetModuleHandle(NULL), NULL);
        break;
    }
    case WM_COMMAND:
        // 버튼 클릭 이벤트 처리
        switch (LOWORD(wParam))
        {
        case 1:
            if (!senderThread.joinable())
            {
                senderThread = std::thread(sendMain);
            }
            else
            {
                MessageBox(hwnd, L"senderThread를 동작시킬 수 없습니다.", L"알림", MB_OK);
            }
            break;
        case 2:
            if (!receiverThread.joinable())
            {
                receiverThread = std::thread(recvMain);
            }
            else
            {
                MessageBox(hwnd, L"receiverThread를 동작시킬 수 없습니다.", L"알림", MB_OK);
            }
            break;
        case 3:
            changeVideoResolution(320, 240);
            break;
        case 4:
            changeVideoResolution(640, 480);
            break;
        default:
            break;
        }

        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int main(int argc, char* argv[])
{
    // Initialize GStreamer
    gst_init(&argc, &argv);

    // 윈도우 클래스 등록
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = L"ButtonWindowClass";
    RegisterClassEx(&wc);

    // 윈도우 생성
    HWND hwnd = CreateWindowEx(0, L"ButtonWindowClass", L"송수신 테스트",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        640, 800, NULL, NULL, GetModuleHandle(NULL), NULL);
    if (hwnd == NULL)
    {
        std::cerr << "윈도우 생성 실패" << std::endl;
        return 1;
    }

    // 윈도우 표시
    ShowWindow(hwnd, SW_SHOW);

    // 메시지 루프
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
