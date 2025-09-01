#ifndef DISASSEMBLYMODEL_H
#define DISASSEMBLYMODEL_H

#include <QAbstractTableModel>
#include "data/disassemblydata.h"

class DisassembyModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    DisassembyModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
    enum class DisassemblyColumn { Address, Bytes, Mnemonic, Operands, Comment, ColumnCount };

public:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QVector<DisassemblyData> m_disassemblyData;
};

#endif // DISASSEMBLYMODEL_H
