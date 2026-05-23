#include "launcher.h"

using namespace Gdiplus;

// --- Allocation of Multimedia Global Variables ---
IGraphBuilder *pGraphBG = NULL, *pGraphBtn = NULL;
IMediaControl *pControlBG = NULL, *pControlBtn = NULL;
IMediaPosition *pPosBG = NULL;
ULONG_PTR gdiToken;

void PlayBtnSound() {
    if (pControlBtn) {
        pControlBtn->Stop();
        IMediaPosition* pPosBtn = NULL;
        pGraphBtn->QueryInterface(IID_IMediaPosition, (void**)&pPosBtn);
        if (pPosBtn) { pPosBtn->put_CurrentPosition(0); pPosBtn->Release(); }
        pControlBtn->Run();
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            LoadINI();
            CoInitialize(NULL);
            if (!bgSound.empty()) {
                CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraphBG);
                pGraphBG->QueryInterface(IID_IMediaControl, (void**)&pControlBG);
                pGraphBG->QueryInterface(IID_IMediaPosition, (void**)&pPosBG);
                wchar_t fp[MAX_PATH]; GetFullPathNameW(bgSound.c_str(), MAX_PATH, fp, NULL);
                if (SUCCEEDED(pGraphBG->RenderFile(fp, NULL))) pControlBG->Run();
            }
            if (!btnSound.empty()) {
                CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraphBtn);
                pGraphBtn->QueryInterface(IID_IMediaControl, (void**)&pControlBtn);
                wchar_t fp[MAX_PATH]; GetFullPathNameW(btnSound.c_str(), MAX_PATH, fp, NULL);
                pGraphBtn->RenderFile(fp, NULL);
            }
            SetTimer(hwnd, 1, 1000, NULL);
            return 0;
        }
        case WM_RBUTTONUP:
        case WM_NCRBUTTONUP: ShowWindow(hwnd, SW_MINIMIZE); return 0;
        case WM_NCHITTEST: {
            POINT pt; GetCursorPos(&pt); ScreenToClient(hwnd, &pt);
            for (auto& b : buttons) if (b.rect.Contains(pt.x, pt.y)) return HTCLIENT;
            return HTCAPTION;
        }
        case WM_SETCURSOR: {
            if (LOWORD(lParam) == HTCLIENT) { SetCursor(LoadCursor(NULL, IDC_HAND)); return TRUE; }
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam), y = HIWORD(lParam), old = hoverIdx;
            if (!mouseTrack) { TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0}; TrackMouseEvent(&tme); mouseTrack = true; }
            hoverIdx = -1;
            for (int i = 0; i < (int)buttons.size(); i++) if (buttons[i].rect.Contains(x, y)) hoverIdx = i;
            if (old != hoverIdx && hoverIdx != -1) { PlayBtnSound(); InvalidateRect(hwnd, NULL, FALSE); }
            else if (old != hoverIdx) InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        case WM_MOUSELEAVE: mouseTrack = false; hoverIdx = -1; InvalidateRect(hwnd, NULL, FALSE); return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps); Graphics g(hdc);
            g.SetTextRenderingHint(TextRenderingHintAntiAlias);
            if (hasImg) { Image img(bgPath.c_str()); g.DrawImage(&img, 0, 0, winW, winH); }
            else g.Clear(winBgColor);
            for (int i = 0; i < (int)buttons.size(); i++) {
                Font f(buttons[i].font.c_str(), (REAL)buttons[i].fontSize, FontStyleBold);
                SolidBrush br(i == hoverIdx ? buttons[i].hoverColor : buttons[i].color);
                g.DrawString(buttons[i].text.c_str(), -1, &f, PointF((REAL)buttons[i].x, (REAL)buttons[i].y), &br);
            }
            wchar_t s[256];
            if (timerActive) swprintf(s, 256, timerCfg.textRunning.c_str(), timeoutLeft);
            else swprintf(s, 256, L"%ls", timerCfg.textPaused.c_str());
            Font fs(timerCfg.font.c_str(), (REAL)timerCfg.fontSize, FontStyleBold);
            SolidBrush bs(timerCfg.color);
            g.DrawString(s, -1, &fs, PointF((REAL)timerCfg.x, (REAL)timerCfg.y), &bs);
            EndPaint(hwnd, &ps); return 0;
        }
        case WM_LBUTTONDOWN: {
            int x = LOWORD(lParam), y = HIWORD(lParam); timerActive = false;
            for (auto& b : buttons) if (b.rect.Contains(x, y)) Execute(b.command);
            InvalidateRect(hwnd, NULL, FALSE); return 0;
        }
        case WM_KEYDOWN:
            timerActive = false;
            if (wParam == VK_ESCAPE) PostQuitMessage(0);
            if (wParam == VK_TAB) InvalidateRect(hwnd, NULL, FALSE);
            if (wParam == VK_SPACE && !buttons.empty()) Execute(buttons[0].command);
            for (auto& b : buttons) if (b.hotkey && wParam == (WPARAM)b.hotkey) Execute(b.command);
            return 0;
        case WM_TIMER: {
            if (timerActive) {
                if (--timeoutLeft <= 0) { 
                    KillTimer(hwnd, 1); 
                    if(!buttons.empty()) Execute(buttons[0].command); 
                }
                else InvalidateRect(hwnd, NULL, FALSE);
            }
            
            if (loopBg && pPosBG && pControlBG) {
                REFTIME cur = 0, dur = 0;
                pPosBG->get_CurrentPosition(&cur);
                pPosBG->get_Duration(&dur);

                if (cur >= (dur - 0.3) || cur < 0) {
                    pControlBG->Stop();           
                    pPosBG->put_CurrentPosition(0); 
                    pControlBG->Run();            
                }
            }
            return 0;
        }
        case WM_DESTROY: {
            if (pControlBG) pControlBG->Stop(); if (pControlBtn) pControlBtn->Stop();
            if (pPosBG) pPosBG->Release(); if (pControlBG) pControlBG->Release(); if (pControlBtn) pControlBtn->Release();
            if (pGraphBG) pGraphBG->Release(); if (pGraphBtn) pGraphBtn->Release();
            CoUninitialize(); PostQuitMessage(0); return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int nS) {
    GdiplusStartupInput gsi; GdiplusStartup(&gdiToken, &gsi, NULL);
    HICON hIcon = LoadIcon(hI, "MAINICON");
    WNDCLASS wc = {0}; wc.lpfnWndProc = WindowProc; wc.hInstance = hI;
    wc.hIcon = hIcon; wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); wc.lpszClassName = "GameLauncher";
    RegisterClass(&wc);
    int sw = GetSystemMetrics(SM_CXSCREEN), sh = GetSystemMetrics(SM_CYSCREEN);
    HWND hwnd = CreateWindowEx(WS_EX_APPWINDOW, "GameLauncher", "Game Launcher", WS_POPUP | WS_MINIMIZEBOX | WS_VISIBLE, (sw-600)/2, (sh-300)/2, 600, 300, NULL, NULL, hI, NULL);
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    MSG msg; while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    GdiplusShutdown(gdiToken); return 0;
}
