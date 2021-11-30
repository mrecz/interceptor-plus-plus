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

Interceptor::Interceptor(class QWidget* widget):
    originalWidth(0)
  , originalHeight(0)
  , scaledWidth(0)
  , scaledHeight(0)
{
    mainWidget = widget;
    getScreenPointers();
}

void Interceptor::saveScreenPartAsPixelMap(QRect rect, QString srcName)
{

    QRect cropped = QRect(rect.x(), rect.y(), rect.width(), rect.height());
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
        originalWidth = scrGrab.width();
        originalHeight = scrGrab.height();

        /** Original screen grab is scaled by using the scale factor defined in the header file */
        scrGrab = scrGrab.scaledToWidth(scrGrab.width() * static_cast<int>(SCALE::SCALE_FACTOR));

        /** Store scaled dimensions */
        scaledWidth = scrGrab.width();
        scaledHeight = scrGrab.height();

        /** Each screen scaled background is stored as well for magnifying glass around cursor effect */
        screensBackgroundScaledMap.insert(std::make_pair(screen.second->name(), QSharedPointer<QPixmap>(new QPixmap(scrGrab))));
    }
}

void Interceptor::getZoomedRectangle(QPixmap& destArea, QPoint cursorPos, QString srcName)
{
    QPoint scaledCursorPos = getTransformedCursorPosition(cursorPos);
    QRect cropped = QRect(scaledCursorPos.x() - (static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) / 2), scaledCursorPos.y() - (static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT) / 2),
                          static_cast<int>(SCALE::ZOOMED_AREA_WIDTH),
                          static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT));
    destArea = getScaledBackgroundForWidget(srcName)->copy(cropped);
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
    /**Multiple screens setup */
    screensBackgroundMap.clear();
    screensBackgroundScaledMap.clear();
}

Interceptor::~Interceptor()
{
    screenshotMap.clear();
}
