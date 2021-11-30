#ifdef _WIN32
#include "customeventfilter.h"

CustomEventFilter::CustomEventFilter(QObject* parent)
{
}

bool CustomEventFilter::nativeEventFilter(const QByteArray &eventType, void* message, long long* result)
{
   MSG* msg = static_cast<MSG*>(message);
   if (msg->message == WM_HOTKEY)
   {
       /** Current screen must be captured immediately before the active window lost focus; e.g. if some context menu is opened, it must persist. */
       emit captureCurrentScreen();
       /** Even if the application is not active (minimized, without focus, etc.) the overlay window must be on the top and receive keyboard strokes */
       for(const auto& id : winIDs)
       {
           SetForegroundWindow(id);
       }
       /** Emit signal which will be received by main window which handles the rest */
       emit registredKeyPressed();
       return true;
   }
   return false;
}
#endif // _WIN32
