#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graph/graphwidget.h"
#include "library/exprtk.hpp"

#include <QMessageBox>
#include <memory>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::handlePushButtonClicked);
    setupGraphWidget();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupGraphWidget()
{
    graphWidget = new GraphWidget(this);
    ui->graphLayout->addWidget(graphWidget);
}

void MainWindow::handlePushButtonClicked()
{
    QString exprStr = ui->lineEdit->text().trimmed();

    bool isX = isXofYGraph(exprStr);
    QString preparedExpr = prepareExpression(exprStr);

    if (!graphWidget)
        return;

    setupGraphFunction(preparedExpr, isX);
}

bool MainWindow::isXofYGraph(const QString &expr) const
{
    return expr.startsWith("x=", Qt::CaseInsensitive) || expr.startsWith("x =", Qt::CaseInsensitive);
}

QString MainWindow::prepareExpression(const QString &expr) const
{
    int index = expr.indexOf('=');
    if (index == -1)
        return expr;
    return expr.mid(index + 1).trimmed();
}

void MainWindow::setupGraphFunction(const QString &expr, bool isX)
{
    using T = double;

    auto variablePtr = std::make_shared<T>(0.0);
    QString varName = (isX ? "y" : "x");

    auto exprPtr = compileExpression(expr, varName, variablePtr);
    if (!exprPtr)
    {
        QMessageBox::warning(this, "Ошибка", "Неверное выражение!");
        return;
    }

    std::function<double(double)> func = createFunction(exprPtr, variablePtr);
    graphWidget->setGraphType(isX ? GraphWidget::XofY : GraphWidget::YofX);
    graphWidget->setFunction(func);
}

std::shared_ptr<exprtk::expression<double>> MainWindow::compileExpression(
    const QString &expr,
    const QString &varName,
    std::shared_ptr<double> varPtr) const
{
    exprtk::symbol_table<double> symbolTable;
    symbolTable.add_variable(varName.toStdString(), *varPtr);
    symbolTable.add_constants();

    auto expressionPtr = std::make_shared<exprtk::expression<double>>();
    expressionPtr->register_symbol_table(symbolTable);

    exprtk::parser<double> parser;

    if (!parser.compile(expr.toStdString(), *expressionPtr))
        return nullptr;

    return expressionPtr;
}

std::function<double(double)> MainWindow::createFunction(
    std::shared_ptr<exprtk::expression<double>> exprPtr,
    std::shared_ptr<double> varPtr) const
{
    return [exprPtr, varPtr](double val) mutable -> double
    {
        *varPtr = val;
        return exprPtr->value();
    };
}
