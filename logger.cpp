#include "logger.h"

Logger::Logger(QObject *parent, QPlainTextEdit *plainTextEdit) :
    QObject(parent),
    m_plainTextEdit(plainTextEdit)
{

}

void Logger::writeLog(const QString &log) {
    if (m_plainTextEdit != 0) {
        m_plainTextEdit->appendPlainText(log);
    }
}
