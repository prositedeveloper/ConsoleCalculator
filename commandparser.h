#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>

/**
 * @brief Парсер команд пользователя.
 *
 * Отвечает за разбор строки "add 5 3" на команду и операнды,
 * а также за вывод справки.
 */
class CommandParser : public QObject
{
    Q_OBJECT

public:
    explicit CommandParser(QObject *parent = nullptr);

    bool parse(const QString &line, QString &command, QList<double> &args);

    static QString helpText();

    static QStringList supportedCommands();

signals:
    void parseError(const QString &message);

private:
    static const QHash<QString, int> s_arity;
};

#endif // COMMANDPARSER_H