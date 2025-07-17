#ifndef DRIVERINFO_H
#define DRIVERINFO_H

#include <QString>

struct DriverInfo
{
    QString name{};
    QString path{};
    quint64 baseAddress{};
};

#endif // DRIVERINFO_H
