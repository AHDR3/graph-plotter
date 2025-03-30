#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "library/exprtk.hpp"
#include <memory>
#include <functional>

namespace Ui {
class MainWindow;
}

class GraphWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handlePushButtonClicked();

private:
    Ui::MainWindow *ui;
    GraphWidget *graphWidget = nullptr;

    void setupGraphWidget();
    bool isXofYGraph(const QString &expr) const;
    QString prepareExpression(const QString &expr) const;
    void setupGraphFunction(const QString &expr, bool isXofY);

    std::shared_ptr<exprtk::expression<double>> compileExpression(const QString &expr,
                                                                  const QString &varName,
                                                                  std::shared_ptr<double> varPtr) const;

    std::function<double(double)> createFunction(std::shared_ptr<exprtk::expression<double>> exprPtr,
                                                 std::shared_ptr<double> varPtr) const;
};

#endif // MAINWINDOW_H
