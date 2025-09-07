#ifndef DEBUGINFO_H
#define DEBUGINFO_H

#include <QString>

struct DebugInfo
{
    QString type;
    quint32 characteristics;
    quint32 timestamp;
    quint16 majorVersion;
    quint16 minorVersion;
    quint32 sizeOfData;
    quint32 addressOfRawData;
    quint32 pointerToRawData;
    QByteArray payload;
};
#endif // DEBUGINFO_H
