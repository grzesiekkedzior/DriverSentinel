#ifndef DRIVERCONTROLLER_H
#define DRIVERCONTROLLER_H

#include <QObject>
#include "model/drivermodel.h"
#include <windows.h>

class DriverController : public QObject
{
    Q_OBJECT
public:
    DriverController(QSharedPointer<DriverModel> driverModel);

    QSharedPointer<DriverModel> getDriverModel() const;
    void setDriverModel(QSharedPointer<DriverModel> newDriverModel);
    void start();

private:
    QSharedPointer<DriverModel> m_driverModel;
    QVector<DriverInfo> loadDrivers();
};

#endif // DRIVERCONTROLLER_H
