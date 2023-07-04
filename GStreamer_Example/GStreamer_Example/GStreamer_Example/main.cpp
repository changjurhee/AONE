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
    static bool senderActive = false; // 송신 활성화 플래그

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
        HWND hText = CreateWindow(L"STATIC", L"해상도 설정", WS_VISIBLE | WS_CHILD,
            200, 400, 200, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);
        // 버튼 생성
        HWND hComboBox = CreateWindow(L"COMBOBOX", NULL, CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
            200, 430, 200, 300, hwnd, (HMENU)3, GetModuleHandle(NULL), NULL);

        // 드롭다운 목록에 아이템 추가
        const wchar_t* items[] = { L"1280 * 720", L"640 * 480", L"320 * 240" };
        for (int i = 0; i < 3; i++)
        {
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)items[i]);
        }

        EnableWindow(hComboBox, FALSE); // 드롭다운 목록 버튼 비활성화
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
                SetWindowText((HWND)lParam, L"송신비활성화");
                senderActive = true; // 송신 활성화 설정
                EnableWindow(GetDlgItem(hwnd, 3), TRUE); // 드롭다운 목록 버튼 활성화
            }
            else
            {
                MessageBox(hwnd, L"senderThread를 동작시킬 수 없으므로 쓰레드를 종료합니다.", L"알림", MB_OK);
                quiteSenderMain();
                senderThread.join();
                SetWindowText((HWND)lParam, L"송신활성화");
                senderActive = false; // 송신 비활성화 설정
                EnableWindow(GetDlgItem(hwnd, 3), FALSE); // 드롭다운 목록 버튼 비활성화
            }
            break;
        case 2:
            if (!receiverThread.joinable())
            {
                receiverThread = std::thread(recvMain);
                SetWindowText((HWND)lParam, L"수신비활성화");
            }
            else
            {
                MessageBox(hwnd, L"receiverThread를 동작시킬 수 없으므로 쓰레드를 종료합니다.", L"알림", MB_OK);
                quiteReceiverMain();
                receiverThread.join();
                SetWindowText((HWND)lParam, L"수신활성화");
            }
            break;
        case 3:
            if (HIWORD(wParam) == CBN_SELCHANGE && senderActive)
            {
                HWND hComboBox = GetDlgItem(hwnd, 3);
                int selectedIndex = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
                wchar_t selectedText[256];
                SendMessage(hComboBox, CB_GETLBTEXT, selectedIndex, (LPARAM)selectedText);

                // 선택된 항목에 따라 동작 수행
                if (wcscmp(selectedText, L"1280 * 720") == 0)
                {
                    // 선택된 해상도가 "1280 * 720"일 때의 동작 수행
                    // ...
                    changeVideoResolution(1280, 720);
                }
                else if (wcscmp(selectedText, L"640 * 480") == 0)
                {
                    // 선택된 해상도가 "640 * 480"일 때의 동작 수행
                    // ...
                    changeVideoResolution(640, 480);
                }
                else if (wcscmp(selectedText, L"320 * 240") == 0)
                {
                    // 선택된 해상도가 "320 * 240"일 때의 동작 수행
                    // ...
                    changeVideoResolution(320, 240);
                }
            }
            else
            {
                /* NOP */
            }
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
