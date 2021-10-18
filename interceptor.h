#ifndef INTERCEPTOR_H
#define INTERCEPTOR_H
#include <QPixmap>

class Interceptor
{
public:

    ~Interceptor();
    static void saveScreenAsPixelMap(class QRect rect, class QWidget* widget);
    inline static QPixmap* screenshotMap{nullptr};

    static void saveIntoClipboard(const class QImage* grabbedImage);

    static void cleanup();

private:    
    Interceptor() = default;

};

#endif // INTERCEPTOR_H
