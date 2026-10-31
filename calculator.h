#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QString>
#include <QWidget>

class QGridLayout;
class QLabel;

class Calculator : public QWidget
{
    Q_OBJECT

public:
    // 待执行的操作符
    enum class Operator {
        None,
        Add,
        Subtract,
        Multiply,
        Divide
    };

    explicit Calculator(QWidget *parent = nullptr);
    ~Calculator() override;

private slots:
    // ---- 鼠标点击触发的槽函数 ----
    void handleDigit(const QString &digit);
    void handleDot();
    void handleOperator(Calculator::Operator op);
    void handleEqual();
    void handleBackspace();
    void handleClear();       // C  ：清除全部
    void handleClearEntry();  // CE ：清除当前输入
    void handleToggleSign();  // +/-：切换正负号

private:
    void buildUi();
    void applyStyles();

    void resetAll();
    void updateDisplay();
    void updateExpressionWhileTyping();
    void enterErrorState(const QString &expressionText);

    static double applyOperator(double lhs, double rhs, Operator op, bool *ok);
    static QString formatNumber(double value);
    static QString opText(Operator op);

    QLabel *labelExpression = nullptr;
    QLabel *labelResult = nullptr;

    // ---- 计算器状态 ----
    QString m_currentInput = QStringLiteral("0");  // 当前输入（保留原始文本，便于退格编辑）
    double m_firstOperand = 0.0;                   // 第一操作数
    Operator m_pendingOp = Operator::None;         // 待执行操作符
    bool m_waitingForSecond = false;               // 是否等待第二操作数
    bool m_resultShown = false;                    // 结果已显示
    bool m_error = false;                          // 错误状态（如除数为 0）

    QString m_expressionText;                      // 表达式区文本
};

#endif // CALCULATOR_H
