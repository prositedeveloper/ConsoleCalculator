#include "calculator.h"
#include <QDebug>

Calculator::Calculator(QObject* parent)
    : QObject(parent), m_result(0.0), m_hasError(false)
{
    qDebug() << "Calculator created";
}

void Calculator::setResult(double value) {
    m_result = value;
    m_hasError = false;
    m_errorMessage.clear();
    emit resultReady(m_result);
}

void Calculator::reportError(const QString &message)
{
    m_hasError = true;
    m_errorMessage = message;
    emit errorOccurred(m_errorMessage);
}

void Calculator::add(double a, double b) {
    qDebug() << "add(" << a << "," << b << ")";
    setResult(a + b);
}

void Calculator::subtract(double a, double b) {
    qDebug() << "subtract(" << a << "," << b << ")";
    setResult(a - b);
}

void Calculator::multiply(double a, double b) {
    qDebug() << "multiply(" << a << "," << b << ")";
    setResult(a * b);
}

void Calculator::divide(double a, double b) {
    qDebug() << "divide(" << a << "," << b <<")";

    if (qFuzzyIsNull(b)) {
        m_hasError = true;
        m_errorMessage = "Деление на ноль невозможно";
        emit errorOccurred(m_errorMessage);
        return;
    }
    setResult(a / b);
}

void Calculator::power(double base, double exp)
{
    qDebug() << "[Calculator] power(" << base << "," << exp << ")";
    setResult(qPow(base, exp));
}

void Calculator::sqrt(double a)
{
    qDebug() << "[Calculator] sqrt(" << a << ")";
    if (a < 0.0) {
        reportError(QStringLiteral("Квадратный корень из отрицательного числа!"));
        return;
    }
    setResult(qSqrt(a));
}

void Calculator::modulo(int a, int b)
{
    qDebug() << "[Calculator] modulo(" << a << "," << b << ")";
    if (b == 0) {
        reportError(QStringLiteral("Остаток от деления на ноль невозможен!"));
        return;
    }
    setResult(a % b);
}

void Calculator::reset() {
    qDebug() << "reset()";
    m_result = 0.0;
    m_hasError = false;
    m_errorMessage.clear();
    emit resultReady(m_result);
}

bool Calculator::invokeByName(const QString &opName, double a, double b)
{
    static const QHash<QString, QString> opMap = {
        { "add",  "add"      },
        { "sub",  "subtract" },
        { "mul",  "multiply" },
        { "div",  "divide"   },
        { "pow",  "power"    },
        { "mod",  "modulo"   },
        { "sqrt", "sqrt"     },
    };

    const QString methodName = opMap.value(opName.toLower());
    if (methodName.isEmpty()) {
        emit errorOccurred(QStringLiteral("Неизвестная операция: %1").arg(opName));
        return false;
    }

    emit operationInvoked(methodName, a, b);

    bool ok = QMetaObject::invokeMethod(
        this,
        methodName.toUtf8().constData(),
        Qt::DirectConnection,
        Q_ARG(double, a),
        Q_ARG(double, b)
        );

    if (!ok) {
        if (methodName == "modulo") {
            ok = QMetaObject::invokeMethod(
                this, "modulo", Qt::DirectConnection,
                Q_ARG(int, static_cast<int>(a)),
                Q_ARG(int, static_cast<int>(b))
                );
        }
    }

    if (!ok) {
        if (methodName == "sqrt") {
            ok = QMetaObject::invokeMethod(
                this, "sqrt", Qt::DirectConnection,
                Q_ARG(double, a)
                );
        }
    }

    if (!ok) {
        emit errorOccurred(
            QStringLiteral("Не удалось вызвать метод '%1' через метасистему")
                .arg(methodName));
    }
    return ok;
}

