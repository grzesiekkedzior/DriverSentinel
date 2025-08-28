#ifndef DRIVERCONTROLLER_H
#define DRIVERCONTROLLER_H

#include <QObject>
#include "model/drivermodel.h"
#include "toolbar/drivertoolbar.h"
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

    DriverToolbar *driverToolbar() const;
    void setDriverToolbar(DriverToolbar *newDriverToolbar);

public slots:
    void refresh();
    void clear();
    void openPEFile();

private:
    QSharedPointer<DriverModel> m_driverModel;
    Ui::MainWindow *m_ui;
    QVector<DriverInfo> loadDrivers();
    QString m_selectedPEFile{};
    DriverToolbar *m_driverToolbar;
};

#endif // DRIVERCONTROLLER_H
