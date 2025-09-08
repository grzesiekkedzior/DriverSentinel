#include "model/functioninfomodel.h"

FunctionInfoModel::FunctionInfoModel(QObject *parent)
    : QAbstractTableModel{parent}
{}

int FunctionInfoModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_functionInfo.size();
}

int FunctionInfoModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(FunctionColumn::Count);
}

QVariant FunctionInfoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const FunctionInfo &fi = m_functionInfo.at(index.row());
    auto column = static_cast<FunctionColumn>(index.column());

    switch (column) {
    case FunctionColumn::DllName:
        return fi.dllName;
    case FunctionColumn::FunctionName:
        return fi.functionName;
    case FunctionColumn::IsImport:
        return fi.isImport ? "Yes" : "No";
    case FunctionColumn::IsExport:
        return fi.isExport ? "Yes" : "No";
    case FunctionColumn::IsForwarded:
        return fi.isForwarded ? "Yes" : "No";
    case FunctionColumn::ForwardInfo:
        return fi.forwardInfo;

    // Imports
    case FunctionColumn::IAT_Address:
        return QString("0x%1").arg(fi.iatAddress, 0, 16);
    case FunctionColumn::IAT_Value:
        return QString("0x%1").arg(fi.iatValue, 0, 16);
    case FunctionColumn::ILT_Value:
        return QString("0x%1").arg(fi.iltValue, 0, 16);
    case FunctionColumn::IsOrdinal:
        return fi.isOrdinal ? "Yes" : "No";
    case FunctionColumn::Hint:
        return fi.hint;
    case FunctionColumn::HintNameRVA:
        return QString("0x%1").arg(fi.hintNameRVA, 0, 16);
    case FunctionColumn::EntryData:
        return QString("0x%1").arg(fi.entryData, 0, 16);
    case FunctionColumn::ImportOrdinal:
        return fi.importOrdinal;

    // Exports
    case FunctionColumn::FunctionRVA:
        return QString("0x%1").arg(fi.functionRVA, 0, 16);
    case FunctionColumn::Address:
        return QString("0x%1").arg(fi.address, 0, 16);
    case FunctionColumn::IsExtern:
        return fi.isExtern ? "Yes" : "No";
    case FunctionColumn::ExportOrdinal:
        return fi.exportOrdinal;

    case FunctionColumn::Count:
        return {};
    }

    return {};
}

QVariant FunctionInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    auto column = static_cast<FunctionColumn>(section);
    switch (column) {
    case FunctionColumn::DllName:
        return "DLL";
    case FunctionColumn::FunctionName:
        return "Function";
    case FunctionColumn::IsImport:
        return "Import";
    case FunctionColumn::IsExport:
        return "Export";
    case FunctionColumn::IsForwarded:
        return "Forwarded";
    case FunctionColumn::ForwardInfo:
        return "Forward Info";

    // Imports
    case FunctionColumn::IAT_Address:
        return "IAT Addr";
    case FunctionColumn::IAT_Value:
        return "IAT Value";
    case FunctionColumn::ILT_Value:
        return "ILT Value";
    case FunctionColumn::IsOrdinal:
        return "Ordinal";
    case FunctionColumn::Hint:
        return "Hint";
    case FunctionColumn::HintNameRVA:
        return "HintName RVA";
    case FunctionColumn::EntryData:
        return "Entry Data";
    case FunctionColumn::ImportOrdinal:
        return "Import Ordinal";

    // Exports
    case FunctionColumn::FunctionRVA:
        return "Func RVA";
    case FunctionColumn::Address:
        return "Address";
    case FunctionColumn::IsExtern:
        return "Extern";
    case FunctionColumn::ExportOrdinal:
        return "Export Ordinal";

    case FunctionColumn::Count:
        return {};
    }

    return {};
}

void FunctionInfoModel::setFunInfoSection(const QVector<FunctionInfo> &fi)
{
    beginResetModel();
    m_functionInfo = fi;
    endResetModel();
}

void FunctionInfoModel::clear()
{
    beginResetModel();
    m_functionInfo.clear();
    endResetModel();
}
