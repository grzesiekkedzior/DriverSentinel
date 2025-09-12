#include "model/resourcemodel.h"

TreeNode::TreeNode(const ResourceTreeEntry &entry, TreeNode *parent)
    : m_entry(entry)
    , m_parent(parent)
{}

TreeNode::~TreeNode()
{
    qDeleteAll(m_children);
}

void TreeNode::appendChild(TreeNode *child)
{
    m_children.push_back(child);
}

TreeNode *TreeNode::child(int row) const
{
    return m_children.value(row, nullptr);
}

int TreeNode::childCount() const
{
    return m_children.size();
}

int TreeNode::row() const
{
    return m_parent ? m_parent->m_children.indexOf(const_cast<TreeNode *>(this)) : 0;
}

TreeNode *TreeNode::parent() const
{
    return m_parent;
}

// Resource model
ResourceTreeModel::ResourceTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    m_root = new TreeNode({"", "", "", ""});
}

ResourceTreeModel::~ResourceTreeModel()
{
    delete m_root;
}

QModelIndex ResourceTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return {};

    TreeNode *parentNode = parent.isValid() ? static_cast<TreeNode *>(parent.internalPointer())
                                            : m_root;

    TreeNode *childNode = parentNode->child(row);
    if (childNode)
        return createIndex(row, column, childNode);
    return {};
}

QModelIndex ResourceTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};

    TreeNode *childNode = static_cast<TreeNode *>(index.internalPointer());
    TreeNode *parentNode = childNode->parent();

    if (parentNode == m_root || !parentNode)
        return {};

    return createIndex(parentNode->row(), 0, parentNode);
}

int ResourceTreeModel::rowCount(const QModelIndex &parent) const
{
    TreeNode *parentNode = parent.isValid() ? static_cast<TreeNode *>(parent.internalPointer())
                                            : m_root;
    return parentNode->childCount();
}

int ResourceTreeModel::columnCount(const QModelIndex &) const
{
    return 4; // type, idName, lang, size
}

QVariant ResourceTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    TreeNode *node = static_cast<TreeNode *>(index.internalPointer());
    const auto &entry = node->entry();

    switch (index.column()) {
    case 0:
        return entry.type;
    case 1:
        return entry.idName;
    case 2:
        return entry.lang;
    case 3:
        return entry.size;
    default:
        return {};
    }
}

QVariant ResourceTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return "Type";
        case 1:
            return "ID/Name";
        case 2:
            return "Lang";
        case 3:
            return "Size";
        }
    }
    return {};
}

void ResourceTreeModel::clear()
{
    beginResetModel();
    delete m_root;
    m_root = new TreeNode({"", "", "", ""});
    endResetModel();
}

void ResourceTreeModel::setRoot(TreeNode *root)
{
    beginResetModel();
    delete m_root;
    m_root = root;
    endResetModel();
}
