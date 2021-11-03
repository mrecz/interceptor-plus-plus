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
    void saveScreenPartAsPixelMap(class QRect rect);

    /** Grabbed Image is coppied into the clipboard */
    void saveIntoClipboard(const class QImage* grabbedImage);

    /** Created objects in the shared pointers are destroyed */
    void cleanup();

    /** Capture the whole screen into the Pixmap object */
    void saveWholeScreenAsPixmap();

    void getZoomedRectangle(QPixmap& destArea, QPoint cursorPos);

    /** Public Getters */
    inline QSharedPointer<QPixmap> getScreenshotMap() {return screenshotMap; };
    inline QSharedPointer<QPixmap> getWholeScreenMap() {return wholeScreenMap; };
    inline class QScreen* getScreen() {return mainScreen;};

private:    
    QSharedPointer<QPixmap> screenshotMap{nullptr};
    QSharedPointer<QPixmap> wholeScreenMap{nullptr};
    QSharedPointer<QPixmap> wholeScreenMapScaled{nullptr};
    QScreen* mainScreen;
    QWidget* mainWidget;

    /** Variables for storing original and scaled screen size to transform cursor position */
    uint32_t originalWidth;
    uint32_t originalHeight;
    uint32_t scaledWidth;
    uint32_t scaledHeight;

    /** Helper function to get the pointer to the main screen */
    QScreen* getScreenPointer();

    /** Transform cursor coords from original screen to scaled version of screen grabbed image */
    QPoint getTransformedCursorPosition(const QPoint& originalCursorPosition);
    int rangeTransformFormula(const int& value, const int& origMin, const int& origMax, const int& newMin, const int& newMax);

    /** Without Widget provided, Interceptor class cannot be instanciated */
    Interceptor() = default;
};

#endif // INTERCEPTOR_H
