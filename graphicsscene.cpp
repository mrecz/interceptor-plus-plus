#include "graphicsscene.h"
#include "interceptor.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QRectF>
#include <QPointF>
#include <QSize>
#include <QGraphicsPixmapItem>
#include <QCoreApplication>
#include <QPen>
#include <QColor>
#include <QFileDialog>
#include <map>
#include <QGraphicsItem>
#include <QSharedPointer>
#include <QString>
#include <QGraphicsSvgItem>

GraphicsScene::GraphicsScene(Interceptor* interceptor, QObject *parent)
  : QGraphicsScene(parent)
  , parent(static_cast<QWidget*>(parent))
  , numberScaleLevel(1)
  , bDrawRectEnabled(false)  
  , bAddNumbersEnabled(false)  
  , interceptor(interceptor)  
  , savePath("")  
  , itemUnderCursor(nullptr)

{
    connect(parent, SIGNAL(rectButtonChanged(bool)), this, SLOT(setDrawRectStatus(bool)));
    connect(parent, SIGNAL(numbersButtonChanged(bool)), this, SLOT(handleNumbersButtonChanged(bool)));
    connect(parent, SIGNAL(borderButtonChanged(bool)), this, SLOT(handleBorderButtonChanged(bool)));
}

void GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (bDrawRectEnabled)
    {
        deleteItem(mouseEvent, 50);
    }
    else if (bAddNumbersEnabled)
    {
       deleteItem(mouseEvent, 60);
    }
    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    /** If drawing rectangles functionality is ON, save the place where user has started the selection */
    if (bDrawRectEnabled)
    {
        /** Track which mouse button has been pressed to determine the action */
        pressedButton = mouseEvent->button();
        switch(pressedButton)
        {
            case Qt::MouseButton::RightButton:
                origin = mouseEvent->scenePos();
                rect = QRectF(origin, QSize());
                break;
            default:
                QGraphicsScene::mouseMoveEvent(mouseEvent);
        }
    }
    else if (bAddNumbersEnabled)
    {
        /** Track which mouse button has been pressed to determine the action */
        pressedButton = mouseEvent->button();
        switch(pressedButton)
        {
            case Qt::MouseButton::RightButton:
                addNumber(mouseEvent);
                break;
            default:
                QGraphicsScene::mouseMoveEvent(mouseEvent);
        }

    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
     /** If drawing rectangles functionality is ON, update the rectangle size according to the selection */
    if (bDrawRectEnabled)
    {
        updateItemUnderMouseCursor(mouseEvent->scenePos());

        switch(pressedButton)
        {
            case Qt::MouseButton::RightButton:
                /** Is the rect drawn from top left or bottom right corner */
                if (origin.x() < mouseEvent->scenePos().x())
                {
                    rect = QRectF(origin, mouseEvent->scenePos());
                }
                else
                {
                    rect = QRectF(mouseEvent->scenePos(), origin);
                }
                break;
            default:
                QGraphicsScene::mouseMoveEvent(mouseEvent);
        }
    }
   QGraphicsScene::mousePressEvent(mouseEvent);
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
     /** If drawing rectangles functionality is ON, draw a red ractangle */
    if (bDrawRectEnabled)
    {
        switch(pressedButton)
        {
            case Qt::MouseButton::RightButton:
                /** Draw rect only if the size is bigger than "dot" to make sure that no rects will be added on double click */
                if (rect.size().width() > 3 && rect.size().height() > 3)
                {
                    drawRect(rect);
                }
                break;
            default:
                QGraphicsScene::mouseMoveEvent(mouseEvent);
        }
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void GraphicsScene::wheelEvent(QGraphicsSceneWheelEvent* wheelEvent)
{
    /** Resize the rect under the mouse cursor according to the wheel movement: Wheel Up = +size, Wheel Down = -size */
    if (bDrawRectEnabled)
    {
        if (itemUnderCursor)
        {
            if (wheelEvent->delta() < 0)
            {
               resizeRect(RESIZE_MODE::MINUS);
            }
            else
            {
               resizeRect(RESIZE_MODE::PLUS);
            }

        }
        else
        {
          QGraphicsScene::wheelEvent(wheelEvent);
        }
    }
    if (bAddNumbersEnabled)
    {
         if (wheelEvent->delta() < 0)
         {
            resizeNumbers(RESIZE_MODE::MINUS);
         }
         else
         {
            resizeNumbers(RESIZE_MODE::PLUS);
         }
    }
    QGraphicsScene::wheelEvent(wheelEvent);
    wheelEvent->accept();
}

void GraphicsScene::setDrawRectStatus(bool bIsChecked)
{
    bDrawRectEnabled = bIsChecked;
}

void GraphicsScene::addBorderToScene()
{
    /** Image object exists on the scene, so it makes sense to handle border */
    if (objects.find("img") != objects.end())
    {
        /** Another check if the border object has been already created previously
        *   If so, just display it since QGraphicsScene does not delete removed items
        */
        if (objects.find("border") == objects.end())
        {
            QGraphicsPixmapItem* img = objects["img"];
            QPixmap border = QPixmap(img->boundingRect().width() + 4, img->boundingRect().height() + 4);
            border.fill(BORDER_COLOR);
            QGraphicsPixmapItem* item = addPixmap(border);
            item->setZValue(0);

            objects.insert(std::make_pair("border", item));
        }
        else
        {
            addItem(objects["border"]);
        }
    }
}

void GraphicsScene::handleBorderButtonChanged(bool bIsChecked)
{
    /** Add Border button works in two modes
    *   1. If the screenshot does not exist, it specifies if the border will be added by default
    *   2. If the screenshot exists on the scene, it displayes or hide the border
    */
    if (objects.find("img") != objects.end())
    {
        if (bIsChecked)
        {
            addBorderToScene();
        }
        else
        {
            /** By remove, just remove it from scene, keep the existing object referenced since it is not deleted by the scene */
            this->removeItem(objects["border"]);
        }
    }
}

void GraphicsScene::handleNumbersButtonChanged(bool bIsChecked)
{
    /** Set member variable to the value of the NumberAction - true/false */
    bAddNumbersEnabled = bIsChecked;
}

void GraphicsScene::addImageToScene()
{
    /** Take a captured screen from the static Interceptor class add add it to scene; store the reference in the map */
    QGraphicsPixmapItem* img = addPixmap(*interceptor->getScreenshotMap());
    img->setPos(img->x() + 2, img->y() + 2);
    img->setZValue(1);

    objects.insert(std::make_pair("img", img));

    setSceneRect(QRectF(QPointF(), interceptor->getScreenshotMap()->size()));

    interceptor->cleanup();
}

void GraphicsScene::render(MODE mode)
{
    /** If the screenshot is captured, open save file dialog and save it */
    if(objects.find("img") != objects.end())
    {
        QRectF croppedImage = itemsBoundingRect();
        QImage img = QImage(croppedImage.toAlignedRect().size(), QImage::Format_ARGB32_Premultiplied);
        QPainter p{ QPainter(&img) };

        QGraphicsScene::render(&p, img.rect(), croppedImage);
        /** Check if SAVE or COPY TO CLIPBOARD button has been pressed */
        if (mode == MODE::FILE)
        {
            /** Check if the previous file location exists, if not set it to applicationDirPath */
            if (savePath == "")
            {
                savePath = QCoreApplication::applicationDirPath();
            }

            QString filename = QFileDialog::getSaveFileName(parent, "Save Image", savePath, "PNG (*.png);;JPEG (*.JPEG);;BMP Files (*.bmp)");

            /** Remember the last location selected for storing the image*/
            if (filename.lastIndexOf('/') != -1)
            {
                savePath = filename.chopped(filename.lastIndexOf('/'));
            }

            if (!filename.isNull())
            {
                img.save(filename);
            }
        }
        else
        {
            interceptor->saveIntoClipboard(&img);
        }
    }
}

void GraphicsScene::updateItemUnderMouseCursor(const QPointF mousePos)
{
    /** Check if item is not the background and is not nullptr */
    QGraphicsItem* item = itemAt(mousePos, QTransform());
    if (item && item->zValue() == 50)
    {
        itemUnderCursor = itemAt(mousePos, QTransform());
    }
    else
    {
        itemUnderCursor = nullptr;
    }
}


void GraphicsScene::drawRect(QRectF rectangle)
{
    /** To ensure that rect will have correct coords in the scene, draw it on [0,0] and then set the correct scene position */
    QGraphicsRectItem* newRect = addRect(QRectF(0, 0, rectangle.width(), rectangle.height()), PEN);
    newRect->setPos(rectangle.topLeft());
    newRect->setZValue(50);
    newRect->setFlags(QGraphicsItem::ItemIsMovable);
    newRect->setAcceptedMouseButtons(Qt::MouseButton::LeftButton);
}

void GraphicsScene::resizeRect(RESIZE_MODE mode)
{
    /** Fake resize, it create a new object, either bigger or smaller and delete the old one */
    QRectF box = itemUnderCursor->boundingRect();
    QPointF pos = itemUnderCursor->scenePos();
    QPointF center = QPointF(pos.x() + box.width() / 2, pos.y() + box.height() / 2);
    if (mode == RESIZE_MODE::MINUS)
    {
        /** The rect size can be reduced max to the min size */
        if (box.width() > 20 && box.height() > 20)
        {
           drawRect(QRectF(pos.x() + 5, pos.y() + 5, box.width() - 10, box.height() - 10));
           removeItem(itemUnderCursor);
           delete itemUnderCursor;
           updateItemUnderMouseCursor(center);
        }
    }
    else
    {
        /** Max size is defined by the underlying image size if exists */
        if (objects.find("img") != objects.end())
        {
            if (box.width() < objects["img"]->boundingRect().width() && box.height() < objects["img"]->boundingRect().height())
            {
                drawRect(QRectF(pos.x() - 5, pos.y() - 5, box.width() + 10, box.height() + 10));
                removeItem(itemUnderCursor);
                delete itemUnderCursor;
                updateItemUnderMouseCursor(center);
            }
        }
    }

}

void GraphicsScene::resizeNumbers(RESIZE_MODE mode)
{
    if (mode == RESIZE_MODE::PLUS)
    {
        if (numberScaleLevel < 11.f)
        {
            numberScaleLevel += 0.2f;
        }
    }
    else
    {
        if (numberScaleLevel > 0.5f)
        {
           numberScaleLevel -= 0.2f;
        }
    }
    /** All existing numbers are resized at once by a Scale factor */
    for (const auto& number: numbers)
    {
        number.second->setScale(numberScaleLevel);
    }
}

void GraphicsScene::clearReferencedObjects()
{
    /** Clear the map and make sure that no objects are left in memory */
    for (const auto& object : objects)
    {
        if (object.second)
        {
            delete object.second;
        }
    }
    objects.clear();
    for (const auto& number : numbers)
    {
        delete number.second;
    }
    numbers.clear();
}

void GraphicsScene::addNumber(QGraphicsSceneMouseEvent* mouseEvent)
{
    /** To add number, first we must check already existing numbers to return the correct one - max numbers == 10;
     *  if there are already 10 numbers do not add a new one */

    for(size_t i = 1; i < 11; i++)
    {
        if (numbers.find(i) == numbers.end())
        {
            QString numberName =  ":/numbering/";
            numberName.append(QString::number(i));
            QGraphicsSvgItem* item = new QGraphicsSvgItem(numberName);
            /** Defines an element which should be taken from the SVG file
            * In SVG Number files, the desired elements are named after the number which is drawn in the SVG file
            */
            item->setElementId(QString::number(i));
            item->setZValue(60);
            addItem(item);
            item->setPos(mouseEvent->scenePos());
            item->setFlags(QGraphicsItem::ItemIsMovable);
            item->setAcceptedMouseButtons(Qt::MouseButton::LeftButton);
            /** Apply current scaler factor */
            item->setScale(numberScaleLevel);
            numbers.insert(std::make_pair(i, item));
            break;
        }
    }
}

void GraphicsScene::deleteItem(QGraphicsSceneMouseEvent* mouseEvent, int zIndex)
{
    /** Deletes an item on the scene if exits udner the mouse pointer and with defined z-index */
    QGraphicsItem* item = itemAt(mouseEvent->scenePos(), QTransform());
    if (item)
    {
        if (item->zValue() == zIndex)
        {
           /** Check if the deleted item is a number and if so, delete it from numbers map by using the element ID */
           QGraphicsSvgItem* number = dynamic_cast<QGraphicsSvgItem*>(item);
           if (number)
           {
              numbers.erase(number->elementId().toLongLong());
           }
           removeItem(item);
           delete item;
        }
    }
}

GraphicsScene::~GraphicsScene()
{

}
