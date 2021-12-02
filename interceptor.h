#ifndef INTERCEPTOR_H
#define INTERCEPTOR_H
#include <QPixmap>
#include <QPoint>
#include <QSharedPointer>
#include <QPair>

enum class SCALE
{
    SCALE_FACTOR                = 4,
    ZOOMED_AREA_WIDTH           = 100,
    ZOOMED_AREA_HEIGHT          = 100,
    ZOOMED_AREA_CURSOR_OFFSET   = 50
};

constexpr int CURSOR_MOVE_FACTOR{1};

/** Struct is used for storing original and scaled dimensions of each connected screen */
typedef struct Dimensions{
    Dimensions(uint32_t origWidth, uint32_t origHeight, uint32_t scalWidth, uint32_t scalHeight)
        : originalWidth(origWidth)
        , originalHeight(origHeight)
        , scaledWidth(scalWidth)
        , scaledHeight(scalHeight)
    {
    };

    uint32_t originalWidth;
    uint32_t originalHeight;
    uint32_t scaledWidth;
    uint32_t scaledHeight;

private:
    Dimensions() = default;
} Dimensions;

class Interceptor
{
public:
    Interceptor(class QWidget* widget);
    ~Interceptor();    

    /** Selected part of the screen is saved into the Pixmap object */
    void saveScreenPartAsPixelMap(class QRect rect, const QString scrName);

    /** Grabbed Image is coppied into the clipboard */
    void saveIntoClipboard(const class QImage* grabbedImage);

    /** Created objects in the shared pointers are destroyed */
    void cleanup();

    /** Capture the whole screen into the Pixmap object */
    void saveScreensBackgroundAsPixmap();

    /** Return selected rectangle on defined coords from the scaled background of the particular screen */
    QPair<int, int> getZoomedRectangle(QPixmap& destArea, QPoint cursorPos, const QString srcName);

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

    /** Original and Scaled dimensions of all available screens */
    std::map<const QString, Dimensions> dimensionMap;

    /** Transform cursor coords from original screen to scaled version of screen grabbed image */
    QPoint getTransformedCursorPosition(const QPoint& originalCursorPosition, const QString srcName);
    int rangeTransformFormula(const int& value, const int& origMin, const int& origMax, const int& newMin, const int& newMax);

    /** Without Widget provided, Interceptor class cannot be instanciated */
    Interceptor() = default;

public:
    /** Helper function to get the pointers to all available screens */
    void getScreenPointers();
};

#endif // INTERCEPTOR_H
