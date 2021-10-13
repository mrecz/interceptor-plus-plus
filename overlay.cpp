#include "overlay.h"
#include <QObject>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QRubberBand>
#include <QPoint>
#include <QRect>
#include <QSize>
#include "interceptor.h"

Overlay::Overlay(QWidget *parent) : QWidget(parent), bMousePressed(false), origin(QPoint()), selectedArea(QRect()), rubberBand(nullptr)
{
    setWindowState(Qt::WindowFullScreen);
    setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
    hide();
    setWindowOpacity(0.2);
    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
}


bool Overlay::event(QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

            if (keyEvent->key() == Qt::Key_Escape)
            {
                this->hide();
            }
        }

    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouse = static_cast<QMouseEvent*>(event);
        bMousePressed = true;
        origin = mouse->pos();
        selectedArea = QRect(origin, QSize());
        rubberBand->setGeometry(selectedArea);
        rubberBand->show();
    }

    if (event->type() == QEvent::MouseMove)
    {
       QMouseEvent* mouse = static_cast<QMouseEvent*>(event);

       selectedArea = QRect(origin, mouse->pos());
       if (bMousePressed)
       {
          rubberBand->setGeometry(selectedArea.normalized());
       }
    }

    if (event->type() == QEvent::MouseButtonRelease)
    {
       bMousePressed = false;
       rubberBand->hide();
       this->hide();
       Interceptor::saveScreenAsPixelMap(selectedArea, this);
       emit screenshotCreated();
    }

    return QWidget::event(event);
}


Overlay::~Overlay()
{
   delete rubberBand;
}
