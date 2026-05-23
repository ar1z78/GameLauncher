# Custom Windows Game Launcher

A ultra-lightweight, high-performance borderless Windows game launcher built with pure Win32 API for speed and size. It features an automated countdown timer, background loop music, button hover sound effects, and custom hotkey configurations.

The entire application runs from a single optimized executable and is configured completely via an external configuration file.

---

## 🛠 How to Compile

This workspace is explicitly built and optimized for the **`w64devkit`** compilation environment. 

### Prerequisites
1. Ensure `w64devkit` is installed and added to your system environment variables.
2. Compile your resource file first if you are packing custom icons (`MAINICON`):
   ```bash
   windres resource.rc -o resource.o
   ```

### Execution Build Statement
Run this command from your terminal to link the object maps and compile your application binary:

```bash
g++ launcher.cpp config.cpp resource.o -o launcher.exe -static -Os -s -ffunction-sections -fdata-sections -Wl,--gc-sections -lgdiplus -lshlwapi -lole32 -loleaut32 -lstrmiids -mwindows -lstdc++ -lcomctl32
```

### Explaining the Size Optimization Flags:
- `-static`: Eliminates external dynamic library linking dependencies.
- `-Os`: Optimizes compiler output sizes tightly.
- `-s`: Strips symbolic code mapping tags entirely out of memory.
- `-ffunction-sections -fdata-sections -Wl,--gc-sections`: Aggressively trims unused dead-code elements out of final compiled binaries.

---

## 🎯 Application Controls
- **Left-Click Button**: Disables the timer sequence and executes the bound link command immediately.
- **Right-Click Workspace**: Automatically minimizes the interface out of target screens.
- **Space Bar**: Triggers the command tied to `[Button0]` instantly.
- **Escape Key**: Shuts down the interface process cleanly.
- **Custom Hotkeys**: Hit any keyboard letter matched inside your custom `Key=` configuration rules to execute that action.

---

## ⚙️ Configuration Setup (`launcher.ini`)

The launcher does **not** feature an in-app configurations menu. **You must manually create and edit `launcher.ini`** in the same directory as your compiled `launcher.exe` executable. 

Create a file named `launcher.ini` and populate it using the comprehensive template below:

```ini
[Common]
# Filename of your background image. Safe fallback to 'BgColor' if this file cannot be found.
BgImage=launcher_bg.jpg

# The background color in RGB format (Red,Green,Blue), used if BgImage is missing.
BgColor=0,0,0

# File path to the background audio track. Supports MP3, WAV, and standard DirectShow media.
BackgroundSound=bg_music.mp3

# Sound file played once whenever the mouse pointer hovers over an active button.
ButtonSound=click.wav

# Set to 1 to seamlessly replay the background sound once it ends. Set to 0 to play it only once.
LoopBackground=1

# Width of the borderless window in screen pixels.
WindowWidth=600

# Height of the borderless window in screen pixels.
WindowHeight=300


[Timer]
# Countdown timer length in seconds before the primary button (Button0) executes automatically.
Timeout=5

# Text size of the countdown clock text.
FontSize=12

# X coordinate position (horizontal offset from the left edge of the window) for the timer text.
X=30

# Y coordinate position (vertical offset from the top edge of the window) for the timer text.
Y=265

# Font family name used to display the countdown state. Must be installed on the system (e.g., Arial, Courier New).
Font=Courier New

# Text layout shown while the countdown is ticking down. %ds will automatically swap with remaining seconds.
TextRunning=Initiating in %ds...

# Text layout shown once the user interacts with the app (clicks a button or taps keys), pausing the countdown.
TextPaused=PAUSED

# Color of the timer text in RGB format (Red,Green,Blue).
Color=24,255,3


[Button0]
# The literal text rendered on the face of the button.
Text=LAUNCH GAME

# Font family name used strictly for this specific button.
Font=Courier New

# Font text size for this button.
FontSize=14

# X coordinate position (horizontal offset from the left edge) where the button text begins.
X=50

# Y coordinate position (vertical offset from the top edge) where the button text begins.
Y=50

# Single keyboard character used as a quick shortcut key. Pressing this letter fires the command directly.
Key=L

# Default resting color of the text in RGB format.
Color=24,255,3

# Highlight color of the text when the mouse pointer hovers over the hit box rectangle.
HoverColor=255,255,255

# Program path to execute. Can be a full direct path, a local relative filename, or the keyword 'EXIT'.
Command=C:\Games\YourGame\game.exe
```
