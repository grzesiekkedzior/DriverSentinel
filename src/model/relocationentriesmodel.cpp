#include "model/relocationentriesmodel.h"
#include <QString>

RelocationEntriesModel::RelocationEntriesModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int RelocationEntriesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_entries.size();
}

int RelocationEntriesModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return static_cast<int>(Column::COLUMN_COUNT);
}

QVariant RelocationEntriesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const auto &entry = m_entries.at(index.row());

    switch (static_cast<Column>(index.column())) {
    case Column::OffsetInPage:
        return QString("0x%1").arg(entry.offsetInPage, 0, 16).toUpper();
    case Column::RelocRVA:
        return QString("0x%1").arg(entry.relocRVA, 0, 16).toUpper();
    case Column::Type:
        return entry.type;
    case Column::Value:
        return QString("0x%1").arg(entry.value, 0, 16).toUpper();
    default:
        return {};
    }
}

QVariant RelocationEntriesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal) {
        switch (static_cast<Column>(section)) {
        case Column::OffsetInPage:
            return QStringLiteral("Offset in Page");
        case Column::RelocRVA:
            return QStringLiteral("Reloc RVA");
        case Column::Type:
            return QStringLiteral("Type");
        case Column::Value:
            return QStringLiteral("Value");
        default:
            return {};
        }
    }

    return {};
}

void RelocationEntriesModel::setRelocationEntriesToView(
    const QVector<LIEF_RELOCATION::RelocationEntryInfo> &entries)
{
    beginResetModel();
    m_entries = entries;
    endResetModel();
}

void RelocationEntriesModel::clear()
{
    beginResetModel();
    m_entries.clear();
    endResetModel();
}
