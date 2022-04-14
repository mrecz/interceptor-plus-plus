#ifdef _WIN32
/** Global Hotkeys functionality works only on Windows operation system */
#ifndef HOTKEY_H
#define HOTKEY_H

#include "Windows.h"
#include "QString"
#include "utilities.h"

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

class Hotkey
{
public:
    Hotkey(QWidget* application);
    Hotkey(QWidget* application, uts::MODIFIERS fsModifiers, uts::KEYCODES keyCode, int winId, QString shortcutName, QString windowTitle);
    ~Hotkey();

private:
    Hotkey() = default;
    HWND hWnd;
    unsigned int id;
    uts::MODIFIERS fsModifiers;
    uts::KEYCODES keyCode;
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
