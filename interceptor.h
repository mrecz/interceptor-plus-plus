#ifndef INTERCEPTOR_H
#define INTERCEPTOR_H
#include <QPixmap>
#include <QPoint>
#include <QSharedPointer>
#include <QPair>
#include "utilities.h"

QT_BEGIN_NAMESPACE
class QWidget;
class QScreen;
class QRect;
class QImage;
QT_END_NAMESPACE


class Interceptor
{
public:
    Interceptor(QWidget* widget);
    ~Interceptor();    

private:    
    QSharedPointer<QPixmap> screenshotMap{ nullptr };
    QWidget* mainWidget;

    /** Pointers to all available screens are stored in map */
    std::map<const QString, QSharedPointer<QPixmap>> screensBackgroundMap;
    std::map<const QString, QSharedPointer<QPixmap>> screensBackgroundScaledMap;
    std::map<const QString, QScreen*> screens;

    /** Original and Scaled dimensions of all available screens */
    std::map<const QString, uts::Dimensions> dimensionMap;

    /** Transform cursor coords from original screen to scaled version of screen grabbed image */
    QPoint getTransformedCursorPosition(const QPoint& originalCursorPosition, const QString srcName);
    int rangeTransformFormula(const int& value, const int& origMin, const int& origMax, const int& newMin, const int& newMax);

    /** Without Widget provided, Interceptor class cannot be instanciated */
    Interceptor() = default;

public:
    /** Selected part of the screen is saved into the Pixmap object */
    void saveScreenPartAsPixelMap(QRect rect, const QString scrName);

    /** Grabbed Image is coppied into the clipboard */
    void saveIntoClipboard(const QImage* grabbedImage);

    /** Created objects in the shared pointers are destroyed */
    void cleanup();

    /** Capture the whole screen into the Pixmap object */
    void saveScreensBackgroundAsPixmap();

    /** Return selected rectangle on defined coords from the scaled background of the particular screen */
    QPair<int, int> getZoomedRectangle(QPixmap& destArea, QPoint cursorPos, const QString srcName);

    /** Public Getters */
    inline QSharedPointer<QPixmap> getScreenshotMap() const { return screenshotMap; };
    inline QScreen* getScreenByName(QString name) const { return screens.at(name); };
    inline const std::map<const QString, QScreen*>& getScreens() const { return screens; };
    inline QSharedPointer<QPixmap> getBackgroundForWidget(const QString displayName) const { return screensBackgroundMap.at(displayName); };
    inline QSharedPointer<QPixmap> getScaledBackgroundForWidget(const QString displayName) const { return screensBackgroundScaledMap.at(displayName); };

    /** Check if screens backgrounds are saved */
    inline bool isBackgroundSaved() const { return screensBackgroundMap.size() == 0 ? false : true; };

    /** Helper function to get the pointers to all available screens */
    void getScreenPointers();
};

#endif // INTERCEPTOR_H
