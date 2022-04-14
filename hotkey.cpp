#ifdef _WIN32
#include "hotkey.h"
#include "math.h"
#include "QMessageBox"
#include "QWidget"

Hotkey::Hotkey(QWidget* application)
  : hWnd(NULL)
  , id(generateHotkeyId())
  , fsModifiers(uts::MODIFIERS::CONTROL | uts::MODIFIERS::NOREPEAT)
  , keyCode(uts::KEYCODES::KEY_T)
  , bIsRegistred(false)
  , application(application)
  , SHORTCUT_NAME("")
  , WINDOW_TITLE("")
{
   registerHotKey();
}

Hotkey::Hotkey(QWidget* application, uts::MODIFIERS fsModifiers, uts::KEYCODES keyCode, int winId, QString shortcutName, QString windowTitle)
  : hWnd(static_cast<HWND>(IntToPtr(winId)))
  , id(generateHotkeyId())
  , fsModifiers(fsModifiers)
  , keyCode(keyCode)
  , bIsRegistred(false)
  , application(application)
  , SHORTCUT_NAME(shortcutName)
  , WINDOW_TITLE(windowTitle)
{
    registerHotKey();
}

/** Function is called in the constructor since the the hotkey must be registred to have a meaning */
bool Hotkey::registerHotKey()
{
    /** Win32 API call to register the hotkey */
    bIsRegistred = RegisterHotKey(
                hWnd,
                id,
                static_cast<unsigned int>(fsModifiers),
                static_cast<unsigned int>(keyCode)
                );

    /** Check if the operation is successfull and notice the end user */
    if (!bIsRegistred)
    {
       QMessageBox msgBox;
       QString content;
       content.append("The shortcut ");
       content.append("<b>");
       content.append(SHORTCUT_NAME);
       content.append("</b>");
       content.append(" cannot be registred.\n\nYou will not be able to create screenshots by pressing the shortcut.");
       msgBox.warning(application, WINDOW_TITLE, content);
       return false;
    }
    else
    {
        QMessageBox msgBox;
        QString content;
        content.append("The shortcut ");
        content.append("<b>");
        content.append(SHORTCUT_NAME);
        content.append("</b>");
        content.append(" has been registred.\n\nIt is now reserved for taking screenshots by this application.\n\nAs long as the application is running the other functionalities of the shortcut are blocked.");
        msgBox.information(application, WINDOW_TITLE, content);
        return true;
    }
}

void Hotkey::unregisterHotKey()
{
    /** The unregisterHotKey function is called upon destruction, the result of the operation does not have to be returned */
    UnregisterHotKey(hWnd, id);
}

unsigned int Hotkey::generateHotkeyId()
{
    /** The id of the hotkey is random integer, it is used in register and unregister functions */
    return rand() % 100 + 1;
}

Hotkey::~Hotkey()
{
    /** If the hotkey has been registered during its life cycle, it must be unregistred before destroy */
    if (bIsRegistred)
    {
       unregisterHotKey();
    }
}
#endif  // _WIN32
