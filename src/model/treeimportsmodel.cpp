#include "model/treeimportsmodel.h"

TreeImportsModel::TreeImportsModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_rootItem(std::make_unique<TreeImportsItem>(QVariantList{"Root"}))
{}

QModelIndex TreeImportsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeImportsItem *parentItem = parent.isValid()
                                      ? static_cast<TreeImportsItem *>(parent.internalPointer())
                                      : m_rootItem.get();

    TreeImportsItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex TreeImportsModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeImportsItem *childItem = static_cast<TreeImportsItem *>(index.internalPointer());
    TreeImportsItem *parentItem = childItem->parent();

    if (parentItem == m_rootItem.get())
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeImportsModel::rowCount(const QModelIndex &parent) const
{
    if (!m_rootItem)
        return 0;

    TreeImportsItem *parentItem = parent.isValid()
                                      ? static_cast<TreeImportsItem *>(parent.internalPointer())
                                      : m_rootItem.get();

    return parentItem ? parentItem->childCount() : 0;
}

int TreeImportsModel::columnCount(const QModelIndex &parent) const
{
    // For now flat model
    Q_UNUSED(parent);
    return m_rootItem ? m_rootItem->columnCount() : 0;
}

QVariant TreeImportsModel::data(const QModelIndex &index, int role) const
{
    if (!m_rootItem || !index.isValid() || role != Qt::DisplayRole)
        return {};

    TreeImportsItem *item = static_cast<TreeImportsItem *>(index.internalPointer());
    return item ? item->data(index.column()) : QVariant{};
}

Qt::ItemFlags TreeImportsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void TreeImportsModel::setRootItem(std::unique_ptr<TreeImportsItem> root)
{
    beginResetModel();
    m_rootItem = std::move(root);
    endResetModel();
}
