#ifndef OVERLAY_H
#define OVERLAY_H

#include <QWidget>

class Overlay : public QWidget
{
    Q_OBJECT
public:
    explicit Overlay(class Interceptor* interceptor, QScreen* scr, QWidget* parent = nullptr);
    ~Overlay();

protected:
    bool event(QEvent* event) override;
    void paintEvent(QPaintEvent* paintEvent) override;

private:
    bool bMousePressed;
    bool bWasMainWindowVisible;
    class QPoint origin;
    class QRect selectedArea;
    class QRubberBand* rubberBand;
    Interceptor* interceptor;
    QPixmap zoomedArea;
    QString screenName;
    void drawZoomedArea(QPainter& painter, int& zoomedAreaX, int& zoomedAreaY, QPair<int, int>& corrections);
    int screenWidth;
    int screenHeight;

public:
    inline void setWasMainWindowVisible(bool bWasActive) { bWasMainWindowVisible = bWasActive; };

signals:
    void screenshotCreated();
    void cancelled();
    void cancelledDisplayMainApp();

private slots:

};

#endif // OVERLAY_H
