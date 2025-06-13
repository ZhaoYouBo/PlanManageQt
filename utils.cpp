#include "utils.h"
#include <QStringList>

Utils::Utils()
{

}

QString Utils::taskStatusToString(int status)
{
    QStringList list = taskStatusList();
    if (status >= 0 && status < list.size()) {
        return list[status];
    }
    return "未知状态";
}

QString Utils::habitStatusToString(int status)
{
    QStringList list = habitStatusList();
    if (status >= 0 && status < list.size()) {
        return list[status];
    }
    return "未知状态";
}

QString Utils::planStatusToString(int status)
{
    QStringList list = planStatusList();
    if (status >= 0 && status < list.size()) {
        return list[status];
    }
    return "未知状态";
}

QStringList Utils::taskStatusList()
{
    return {"进行中", "已完成", "未完成", "超时完成", "已取消"};
}

QStringList Utils::habitStatusList()
{
    return {"进行中", "已完成", "已取消"};
}

QStringList Utils::habitFrequencyList()
{
    return {"每日一次", "每二日一次", "每三日一次", "每周周五", "每周周六"};
}

QStringList Utils::planStatusList()
{
    return {"进行中", "已完成", "未完成"};
}

int Utils::taskStatusFromString(const QString &str)
{
    QStringList list = taskStatusList();
    int idx = list.indexOf(str);
    return idx >= 0 ? idx : -1;
}

int Utils::habitStatusFromString(const QString &str)
{
    QStringList list = habitStatusList();
    int idx = list.indexOf(str);
    return idx >= 0 ? idx : -1;
}

int Utils::planStatusFromString(const QString &str)
{
    QStringList list = planStatusList();
    int idx = list.indexOf(str);
    return idx >= 0 ? idx : -1;
}
