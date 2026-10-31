#include "calculator.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>

#include <cmath>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
{
    // 自定义 QWidget 子类要显示样式表背景，必须开启该属性
    setAttribute(Qt::WA_StyledBackground, true);

    buildUi();
    applyStyles();

    // 窗口自身接收键盘事件；按钮统一 NoFocus，不会抢走焦点
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle(tr("计算器"));
    resize(380, 560);
}

Calculator::~Calculator() = default;

void Calculator::buildUi()
{
    labelExpression = new QLabel(this);
    labelExpression->setObjectName(QStringLiteral("labelExpression"));
    labelExpression->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    labelExpression->setTextInteractionFlags(Qt::TextSelectableByMouse);
    labelExpression->setMinimumHeight(32);

    labelResult = new QLabel(QStringLiteral("0"), this);
    labelResult->setObjectName(QStringLiteral("labelResult"));
    labelResult->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    labelResult->setTextInteractionFlags(Qt::TextSelectableByMouse);
    labelResult->setMinimumHeight(72);

    auto *displayLayout = new QVBoxLayout;
    displayLayout->setContentsMargins(16, 16, 16, 4);
    displayLayout->setSpacing(6);
    displayLayout->addWidget(labelExpression);
    displayLayout->addWidget(labelResult);

    auto *grid = new QGridLayout;
    grid->setContentsMargins(16, 8, 16, 16);
    grid->setSpacing(8);

    auto addButton = [this, grid](const QString &text, int row, int column,
                                  const QString &objectName) {
        auto *button = new QPushButton(text, this);
        button->setFocusPolicy(Qt::NoFocus);  // 避免按钮抢走窗口焦点，保证键盘可用
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        button->setMinimumSize(64, 56);
        button->setCursor(Qt::PointingHandCursor);
        button->setObjectName(objectName);
        grid->addWidget(button, row, column);
        return button;
    };

    // 第 0 行：C / CE / 退格 / 除
    connect(addButton(tr("C"), 0, 0, QStringLiteral("buttonFunction")),
            &QPushButton::clicked, this, &Calculator::handleClear);
    connect(addButton(tr("CE"), 0, 1, QStringLiteral("buttonFunction")),
            &QPushButton::clicked, this, &Calculator::handleClearEntry);
    connect(addButton(QStringLiteral("\u232B"), 0, 2, QStringLiteral("buttonFunction")),
            &QPushButton::clicked, this, &Calculator::handleBackspace);
    connect(addButton(QStringLiteral("/"), 0, 3, QStringLiteral("buttonOperator")),
            &QPushButton::clicked, this, [this] { handleOperator(Operator::Divide); });

    // 数字键：7 8 9 / 4 5 6 / 1 2 3 / 0
    const struct { const char *label; int row; int column; } digitKeys[] = {
        {"7", 1, 0}, {"8", 1, 1}, {"9", 1, 2},
        {"4", 2, 0}, {"5", 2, 1}, {"6", 2, 2},
        {"1", 3, 0}, {"2", 3, 1}, {"3", 3, 2},
        {"0", 4, 1},
    };
    for (const auto &entry : digitKeys) {
        const QString digit = QString::fromLatin1(entry.label);
        connect(addButton(digit, entry.row, entry.column, QStringLiteral("buttonDigit")),
                &QPushButton::clicked, this, [this, digit] { handleDigit(digit); });
    }

    // 乘、减、加、等号
    connect(addButton(QStringLiteral("*"), 1, 3, QStringLiteral("buttonOperator")),
            &QPushButton::clicked, this, [this] { handleOperator(Operator::Multiply); });
    connect(addButton(QStringLiteral("-"), 2, 3, QStringLiteral("buttonOperator")),
            &QPushButton::clicked, this, [this] { handleOperator(Operator::Subtract); });
    connect(addButton(QStringLiteral("+"), 3, 3, QStringLiteral("buttonOperator")),
            &QPushButton::clicked, this, [this] { handleOperator(Operator::Add); });
    connect(addButton(QStringLiteral("="), 4, 3, QStringLiteral("buttonEqual")),
            &QPushButton::clicked, this, &Calculator::handleEqual);

    // 第 4 行：+/- / 0 / 小数点 / =
    connect(addButton(QStringLiteral("\u00B1"), 4, 0, QStringLiteral("buttonFunction")),
            &QPushButton::clicked, this, &Calculator::handleToggleSign);
    connect(addButton(QStringLiteral("."), 4, 2, QStringLiteral("buttonDigit")),
            &QPushButton::clicked, this, &Calculator::handleDot);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addLayout(displayLayout);
    mainLayout->addLayout(grid, 1);

    for (int column = 0; column < 4; ++column)
        grid->setColumnStretch(column, 1);
    for (int row = 0; row < 5; ++row)
        grid->setRowStretch(row, 1);
}

void Calculator::applyStyles()
{
    setStyleSheet(QStringLiteral(R"(
        Calculator {
            background-color: #1e1f22;
        }
        QLabel#labelExpression {
            color: #8b8f98;
            font-size: 20px;
        }
        QLabel#labelResult {
            color: #ffffff;
            font-size: 48px;
            font-weight: 600;
        }
        QPushButton {
            background-color: #2b2d31;
            color: #f2f3f5;
            border: none;
            border-radius: 12px;
            font-size: 22px;
        }
        QPushButton:hover {
            background-color: #3a3d43;
        }
        QPushButton:pressed {
            background-color: #4a4e56;
        }
        QPushButton#buttonDigit {
            font-size: 24px;
        }
        QPushButton#buttonFunction {
            background-color: #26282c;
            color: #a9adb5;
            font-size: 20px;
        }
        QPushButton#buttonFunction:hover {
            background-color: #33363b;
        }
        QPushButton#buttonOperator {
            background-color: #33363b;
            color: #6cb0ff;
            font-size: 24px;
        }
        QPushButton#buttonOperator:hover {
            background-color: #3f434a;
        }
        QPushButton#buttonEqual {
            background-color: #2f6fed;
            color: #ffffff;
            font-size: 26px;
        }
        QPushButton#buttonEqual:hover {
            background-color: #4a86ff;
        }
        QPushButton#buttonEqual:pressed {
            background-color: #245bd0;
        }
    )"));
}

// ---------------------------------------------------------------- 输入槽函数

void Calculator::handleDigit(const QString &digit)
{
}

void Calculator::handleDot()
{
}

void Calculator::handleOperator(Operator op)
{
}

void Calculator::handleEqual()
{
}

void Calculator::handleBackspace()
{
}

void Calculator::handleClear()
{
}

void Calculator::handleClearEntry()
{
}

void Calculator::handleToggleSign()
{
}

// ---------------------------------------------------------------- 内部辅助

void Calculator::resetAll()
{
}

void Calculator::updateDisplay()
{
}

void Calculator::updateExpressionWhileTyping()
{
}

void Calculator::enterErrorState(const QString &expressionText)
{
}

double Calculator::applyOperator(double lhs, double rhs, Operator op, bool *ok)
{
    *ok = true;
    return 0.0;
}

QString Calculator::formatNumber(double value)
{
    return QString();
}

QString Calculator::opText(Operator op)
{
    return QString();
}
