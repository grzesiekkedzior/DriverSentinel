#ifndef RESOURCEDATA_H
#define RESOURCEDATA_H
#include <QString>

#include <LIEF/PE/ResourceData.hpp>

struct ResourceTreeEntry
{
    QString type;   // "DIR" or "DATA"
    QString idName; // "ICON", "ID=1"
    QString lang;   // "Lang=1033"
    QString size;   // "512 bytes"
    std::vector<uint8_t> rawContent;
};

#endif // RESOURCEDATA_H
