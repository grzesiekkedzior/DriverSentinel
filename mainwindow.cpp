#include "mainwindow.h"
#include <QObject>
#include "./ui_mainwindow.h"
#include "controller/drivercontroller.h"
#include "toolbar/drivertoolbar.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // ui->setupUi(this);
    // DriverController *dc = new DriverController(QSharedPointer<DriverModel>::create());

    // DriverToolbar *dt = new DriverToolbar;
    // this->addToolBar(dt);
    // connect(dt, &DriverToolbar::refreshRequested, dc, &DriverController::refresh);
    // connect(dt, &DriverToolbar::clearRequested, dc, &DriverController::clear);

    // ui->mainTable->setModel(dc->getDriverModel().data());
}

MainWindow::~MainWindow()
{
    delete ui;
}
