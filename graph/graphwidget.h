#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <functional>

class QMouseEvent;
class QWheelEvent;
class QPainter;
class QPainterPath;

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    enum GraphType { YofX, XofY };

    explicit GraphWidget(QWidget *parent = nullptr);

    void setFunction(std::function<double(double)> f);
    void setDomain(double xMin, double xMax);
    void resetView();
    void setGraphType(GraphType type) { graphType = type; }

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    std::function<double(double)> function;
    double domainMin;
    double domainMax;
    double offsetX;
    double offsetY;
    double scale;
    bool showTemporaryPoint;
    QPointF temporaryPointWorld;
    QPoint lastMousePos;
    GraphType graphType;

    QPointF mapToScreen(double x, double y) const;
    QPointF mapToWorld(const QPointF &pt) const;

    void drawBackground(QPainter &painter);
    void drawGrid(QPainter &painter);
    void drawAxes(QPainter &painter);
    void drawAxisTicks(QPainter &painter);
    void drawGraph(QPainter &painter);
    void drawTemporaryPoint(QPainter &painter);

    std::tuple<double, double, double, double> visibleWorldRect() const;
    double gridSpacing() const;

    void handleLeftMousePress(const QPoint &pos);
    bool trySetTemporaryPoint(const QPoint &pos);
    void handleLeftMouseMove(const QPoint &pos);
    void moveTemporaryPoint(const QPoint &pos);
    void panView(const QPoint &pos);
    void adjustScale(int delta);

    double computeStart(double min, double spacing) const;

    void drawGridLines(
        QPainter &painter,
        double start,
        double end,
        double fixedStart,
        double fixedEnd,
        bool vertical,
        double spacing
    );

    void drawAxisTicksHelper(
        QPainter &painter,
        double start,
        double end,
        double fixedCoord,
        bool vertical,
        double spacing
    );

    void drawGraphYofX(QPainterPath &path);
    void drawGraphXofY(QPainterPath &path);
};

#endif // GRAPHWIDGET_H
