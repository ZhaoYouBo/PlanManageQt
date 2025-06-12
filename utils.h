#ifndef UTILS_H
#define UTILS_H
#include <QStringList>

class Utils
{
public:
    Utils();

public:
    static QString taskStatusToString(int status);
    static QString habitStatusToString(int status);
    static QStringList taskStatusList();
    static QStringList habitStatusList();
    static QStringList habitFrequencyList();
    static int taskStatusFromString(const QString &str);
    static int habitStatusFromString(const QString &str);
};

#endif // UTILS_H
