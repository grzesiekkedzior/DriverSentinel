#ifndef RELOCATIONBLOCKSMODEL_H
#define RELOCATIONBLOCKSMODEL_H
#include <QAbstractTableModel>
#include <QObject>
#include "data/relocationinfo.h"

class RelocationBlocksModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum class Column { PageRVA, BlockSize, EntriesCount, COLUMN_COUNT };

    // QAbstractItemModel interface
public:
    explicit RelocationBlocksModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

public slots:
    void setRelocationBlocksToView(const QVector<LIEF_RELOCATION::RelocationBlockInfo> &rbi);
    void clear();

private:
    QVector<LIEF_RELOCATION::RelocationBlockInfo> m_relocationInfo;
};

#endif // RELOCATIONBLOCKSMODEL_H
