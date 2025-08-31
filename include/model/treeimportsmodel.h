#ifndef TREEIMPORTSMODEL_H
#define TREEIMPORTSMODEL_H

#include <QAbstractItemModel>
#include "data/treeimportsitem.h"
#include <LIEF/PE.hpp>

class TreeImportsModel : public QAbstractItemModel
{
    // QAbstractItemModel interface
public:
    TreeImportsModel(QObject *parent = nullptr);
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setRootItem(std::unique_ptr<TreeImportsItem> root);

    // Exports
    void exportToFile(const QString &filename);
    void exportToJson(const QString &filename);
    void exportToCsv(const QString &filename);

private:
    std::unique_ptr<TreeImportsItem> m_rootItem;

    TreeImportsItem *getItem(const QModelIndex &index) const;

    // Exports
    void exportRecursive(TreeImportsItem *item,
                         QTextStream &out,
                         int depth,
                         const QString &parentIndent = "",
                         bool isLast = false);
    void exportJsonRecursive(TreeImportsItem *item, QJsonObject &jsonObj);
    void exportCsvRecursive(TreeImportsItem *item,
                            QTextStream &out,
                            int depth,
                            const QString &parentName);
};

#endif // TREEIMPORTSMODEL_H
