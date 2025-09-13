#ifndef FILEHEADER_H
#define FILEHEADER_H
#include <QtGlobal>

struct FileInfo
{
    quint16 Machine;
    quint16 NumberOfSections;
    quint32 TimeDateStamp;
    quint32 PointerToSymbolTable;
    quint32 NumberOfSymbols;
    quint16 SizeOfOptionalHeader;
    quint16 Characteristics;
};
#endif // FILEHEADER_H
