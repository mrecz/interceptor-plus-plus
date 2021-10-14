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

GraphicsScene::GraphicsScene(QObject *parent) : QGraphicsScene(parent)
  , parent(static_cast<QWidget*>(parent))
  , bDrawRectEnabled(false)
{
    connect(parent, SIGNAL(rectButtonChanged(bool)), this, SLOT(setDrawRectStatus(bool)));
}

void GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (bDrawRectEnabled)
    {

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
        addRect(rect, PEN);
    }
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void GraphicsScene::setDrawRectStatus(bool bIsChecked)
{
    bDrawRectEnabled = bIsChecked;
}

void GraphicsScene::addBorderToScene()
{
    QPixmap border = QPixmap(Interceptor::screenshotMap->width() + 4, Interceptor::screenshotMap->height() + 4);
    border.fill(BORDER_COLOR);

    items.push_back(addPixmap(border));
}

void GraphicsScene::addImageToScene()
{
    QGraphicsPixmapItem* img = addPixmap(*Interceptor::screenshotMap);
    img->setPos(img->x() + 2, img->y() + 2);

    items.push_back(img);

    setSceneRect(QRectF(QPointF(), Interceptor::screenshotMap->size()));

    Interceptor::cleanup();
}

void GraphicsScene::render()
{
    if(items.size() != 0)
    {
        QGraphicsPixmapItem* biggerItem = items[0];
        QRectF croppedImage = biggerItem->boundingRect();
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
    items.clear();
}

GraphicsScene::~GraphicsScene()
{

}
