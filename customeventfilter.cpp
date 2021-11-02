#ifdef _WIN32
#include "customeventfilter.h"

CustomEventFilter::CustomEventFilter(int winId, QObject* parent)
    : winId(static_cast<HWND>(IntToPtr(winId)))
{
}

bool CustomEventFilter::nativeEventFilter(const QByteArray &eventType, void* message, long long* result)
{
   MSG* msg = static_cast<MSG*>(message);
   if (msg->message == WM_HOTKEY)
   {
       /** Even if the application is not active (minimized, without focus, etc.) the overlay window must be on the top and receive keyboard strokes */
       SetForegroundWindow(winId);
       /** Emit signal which will be received by main window which handles the rest */
       emit registredKeyPressed();
       return true;
   }
   return false;
}
#endif // _WIN32
