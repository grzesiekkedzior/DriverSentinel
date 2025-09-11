#ifndef RELOCATIONENTRIESMODEL_H
#define RELOCATIONENTRIESMODEL_H
#include <QAbstractTableModel>
#include <QString>
#include <QVector>
#include "data/relocationinfo.h"

class RelocationEntriesModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum class Column { OffsetInPage, RelocRVA, Type, Value, COLUMN_COUNT };

    explicit RelocationEntriesModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

public slots:
    void setRelocationEntriesToView(const QVector<LIEF_RELOCATION::RelocationEntryInfo> &entries);
    void clear();

private:
    QVector<LIEF_RELOCATION::RelocationEntryInfo> m_entries;
};

#endif // RELOCATIONENTRIESMODEL_H
