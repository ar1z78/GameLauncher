#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <dshow.h>
#include <string>
#include <vector>

struct LauncherBtn {
    std::wstring text, font, command;
    int fontSize, x, y;
    char hotkey;
    Gdiplus::Color color, hoverColor;
    Gdiplus::Rect rect;
};

struct TimerSettings {
    int timeout, fontSize, x, y;
    std::wstring font, textRunning, textPaused;
    Gdiplus::Color color;
};

// --- Shared Global Declarations (extern) ---
extern IGraphBuilder *pGraphBG, *pGraphBtn;
extern IMediaControl *pControlBG, *pControlBtn;
extern IMediaPosition *pPosBG;

extern TimerSettings timerCfg;
extern std::vector<LauncherBtn> buttons;
extern std::wstring bgPath, bgSound, btnSound;
extern int winW, winH, timeoutLeft, hoverIdx;
extern bool timerActive, hasImg, mouseTrack, loopBg;
extern Gdiplus::Color winBgColor;
extern ULONG_PTR gdiToken;

// --- Shared Function Declarations ---
Gdiplus::Color ParseColor(const char* str, int defR = 0, int defG = 0, int defB = 0);
void LoadINI();
void Execute(std::wstring cmd);
void PlayBtnSound();
