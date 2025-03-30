#include "graphwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>

GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent),
    function(nullptr),
    domainMin(-10),
    domainMax(10),
    offsetX(0),
    offsetY(0),
    scale(30),
    showTemporaryPoint(false),
    graphType(YofX)
{
    setMinimumSize(400, 300);
}

void GraphWidget::setFunction(std::function<double(double)> f)
{
    function = f;
    update();
}

void GraphWidget::setDomain(double xMin, double xMax)
{
    domainMin = xMin;
    domainMax = xMax;
    update();
}

void GraphWidget::resetView()
{
    offsetX = 0;
    offsetY = 0;
    scale = 30;
    update();
}

QPointF GraphWidget::mapToScreen(double x, double y) const
{
    double cx = width() / 2.0 + offsetX;
    double cy = height() / 2.0 + offsetY;
    double sx = cx + x * scale;
    double sy = cy - y * scale;
    return QPointF(sx, sy);
}

QPointF GraphWidget::mapToWorld(const QPointF &pt) const
{
    double cx = width() / 2.0 + offsetX;
    double cy = height() / 2.0 + offsetY;
    double x = (pt.x() - cx) / scale;
    double y = (cy - pt.y()) / scale;
    return QPointF(x, y);
}
