#ifndef OVERLAY_H
#define OVERLAY_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QPoint;
class QRect;
class QRubberBand;
QT_END_NAMESPACE
class Interceptor;

class Overlay : public QWidget
{
    Q_OBJECT
public:
    explicit Overlay(Interceptor* interceptor, QScreen* scr, QWidget* parent = nullptr);
    ~Overlay();

protected:
    bool event(QEvent* event) override;
    void paintEvent(QPaintEvent* paintEvent) override;

private:
    bool bMousePressed;
    bool bWasMainWindowVisible;
    QPoint origin;
    QPoint mousePos;
    QRect selectedArea;
    QRubberBand* rubberBand;
    Interceptor* interceptor;
    QPixmap zoomedArea;
    QString screenName;
    void drawZoomedArea(QPainter& painter, int& zoomedAreaX, int& zoomedAreaY, QPair<int, int>& corrections);
    int screenWidth;
    int screenHeight;
    QScreen* screen;

public:
    inline void setWasMainWindowVisible(bool bWasActive) { bWasMainWindowVisible = bWasActive; };

signals:
    void screenshotCreated();
    void cancelled();
    void cancelledDisplayMainApp();

private slots:

};

#endif // OVERLAY_H
