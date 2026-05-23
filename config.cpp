#include "launcher.h"

using namespace Gdiplus;

// --- Allocation of Shared Global Variables ---
TimerSettings timerCfg;
std::vector<LauncherBtn> buttons;
std::wstring bgPath, bgSound, btnSound;
int winW = 600, winH = 300, timeoutLeft = 5, hoverIdx = -1;
bool timerActive = true, hasImg = false, mouseTrack = false, loopBg = false;
Color winBgColor;

Color ParseColor(const char* str, int defR, int defG, int defB) {
    int r, g, b;
    if (sscanf(str, "%d,%d,%d", &r, &g, &b) != 3) return Color(255, defR, defG, defB);
    return Color(255, r, g, b);
}

void LoadINI() {
    wchar_t p[MAX_PATH]; 
    GetModuleFileNameW(NULL, p, MAX_PATH);
    wchar_t* lastSlash = wcsrchr(p, L'\\');
    if (lastSlash) *lastSlash = L'\0';
    
    std::wstring ini = std::wstring(p) + L"\\launcher.ini";
    wchar_t buf[256];

    GetPrivateProfileStringW(L"Common", L"BgImage", L"launcher_bg.jpg", buf, 256, ini.c_str());
    bgPath = buf;
    GetPrivateProfileStringW(L"Common", L"BgColor", L"0,0,0", buf, 256, ini.c_str());
    
    char bufA[256];
    WideCharToMultiByte(CP_ACP, 0, buf, -1, bufA, 256, NULL, NULL);
    winBgColor = ParseColor(bufA);
    
    GetPrivateProfileStringW(L"Common", L"BackgroundSound", L"", buf, 256, ini.c_str());
    bgSound = buf;
    GetPrivateProfileStringW(L"Common", L"ButtonSound", L"", buf, 256, ini.c_str());
    btnSound = buf;
    loopBg = GetPrivateProfileIntW(L"Common", L"LoopBackground", 0, ini.c_str()) == 1;

    winW = GetPrivateProfileIntW(L"Common", L"WindowWidth", 600, ini.c_str());
    winH = GetPrivateProfileIntW(L"Common", L"WindowHeight", 300, ini.c_str());

    timerCfg.timeout = GetPrivateProfileIntW(L"Timer", L"Timeout", 5, ini.c_str());
    timeoutLeft = timerCfg.timeout;
    timerCfg.fontSize = GetPrivateProfileIntW(L"Timer", L"FontSize", 12, ini.c_str());
    timerCfg.x = GetPrivateProfileIntW(L"Timer", L"X", 30, ini.c_str());
    timerCfg.y = GetPrivateProfileIntW(L"Timer", L"Y", 265, ini.c_str());
    
    GetPrivateProfileStringW(L"Timer", L"Font", L"Courier New", buf, 256, ini.c_str());
    timerCfg.font = buf;
    GetPrivateProfileStringW(L"Timer", L"TextRunning", L"Initiating in %ds...", buf, 256, ini.c_str());
    timerCfg.textRunning = buf;
    GetPrivateProfileStringW(L"Timer", L"TextPaused", L"PAUSED", buf, 256, ini.c_str());
    timerCfg.textPaused = buf;
    GetPrivateProfileStringW(L"Timer", L"Color", L"24,255,3", buf, 256, ini.c_str());
    WideCharToMultiByte(CP_ACP, 0, buf, -1, bufA, 256, NULL, NULL);
    timerCfg.color = ParseColor(bufA, 24, 255, 3);

    buttons.clear();
    for (int i = 0; i < 6; i++) {
        wchar_t sec[32];
        swprintf(sec, 32, L"Button%d", i);
        
        GetPrivateProfileStringW(sec, L"Text", L"", buf, 256, ini.c_str());
        if (wcslen(buf) == 0) continue;
        
        LauncherBtn b;
        b.text = buf;
        GetPrivateProfileStringW(sec, L"Font", L"Courier New", buf, 256, ini.c_str());
        b.font = buf;
        b.fontSize = GetPrivateProfileIntW(sec, L"FontSize", 14, ini.c_str());
        b.x = GetPrivateProfileIntW(sec, L"X", 0, ini.c_str());
        b.y = GetPrivateProfileIntW(sec, L"Y", 0, ini.c_str());
        
        GetPrivateProfileStringW(sec, L"Key", L"", buf, 10, ini.c_str());
        b.hotkey = (wcslen(buf) > 0) ? (char)towupper(buf[0]) : 0;
        
        GetPrivateProfileStringW(sec, L"Color", L"24,255,3", buf, 256, ini.c_str());
        WideCharToMultiByte(CP_ACP, 0, buf, -1, bufA, 256, NULL, NULL);
        b.color = ParseColor(bufA, 24, 255, 3);
        
        GetPrivateProfileStringW(sec, L"HoverColor", L"255,255,255", buf, 256, ini.c_str());
        WideCharToMultiByte(CP_ACP, 0, buf, -1, bufA, 256, NULL, NULL);
        b.hoverColor = ParseColor(bufA, 255, 255, 255);
        
        GetPrivateProfileStringW(sec, L"Command", L"", buf, 256, ini.c_str());
        b.command = buf;
        b.rect = Rect(b.x, b.y, 200, 35);
        buttons.push_back(b);
    }
    hasImg = (GetFileAttributesW(bgPath.c_str()) != INVALID_FILE_ATTRIBUTES);
}

void Execute(std::wstring cmd) {
    if (cmd == L"EXIT") PostQuitMessage(0);
    else {
        ShellExecuteW(NULL, L"open", cmd.c_str(), NULL, NULL, SW_SHOWNORMAL);
        PostQuitMessage(0);
    }
}
