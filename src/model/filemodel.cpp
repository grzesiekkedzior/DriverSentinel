#include "model/filemodel.h"

FileModel::FileModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int FileModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(FileInfoColumn::ColumnCount);
}

int FileModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2; // Field | Value
}

QVariant FileModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    if (m_fileInfo.isEmpty())
        return {};

    const FileInfo &fi = m_fileInfo.first();
    auto row = static_cast<FileInfoColumn>(index.row());

    if (index.column() == 0) { // Field
        switch (row) {
        case FileInfoColumn::Machine:
            return "Machine";
        case FileInfoColumn::NumberOfSections:
            return "Number Of Sections";
        case FileInfoColumn::TimeDateStamp:
            return "Time Date Stamp";
        case FileInfoColumn::PointerToSymbolTable:
            return "Pointer To Symbol Table";
        case FileInfoColumn::NumberOfSymbols:
            return "Number Of Symbols";
        case FileInfoColumn::SizeOfOptionalHeader:
            return "Size Of Optional Header";
        case FileInfoColumn::Characteristics:
            return "Characteristics";
        default:
            return {};
        }
    } else { // Value
        switch (row) {
        case FileInfoColumn::Machine:
            return machineToString(fi.Machine);
        case FileInfoColumn::NumberOfSections:
            return fi.NumberOfSections;
        case FileInfoColumn::TimeDateStamp:
            return QString("0x%1").arg(fi.TimeDateStamp, 8, 16, QLatin1Char('0'));
        case FileInfoColumn::PointerToSymbolTable:
            return QString("0x%1").arg(fi.PointerToSymbolTable, 8, 16, QLatin1Char('0'));
        case FileInfoColumn::NumberOfSymbols:
            return fi.NumberOfSymbols;
        case FileInfoColumn::SizeOfOptionalHeader:
            return fi.SizeOfOptionalHeader;
        case FileInfoColumn::Characteristics:
            return characteristicsToStrings(fi.Characteristics).join(", ");
        default:
            return {};
        }
    }
}

QVariant FileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal) {
        return (section == 0) ? "Field" : "Value";
    } else {
        return section + 1;
    }
}

void FileModel::loadFileHeaderDataToView(const QVector<FileInfo> &fh)
{
    beginResetModel();
    m_fileInfo = fh;
    endResetModel();
}

void FileModel::clear()
{
    beginResetModel();
    m_fileInfo.clear();
    endResetModel();
}

QString FileModel::machineToString(quint16 machine)
{
    switch (machine) {
    case 0x014c:
        return "Intel 386";
    case 0x8664:
        return "x64";
    case 0x01c0:
        return "ARM";
    case 0xaa64:
        return "ARM64";
    case 0x0200:
        return "Intel Itanium";
    default:
        return QString("Unknown (0x%1)").arg(machine, 4, 16, QLatin1Char('0'));
    }
}

QStringList FileModel::characteristicsToStrings(quint16 characteristics)
{
    QStringList list;

    auto addIf = [&](quint16 flag, const char *name) {
        if (characteristics & flag)
            list << name;
    };

    addIf(0x0001, "RelocsStripped");
    addIf(0x0002, "ExecutableImage");
    addIf(0x0004, "LineNumsStripped");
    addIf(0x0008, "LocalSymsStripped");
    addIf(0x0010, "AggressiveWsTrim");
    addIf(0x0020, "LargeAddressAware");
    addIf(0x0080, "BytesReversedLo");
    addIf(0x0100, "Machine32Bit");
    addIf(0x0200, "DebugStripped");
    addIf(0x0400, "RemovableRunFromSwap");
    addIf(0x0800, "NetRunFromSwap");
    addIf(0x1000, "System");
    addIf(0x2000, "Dll");
    addIf(0x4000, "UpSystemOnly");
    addIf(0x8000, "BytesReversedHi");

    return list;
}
