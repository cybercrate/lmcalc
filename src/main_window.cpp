#include <biginteger/bigInteger.h>

#include "lmcalc/main_window.h"
#include "lmcalc/constants.h"

#include "ui_main_window.h"

MainWindow::MainWindow(QString&& title, QWidget* parent)
    : QMainWindow{parent}, ui_{new Ui::MainWindow}
{
    ui_->setupUi(this);
    setWindowTitle(title);

    // Digits.
    connect(ui_->pushButton_zero, &QPushButton::clicked, this, &MainWindow::digitPressed);
    connect(ui_->pushButton_one, &QPushButton::clicked, this, &MainWindow::digitPressed);
    connect(ui_->pushButton_two, &QPushButton::clicked, this, &MainWindow::digitPressed);
    connect(ui_->pushButton_three, &QPushButton::clicked, this, &MainWindow::digitPressed);
    connect(ui_->pushButton_four, &QPushButton::clicked, this, &MainWindow::digitPressed);
    connect(ui_->pushButton_five, &QPushButton::clicked, this, &MainWindow::digitPressed);
    connect(ui_->pushButton_six, &QPushButton::clicked, this, &MainWindow::digitPressed);
    connect(ui_->pushButton_seven, &QPushButton::clicked, this, &MainWindow::digitPressed);
    connect(ui_->pushButton_eight, &QPushButton::clicked, this, &MainWindow::digitPressed);
    connect(ui_->pushButton_nine, &QPushButton::clicked, this, &MainWindow::digitPressed);

    // Operators.
    connect(ui_->pushButton_plus, &QPushButton::clicked, this, &MainWindow::addPressed);
    connect(ui_->pushButton_minus, &QPushButton::clicked, this, &MainWindow::subtractPressed);
    connect(ui_->pushButton_multiply, &QPushButton::clicked, this, &MainWindow::multiplyPressed);
    connect(ui_->pushButton_divide, &QPushButton::clicked, this, &MainWindow::dividePressed);
    connect(ui_->pushButton_changeSign, &QPushButton::clicked, this, &MainWindow::changeSignPressed);
    connect(ui_->pushButton_equal, &QPushButton::clicked, this, &MainWindow::equalPressed);

    // Modifiers.
    connect(ui_->pushButton_backspace, &QPushButton::clicked, this, &MainWindow::backspacePressed);
    connect(ui_->pushButton_clear, &QPushButton::clicked, this, &MainWindow::clearPressed);
    connect(ui_->pushButton_clearAll, &QPushButton::clicked, this, &MainWindow::clearAllPressed);

    // Memory.
    connect(ui_->pushButton_clearMemory, &QPushButton::clicked, this, &MainWindow::memoryPressed);
    connect(ui_->pushButton_readMemory, &QPushButton::clicked, this, &MainWindow::memoryPressed);
    connect(ui_->pushButton_addToMemory, &QPushButton::clicked, this, &MainWindow::memoryPressed);
}

MainWindow::~MainWindow() {
    delete ui_;
}

void MainWindow::digitPressed() {
    auto button = qobject_cast<QPushButton*>(sender());

    if (operationComplete_) {
        resetDisplay();
        resetCalculationPanel();

        operationComplete_ = false;
        waitingForOperand_ = true;
        waitingForOperator_ = true;

        currentOperation_ = Operation::None;
        value_ = Consts::empty;
    }
    if (ui_->display->text() == Consts::defaultVal) {
        resetDisplay(true);
    }
    ui_->display->setText(ui_->display->text() + button->text());
}

void MainWindow::addPressed() {
    if (operationComplete_) {
        return;
    }
    currentOperation_ = Operation::Add;
    setArithmeticOperation();
}

void MainWindow::subtractPressed() {
    if (operationComplete_) {
        return;
    }
    currentOperation_ = Operation::Subtract;
    setArithmeticOperation();
}

void MainWindow::multiplyPressed() {
    if (operationComplete_) {
        return;
    }
    currentOperation_ = Operation::Multiply;
    setArithmeticOperation();
}

void MainWindow::dividePressed() {
    if (operationComplete_) {
        return;
    }
    currentOperation_ = Operation::Divide;
    setArithmeticOperation();
}

void MainWindow::setArithmeticOperation() {
    switch (currentOperation_) {
    case Operation::Add:
        ui_->calculationPanel->setText(Consts::add);
        break;
    case Operation::Subtract:
        ui_->calculationPanel->setText(Consts::subtract);
        break;
    case Operation::Multiply:
        ui_->calculationPanel->setText(Consts::multiply);
        break;
    case Operation::Divide:
        ui_->calculationPanel->setText(Consts::divide);
        break;
    default:
        break;
    }
    waitingForOperator_ = false;
    waitingForOperand_ = true;

    if (value_.isEmpty()) {
        value_ = ui_->display->text();
    }
    resetDisplay();
}

void MainWindow::changeSignPressed() {
    if (operationComplete_) {
        return;
    }
    auto text = ui_->display->text();

    if (text.startsWith(Consts::negative)) {
        text.remove(0, 1);
    } else if (text != Consts::defaultVal) {
        text.prepend(Consts::negative);
    }
    ui_->display->setText(text);
}

void MainWindow::equalPressed() {
    if (waitingForOperator_ || operationComplete_) {
        return;
    }
    QString op;

    switch (currentOperation_) {
    case Operation::Add:
        op = Consts::add;
        break;
    case Operation::Subtract:
        op = Consts::subtract;
        break;
    case Operation::Multiply:
        op = Consts::multiply;
        break;
    case Operation::Divide:
        op = Consts::divide;
        break;
    default:
        break;
    }
    auto text = value_ + " " + op + " " + ui_->display->text();
    ui_->calculationPanel->setText(text);

    operationComplete_ = true;
    result_ = calculate(currentOperation_);
    value_ = Consts::empty;

    ui_->display->setText(result_);
}

void MainWindow::backspacePressed() {
    if (operationComplete_) {
        return;
    }
    auto text = ui_->display->text();
    text.chop(1);

    if (text.isEmpty()) {
        text = Consts::defaultVal;
        waitingForOperand_ = true;
    }
    ui_->display->setText(text);
}

void MainWindow::clearPressed()
{
    ui_->display->setText(Consts::defaultVal);
    waitingForOperand_ = true;
}

void MainWindow::clearAllPressed()
{
    waitingForOperand_ = true;
    waitingForOperator_ = false;
    operationComplete_ = false;

    resetDisplay();
    resetCalculationPanel();
}

void MainWindow::memoryPressed()
{
    auto currentOperation = qobject_cast<QPushButton*>(sender())->text();

    if (currentOperation == Consts::addToMemory) {
        memory_.append(result_);
    }
    if (memory_.isEmpty()) {
        return;
    }
    if (currentOperation == Consts::clearMemory) {
        memory_ = Consts::empty;
    } else if (currentOperation == Consts::readMemory) {
        ui_->display->setText(memory_);
    }
}

void MainWindow::resetDisplay(bool clearAllText)
{
    ui_->display->setText(clearAllText ? Consts::empty : Consts::defaultVal);
}

void MainWindow::resetCalculationPanel()
{
    ui_->calculationPanel->setText(Consts::empty);
}

QString MainWindow::calculate(Operation operation)
{
    wingmann::numerics::BigInteger lhs = value_.toStdString();
    wingmann::numerics::BigInteger rhs = ui_->display->text().toStdString();

    switch (operation) {
    case Operation::Add:
        return (lhs + rhs).to_string().c_str();
    case Operation::Subtract:
        return (lhs - rhs).to_string().c_str();
    case Operation::Multiply:
        return (lhs * rhs).to_string().c_str();
    case Operation::Divide:
        return (lhs / rhs).to_string().c_str();
    default:
        break;
    }
    return {};
}
