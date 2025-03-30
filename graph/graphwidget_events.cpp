#include "graphwidget.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QLineF>
#include <algorithm>

void GraphWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        handleLeftMousePress(event->pos());
}

void GraphWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
        handleLeftMouseMove(event->pos());
}

void GraphWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        showTemporaryPoint = false;
        update();
    }
}

void GraphWidget::wheelEvent(QWheelEvent *event)
{
    adjustScale(event->angleDelta().y());
}

void GraphWidget::handleLeftMousePress(const QPoint &pos)
{
    if (function && trySetTemporaryPoint(pos))
        return;

    showTemporaryPoint = false;
    lastMousePos = pos;
}

bool GraphWidget::trySetTemporaryPoint(const QPoint &pos)
{
    double worldX = mapToWorld(pos).x();
    double worldY = (graphType == YofX)
                        ? function(worldX)
                        : mapToWorld(pos).y();

    double xForScreen = (graphType == YofX) ? worldX : function(worldY);
    QPointF curveScreenPoint = mapToScreen(xForScreen, (graphType == YofX) ? worldY : worldY);

    double distance = QLineF(pos, curveScreenPoint).length();
    const double threshold = 10.0;
    if (distance > threshold)
        return false;

    temporaryPointWorld = (graphType == YofX)
                              ? QPointF(worldX, worldY)
                              : QPointF(xForScreen, worldY);

    showTemporaryPoint = true;
    update();
    return true;
}

void GraphWidget::handleLeftMouseMove(const QPoint &pos)
{
    if (showTemporaryPoint)
        moveTemporaryPoint(pos);
    else
        panView(pos);
}

void GraphWidget::moveTemporaryPoint(const QPoint &pos)
{
    if (!function)
        return;

    double coord = (graphType == YofX)
                       ? mapToWorld(pos).x()
                       : mapToWorld(pos).y();

    if (graphType == YofX)
        temporaryPointWorld = QPointF(coord, function(coord));
    else
        temporaryPointWorld = QPointF(function(coord), coord);

    update();
}

void GraphWidget::panView(const QPoint &pos)
{
    QPoint delta = pos - lastMousePos;
    offsetX += delta.x();
    offsetY += delta.y();
    lastMousePos = pos;
    update();
}

void GraphWidget::adjustScale(int delta)
{
    scale *= (1.0 + delta / 1000.0);
    scale = std::clamp(scale, 1e-8, 1e8);
    update();
}
