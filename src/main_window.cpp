#include "constants.h"
#include "main_window.h"
#include "big_integer.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QString&& title, QWidget* parent)
    : QMainWindow{parent},
      ui_{new Ui::MainWindow}
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

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::digitPressed()
{
    auto button = qobject_cast<QPushButton*>(sender());

    if (operationCompleted_) {
        resetDisplay();
        resetCalculationPanel();

        operationCompleted_ = false;
        waitingForOperand_ = true;
        waitingForOperator_ = true;

        currentOperation_ = Operation::None;
        value_ = Consts::empty;
    }

    if (ui_->display->text() == Consts::defaultVal)
        resetDisplay(true);

    ui_->display->setText(ui_->display->text() + button->text());
}

void MainWindow::addPressed()
{
    if (operationCompleted_) return;

    currentOperation_ = Operation::Add;
    setArithmeticOperation();
}

void MainWindow::subtractPressed()
{
    if (operationCompleted_) return;

    currentOperation_ = Operation::Subtract;
    setArithmeticOperation();
}

void MainWindow::multiplyPressed()
{
    if (operationCompleted_) return;

    currentOperation_ = Operation::Multiply;
    setArithmeticOperation();
}

void MainWindow::dividePressed()
{
    if (operationCompleted_) return;

    currentOperation_ = Operation::Divide;
    setArithmeticOperation();
}

void MainWindow::setArithmeticOperation()
{
    if (currentOperation_ == Operation::Add)
        ui_->calculationPanel->setText(Consts::add);
    else if (currentOperation_ == Operation::Subtract)
        ui_->calculationPanel->setText(Consts::subtract);
    else if (currentOperation_ == Operation::Multiply)
        ui_->calculationPanel->setText(Consts::multiply);
    else if (currentOperation_ == Operation::Divide)
        ui_->calculationPanel->setText(Consts::divide);

    waitingForOperator_ = false;
    waitingForOperand_ = true;

    if (value_.isEmpty())
        value_ = ui_->display->text();

    resetDisplay();
}

void MainWindow::changeSignPressed()
{
    if (operationCompleted_) return;

    auto text = ui_->display->text();

    if (text.startsWith(Consts::negative))
        text.remove(0, 1);
    else if (text != Consts::defaultVal)
        text.prepend(Consts::negative);

    ui_->display->setText(text);
}

void MainWindow::equalPressed()
{
    if (waitingForOperator_) return;
    if (operationCompleted_) return;

    QString op;
    if (currentOperation_ == Operation::Add)
        op = Consts::add;
    else if (currentOperation_ == Operation::Subtract)
        op = Consts::subtract;
    else if (currentOperation_ == Operation::Multiply)
        op = Consts::multiply;
    else if (currentOperation_ == Operation::Divide)
        op = Consts::divide;

    auto text = value_ + " " + op + " " + ui_->display->text();
    ui_->calculationPanel->setText(text);

    operationCompleted_ = true;
    result_ = calculate(currentOperation_);
    value_ = Consts::empty;

    ui_->display->setText(result_);
}

void MainWindow::backspacePressed()
{
    if (operationCompleted_)
        return;

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
    operationCompleted_ = false;

    resetDisplay();
    resetCalculationPanel();
}

void MainWindow::memoryPressed()
{
    auto currentOperation = qobject_cast<QPushButton*>(sender())->text();

    if (currentOperation == Consts::addToMemory)
        memory_.append(result_);

    if (!memory_.isEmpty()) {
        if (currentOperation == Consts::clearMemory)
            memory_ = Consts::empty;
        else if (currentOperation == Consts::readMemory)
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
    wingmann::big_integer number{value_.toStdString()};
    std::string result;

    if (operation == Operation::Add) {
        result = number
            .add(ui_->display->text().toStdString())
            .to_string();
    }
    else if (operation == Operation::Subtract) {
        result = number
            .subtract(ui_->display->text().toStdString())
            .to_string();
    }
    else if (operation == Operation::Multiply) {
        result = number
            .multiply(ui_->display->text().toStdString())
            .to_string();
    }
    else if (operation == Operation::Divide) {
        result = number
            .divide(ui_->display->text().toStdString())
            .to_string();
    }
    return QString::fromStdString(result);
}
