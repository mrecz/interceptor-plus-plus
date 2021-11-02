#ifndef OVERLAY_H
#define OVERLAY_H

#include <QWidget>

class Overlay : public QWidget
{
    Q_OBJECT
public:
    explicit Overlay(class Interceptor* interceptor ,QWidget* parent = nullptr);
    ~Overlay();

protected:
    bool event(QEvent* event) override;
    void paintEvent(QPaintEvent* paintEvent) override;

private:
    bool bMousePressed;
    class QPoint origin;
    class QRect selectedArea;
    class QRubberBand* rubberBand;
    Interceptor* interceptor;
    QPixmap zoomedArea;

signals:
    void screenshotCreated();
    void cancelled();

public slots:


};

#endif // OVERLAY_H
