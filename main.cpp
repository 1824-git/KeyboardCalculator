#include "calculator.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("CalculatorApp"));
    QApplication::setApplicationVersion(QStringLiteral("1.0"));

    Calculator calculator;
    calculator.show();

    return app.exec();
}
