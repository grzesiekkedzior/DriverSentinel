#ifndef RELOCATIONINFO_H
#define RELOCATIONINFO_H
#include <QDebug>
#include <QString>
#include <QVector>

namespace LIEF_RELOCATION {
struct RelocationEntryInfo
{
    quint16 offsetInPage;
    quint64 relocRVA;
    QString type;
    quint64 value;
};

struct RelocationBlockInfo
{
    quint32 pageRVA;
    quint32 blockSize;
    qsizetype entriesCount;
    QVector<RelocationEntryInfo> entries;
};

struct RelocationInfo
{
    QVector<RelocationBlockInfo> blocks;
};
} // namespace LIEF_RELOCATION
#endif // RELOCATIONINFO_H
