#ifndef SECTIONINFO_H
#define SECTIONINFO_H
#include <QString>
#include <QStringList>

// Section Flags (IMAGE_SECTION_HEADER.Characteristics)
// -----------------------------------------------
// Content Type Flags:
// 0x00000020 (IMAGE_SCN_CNT_CODE)                      → Section contains executable code (.text).
// 0x00000040 (IMAGE_SCN_CNT_INITIALIZED_DATA)          → Section contains initialized data (.data, .rdata).
// 0x00000080 (IMAGE_SCN_CNT_UNINITIALIZED_DATA)        → Section contains uninitialized data (.bss).
//
// Memory Permission Flags:
// 0x20000000 (IMAGE_SCN_MEM_EXECUTE)                   → Section is executable.
// 0x40000000 (IMAGE_SCN_MEM_READ)                      → Section is readable.
// 0x80000000 (IMAGE_SCN_MEM_WRITE)                     → Section is writable.
//
// Other common flags
// 0x02000000 (IMAGE_SCN_MEM_DISCARDABLE)               → Section can be discarded (e.g., .reloc).
// 0x10000000 (IMAGE_SCN_MEM_SHARED)                    → Section is shared across processes.
// 0x00200000 (IMAGE_SCN_LNK_INFO)                      → Section contains COFF linker metadata.
// 0x00000200 (IMAGE_SCN_LNK_REMOVE)                    → Section will not be part of the final image.
// -----------------------------------------------
struct SectionInfo
{
    QString name;             // Section name, e.g. ".text"
    QString virtualAddress;   // Virtual address (RVA), e.g. "0x1000"
    QString virtualSize;      // Virtual size, e.g. "0x2000"
    QString rawSize;          // Raw size in file, e.g. "0x1000"
    QString pointerToRawData; // File offset to raw data, e.g. "0x400"
    QString characteristics;  // Section flags in hex, e.g. "0x60000020"

    // Decode characteristics flags to human-readable form
    // IMAGE_SECTION_HEADER.Characteristics
    QString characteristicsDescription() const
    {
        quint32 flags = characteristics.toUInt(nullptr, 16);
        QStringList flagNames;

        if (flags & 0x00000020)
            flagNames << "CODE";
        if (flags & 0x00000040)
            flagNames << "INITIALIZED_DATA";
        if (flags & 0x00000080)
            flagNames << "UNINITIALIZED_DATA";
        if (flags & 0x20000000)
            flagNames << "EXECUTE";
        if (flags & 0x40000000)
            flagNames << "READ";
        if (flags & 0x80000000)
            flagNames << "WRITE";

        return flagNames.join(", ");
    }
};

#endif // SECTIONINFO_H
