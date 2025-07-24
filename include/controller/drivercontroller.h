#ifndef DRIVERCONTROLLER_H
#define DRIVERCONTROLLER_H

#include <QObject>
#include "model/drivermodel.h"
#include <windows.h>

namespace Ui {
class MainWindow;
}

class DriverController : public QObject
{
    Q_OBJECT
public:
    DriverController(QSharedPointer<DriverModel> driverModel, Ui::MainWindow *ui);

    QSharedPointer<DriverModel> getDriverModel() const;
    void setDriverModel(QSharedPointer<DriverModel> newDriverModel);
    void start();

public slots:
    void refresh();
    void clear();

private:
    QSharedPointer<DriverModel> m_driverModel;
    Ui::MainWindow *m_ui;
    QVector<DriverInfo> loadDrivers();
};

#endif // DRIVERCONTROLLER_H
