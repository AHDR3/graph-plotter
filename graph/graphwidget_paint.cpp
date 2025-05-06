#include "graphwidget.h"
#include <QPainter>
#include <QPainterPath>
#include <cmath>

void GraphWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    drawBackground(painter);
    drawGrid(painter);
    drawAxes(painter);
    drawAxisTicks(painter);
    drawGraph(painter);
    drawTemporaryPoint(painter);
}

void GraphWidget::drawBackground(QPainter &painter)
{
    painter.fillRect(rect(), Qt::white);
}

double GraphWidget::computeStart(double min, double spacing) const
{
    return std::floor(min / spacing) * spacing;
}

void GraphWidget::drawGridLines(
    QPainter &painter,
    double start,
    double end,
    double fixedStart,
    double fixedEnd,
    bool vertical,
    double spacing
)
{
    double pos = computeStart(start, spacing);
    for (; pos <= end; pos += spacing)
    {
        if (vertical)
            painter.drawLine(mapToScreen(pos, fixedStart), mapToScreen(pos, fixedEnd));
        else
            painter.drawLine(mapToScreen(fixedStart, pos), mapToScreen(fixedEnd, pos));
    }
}

void GraphWidget::drawGrid(QPainter &painter)
{
    auto [minX, maxX, minY, maxY] = visibleWorldRect();
    double spacing = gridSpacing();
    double minorSpacing = spacing / 5.0;
    painter.setPen(QPen(Qt::lightGray, 1));
    drawGridLines(painter, minX, maxX, minY, maxY, true, minorSpacing);
    drawGridLines(painter, minY, maxY, minX, maxX, false, minorSpacing);
}

void GraphWidget::drawAxes(QPainter &painter)
{
    QPointF origin = mapToScreen(0, 0);
    painter.setPen(QPen(Qt::gray, 1, Qt::DashLine));
    painter.drawLine(QPointF(0, origin.y()), QPointF(width(), origin.y()));
    painter.drawLine(QPointF(origin.x(), 0), QPointF(origin.x(), height()));
}

void GraphWidget::drawAxisTicksHelper(QPainter &painter, double start, double end, double fixedCoord, bool vertical, double spacing)
{
    double pos = computeStart(start, spacing);
    for (; pos <= end; pos += spacing)
    {
        QPointF pt = vertical ? mapToScreen(pos, fixedCoord) : mapToScreen(fixedCoord, pos);
        if (vertical)
            painter.drawLine(pt.x(), pt.y() - 5, pt.x(), pt.y() + 5);
        else
            painter.drawLine(pt.x() - 5, pt.y(), pt.x() + 5, pt.y());

        double labelValue = (std::abs(pos) < 1e-8 ? 0.0 : pos);
        painter.drawText(pt + QPointF(2, -2), QString::number(labelValue));
    }
}

void GraphWidget::drawAxisTicks(QPainter &painter)
{
    auto [minX, maxX, minY, maxY] = visibleWorldRect();
    double spacing = gridSpacing();
    painter.setPen(Qt::gray);
    drawAxisTicksHelper(painter, minX, maxX, 0, true, spacing);
    drawAxisTicksHelper(painter, minY, maxY, 0, false, spacing);
}

void GraphWidget::drawGraphYofX(QPainterPath &path)
{
    bool firstPoint = true;
    for (int px = 0; px < width(); ++px)
    {
        double xw = mapToWorld(QPointF(px, 0)).x();
        double yw = function(xw);
        if (!std::isfinite(yw))
        {
            firstPoint = true;
            continue;
        }
        QPointF pt = mapToScreen(xw, yw);
        if (firstPoint)
        {
            path.moveTo(pt);
            firstPoint = false;
        }
        else
            path.lineTo(pt);
    }
}

void GraphWidget::drawGraphXofY(QPainterPath &path)
{
    bool firstPoint = true;
    for (int py = 0; py < height(); ++py)
    {
        double yw = mapToWorld(QPointF(0, py)).y();
        double xw = function(yw);
        if (!std::isfinite(xw))
        {
            firstPoint = true;
            continue;
        }

        QPointF pt = mapToScreen(xw, yw);

        if (firstPoint)
        {
            path.moveTo(pt);
            firstPoint = false;
        }
        else
            path.lineTo(pt);
    }
}

void GraphWidget::drawGraph(QPainter &painter)
{
    if (!function)
        return;
    painter.setPen(QPen(Qt::blue, 2));
    QPainterPath path;
    if (graphType == YofX)
        drawGraphYofX(path);
    else
        drawGraphXofY(path);
    painter.drawPath(path);
}

void GraphWidget::drawTemporaryPoint(QPainter &painter)
{
    if (!showTemporaryPoint)
        return;
    QPointF screenPt = mapToScreen(temporaryPointWorld.x(), temporaryPointWorld.y());
    painter.setPen(Qt::red);
    painter.setBrush(Qt::red);
    painter.drawEllipse(screenPt, 3, 3);
    QString coords = QString("(%1, %2)").arg(temporaryPointWorld.x(), 0, 'g', 6).arg(temporaryPointWorld.y(), 0, 'g', 6);
    painter.drawText(screenPt + QPointF(5, -5), coords);
}

std::tuple<double, double, double, double> GraphWidget::visibleWorldRect() const
{
    QPointF topLeft = mapToWorld(QPointF(0, 0));
    QPointF bottomRight = mapToWorld(QPointF(width(), height()));
    double minX = topLeft.x();
    double maxX = bottomRight.x();
    double maxY = topLeft.y();
    double minY = bottomRight.y();
    return {minX, maxX, minY, maxY};
}

double GraphWidget::gridSpacing() const
{
    const double desiredPixelSpacing = 50.0;
    double tickSpacingCandidate = desiredPixelSpacing / scale;
    double exponent = std::floor(std::log10(tickSpacingCandidate));
    double fraction = tickSpacingCandidate / std::pow(10, exponent);
    double niceFraction;
    if (fraction < 1.5)
        niceFraction = 1.0;
    else if (fraction < 3.0)
        niceFraction = 2.0;
    else if (fraction < 7.0)
        niceFraction = 5.0;
    else
        niceFraction = 10.0;
    return niceFraction * std::pow(10, exponent);
}
