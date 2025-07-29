#ifndef FUNCTIONINFO_H
#define FUNCTIONINFO_H

#include <QString>

struct FunctionInfo
{
    QString name;                 // Function name (if available)
    uint32_t ordinal = 0;         // Ordinal number of the function
    uint32_t rva = 0;             // Relative Virtual Address of the function
    uint32_t thunk_rva = 0;       // RVA of the thunk (usually in Import Address Table)
    bool isForwarded = false;     // Indicates if the function is forwarded to another DLL
    QString forwarder;            // Forwarder string (name of the forwarded function, if any)
    bool importByOrdinal = false; // True if the import is done by ordinal (no name)
    uint16_t hint;                // Index hint to speed up function lookup by name
    QString dllName;              // Name of the DLL (for imported functions)
};

#endif // FUNCTIONINFO_H
