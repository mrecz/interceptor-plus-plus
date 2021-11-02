#include "interceptor.h"
#include <QRect>
#include <QScreen>
#include <QGuiApplication>
#include <QWindow>
#include <QWidget>
#include <QImage>
#include <QClipboard>
#include <QtMath>

Interceptor::Interceptor(class QWidget* widget):
    originalWidth(0)
  , originalHeight(0)
  , scaledWidth(0)
  , scaledHeight(0)
{
    mainWidget = widget;
    mainScreen = getScreenPointer();
}

void Interceptor::saveScreenPartAsPixelMap(QRect rect, const int id)
{

    QRect cropped = QRect(rect.x(), rect.y(), rect.width(), rect.height());
    screenshotMap = QSharedPointer<QPixmap>(new QPixmap(wholeScreenMap->copy(cropped)));
}

void Interceptor::saveIntoClipboard(const QImage* grabbedImage)
{
    QGuiApplication::clipboard()->setImage(*grabbedImage);
}

void Interceptor::saveWholeScreenAsPixmap()
{
    if (mainWidget->isVisible())
    {
       mainWidget->hide();
    }

    /** Timeout is necessary to get the parent widget time to hide itself */
    QThread::msleep(150);

    /** Grab the whole primary screen */
    QPixmap screenGrab = mainScreen->grabWindow(0);

    /** Save the original screen; it will be used as background in the overlay widget */
    wholeScreenMap = QSharedPointer<QPixmap>(new QPixmap(screenGrab));

    /** Store original dimensions */
    originalWidth = screenGrab.width();
    originalHeight = screenGrab.height();

    /** Original screen grab is scaled by using the scale factor defined in the header file */
    screenGrab = screenGrab.scaledToWidth(screenGrab.width() * static_cast<int>(SCALE::SCALE_FACTOR));

    /** Store scaled dimensions */
    scaledWidth = screenGrab.width();
    scaledHeight = screenGrab.height();

    /** Temporary stack scaled Pixmap is stored in pointer member variable */
    wholeScreenMapScaled = QSharedPointer<QPixmap>(new QPixmap(screenGrab));
}

void Interceptor::getZoomedRectangle(QPixmap& destArea, QPoint cursorPos)
{
    QPoint scaledCursorPos = getTransformedCursorPosition(cursorPos);
    QRect cropped = QRect(scaledCursorPos.x() - (static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) / 2), scaledCursorPos.y() - (static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT) / 2),
                          static_cast<int>(SCALE::ZOOMED_AREA_WIDTH),
                          static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT));
    destArea = wholeScreenMapScaled->copy(cropped);
}

QScreen* Interceptor::getScreenPointer()
{
    QScreen* screen = QGuiApplication::primaryScreen();

    if (const QWindow* window = mainWidget->windowHandle())
    {
       screen = window->screen();
    }

    if (!screen)
    {
        return nullptr;
    }
    else
    {
        return screen;
    }
}

QPoint Interceptor::getTransformedCursorPosition(const QPoint& originalCursorPosition)
{
    int x{originalCursorPosition.x()};
    int y{originalCursorPosition.y()};

    x = rangeTransformFormula(x, 0, originalWidth, 0, scaledWidth);
    y = rangeTransformFormula(y, 0, originalHeight, 0, scaledHeight);

    return QPoint(x, y);
}

int Interceptor::rangeTransformFormula(const int& value, const int& origMin, const int& origMax, const int& newMin, const int& newMax)
{
    /** Second operand has to be casted to float to not have 0 result */
    float result = ((newMax - newMin) * (static_cast<float>(value - origMin) / (origMax - origMin))) + newMin;
    return qFloor<float>(result);
}

void Interceptor::cleanup()
{
    screenshotMap.clear();
    wholeScreenMapScaled.clear();
    wholeScreenMap.clear();
}

Interceptor::~Interceptor()
{
    screenshotMap.clear();
}
