#include "mainwindow.h"
#include <QObject>
#include "./ui_mainwindow.h"
#include "controller/certificatecontroller.h"
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
    DriverController *dc = new DriverController(QSharedPointer<DriverModel>::create(), this->ui);

    DriverToolbar *dt = new DriverToolbar;
    this->addToolBar(dt);

    GeneralController *gc = new GeneralController(QSharedPointer<GeneralInfo>::create(),
                                                  ui->mainTable,
                                                  this->ui);
    SectionInfoController *sc = new SectionInfoController(QSharedPointer<SectionInfo>::create(),
                                                          ui->mainTable,
                                                          ui);
    CertificateController *cc = new CertificateController(QSharedPointer<CertificateInfo>::create(),
                                                          ui->mainTable,
                                                          ui);
    connect(dt, &DriverToolbar::refreshRequested, dc, &DriverController::refresh);
    connect(dt, &DriverToolbar::clearRequested, dc, &DriverController::clear);
    connect(ui->mainTable,
            &QTableView::clicked,
            sc,
            &SectionInfoController::loadPESectionDataToView);
    connect(ui->mainTable,
            &QTableView::clicked,
            cc,
            &CertificateController::loadCertificateDataToView);
    connect(dt, &DriverToolbar::clearRequested, sc, &SectionInfoController::clear);
    connect(dt, &DriverToolbar::clearRequested, gc, &GeneralController::clear);
    connect(dt, &DriverToolbar::clearRequested, cc, &CertificateController::clear);
    ui->mainTable->setModel(dc->getDriverModel().data());
    ui->peSectionView->setModel(sc->sectionInfoModel().data());

    ui->certificateTableView->setModel(cc->certificateModel().data());
    ui->peSectionView->resizeColumnsToContents();
}
