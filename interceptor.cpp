#include "interceptor.h"
#include <QRect>
#include <QScreen>
#include <QGuiApplication>
#include <QWindow>
#include <QWidget>


void Interceptor::saveScreenAsPixelMap(QRect rect, QWidget* widget)
{
    QScreen* screen = QGuiApplication::primaryScreen();
    int winId = 0;

    if (const QWindow* window = widget->windowHandle())
    {
       screen = window->screen();
    }

    if (!screen) return;

    screenshotMap = new QPixmap(screen->grabWindow(winId, rect.x(), rect.y(), rect.width(), rect.height()));
}

void Interceptor::cleanup()
{
    delete screenshotMap;
    screenshotMap = nullptr;
}

Interceptor::~Interceptor()
{
    delete screenshotMap;
}
