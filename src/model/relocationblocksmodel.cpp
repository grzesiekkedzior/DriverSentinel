#include "model/relocationblocksmodel.h"
#include <QString>

RelocationBlocksModel::RelocationBlocksModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int RelocationBlocksModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_relocationInfo.size();
}

int RelocationBlocksModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(Column::COLUMN_COUNT);
}

QVariant RelocationBlocksModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const auto &block = m_relocationInfo.at(index.row());

    switch (static_cast<Column>(index.column())) {
    case Column::PageRVA:
        return QString("0x%1").arg(block.pageRVA, 0, 16).toUpper().replace('X', 'x');
    case Column::BlockSize:
        return QString("0x%1").arg(block.blockSize, 0, 16).toUpper().replace('X', 'x');
    case Column::EntriesCount:
        return static_cast<qulonglong>(block.entriesCount);
    case Column::COLUMN_COUNT:
        return {};
    }

    return {};
}

QVariant RelocationBlocksModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal) {
        switch (static_cast<Column>(section)) {
        case Column::PageRVA:
            return QStringLiteral("Page RVA");
        case Column::BlockSize:
            return QStringLiteral("Block Size");
        case Column::EntriesCount:
            return QStringLiteral("Entries Count");
        case Column::COLUMN_COUNT:
            return {};
        }
    }

    return {};
}

void RelocationBlocksModel::setRelocationBlocksToView(
    const QVector<LIEF_RELOCATION::RelocationBlockInfo> &rbi)
{
    beginResetModel();
    m_relocationInfo = rbi;
    endResetModel();
}

void RelocationBlocksModel::clear()
{
    beginResetModel();
    m_relocationInfo.clear();
    endResetModel();
}
