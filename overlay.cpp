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
#include <QPair>


Overlay::Overlay(Interceptor* interceptor, QScreen* scr, QWidget *parent)
  : QWidget(parent)
  , bMousePressed(false)
  , bWasMainWindowVisible(true)
  , origin(QPoint())
  , selectedArea(QRect())
  , rubberBand(new QRubberBand(QRubberBand::Rectangle, this))
  , interceptor(interceptor)
  , zoomedArea(QPixmap())
  , screenWidth(scr->availableGeometry().width())
  , screenHeight(scr->availableGeometry().height())
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
       int mouseX = mouse->pos().x();
       int mouseY = mouse->pos().y();
       int originX = origin.x();
       int originY = origin.y();

       /** Check if the selection is made from top left/right corner, or from bottom right/left corner */
       selectedArea = QRect(
                   QPoint(mouseX < originX ? mouseX : originX, mouseY < originY ? mouseY : originY),
                   QPoint(mouseX > originX ? mouseX : originX, mouseY > originY ? mouseY : originY)
                   );

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
        QPair<int, int> corrections = interceptor->getZoomedRectangle(zoomedArea, position, screenName);

        if (position.x() + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) + static_cast<int>(SCALE::ZOOMED_AREA_CURSOR_OFFSET) > screenWidth &&
            position.y() + static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT) + static_cast<int>(SCALE::ZOOMED_AREA_CURSOR_OFFSET) > screenHeight )
        {
            int zoomedAreaX = position.x() - (static_cast<int>(SCALE::ZOOMED_AREA_CURSOR_OFFSET) + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH));
            int zoomedAreaY = position.y() - (static_cast<int>(SCALE::ZOOMED_AREA_CURSOR_OFFSET) + static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT));
            drawZoomedArea(painter, zoomedAreaX, zoomedAreaY, corrections);
        }
        else if (position.x() + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) + static_cast<int>(SCALE::ZOOMED_AREA_CURSOR_OFFSET) > screenWidth &&
                 position.y() + static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT) + static_cast<int>(SCALE::ZOOMED_AREA_CURSOR_OFFSET) < screenHeight )
        {
            int zoomedAreaX = position.x() - (static_cast<int>(SCALE::ZOOMED_AREA_CURSOR_OFFSET) + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH));
            int zoomedAreaY = position.y() + (static_cast<int>(SCALE::ZOOMED_AREA_CURSOR_OFFSET));
            drawZoomedArea(painter, zoomedAreaX, zoomedAreaY, corrections);
        }
        else if (position.y() + static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT) + static_cast<int>(SCALE::ZOOMED_AREA_CURSOR_OFFSET) > screenHeight )
        {
            int zoomedAreaX = position.x() + static_cast<int>(SCALE::ZOOMED_AREA_CURSOR_OFFSET);
            int zoomedAreaY = position.y() - (static_cast<int>(SCALE::ZOOMED_AREA_CURSOR_OFFSET) + static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT));
            drawZoomedArea(painter, zoomedAreaX, zoomedAreaY, corrections);
        }
        else
        {
            int zoomedAreaX = position.x() + static_cast<int>(SCALE::ZOOMED_AREA_CURSOR_OFFSET);
            int zoomedAreaY = position.y() + static_cast<int>(SCALE::ZOOMED_AREA_CURSOR_OFFSET);
            drawZoomedArea(painter, zoomedAreaX, zoomedAreaY, corrections);
        }
    }
    else
    {
        painter.eraseRect(rect());
        painter.drawPixmap(rect(), *interceptor->getBackgroundForWidget(screenName).get());
    }
}

void Overlay::drawZoomedArea(QPainter& painter, int& zoomedAreaX, int& zoomedAreaY, QPair<int, int>& corrections)
{
    QPen pen = QPen(Qt::PenStyle::SolidLine);
    pen.setColor(QColor(Qt::black));
    pen.setWidth(2);
    painter.setPen(pen);

    painter.drawRect(zoomedAreaX - 2, zoomedAreaY - 2, zoomedArea.width() + 4, zoomedArea.height() + 4);
    painter.drawPixmap(zoomedAreaX,
                       zoomedAreaY,
                       zoomedArea);

    /** Draw the crosshair over the zoomed area */
    pen.setStyle(Qt::PenStyle::DashDotLine);
    pen.setColor(QColor(Qt::red));
    pen.setWidth(1);
    painter.setPen(pen);

    /** Horizontal */
    painter.drawLine(QLineF(zoomedAreaX,
                            zoomedAreaY + corrections.second + static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT) / 2,
                            zoomedAreaX + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH),
                            zoomedAreaY + corrections.second + static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT) / 2));
    /** Vertical */
    painter.drawLine(QLineF(zoomedAreaX + corrections.first + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) / 2,
                            zoomedAreaY,
                            zoomedAreaX + corrections.first + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) / 2,
                            zoomedAreaY + static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT)));
}

Overlay::~Overlay()
{
   delete rubberBand;
   disconnect();
}
