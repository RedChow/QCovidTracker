#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QPlainTextEdit>

class Logger : public QObject
{
    Q_OBJECT
public:
    Logger(QObject *parent, QPlainTextEdit *plainTextEdit = 0);
    void writeLog(const QString &log);
private:
    QPlainTextEdit *m_plainTextEdit;
};

#endif // LOGGER_H
