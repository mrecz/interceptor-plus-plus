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


Overlay::Overlay(Interceptor* interceptor, QWidget *parent)
  : QWidget(parent)
  , bMousePressed(false)
  , origin(QPoint())
  , selectedArea(QRect())
  , rubberBand(new QRubberBand(QRubberBand::Rectangle, this))
  , backgroundColor(palette().light().color())
  , interceptor(interceptor)
  , zoomedArea(QPixmap())

{
    setWindowState(Qt::WindowFullScreen);
    setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowModality(Qt::ApplicationModal);
    setWindowIcon(QIcon(":/img/IconMain"));
    setMouseTracking(true);
    setCursor(Qt::CrossCursor);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setStyleSheet("selection-background-color: red;");
    backgroundColor.setAlpha(30);
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
            this->hide();
            emit cancelled();
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
           hide();
           interceptor->saveScreenAsPixelMap(selectedArea, 0);
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
    /** Fill wdiget with a semi transparent background */
    painter.fillRect(rect(),backgroundColor);


    /** Creates coord lines */
    painter.drawLine(QLineF(0, QCursor::pos().y(), interceptor->getScreen()->geometry().width(), QCursor::pos().y()));
    painter.drawLine(QLineF(QCursor::pos().x(), 0, QCursor::pos().x(), interceptor->getScreen()->geometry().height()));


    /** Zoom area around the mouse cursor */
    interceptor->getZoomedRectangle(zoomedArea, QCursor::pos());
    painter.drawPixmap(QCursor::pos().x() + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) / 2,
                       QCursor::pos().y() + static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT) / 2,
                       zoomedArea);

    /** Draw the crosshair over the zoomed area */
    QPen pen = QPen(Qt::PenStyle::DashDotLine);
    pen.setColor(QColor(Qt::red));
    painter.setPen(pen);
    painter.drawLine(QLineF(QCursor::pos().x() + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) - static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) / 2,
                            QCursor::pos().y() + static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT),
                            QCursor::pos().x() + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) / 2,
                            QCursor::pos().y() + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH)));
    painter.drawLine(QLineF(QCursor::pos().x() + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH),
                            QCursor::pos().y() + static_cast<int>(SCALE::ZOOMED_AREA_HEIGHT) - static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) / 2,
                            QCursor::pos().x() + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH),
                            QCursor::pos().y() + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) + static_cast<int>(SCALE::ZOOMED_AREA_WIDTH) / 2));
}

Overlay::~Overlay()
{
   delete rubberBand;
}
