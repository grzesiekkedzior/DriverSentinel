#include "model/disassemblymodel.h"

DisassemblyModel::DisassemblyModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int DisassemblyModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_disassemblyData.size();
}

int DisassemblyModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(DisassemblyColumn::ColumnCount);
}

QVariant DisassemblyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const DisassemblyData &data = m_disassemblyData.at(index.row());
    auto column = static_cast<DisassemblyColumn>(index.column());

    switch (column) {
    case DisassemblyColumn::Section:
        return data.section;
    case DisassemblyColumn::Address:
        return QString("0x%1").arg(data.address, 0, 16); // hex
    case DisassemblyColumn::Bytes: {
        QString hexStr;
        for (auto b : data.bytes)
            hexStr += QString("%1 ").arg(static_cast<unsigned char>(b), 2, 16, QChar('0'));
        return hexStr.trimmed();
    }
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

QVariant DisassemblyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};
    auto headerSection = static_cast<DisassemblyColumn>(section);
    switch (headerSection) {
    case DisassemblyColumn::Section:
        return "Section";
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

void DisassemblyModel::setAsmDataView(const QVector<DisassemblyData> &dd)
{
    beginResetModel();
    m_disassemblyData = dd;
    endResetModel();
}

void DisassemblyModel::clearModel()
{
    beginResetModel();
    m_disassemblyData.clear();
    endResetModel();
}
