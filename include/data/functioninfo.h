#ifndef FUNCTIONINFO_H
#define FUNCTIONINFO_H

#include <QString>

struct FunctionInfo
{
    QString dllName;
    QString functionName;
    bool isImport = false;
    bool isExport = false;
    bool isForwarded = false;
    QString forwardInfo; // SYMKRP.DLL.SomeFunction

    // Imports
    uint64_t iatAddress = 0;
    uint64_t iatValue = 0;
    uint64_t iltValue = 0;
    bool isOrdinal = false;
    uint16_t hint = 0;
    uint64_t hintNameRVA = 0;
    uint64_t entryData = 0;
    uint16_t importOrdinal = 0;

    // Exports
    uint64_t functionRVA = 0;
    uint64_t address = 0;
    bool isExtern = false;
    uint16_t exportOrdinal = 0;
};

#endif // FUNCTIONINFO_H
