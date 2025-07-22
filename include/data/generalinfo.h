#ifndef GENERALINFO_H
#define GENERALINFO_H
#include <QDateTime>
#include <QLocale>

struct GeneralInfo
{
    QString internalName;
    QString originalFilename;
    QString productName;
    QString legalCopyright;
    QString path;
    quint64 baseAddress;
    quint64 virtualSize;
    QString imageSize;
    QString checksum;
    QString productVersion;
    QString fileDescription;
    QString fileType;
    QString fileExtention;
    QString companyName;
};

#endif // GENERALINFO_H
