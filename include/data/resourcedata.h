#ifndef RESOURCEDATA_H
#define RESOURCEDATA_H
#include <QString>

struct ResourceTreeEntry
{
    QString type;   // "DIR" or "DATA"
    QString idName; // "ICON", "ID=1"
    QString lang;   // "Lang=1033"
    QString size;   // "512 bytes"
};

#endif // RESOURCEDATA_H
