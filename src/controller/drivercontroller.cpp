#include "controller/drivercontroller.h"
#include <QFileDialog>
#include <QMessageBox>
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

    QVector<DriverInfo> df;

    if (!m_selectedPEFile.isEmpty()) {
        DriverInfo info;
        info.path = m_selectedPEFile;
        info.name = QFileInfo(m_selectedPEFile).fileName();
        //info.baseAddress

        df.append(info);
    } else {
        df = loadDrivers();
    }

    m_driverModel->setDrivers(df);
}

void DriverController::refresh()
{
    emit m_driverToolbar->clearRequested();
    start();
}

void DriverController::clear()
{
    if (m_driverModel) {
        m_driverModel->setDrivers({});
        m_selectedPEFile = "";
    }
}

void DriverController::openPEFile()
{
    QString selectedFile
        = QFileDialog::getOpenFileName(nullptr,
                                       tr("Select PE file"),
                                       "",
                                       tr("PE Files (*.exe *.dll *.sys);;All Files (*)"));

    if (selectedFile.isEmpty()) {
        return;
    }
    emit m_driverToolbar->clearRequested();

    m_selectedPEFile = selectedFile;
    QMessageBox::information(nullptr, tr("Selected file"), m_selectedPEFile);

    start();
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

DriverToolbar *DriverController::driverToolbar() const
{
    return m_driverToolbar;
}

void DriverController::setDriverToolbar(DriverToolbar *newDriverToolbar)
{
    m_driverToolbar = newDriverToolbar;
}
