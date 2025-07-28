#ifndef TREEIMPORTSITEM_H
#define TREEIMPORTSITEM_H
#include <QVariant>
#include <memory>
#include <vector>

/**This is generated class from Qt tutorial*/
class TreeImportsItem
{
public:
    TreeImportsItem();
    explicit TreeImportsItem(const QVariantList &data, TreeImportsItem *parentItem = nullptr);

    void appendChild(std::unique_ptr<TreeImportsItem> child);
    TreeImportsItem *child(int row) const;
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    TreeImportsItem *parent() const;

private:
    QVariantList m_itemData;
    std::vector<std::unique_ptr<TreeImportsItem>> m_childItems;
    TreeImportsItem *m_parentItem;
};
#endif // TREEIMPORTSITEM_H
