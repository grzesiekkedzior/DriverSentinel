#include "model/debuginfomodel.h"

DebugInfoModel::DebugInfoModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int DebugInfoModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_debugInfo.size();
}

int DebugInfoModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(DebugColumn::ColumnCount);
}

QVariant DebugInfoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const DebugInfo &di = m_debugInfo.at(index.row());
    auto column = static_cast<DebugColumn>(index.column());
    switch (column) {
    case DebugColumn::Type:
        return di.type;
    case DebugColumn::Characteristics:
        return QString("0x%1").arg(di.characteristics, 8, 16, QChar('0')).toUpper().replace('X', 'x');
    case DebugColumn::TimeDateStamp:
        return di.timestamp;
    case DebugColumn::MajorVersion:
        return di.majorVersion;
    case DebugColumn::MinorVersion:
        return di.minorVersion;
    case DebugColumn::SizeOfData:
        return di.sizeOfData;
    case DebugColumn::AddressOfRawData:
        return QString("0x%1")
            .arg(di.addressOfRawData, 8, 16, QChar('0'))
            .toUpper()
            .replace('X', 'x');
    case DebugColumn::PointerToRawData:
        return QString("0x%1")
            .arg(di.pointerToRawData, 8, 16, QChar('0'))
            .toUpper()
            .replace('X', 'x');
    case DebugColumn::PayloadButton:
        return "Show";
    default:
        return {};
    }
}

QVariant DebugInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (static_cast<DebugColumn>(section)) {
    case DebugColumn::Type:
        return "Type";
    case DebugColumn::Characteristics:
        return "Characteristics";
    case DebugColumn::TimeDateStamp:
        return "Time Date Stamp";
    case DebugColumn::MajorVersion:
        return "Major Version";
    case DebugColumn::MinorVersion:
        return "Minor Version";
    case DebugColumn::SizeOfData:
        return "SizeOfData";
    case DebugColumn::AddressOfRawData:
        return "Address Of RawData";
    case DebugColumn::PointerToRawData:
        return "Pointer To RawData";
    case DebugColumn::PayloadButton:
        return "Payload";
    default:
        return {};
    }
}

void DebugInfoModel::setDebugDataToView(const QVector<DebugInfo> &di)
{
    beginResetModel();
    m_debugInfo = di;
    endResetModel();
}

void DebugInfoModel::clear()
{
    beginResetModel();
    m_debugInfo.clear();
    endResetModel();
}
