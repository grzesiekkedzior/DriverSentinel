#ifndef GENERALINFO_H
#define GENERALINFO_H
#include <QDateTime>
#include <QLocale>

struct GeneralInfo
{
    QString name;
    QString path;
    quint64 baseAddress;
    quint64 imageSize;
    QString checksum;
    QString fileVersion;
    QString productVersion;
    QDateTime compileTime;
    QString description;
    QString companyName;
    QString originalFilename;
};

#endif // GENERALINFO_H
