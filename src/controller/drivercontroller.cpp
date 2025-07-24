#include "controller/drivercontroller.h"
#include "ui_mainwindow.h"
#include <psapi.h>

DriverController::DriverController(QSharedPointer<DriverModel> driverModel, Ui::MainWindow *ui)
    : m_driverModel(std::move(driverModel))
    , m_ui{ui}
{}

QSharedPointer<DriverModel> DriverController::getDriverModel() const
{
    return m_driverModel;
}

void DriverController::setDriverModel(QSharedPointer<DriverModel> newDriverModel)
{
    m_driverModel = newDriverModel;
}

void DriverController::start()
{
    if (!m_driverModel)
        return;
    QVector<DriverInfo> df = loadDrivers();
    m_driverModel->setDrivers(df);
}

void DriverController::refresh()
{
    start();
}

void DriverController::clear()
{
    if (m_driverModel)
        m_driverModel->setDrivers({});
}

QVector<DriverInfo> DriverController::loadDrivers()
{
    QVector<DriverInfo> drivers;

    LPVOID driversArray[1024];
    DWORD cbNeeded;

    if (EnumDeviceDrivers(driversArray, sizeof(driversArray), &cbNeeded)
        && cbNeeded < sizeof(driversArray)) {
        int count = cbNeeded / sizeof(LPVOID);
        for (int i = 0; i < count; i++) {
            TCHAR szDriver[1024] = {0};
            if (GetDeviceDriverBaseName(driversArray[i],
                                        szDriver,
                                        sizeof(szDriver) / sizeof(TCHAR))) {
                TCHAR szPath[1024] = {0};
                if (GetDeviceDriverFileName(driversArray[i],
                                            szPath,
                                            sizeof(szPath) / sizeof(TCHAR))) {
                    DriverInfo info;
                    info.name = QString::fromWCharArray(szDriver);
                    info.path = QString::fromWCharArray(szPath);
                    info.baseAddress = reinterpret_cast<quint64>(driversArray[i]);
                    drivers.append(info);
                }
            }
        }
    }

    return drivers;
}
