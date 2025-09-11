#include "controller/relocationcontroller.h"
#include "ui_mainwindow.h"

RelocationController::RelocationController(QTableView *mainTableView,
                                           Ui::MainWindow *ui,
                                           QObject *parent)
    : QObject(parent)
    , m_mainTableView(mainTableView)
    , m_ui(ui)
    , m_relocationBlocksModel{QSharedPointer<RelocationBlocksModel>::create()}
    , m_relocationEntriesModel{QSharedPointer<RelocationEntriesModel>::create()}
{
    connect(m_mainTableView,
            &QTableView::clicked,
            this,
            &RelocationController::loadRelocationDataToView);
}

void RelocationController::loadRelocationDataToView(const QModelIndex &index) {}

void RelocationController::clear() {}

void RelocationController::update(const QVector<LIEF_RELOCATION::RelocationInfo> &ri) {}

QSharedPointer<RelocationBlocksModel> RelocationController::relocationBlocksModel() const
{
    return m_relocationBlocksModel;
}

QSharedPointer<RelocationEntriesModel> RelocationController::relocationEntriesModel() const
{
    return m_relocationEntriesModel;
}
