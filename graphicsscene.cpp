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

GraphicsScene::GraphicsScene(QObject *parent) : QGraphicsScene(parent)
  , parent(static_cast<QWidget*>(parent))
  , bDrawRectEnabled(false)
{
    connect(parent, SIGNAL(rectButtonChanged(bool)), this, SLOT(setDrawRectStatus(bool)));
    connect(parent, SIGNAL(borderButtonChanged(bool)), this, SLOT(handleBorderButtonChanged(bool)));
}

void GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (bDrawRectEnabled)
    {
        /** If the rect is double clicked, delete it
        *   To make sure that user will not delete underlaying image or border
        *   Delete only the item with Z-index = 50 which are overlay objects (rects, arrows, etc.)
        */
        QGraphicsItem *item = itemAt(mouseEvent->scenePos(), QTransform());

        if (item)
        {
            if (item->zValue() == 50)
            {
                delete item;
            }
        }
    }
    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (bDrawRectEnabled)
    {
        origin = mouseEvent->scenePos();
        rect = QRectF(origin, QSize());
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (bDrawRectEnabled)
    {
         rect = QRectF(origin, mouseEvent->scenePos());
    }
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (bDrawRectEnabled)
    {
        /** Draw rect only if the size is bigger than "dot" to make sure that no rects will be added on double click */
        if (rect.size().width() > 1)
        {
            QGraphicsRectItem* newRect = addRect(rect, PEN);
            newRect->setZValue(50);
        }
    }
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
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

void GraphicsScene::addImageToScene()
{
    /** Take a captured screen from the static Interceptor class add add it to scene; store the reference in the map */
    QGraphicsPixmapItem* img = addPixmap(*Interceptor::screenshotMap);
    img->setPos(img->x() + 2, img->y() + 2);
    img->setZValue(1);

    objects.insert(std::make_pair("img", img));

    setSceneRect(QRectF(QPointF(), Interceptor::screenshotMap->size()));

    Interceptor::cleanup();
}

void GraphicsScene::render()
{
    /** If the screenshot is captured, open save file dialog and save it */
    if(objects.find("img") != objects.end())
    {
        QRectF croppedImage = itemsBoundingRect();
        QImage img = QImage(croppedImage.toAlignedRect().size(), QImage::Format_ARGB32_Premultiplied);
        QPainter p{ QPainter(&img) };

        QGraphicsScene::render(&p, img.rect(), croppedImage);

        QString filename = QFileDialog::getSaveFileName(parent, "Save Image", QCoreApplication::applicationDirPath(), "PNG (*.png);;JPEG (*.JPEG);;BMP Files (*.bmp)");
        if (!filename.isNull())
        {
            img.save(filename);
        }
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
}

GraphicsScene::~GraphicsScene()
{

}
