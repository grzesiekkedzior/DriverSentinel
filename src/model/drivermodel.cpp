#include "model/drivermodel.h"

DriverModel::DriverModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int DriverModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_driverInfo.size();
}

int DriverModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(DriverColumn::Count);
}

QVariant DriverModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const DriverInfo &info = m_driverInfo.at(index.row());
    auto column = static_cast<DriverColumn>(index.column());

    switch (column) {
    case DriverColumn::Name:
        return info.name;
    case DriverColumn::Path:
        return info.path;
    case DriverColumn::BaseAddress:
        return QString("0x%1").arg(info.baseAddress, 0, 16);
    default:
        return {};
    }
}

QVariant DriverModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (static_cast<DriverColumn>(section)) {
    case DriverColumn::Name:
        return "Name";
    case DriverColumn::Path:
        return "Path";
    case DriverColumn::BaseAddress:
        return "Base Address";
    default:
        return {};
    }
}

void DriverModel::setDrivers(const QVector<DriverInfo> &drivers)
{
    beginResetModel();
    m_driverInfo = drivers;
    endResetModel();
}

void DriverModel::clear()
{
    beginResetModel();
    m_driverInfo.clear();
    endResetModel();
}
