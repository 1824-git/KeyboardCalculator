#include "calculator.h"

#include <QLabel>
#include <QVBoxLayout>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    buildUi();

    setWindowTitle(tr("计算器"));
    resize(380, 560);
}

Calculator::~Calculator() = default;

void Calculator::buildUi()
{
    labelExpression = new QLabel(this);
    labelExpression->setObjectName(QStringLiteral("labelExpression"));
    labelExpression->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    labelExpression->setMinimumHeight(32);

    labelResult = new QLabel(QStringLiteral("0"), this);
    labelResult->setObjectName(QStringLiteral("labelResult"));
    labelResult->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    labelResult->setMinimumHeight(72);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(6);
    layout->addWidget(labelExpression);
    layout->addWidget(labelResult);
    layout->addStretch(1);
}
