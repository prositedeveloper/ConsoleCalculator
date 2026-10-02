#include "commandparser.h"
#include <QDebug>

const QHash<QString, int> CommandParser::s_arity = {
    { "add",  2 },
    { "sub",  2 },
    { "mul",  2 },
    { "div",  2 },
    { "pow",  2 },
    { "mod",  2 },
    { "sqrt", 1 },
    };

CommandParser::CommandParser(QObject *parent) : QObject(parent) {}

QStringList CommandParser::supportedCommands()
{
    return { "add", "sub", "mul", "div", "pow", "mod", "sqrt",
            "reset", "help", "quit" };
}

QString CommandParser::helpText()
{
    return QStringLiteral(
        "=== Консольный калькулятор с метапрограммированием ===\n"
        "Доступные команды:\n"
        "  add  <a> <b>   — сложение\n"
        "  sub  <a> <b>   — вычитание\n"
        "  mul  <a> <b>   — умножение\n"
        "  div  <a> <b>   — деление\n"
        "  pow  <a> <b>   — возведение в степень\n"
        "  mod  <a> <b>   — остаток от деления (целые)\n"
        "  sqrt <a>       — квадратный корень\n"
        "  reset          — сброс результата\n"
        "  help           — эта справка\n"
        "  quit / exit    — выход\n");
}

bool CommandParser::parse(const QString &line, QString &command, QList<double> &args)
{
    args.clear();
    const QStringList parts = line.split(' ', Qt::SkipEmptyParts);
    if (parts.isEmpty())
        return false;

    command = parts.value(0).toLower();

    if (command == "reset" || command == "help"
        || command == "quit" || command == "exit") {
        return true;
    }

    if (!s_arity.contains(command)) {
        emit parseError(QStringLiteral("Неизвестная команда: %1").arg(command));
        return false;
    }

    const int need = s_arity.value(command);
    if (parts.size() - 1 != need) {
        emit parseError(QStringLiteral(
                            "Команда '%1' требует %2 аргумент(а), получено %3")
                            .arg(command).arg(need).arg(parts.size() - 1));
        return false;
    }

    bool ok = false;
    for (int i = 0; i < need; ++i) {
        const double v = parts[i + 1].toDouble(&ok);
        if (!ok) {
            emit parseError(QStringLiteral(
                                "Не удалось преобразовать '%1' в число").arg(parts[i + 1]));
            return false;
        }
        args.append(v);
    }
    return true;
}