#include "model/treeimportsmodel.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>

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

void TreeImportsModel::exportToFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file:" << filename;
        return;
    }

    QTextStream out(&file);
    out << "Import Tree Hierarchy\n";
    out << "=====================\n\n";

    if (m_rootItem) {
        exportRecursive(m_rootItem.get(), out, 0);
    }

    file.close();
    qDebug() << "Tree exported to:" << filename;
}

// AI Generate ASCII
void TreeImportsModel::exportRecursive(
    TreeImportsItem *item, QTextStream &out, int depth, const QString &parentIndent, bool isLast)
{
    if (!item)
        return;

    QString currentIndent = parentIndent;
    QString prefix;

    if (depth == 0) {
        out << item->data(0).toString() << "\n";
        currentIndent = ""; // Reset dla dzieci korzenia
    } else {
        if (isLast) {
            prefix = "└── ";
        } else {
            prefix = "├── ";
        }
        out << currentIndent << prefix << item->data(0).toString() << "\n";
    }

    QString childIndent = currentIndent;
    if (depth >= 1) {
        if (isLast) {
            childIndent += "    ";
        } else {
            childIndent += "│   ";
        }
    }

    for (int i = 0; i < item->childCount(); ++i) {
        bool lastChild = (i == item->childCount() - 1);
        exportRecursive(item->child(i), out, depth + 1, childIndent, lastChild);
    }
}

void TreeImportsModel::exportToJson(const QString &filename)
{
    if (!m_rootItem)
        return;

    QJsonObject rootObject;
    exportJsonRecursive(m_rootItem.get(), rootObject);

    QJsonDocument doc(rootObject);
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << "JSON exported to:" << filename;
    }
}

void TreeImportsModel::exportJsonRecursive(TreeImportsItem *item, QJsonObject &jsonObj)
{
    if (!item)
        return;

    jsonObj["name"] = item->data(0).toString();
    jsonObj["childCount"] = item->childCount();

    if (item->childCount() > 0) {
        QJsonArray childrenArray;
        for (int i = 0; i < item->childCount(); ++i) {
            QJsonObject childObj;
            exportJsonRecursive(item->child(i), childObj);
            childrenArray.append(childObj);
        }
        jsonObj["children"] = childrenArray;
    }
}
