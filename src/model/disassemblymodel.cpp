#include "model/disassemblymodel.h"

DisassembyModel::DisassembyModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int DisassembyModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_disassemblyData.size();
}

int DisassembyModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(DisassemblyColumn::ColumnCount);
}

QVariant DisassembyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const DisassemblyData &data = m_disassemblyData.at(index.row());
    auto column = static_cast<DisassemblyColumn>(index.column());

    switch (column) {
    case DisassemblyColumn::Address:
        return data.address;
    case DisassemblyColumn::Bytes:
        return data.bytes.toHex(' ').toUpper();
    case DisassemblyColumn::Mnemonic:
        return data.mnemonic;
    case DisassemblyColumn::Operands:
        return data.operands;
    case DisassemblyColumn::Comment:
        return data.comment;
    default:
        return {};
    }
}

QVariant DisassembyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};
    auto headerSection = static_cast<DisassemblyColumn>(section);
    switch (headerSection) {
    case DisassemblyColumn::Address:
        return "Address";
    case DisassemblyColumn::Bytes:
        return "Bytes";
    case DisassemblyColumn::Mnemonic:
        return "Mnemonic";
    case DisassemblyColumn::Operands:
        return "Operands";
    case DisassemblyColumn::Comment:
        return "Comment";
    default:
        return {};
    }
}
