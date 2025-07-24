#include "mainwindow.h"
#include <QObject>
#include "./ui_mainwindow.h"
#include "controller/drivercontroller.h"
#include "controller/generalcontroller.h"
#include "controller/sectioninfocontroller.h"
#include "toolbar/drivertoolbar.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::start()
{
    DriverController *dc = new DriverController(QSharedPointer<DriverModel>::create());

    DriverToolbar *dt = new DriverToolbar;
    this->addToolBar(dt);
    connect(dt, &DriverToolbar::refreshRequested, dc, &DriverController::refresh);
    connect(dt, &DriverToolbar::clearRequested, dc, &DriverController::clear);

    GeneralController *gc = new GeneralController(QSharedPointer<GeneralInfo>::create(),
                                                  ui->mainTable,
                                                  this->ui);
    SectionInfoController *sc = new SectionInfoController(QSharedPointer<SectionInfo>::create(),
                                                          ui->mainTable,
                                                          this->ui);
    connect(ui->mainTable,
            &QTableView::clicked,
            sc,
            &SectionInfoController::loadPESectionDataToView);
    ui->mainTable->setModel(dc->getDriverModel().data());
    ui->peSectionView->setModel(sc->sectionInfoModel().data());
}
