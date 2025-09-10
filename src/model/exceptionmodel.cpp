#include "model/exceptionmodel.h"

ExceptionModel::ExceptionModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int ExceptionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_exceptionInfo.size();
}

int ExceptionModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(ExceptionColumn::ColumnCount);
}

QVariant ExceptionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const ExceptionInfo &ei = m_exceptionInfo.at(index.row());
    auto column = static_cast<ExceptionColumn>(index.column());

    switch (column) {
    case ExceptionColumn::FileOffset:
        return QString("0x%1").arg(ei.fileOffset, 8, 16, QChar('0'));
    case ExceptionColumn::BeginRVA:
        return QString("0x%1").arg(ei.beginRVA, 8, 16, QChar('0'));
    case ExceptionColumn::EndRVA:
        return QString("0x%1").arg(ei.endRVA, 8, 16, QChar('0'));
    case ExceptionColumn::UnwindRVA:
        return QString("0x%1").arg(ei.unwindRVA, 8, 16, QChar('0'));
    case ExceptionColumn::Version:
        return ei.version;
    case ExceptionColumn::Flags:
        return ei.flags;
    case ExceptionColumn::SizeOfProlog:
        return ei.sizeOfProlog;
    case ExceptionColumn::CountOfCodes:
        return ei.countOfCodes;
    case ExceptionColumn::RegistersSaved: {
        QStringList regs;
        for (const auto &op : ei.unwindOps)
            if (!op.regName.isEmpty())
                regs << op.regName;
        return regs.join(", ");
    }
    case ExceptionColumn::OpcodesDetail: {
        QStringList ops;
        for (const auto &op : ei.unwindOps) {
            ops << QString("Opcode=%1, Offset=%2, OpInfo=%3, Reg=%4")
                       .arg(op.opcode)
                       .arg(op.offset)
                       .arg(op.opinfo)
                       .arg(op.regName.isEmpty() ? "-" : op.regName);
        }
        return ops.join("\n");
    }

    case ExceptionColumn::HasEpilog:
        return ei.hasEpilog ? "Yes" : "No";
    case ExceptionColumn::EpilogFlags:
        return QString("0x%1").arg(ei.epilogFlags, 2, 16, QChar('0'));
    case ExceptionColumn::EpilogSize:
        return ei.epilogSize;
    default:
        return {};
    }
}

QVariant ExceptionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    auto column = static_cast<ExceptionColumn>(section);

    switch (column) {
    case ExceptionColumn::FileOffset:
        return "File Offset";
    case ExceptionColumn::BeginRVA:
        return "Begin RVA";
    case ExceptionColumn::EndRVA:
        return "End RVA";
    case ExceptionColumn::UnwindRVA:
        return "Unwind RVA";
    case ExceptionColumn::Version:
        return "Version";
    case ExceptionColumn::Flags:
        return "Flags";
    case ExceptionColumn::SizeOfProlog:
        return "Prolog Size";
    case ExceptionColumn::CountOfCodes:
        return "Nb Opcodes";
    case ExceptionColumn::RegistersSaved:
        return "Registers Saved";
    case ExceptionColumn::OpcodesDetail:
        return "Opcodes Detail";
    case ExceptionColumn::HasEpilog:
        return "Has Epilog";
    case ExceptionColumn::EpilogFlags:
        return "Epilog Flags";
    case ExceptionColumn::EpilogSize:
        return "Epilog Size";
    default:
        return {};
    }
}

void ExceptionModel::loadExceptionDataToView(const QVector<ExceptionInfo> &ei)
{
    beginResetModel();
    m_exceptionInfo = ei;
    endResetModel();
}

void ExceptionModel::clear()
{
    beginResetModel();
    m_exceptionInfo.clear();
    endResetModel();
}
