#ifndef DISASSEMBLYDATA_H
#define DISASSEMBLYDATA_H
#include <QLIst>
#include <QString>

struct DisassemblyData
{
    uint64_t address;
    QByteArray bytes;
    QString mnemonic;
    QString operands;
    QString comment;
};

#endif // DISASSEMBLYDATA_H
