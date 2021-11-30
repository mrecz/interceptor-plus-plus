#include "overlay.h"
#include "interceptor.h"

#include <QObject>
#include <QEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QRubberBand>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QIcon>
#include <QWindow>


Overlay::Overlay(Interceptor* interceptor, QScreen* scr, QWidget *parent)
  : QWidget(parent)
  , bMousePressed(false)
  , bWasMainWindowVisible(true)
  , origin(QPoint())
  , selectedArea(QRect())
  , rubberBand(new QRubberBand(QRubberBand::Rectangle, this))
  , interceptor(interceptor)
  , zoomedArea(QPixmap())
{
    setWindowState(Qt::WindowMaximized);
    windowHandle()->setScreen(scr);
    setWindowState(Qt::WindowFullScreen);
    setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowIcon(QIcon(":/img/IconMain"));
    setMouseTracking(true);
    setCursor(Qt::CrossCursor);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setStyleSheet("selection-background-color: red;");
    screenName = windowHandle()->screen()->name();
    hide();
}

/** Fullscreen overlay transparent widget, handle only mouse events + ESC for closing the widget + Arrows Keys for moving the cursor precisely */
bool Overlay::event(QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        QPoint cursorPos = QCursor::pos();

        if (keyEvent->key() == Qt::Key_Escape)
        {
            hide();
            interceptor->cleanup();

            /** Emit the signal for restoring main window only if it was previously visible */
            if (bWasMainWindowVisible)
            {
              emit cancelledDisplayMainApp();
            }
            else
            {
              emit cancelled();
            }
        }

        if (keyEvent->key() == Qt::Key_Left)
        {
            QCursor::setPos(cursorPos.x() - CURSOR_MOVE_FACTOR, cursorPos.y());
        }

        if (keyEvent->key() == Qt::Key_Right)
        {
            QCursor::setPos(cursorPos.x() + CURSOR_MOVE_FACTOR, cursorPos.y());
        }

        if (keyEvent->key() == Qt::Key_Up)
        {
            QCursor::setPos(cursorPos.x(), cursorPos.y() - CURSOR_MOVE_FACTOR);
        }

        if (keyEvent->key() == Qt::Key_Down)
        {
            QCursor::setPos(cursorPos.x(), cursorPos.y() + CURSOR_MOVE_FACTOR);
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

       /** Check if the selection is made from top left corner, or from bottom right corner */
       if (origin.x() < mouse->pos().x())
       {
           selectedArea = QRect(origin, mouse->pos());
       }
       else
       {
           selectedArea = QRect(mouse->pos(), origin);
       }

       if (bMousePressed)
       {
          rubberBand->setGeometry(selectedArea.normalized());
       }
       update();
    }

    if (event->type() == QEvent::MouseButtonRelease)
    {
       bMousePressed = false;
       rubberBand->hide();

       /** Take a screenshot only if user has made a selection, do not if he has clicked accidentally */
       if (selectedArea.width() != -1 && selectedArea.height() != -1)
       {           
           interceptor->saveScreenPartAsPixelMap(selectedArea, screenName);
           hide();
           emit screenshotCreated();
       }

    }

    return QWidget::event(event);
}

/** Overriden version of PaintEvent function
*   It fills widget with semi transparent background
*   It draw coord lines which help to be more precise while doing screenshot
*/
void Overlay::paintEvent(QPaintEvent* paintEvent)
{
    /** Call parent function */
    QWidget::paintEvent(paintEvent);

    QPainter painter(this);
    /** Fill widget with the pixmap background */
    painter.drawPixmap(rect(), *interceptor->getBackgroundForWidget(screenName).get());

    /** Multiplescreens support, check if the cursor is over the overlay on the particular screen */
    QPoint position = mapFromGlobal(QCursor::pos());
    if (rect().contains(position)) {
        /** Creates coord lines */
        painter.drawLine(QLineF(0, position.y(), interceptor->getScreenByName(screenName)->geometry().width(), position.y()));
        painter.drawLine(QLineF(position.x(), 0, position.x(), interceptor->getScreenByName(screenName)->geometry().height()));

        /** Zoom area around the mouse cursor */
        interceptor->getZoomedRectangle(zoomedArea, position, screenName);
        painter.drawPixmap(position.x() + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) / 2,
                           position.y() + static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT) / 2,
                           zoomedArea);

        /** Draw the crosshair over the zoomed area */
        QPen pen = QPen(Qt::PenStyle::DashDotLine);
        pen.setColor(QColor(Qt::red));
        painter.setPen(pen);
        painter.drawLine(QLineF(position.x() + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) - static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) / 2,
                                position.y() + static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT),
                                position.x() + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) / 2,
                                position.y() + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH)));
        painter.drawLine(QLineF(position.x() + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH),
                                position.y() + static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT) - static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) / 2,
                                position.x() + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH),
                                position.y() + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) / 2));
    }
}

Overlay::~Overlay()
{
   delete rubberBand;
   disconnect();
}
