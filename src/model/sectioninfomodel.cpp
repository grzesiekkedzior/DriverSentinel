#include "model/sectioninfomodel.h"

SectionInfoModel::SectionInfoModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int SectionInfoModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_sectionInfo.size();
}

int SectionInfoModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(SectionColumn::Count);
}

QVariant SectionInfoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const SectionInfo &sectionInfo = m_sectionInfo.at(index.row());
    auto column = static_cast<SectionColumn>(index.column());

    switch (column) {
    case SectionColumn::Name:
        return sectionInfo.name;
    case SectionColumn::VirtualAddress:
        return sectionInfo.virtualAddress;
    case SectionColumn::VirtualSize:
        return sectionInfo.virtualSize;
    case SectionColumn::RawSize:
        return sectionInfo.rawSize;
    case SectionColumn::PointerToRawData:
        return sectionInfo.pointerToRawData;
    case SectionColumn::Characteristics:
        return sectionInfo.characteristics;
    case SectionColumn::Entropy:
        return sectionInfo.entropy;
    case SectionColumn::SectionAlignment:
        return sectionInfo.sectionAlignment;
    case SectionColumn::PointerToRelocations:
        return sectionInfo.pointerToRelocations;
    case SectionColumn::NumberOfRelocations:
        return sectionInfo.numberOfRelocations;
    case SectionColumn::CharacteristicsDesc:
        return sectionInfo.characteristicsDesc;
    case SectionColumn::Count:
        return {};
    }

    return {};
}

QVariant SectionInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (static_cast<SectionColumn>(section)) {
    case SectionColumn::Name:
        return "Name";
    case SectionColumn::VirtualAddress:
        return "Virtual Address";
    case SectionColumn::VirtualSize:
        return "Virtual Size";
    case SectionColumn::RawSize:
        return "Raw Size";
    case SectionColumn::PointerToRawData:
        return "Pointer to Raw Data";
    case SectionColumn::Characteristics:
        return "Characteristics";
    case SectionColumn::Entropy:
        return "Entropy";
    case SectionColumn::SectionAlignment:
        return "Section Alignment";
    case SectionColumn::PointerToRelocations:
        return "Pointer to Relocations";
    case SectionColumn::NumberOfRelocations:
        return "Number of Relocations";
    case SectionColumn::CharacteristicsDesc:
        return "Characteristics (desc)";
    case SectionColumn::Count:
        return {};
    }

    return {};
}

void SectionInfoModel::setPESecton(const QVector<SectionInfo> &sectionInfo)
{
    beginResetModel();
    m_sectionInfo = sectionInfo;
    endResetModel();
}

void SectionInfoModel::clear()
{
    beginResetModel();
    m_sectionInfo.clear();
    endResetModel();
}
