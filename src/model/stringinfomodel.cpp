#include "model/stringinfomodel.h"

StringInfoModel::StringInfoModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int StringInfoModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_stringInfo.size();
}

int StringInfoModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(StringsColumn::Count);
}

QVariant StringInfoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const auto &sectionInfo = m_stringInfo.at(index.row());
    auto column = static_cast<StringsColumn>(index.column());

    switch (column) {
    case StringsColumn::Section:
        return sectionInfo.section;
    case StringsColumn::Offset:
        return sectionInfo.offset;
    case StringsColumn::String:
        return sectionInfo.sectionStrng;
    case StringsColumn::Count:
        return {};
    }

    return {};
}

QVariant StringInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};
    auto header = static_cast<StringsColumn>(section);

    switch (header) {
    case StringsColumn::Section:
        return "Section";
    case StringsColumn::Offset:
        return "Offset";
    case StringsColumn::String:
        return "String";
    case StringsColumn::Count:
        return {};
    }
    return {};
}

void StringInfoModel::setStringInfo(const QVector<StringInfo> &si)
{
    beginResetModel();
    m_stringInfo = si;
    endResetModel();
}

void StringInfoModel::clear()
{
    beginResetModel();
    m_stringInfo.size();
    endResetModel();
}
