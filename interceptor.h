#ifndef INTERCEPTOR_H
#define INTERCEPTOR_H
#include <QPixmap>
#include <QPoint>
#include <QSharedPointer>

enum class SCALE
{
    SCALE_FACTOR        = 4,
    ZOOMED_AREA_WIDTH   = 100,
    ZOOMED_AREA_HEIGHT  = 100
};

constexpr int CURSOR_MOVE_FACTOR{1};

class Interceptor
{
public:
    Interceptor(class QWidget* widget);
    ~Interceptor();    

    /** Selected part of the screen is saved into the Pixmap object */
    void saveScreenPartAsPixelMap(class QRect rect, QString scrName);

    /** Grabbed Image is coppied into the clipboard */
    void saveIntoClipboard(const class QImage* grabbedImage);

    /** Created objects in the shared pointers are destroyed */
    void cleanup();

    /** Capture the whole screen into the Pixmap object */
    void saveScreensBackgroundAsPixmap();

    /** Return selected rectangle on defined coords from the scaled background of the particular screen */
    void getZoomedRectangle(QPixmap& destArea, QPoint cursorPos, QString srcName);

    /** Public Getters */
    inline QSharedPointer<QPixmap> getScreenshotMap() const { return screenshotMap; };
    inline class QScreen* getScreenByName(QString name) const { return screens.at(name); };
    inline const class std::map<const QString, QScreen*>& getScreens() const { return screens; };
    inline QSharedPointer<QPixmap> getBackgroundForWidget(const QString displayName) const { return screensBackgroundMap.at(displayName); };
    inline QSharedPointer<QPixmap> getScaledBackgroundForWidget(const QString displayName) const { return screensBackgroundScaledMap.at(displayName); };

    /** Check if screens backgrounds are saved */
    inline bool isBackgroundSaved() const { return screensBackgroundMap.size() == 0 ? false : true; };

private:    
    QSharedPointer<QPixmap> screenshotMap{ nullptr };
    QWidget* mainWidget;

    /** Pointers to all available screens are stored in map */
    std::map<const QString, QSharedPointer<QPixmap>> screensBackgroundMap;
    std::map<const QString, QSharedPointer<QPixmap>> screensBackgroundScaledMap;
    std::map<const QString, QScreen*> screens;

    /** Variables for storing original and scaled screen size to transform cursor position */
    uint32_t originalWidth;
    uint32_t originalHeight;
    uint32_t scaledWidth;
    uint32_t scaledHeight;

    /** Transform cursor coords from original screen to scaled version of screen grabbed image */
    QPoint getTransformedCursorPosition(const QPoint& originalCursorPosition);
    int rangeTransformFormula(const int& value, const int& origMin, const int& origMax, const int& newMin, const int& newMax);

    /** Without Widget provided, Interceptor class cannot be instanciated */
    Interceptor() = default;

public:
    /** Helper function to get the pointers to all available screens */
    void getScreenPointers();
};

#endif // INTERCEPTOR_H
