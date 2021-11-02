#ifdef _WIN32
/** Global Hotkeys functionality works only on Windows operation system */
#ifndef HOTKEY_H
#define HOTKEY_H

#include "Windows.h"
#include "QString"

/** All modifiers supported by Win32 API */
enum class MODIFIERS
{
    ALT         = 0x0001,
    CONTROL     = 0x0002,
    NOREPEAT    = 0x4000,
    SHIFT       = 0x0004,
    WIN         = 0x0008
};

inline MODIFIERS operator|(MODIFIERS mod1, MODIFIERS mod2)
{
    return static_cast<MODIFIERS>(static_cast<int>(mod1) | static_cast<int>(mod2));
}

/** Subset of Virtual Key codes supported by Win32 API */
enum class KEYCODES
{
    KEY_T           = 0x54,
    KEY_P           = 0x50,
    KEY_PRINTSCR    = 0x2C
};

class Hotkey
{
public:
    Hotkey(class QWidget* application);
    Hotkey(QWidget* application, MODIFIERS fsModifiers, KEYCODES keyCode, int winId, QString shortcutName, QString windowTitle);
    ~Hotkey();

private:
    Hotkey() = default;
    HWND hWnd;
    unsigned int id;
    MODIFIERS fsModifiers;
    KEYCODES keyCode;
    bool bIsRegistred;
    QWidget* application;
    const QString SHORTCUT_NAME;
    const QString WINDOW_TITLE;

    bool registerHotKey();
    void unregisterHotKey();
    unsigned int generateHotkeyId();
};

#endif // HOTKEY_H
#endif // _WIN32
