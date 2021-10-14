#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QString>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QObject>
#include <QPen>
#include <QColor>

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GraphicsScene(QObject *parent = nullptr);
    ~GraphicsScene();

    void mouseDoubleClickEvent(class QGraphicsSceneMouseEvent* mouseEvent) override;
    void mouseMoveEvent(class QGraphicsSceneMouseEvent* mouseEvent) override;
    void mousePressEvent(class QGraphicsSceneMouseEvent* mouseEvent) override;
    void mouseReleaseEvent(class QGraphicsSceneMouseEvent* mouseEvent) override;

    void addBorderToScene();
    void addImageToScene();
    void render();
    void clearReferencedObjects();

private:
    QWidget* parent;
    bool bDrawRectEnabled;
    class std::vector<class QGraphicsPixmapItem*> items;
    class QRectF rect;
    class QPointF origin;
    const QPen PEN = QPen(QPen(Qt::red, 2));
    const QColor BORDER_COLOR = QColor(203, 203, 203);

private slots:
    void setDrawRectStatus(bool bIsChecked);
};

#endif // GRAPHICSSCENE_H
