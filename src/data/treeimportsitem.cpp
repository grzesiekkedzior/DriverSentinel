#include "data/treeimportsitem.h"

TreeImportsItem::TreeImportsItem() {}

TreeImportsItem::TreeImportsItem(const QVariantList &data, TreeImportsItem *parentItem)
    : m_itemData(data)
    , m_parentItem(parentItem)
{}

void TreeImportsItem::appendChild(std::unique_ptr<TreeImportsItem> child)
{
    child->m_parentItem = this;
    m_childItems.push_back(std::move(child));
}

TreeImportsItem *TreeImportsItem::child(int row) const
{
    if (row < 0 || row >= static_cast<int>(m_childItems.size()))
        return nullptr;
    return m_childItems[row].get();
}

int TreeImportsItem::childCount() const
{
    return static_cast<int>(m_childItems.size());
}

int TreeImportsItem::columnCount() const
{
    return static_cast<int>(m_itemData.size());
}

QVariant TreeImportsItem::data(int column) const
{
    if (column < 0 || column >= m_itemData.size())
        return {};
    return m_itemData[column];
}

int TreeImportsItem::row() const
{
    if (!m_parentItem)
        return 0;

    for (int i = 0; i < m_parentItem->childCount(); ++i) {
        if (m_parentItem->child(i) == this)
            return i;
    }

    return 0;
}

TreeImportsItem *TreeImportsItem::parent() const
{
    return m_parentItem;
}
