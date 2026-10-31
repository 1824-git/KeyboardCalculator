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
    // 错误状态或结果已显示：数字键开始一次全新的计算
    if (m_error || m_resultShown)
        resetAll();

    if (m_waitingForSecond) {
        m_currentInput = digit;
        m_waitingForSecond = false;
    } else if (m_currentInput == QStringLiteral("0")) {
        m_currentInput = digit;
    } else if (m_currentInput == QStringLiteral("-0")) {
        m_currentInput = QStringLiteral("-") + digit;
    } else {
        m_currentInput += digit;
    }

    updateExpressionWhileTyping();
    updateDisplay();
}

void Calculator::handleDot()
{
    if (m_error || m_resultShown)
        resetAll();

    if (m_waitingForSecond) {
        m_currentInput = QStringLiteral("0.");
        m_waitingForSecond = false;
    } else if (!m_currentInput.contains(QLatin1Char('.'))) {
        m_currentInput += QLatin1Char('.');
    }
    // 已含小数点时直接忽略，保证 1..2 只保留一个点

    updateExpressionWhileTyping();
    updateDisplay();
}

void Calculator::handleOperator(Operator op)
{
    if (m_error)
        return;  // 错误状态下先按 C / CE 或直接输入数字恢复

    if (m_pendingOp == Operator::None) {
        // 第一次按运算符：把当前输入记为第一操作数
        m_firstOperand = m_currentInput.toDouble();
        m_pendingOp = op;
        m_waitingForSecond = true;
        m_resultShown = false;
    } else if (m_waitingForSecond) {
        // 连续运算符：只替换，不做运算（2++3 -> 2+3）
        m_pendingOp = op;
    } else {
        // 连续运算：先算出前面的结果（2+3+4，按第二个 + 时先算 2+3）
        bool ok = true;
        const double result =
            applyOperator(m_firstOperand, m_currentInput.toDouble(), m_pendingOp, &ok);
        if (!ok) {
            enterErrorState(formatNumber(m_firstOperand) + QLatin1Char(' ')
                            + opText(m_pendingOp) + QLatin1Char(' ')
                            + formatNumber(m_currentInput.toDouble()) + QStringLiteral(" ="));
            return;
        }
        m_firstOperand = result;
        m_currentInput = formatNumber(result);
        m_pendingOp = op;
        m_waitingForSecond = true;
        m_resultShown = false;
    }

    updateExpressionWhileTyping();
    updateDisplay();
}

void Calculator::handleEqual()
{
    if (m_error)
        return;

    if (m_pendingOp == Operator::None) {
        // 没有待执行的运算：当前输入就是结果
        m_expressionText = formatNumber(m_currentInput.toDouble()) + QStringLiteral(" =");
        m_currentInput = formatNumber(m_currentInput.toDouble());
        m_resultShown = true;
        updateDisplay();
        return;
    }

    // 例如 2 +=，用第一操作数与自身运算（与常见计算器一致）
    const double second = m_waitingForSecond ? m_firstOperand : m_currentInput.toDouble();
    const QString expression =
        formatNumber(m_firstOperand) + QLatin1Char(' ') + opText(m_pendingOp)
        + QLatin1Char(' ') + formatNumber(second) + QStringLiteral(" =");

    bool ok = true;
    const double result = applyOperator(m_firstOperand, second, m_pendingOp, &ok);
    if (!ok) {
        enterErrorState(expression);
        return;
    }

    m_expressionText = expression;
    m_currentInput = formatNumber(result);
    m_firstOperand = result;
    m_pendingOp = Operator::None;
    m_waitingForSecond = false;
    m_resultShown = true;
    m_error = false;
    updateDisplay();
}

void Calculator::handleBackspace()
{
    if (m_error)
        return;

    if (m_resultShown) {
        resetAll();  // 结果状态下退格 -> 重新开始输入
        return;
    }

    if (m_waitingForSecond)
        return;  // 还没有输入第二操作数

    if (!m_currentInput.isEmpty())
        m_currentInput.chop(1);

    if (m_currentInput.isEmpty() || m_currentInput == QStringLiteral("-"))
        m_currentInput = QStringLiteral("0");

    updateExpressionWhileTyping();
    updateDisplay();
}

void Calculator::handleClear()
{
    resetAll();
}

void Calculator::handleClearEntry()
{
    if (m_error || m_resultShown) {
        resetAll();
        return;
    }

    m_currentInput = QStringLiteral("0");
    updateExpressionWhileTyping();
    updateDisplay();
}

void Calculator::handleToggleSign()
{
    if (m_error)
        return;

    if (m_resultShown) {
        // 以结果作为新的输入继续编辑
        m_resultShown = false;
        m_pendingOp = Operator::None;
        m_waitingForSecond = false;
        m_firstOperand = 0.0;
        m_expressionText.clear();
    } else if (m_waitingForSecond) {
        m_currentInput = QStringLiteral("0");
        m_waitingForSecond = false;
    }

    if (m_currentInput.startsWith(QLatin1Char('-')))
        m_currentInput.remove(0, 1);
    else
        m_currentInput.prepend(QLatin1Char('-'));

    if (m_currentInput == QStringLiteral("-"))
        m_currentInput = QStringLiteral("0");

    updateExpressionWhileTyping();
    updateDisplay();
}

// ---------------------------------------------------------------- 内部辅助

void Calculator::resetAll()
{
    m_currentInput = QStringLiteral("0");
    m_firstOperand = 0.0;
    m_pendingOp = Operator::None;
    m_waitingForSecond = false;
    m_resultShown = false;
    m_error = false;
    m_expressionText.clear();
    updateDisplay();
}

void Calculator::updateDisplay()
{
    labelExpression->setText(m_expressionText);
    labelResult->setText(m_error ? tr("除数为0") : m_currentInput);
}

void Calculator::updateExpressionWhileTyping()
{
    if (m_pendingOp == Operator::None) {
        m_expressionText.clear();
    } else if (m_waitingForSecond) {
        m_expressionText =
            formatNumber(m_firstOperand) + QLatin1Char(' ') + opText(m_pendingOp);
    } else {
        m_expressionText = formatNumber(m_firstOperand) + QLatin1Char(' ')
                           + opText(m_pendingOp) + QLatin1Char(' ') + m_currentInput;
    }
}

void Calculator::enterErrorState(const QString &expressionText)
{
    m_error = true;
    m_pendingOp = Operator::None;
    m_waitingForSecond = false;
    m_resultShown = false;
    m_expressionText = expressionText;

    labelExpression->setText(m_expressionText);
    labelResult->setText(tr("除数为0"));
}

double Calculator::applyOperator(double lhs, double rhs, Operator op, bool *ok)
{
    *ok = true;
    switch (op) {
    case Operator::Add:
        return lhs + rhs;
    case Operator::Subtract:
        return lhs - rhs;
    case Operator::Multiply:
        return lhs * rhs;
    case Operator::Divide:
        if (rhs == 0.0) {
            *ok = false;
            return 0.0;
        }
        return lhs / rhs;
    case Operator::None:
        break;
    }
    return rhs;
}

QString Calculator::formatNumber(double value)
{
    if (!std::isfinite(value))
        return QStringLiteral("0");
    if (value == 0.0)
        return QStringLiteral("0");
    return QString::number(value, 'g', 15);
}

QString Calculator::opText(Operator op)
{
    switch (op) {
    case Operator::Add:
        return QStringLiteral("+");
    case Operator::Subtract:
        return QStringLiteral("-");
    case Operator::Multiply:
        return QStringLiteral("*");
    case Operator::Divide:
        return QStringLiteral("/");
    case Operator::None:
        break;
    }
    return QString();
}
