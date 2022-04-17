#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "operation.h"

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QString&& title, QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow* ui_;

    Operation currentOperation_{Operation::None};

    QString value_;
    QString result_;
    QString memory_;

    bool waitingForOperand_{false};
    bool waitingForOperator_{true};
    bool operationCompleted_{false};

private:
    void setArithmeticOperation();
    void resetDisplay(bool clearAllText = false);
    void resetCalculationPanel();
    QString calculate(Operation operation);

private slots:
    void digitPressed();

    void addPressed();
    void subtractPressed();
    void changeSignPressed();
    void equalPressed();

    void backspacePressed();
    void clearPressed();
    void clearAllPressed();
    void memoryPressed();
};

#endif // MAIN_WINDOW_H
