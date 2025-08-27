#ifndef OPTIONALHEADERINFO_H
#define OPTIONALHEADERINFO_H
#include <QString>
#include <QtGlobal>
#include <LIEF/PE.hpp>

struct OptionalHeaderInfo
{
    quint16 magic = 0;
    quint8 majorLinkerVersion = 0;
    quint8 minorLinkerVersion = 0;
    quint32 sizeOfCode = 0;
    quint32 sizeOfInitializedData = 0;
    quint32 sizeOfUninitializedData = 0;
    quint32 entryPoint = 0;
    quint32 baseOfCode = 0;
    quint32 baseOfData = 0;
    quint64 imageBase = 0;
    quint32 sectionAlign = 0;
    quint32 fileAlign = 0;
    quint16 majorOSVersion = 0;
    quint16 minorOSVersion = 0;
    quint16 majorImageVersion = 0;
    quint16 minorImageVersion = 0;
    quint16 majorSubsystemVersion = 0;
    quint16 minorSubsystemVersion = 0;
    quint32 win32VersionValue = 0;
    quint32 sizeOfImage = 0;
    quint32 sizeOfHeaders = 0;
    quint32 checksum = 0;
    LIEF::PE::OptionalHeader::SUBSYSTEM subsystem;
    quint32 dllCharacteristics = 0;
    quint64 sizeOfStackReserve = 0;
    quint64 sizeOfStackCommit = 0;
    quint64 sizeOfHeapReserve = 0;
    quint64 sizeOfHeapCommit = 0;
    quint32 loaderFlags = 0;
    quint32 numberOfRvaAndSizes = 0;
};
#endif // OPTIONALHEADERINFO_H
