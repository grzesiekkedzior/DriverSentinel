#ifndef DOSHEADER_H
#define DOSHEADER_H
#include <QtGlobal>
#include <array>

/**
 * @brief The DosHeader class
 * typedef struct _IMAGE_DOS_HEADER {      // DOS .EXE header
    WORD e_magic;       // Magic number
    WORD e_cblp;        // Bytes on last page of file
    WORD e_cp;          // Pages in file
    WORD e_crlc;        // Relocations
    WORD e_cparhdr;     // Size of header in paragraphs
    WORD e_minalloc;    // Minimum extra paragraphs needed
    WORD e_maxalloc;    // Maximum extra paragraphs needed
    WORD e_ss;          // Initial (relative) SS value
    WORD e_sp;          // Initial SP value
    WORD e_csum;        // Checksum
    WORD e_ip;          // Initial IP value
    WORD e_cs;          // Initial (relative) CS value
    WORD e_lfarlc;      // File address of relocation table
    WORD e_ovno;        // Overlay number
    WORD e_res[4];      // Reserved words
    WORD e_oemid;       // OEM identifier (for e_oeminfo)
    WORD e_oeminfo;     // OEM information; e_oemid specific
    WORD e_res2[10];    // Reserved words
    LONG e_lfanew;      // File address of new exe header
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

 */

struct DosHeader
{
    quint16 magic{0};
    quint16 usedBytesInLastPage{0};
    quint16 fileSizeInPages{0};
    quint16 numberOfRelocations{0};
    quint16 headerSizeInParagraphs{0};
    quint16 minExtraParagraphs{0};
    quint16 maxExtraParagraphs{0};
    quint16 initialRelativeSs{0};
    quint16 initialSp{0};
    quint16 checksum{0};
    quint16 initialIp{0};
    quint16 initialRelativeCs{0};
    quint16 addressRelocationTable{0};
    quint16 overlayNumber{0};
    std::array<quint16, 4> reserved{{0, 0, 0, 0}};
    quint16 oemId{0};
    quint16 oemInfo{0};
    std::array<quint16, 10> reserved2{{0}};
    quint32 addressNewExeHeader{0};
};
#endif // DOSHEADER_H
