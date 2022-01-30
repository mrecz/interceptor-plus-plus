#include "interceptor.h"
#include <QRect>
#include <QScreen>
#include <QGuiApplication>
#include <QWindow>
#include <QWidget>
#include <QImage>
#include <QClipboard>
#include <QtMath>
#include <map>

Interceptor::Interceptor(class QWidget* widget)
{
    mainWidget = widget;
    getScreenPointers();
}

void Interceptor::saveScreenPartAsPixelMap(QRect rect, const QString srcName)
{
    /** To make sure that the correct part of the screen is taken, multiply the coords by the device pixel ratio
    *   If Windows Scaling is enabled, this is necessary step
    */
    float DPI = getScreenByName(srcName)->devicePixelRatio(); // TODO
    QRect cropped = QRect(rect.x() * DPI, rect.y() * DPI, rect.width() * DPI, rect.height() * DPI);
    screenshotMap = QSharedPointer<QPixmap>(new QPixmap(getBackgroundForWidget(srcName)->copy(cropped)));
}

void Interceptor::saveIntoClipboard(const QImage* grabbedImage)
{
    QGuiApplication::clipboard()->setImage(*grabbedImage);
}

void Interceptor::saveScreensBackgroundAsPixmap()
{
    /** Recheck available screens, a user may disconnect a monitor meanwhile */
    getScreenPointers();

    /** Timeout is necessary to get the parent widget time to hide itself */
    QThread::msleep(180);

    /** Multiple screen setup */
    for (const auto& screen : screens)
    {
        /** Each screen will have an overlay with the background stored here */
        QPixmap scrGrab = screen.second->grabWindow(0);
        screensBackgroundMap.insert(std::make_pair(screen.second->name(), QSharedPointer<QPixmap>(new QPixmap(scrGrab))));        
        /** Store original dimensions */
        int originalWidth{ scrGrab.width() };
        int originalHeight{ scrGrab.height() };

        /** Original screen grab is scaled by using the scale factor defined in the header file */
        scrGrab = scrGrab.scaledToWidth(scrGrab.width() * static_cast<int>(SCALE::SCALE_FACTOR));

        /** Store scaled dimensions */
        int scaledWidth{ scrGrab.width() };
        int scaledHeight{ scrGrab.height() };

        /** Each screen scaled background is stored as well for magnifying glass around cursor effect */
        screensBackgroundScaledMap.insert(std::make_pair(screen.second->name(), QSharedPointer<QPixmap>(new QPixmap(scrGrab))));

        /** Store dimensions of a screen for further usage */
        dimensionMap.insert(std::make_pair(screen.first, Dimensions(originalWidth, originalHeight, scaledWidth, scaledHeight)));
    }
}

QPair<int, int> Interceptor::getZoomedRectangle(QPixmap& destArea, QPoint cursorPos, const QString srcName)
{
    float DPI = getScreenByName(srcName)->devicePixelRatio();
    QPoint scaledCursorPos = getTransformedCursorPosition(cursorPos * DPI, srcName);
    uint32_t scaledWidth = dimensionMap.at(srcName).scaledWidth;
    uint32_t scaledHeight = dimensionMap.at(srcName).scaledHeight;
    int offsetX = scaledCursorPos.x() - (static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) / 2);
    int offsetY = scaledCursorPos.y() - (static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT) / 2);
    /** Check if the offset is a negative number, if so set the minimum - 0 */
    int positionX = offsetX < 0 ? 0 : offsetX;
    int positionY = offsetY < 0 ? 0 : offsetY;
    /** Check if the mouseCursor position + zoomed area size is higher than max dimension, if so set the maximum value - (scaled - ZOOMED_AREA_WIDTH) */
    int overflowX = static_cast<int>(scaledWidth - (scaledCursorPos.x() + (static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) / 2)));
    int overflowY = static_cast<int>(scaledHeight - (scaledCursorPos.y() + (static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT) / 2)));
    if (overflowX < 0) positionX = scaledWidth  - static_cast<int>(SCALE::ZOOMED_AREA_WIDTH);
    if (overflowY < 0) positionY = scaledHeight - static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT);

    QRect cropped = QRect(positionX,
                          positionY,
                          static_cast<int>(SCALE::ZOOMED_AREA_WIDTH),
                          static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT));
    destArea = getScaledBackgroundForWidget(srcName)->copy(cropped);
    destArea.setDevicePixelRatio(1.f);

    /** Corrections for crosshair drawn by Overlay widget: negative for offset and positive for overflow */
    int correctionX{ 0 };
    int correctionY{ 0 };
    if (offsetX < 0) correctionX = offsetX;
    if (offsetY < 0) correctionY = offsetY;
    if (overflowX < 0) correctionX = overflowX * -1;
    if (overflowY < 0) correctionY = overflowY * -1;

    return QPair<int, int>(correctionX, correctionY);
}

void Interceptor::getScreenPointers()
{
    /** Multiple screens setup */
    screens.clear();
    for (const auto& scr : QGuiApplication::screens() )
    {
        screens.insert(std::make_pair(scr->name(), scr));
    }
}

QPoint Interceptor::getTransformedCursorPosition(const QPoint& originalCursorPosition, const QString srcName)
{
    int x{ originalCursorPosition.x() };
    int y{ originalCursorPosition.y() };

    x = rangeTransformFormula(x, 0, dimensionMap.at(srcName).originalWidth, 0, dimensionMap.at(srcName).scaledWidth);
    y = rangeTransformFormula(y, 0, dimensionMap.at(srcName).originalHeight, 0, dimensionMap.at(srcName).scaledHeight);

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
    /** Multiple screens setup */
    screensBackgroundMap.clear();
    screensBackgroundScaledMap.clear();
}

Interceptor::~Interceptor()
{
    screenshotMap.clear();
}
