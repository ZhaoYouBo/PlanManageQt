#include "utils.h"
#include <QStringList>
#include <QMap>

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

const QMap<QString, QColor>& Utils::statusColorMap()
{
    static const QMap<QString, QColor> map = {
        {"进行中", QColor(0, 122, 204)},
        {"已完成", QColor(50, 205, 50)},
        {"未完成", QColor(255, 87, 34)},
        {"超时完成", QColor(255, 165, 0)},
        {"已取消", QColor(128, 128, 128)}
    };
    return map;
}
