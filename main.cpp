#include <QCoreApplication>
#include <QTextStream>
#include <QStringList>
#include <QFile>
#include <QDebug>

#include "calculator.h"
#include "commandparser.h"

static void appendToHistory(const QString &text)
{
    QFile file(QStringLiteral("history.txt"));
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream ts(&file);
        ts << text << '\n';
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("MetaCalculator"));

    QTextStream in(stdin);
    QTextStream out(stdout);

    Calculator calc;
    CommandParser parser;

    QObject::connect(&calc, &Calculator::resultReady,
                     [&out](double result) {
                         out << "Результат: " << result << '\n';
                         out.flush();
                         appendToHistory(QStringLiteral("Result: %1").arg(result));
                     });

    QObject::connect(&calc, &Calculator::errorOccurred,
                     [&out](const QString &msg) {
                         out << "Ошибка: " << msg << '\n';
                         out.flush();
                         appendToHistory(QStringLiteral("Error: %1").arg(msg));
                     });

    QObject::connect(&calc, &Calculator::operationInvoked,
                     [](const QString &op, double a, double b) {
                         qDebug() << "[META] invokeMethod ->" << op
                                  << "с аргументами" << a << "," << b;
                     });

    QObject::connect(&parser, &CommandParser::parseError,
                     [&out](const QString &msg) {
                         out << "Ошибка ввода: " << msg << '\n';
                         out.flush();
                     });

    out << CommandParser::helpText() << '\n';
    out << "> ";
    out.flush();

    QString line;
    while (in.readLineInto(&line)) {
        line = line.trimmed();
        if (line.isEmpty()) {
            out << "> ";
            out.flush();
            continue;
        }

        QString command;
        QList<double> args;

        if (!parser.parse(line, command, args)) {
            out << "> ";
            out.flush();
            continue;
        }

        if (command == "quit" || command == "exit") {
            out << "До свидания!\n";
            break;
        }
        if (command == "help") {
            out << CommandParser::helpText() << '\n';
            out << "> ";
            out.flush();
            continue;
        }
        if (command == "reset") {
            calc.reset();
            out << "> ";
            out.flush();
            continue;
        }

        const double a = args.value(0);
        const double b = args.value(1);
        calc.invokeByName(command, a, b);

        out << "> ";
        out.flush();
    }

    return 0;
}