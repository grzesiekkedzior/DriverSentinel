#ifndef DRIVERMODEL_H
#define DRIVERMODEL_H
#include <QAbstractTableModel>
#include "data/driverinfo.h"

class DriverModel : public QAbstractTableModel
{
    // QAbstractItemModel interface
    Q_OBJECT

public:
    DriverModel(QObject *parent = nullptr);

    enum class DriverColumn { Name = 0, Path, BaseAddress, Count };

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    void setDrivers(const QVector<DriverInfo> &drivers);
    void clear();

private:
    QVector<DriverInfo> m_driverInfo;
};

#endif // DRIVERMODEL_H
