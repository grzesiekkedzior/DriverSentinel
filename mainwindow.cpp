#include "mainwindow.h"
#include <QObject>
#include "./ui_mainwindow.h"
#include "controller/certificatecontroller.h"
#include "controller/debuginfocontroller.h"
#include "controller/disassemblycontroller.h"
#include "controller/dosheadercontroller.h"
#include "controller/drivercontroller.h"
#include "controller/exceptioncontroller.h"
#include "controller/functioninfocontroller.h"
#include "controller/generalcontroller.h"
#include "controller/optionalheadercontroller.h"
#include "controller/richhdrcontroller.h"
#include "controller/sectioninfocontroller.h"
#include "controller/stringinfocontroller.h"
#include "controller/stringtoolboxcontroller.h"
#include "controller/treeimportscontroller.h"
#include "data/stringinfo.h"
#include "model/stringfilterproxymodel.h"
#include "toolbar/drivertoolbar.h"
#include "utils/disassemblydelegate.h"
#include <data/dosheader.h>
#include <data/optionalheaderinfo.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->comboSort->hide();
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
    dc->setDriverToolbar(dt);

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
    connect(dt, &DriverToolbar::openPERequested, dc, &DriverController::openPEFile);
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

    StringFilterProxyModel *sfpm = new StringFilterProxyModel;
    sfpm->setSourceModel(si->stringInfoModel().data());
    ui->stringsView->setModel(sfpm);
    ui->spinBoxMax->setValue(INT_MAX);
    connect(ui->searchPushButton, &QPushButton::clicked, this, [this, sfpm]() {
        sfpm->setFilteringString(ui->searchEdit->text());
        sfpm->setCaseSensitive(ui->checkCaseSensitive->isChecked());

        sfpm->setMin(ui->spinBoxMin->value() > 0 ? std::optional<int>(ui->spinBoxMin->value())
                                                 : std::nullopt);
        sfpm->setMax(ui->spinBoxMax->value() > 0 ? std::optional<int>(ui->spinBoxMax->value())
                                                 : std::nullopt);

        sfpm->setRegex(ui->checkRegex->isChecked());
    });

    connect(ui->pushButtonClearFilters, &QPushButton::clicked, this, [this, sfpm]() {
        // Reset UI
        ui->searchEdit->clear();
        ui->checkRegex->setChecked(false);
        ui->checkCaseSensitive->setChecked(false);
        ui->spinBoxMin->setValue(0);
        ui->spinBoxMax->setValue(INT_MAX);

        sfpm->resetModel();
    });

    StringToolBoxController *stc = new StringToolBoxController{sfpm, ui, this};
    DosHeaderController *dhc = new DosHeaderController(QSharedPointer<DosHeader>::create(),
                                                       ui->mainTable,
                                                       this->ui);
    connect(dt, &DriverToolbar::clearRequested, dhc, &DosHeaderController::clear);

    OptionalHeaderController *ohc
        = new OptionalHeaderController(QSharedPointer<OptionalHeaderInfo>::create(),
                                       ui->mainTable,
                                       this->ui);
    connect(dt, &DriverToolbar::clearRequested, ohc, &OptionalHeaderController::clear);

    DisassemblyController *dac = new DisassemblyController{QSharedPointer<DisassemblyData>::create(),
                                                           ui->mainTable,
                                                           this->ui};
    connect(dt, &DriverToolbar::clearRequested, dac, &DisassemblyController::clear);
    ui->tableViewAsm->setModel(dac->disassemblyModel().get());
    ui->tableViewAsm->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableViewAsm->verticalHeader()->setDefaultSectionSize(20);

    RichHeaderController *rhc = new RichHeaderController{ui->mainTable, this->ui};
    ui->richHdrTableView->setModel(rhc->richHdrModel().get());
    connect(dt, &DriverToolbar::clearRequested, rhc, &RichHeaderController::clear);

    DebugInfoController *dic = new DebugInfoController{ui->mainTable, this->ui};
    ui->debugInfotableView->setModel(dic->debugInfoModel().get());
    connect(dt, &DriverToolbar::clearRequested, dic, &DebugInfoController::clear);

    ExceptionController *exc = new ExceptionController{ui->mainTable, this->ui};
    ui->exceptionTableView->setModel(exc->exceptionModel().get());
    connect(dt, &DriverToolbar::clearRequested, exc, &ExceptionController::clear);
}
