#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QObject>
#include <string>

enum class MODE
{
    FILE        = 0x00,
    CLIPBOARD   = 0x01
};

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    /** Interceptor pointer is needed for screen Pixmap manipulations */
    explicit GraphicsScene(class Interceptor* interceptor, QObject *parent = nullptr);
    ~GraphicsScene();

    void mouseDoubleClickEvent(class QGraphicsSceneMouseEvent* mouseEvent) override;
    void mouseMoveEvent(class QGraphicsSceneMouseEvent* mouseEvent) override;
    void mousePressEvent(class QGraphicsSceneMouseEvent* mouseEvent) override;
    void mouseReleaseEvent(class QGraphicsSceneMouseEvent* mouseEvent) override;

    void addBorderToScene();
    void addImageToScene();
    void render(MODE mode);
    void clearReferencedObjects();

private:
    QWidget* parent;
    bool bDrawRectEnabled;
    class std::map<std::string, class QGraphicsPixmapItem*> objects;
    class QRectF rect;
    class QPointF origin;
    const QPen PEN{ QPen(QPen(Qt::red, 2)) };
    const QColor BORDER_COLOR{ QColor(203, 203, 203) };
    Interceptor* interceptor;
    class QString savePath;

private slots:
    void setDrawRectStatus(bool bIsChecked);
    void handleBorderButtonChanged(bool bIsChecked);
};

#endif // GRAPHICSSCENE_H
