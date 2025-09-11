#ifndef RESOURCEMODEL_H
#define RESOURCEMODEL_H
#include <QAbstractItemModel>
#include <QVector>
#include "data/resourcedata.h"

class TreeNode
{
public:
    explicit TreeNode(const ResourceTreeEntry &entry, TreeNode *parent = nullptr);
    ~TreeNode();

    void appendChild(TreeNode *child);
    TreeNode *child(int row) const;
    int childCount() const;
    int row() const;
    TreeNode *parent() const;

    const ResourceTreeEntry &entry() const { return m_entry; }

private:
    ResourceTreeEntry m_entry;
    TreeNode *m_parent;
    QVector<TreeNode *> m_children;
};

class ResourceTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ResourceTreeModel(QObject *parent = nullptr);
    ~ResourceTreeModel();

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    TreeNode *root() const { return m_root; }
    void clear();
    void setRoot(TreeNode *root);

private:
    TreeNode *m_root;
};
#endif // RESOURCEMODEL_H
