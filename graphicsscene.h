#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QObject>
#include <string>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QGraphicsPixmapItem;
class QGraphicsSvgItem;
QT_END_NAMESPACE

class Interceptor;

enum class MODE
{
    FILE        = 0x00,
    CLIPBOARD   = 0x01
};

enum class RESIZE_MODE
{
    PLUS        = 0x00,
    MINUS       = 0x01
};

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    /** Interceptor pointer is needed for screen Pixmap manipulations */
    explicit GraphicsScene(Interceptor* interceptor, QObject *parent = nullptr);
    ~GraphicsScene();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void wheelEvent(QGraphicsSceneWheelEvent* wheelEvent) override;

public:
    void addBorderToScene();
    void addImageToScene();
    void render(MODE mode);
    void clearReferencedObjects();

private:
    QWidget* parent;
    float numberScaleLevel;
    bool bDrawRectEnabled;
    bool bAddNumbersEnabled;
    class std::map<std::string, QGraphicsPixmapItem*> objects;
    class QRectF rect;
    class QPointF origin;
    const QPen PEN{ QPen(QPen(Qt::red, 2)) };
    const QColor BORDER_COLOR{ QColor(228, 228, 228) };
    Interceptor* interceptor;
    class QString savePath;
    Qt::MouseButton pressedButton;
    QGraphicsItem* itemUnderCursor;
    std::map<size_t, QGraphicsSvgItem*> numbers;

    void updateItemUnderMouseCursor(const QPointF mousePos);
    void drawRect(QRectF rectangle);
    void resizeRect(RESIZE_MODE mode);
    void deleteItem(QGraphicsSceneMouseEvent* mouseEvent, int zIndex);
    void addNumber(QGraphicsSceneMouseEvent* mouseEvent);
    void resizeNumbers(RESIZE_MODE mode);

private slots:
    void setDrawRectStatus(bool bIsChecked);
    void handleBorderButtonChanged(bool bIsChecked);
    void handleNumbersButtonChanged(bool bIsChecked);
};

#endif // GRAPHICSSCENE_H
