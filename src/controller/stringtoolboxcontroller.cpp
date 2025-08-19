#include "controller/stringtoolboxcontroller.h"
#include "ui_mainwindow.h"

StringToolBoxController::StringToolBoxController(StringFilterProxyModel *model,
                                                 Ui::MainWindow *ui,
                                                 QObject *parent)
    : m_proxyModel{model}
    , m_ui{ui}
    , QObject{parent}
{
    pdfAction = new QAction{"PDF", this};
    txtAction = new QAction{"TXT", this};
    csvAction = new QAction{"CSV", this};
    actionMenu = new QMenu;
    actionMenu->addAction(pdfAction);
    actionMenu->addAction(txtAction);
    actionMenu->addAction(csvAction);
    m_ui->saveButton->setMenu(actionMenu);
    connect(pdfAction, &QAction::triggered, this, &StringToolBoxController::saveAsPdf);
    connect(txtAction, &QAction::triggered, this, &StringToolBoxController::saveAsTxt);
    connect(csvAction, &QAction::triggered, this, &StringToolBoxController::saveAsCsv);
}

void StringToolBoxController::saveAsPdf()
{
    qDebug() << "saveAsPdf";
}

void StringToolBoxController::saveAsTxt()
{
    qDebug() << "saveAsTxt";
}

void StringToolBoxController::saveAsCsv()
{
    qDebug() << "saveAsCsv";
}
