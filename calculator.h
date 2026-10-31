#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>

class QLabel;

// 计算器主窗口：先搭出可运行的空壳，后续逐步补全布局与逻辑
class Calculator : public QWidget
{
    Q_OBJECT

public:
    explicit Calculator(QWidget *parent = nullptr);
    ~Calculator() override;

private:
    void buildUi();

    QLabel *labelExpression = nullptr;
    QLabel *labelResult = nullptr;
};

#endif // CALCULATOR_H
