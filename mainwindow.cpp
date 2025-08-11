#include "mainwindow.h"
#include <QObject>
#include "./ui_mainwindow.h"
#include "controller/certificatecontroller.h"
#include "controller/drivercontroller.h"
#include "controller/functioninfocontroller.h"
#include "controller/generalcontroller.h"
#include "controller/sectioninfocontroller.h"
#include "controller/stringinfocontroller.h"
#include "controller/treeimportscontroller.h"
#include "data/stringinfo.h"
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
    TreeImportsController *tc = new TreeImportsController(QSharedPointer<TreeImportsItem>::create(),
                                                          ui->treeImportsView,
                                                          ui);
    FunctionInfoController *fc = new FunctionInfoController(QSharedPointer<FunctionInfo>::create(),
                                                            ui);
    StringInfoController *si = new StringInfoController(QSharedPointer<StringInfo>::create(),
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

    connect(ui->mainTable, &QTableView::clicked, tc, &TreeImportsController::loadImportsDataToView);

    connect(dt, &DriverToolbar::clearRequested, sc, &SectionInfoController::clear);
    connect(dt, &DriverToolbar::clearRequested, gc, &GeneralController::clear);
    connect(dt, &DriverToolbar::clearRequested, cc, &CertificateController::clear);
    connect(dt, &DriverToolbar::clearRequested, tc, &TreeImportsController::clear);
    connect(dt, &DriverToolbar::clearRequested, fc, &FunctionInfoController::clear);
    connect(dt, &DriverToolbar::clearRequested, si, &StringInfoController::clear);

    connect(ui->treeImportsView, &QTreeView::clicked, tc, &TreeImportsController::onTreeItemClicked);
    connect(tc, &TreeImportsController::dllSelected, fc, &FunctionInfoController::onDllSelected);
    connect(ui->treeImportsView,
            &QTreeView::clicked,
            fc,
            &FunctionInfoController::loadFunctionInfoToView);
    connect(ui->mainTable, &QTableView::clicked, si, &StringInfoController::loadStringDataToView);

    ui->mainTable->setModel(dc->getDriverModel().data());
    ui->peSectionView->setModel(sc->sectionInfoModel().data());

    ui->certificateTableView->setModel(cc->certificateModel().data());
    ui->peSectionView->resizeColumnsToContents();

    ui->treeImportsView->setModel(tc->treeImportsModel().data());
    ui->treeTableView->setModel(fc->functionInfoModel().data());

    ui->stringsView->setModel(si->stringInfoModel().data());
}
