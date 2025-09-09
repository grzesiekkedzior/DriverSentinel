#include "controller/exceptioncontroller.h"
#include "ui_mainwindow.h"
#include "utils/peutils.h"
#include <LIEF/PE.hpp>

ExceptionController::ExceptionController(QTableView *mainTableView,
                                         Ui::MainWindow *ui,
                                         QObject *parent)
    : QObject(parent)
    , m_mainTableView(mainTableView)
    , m_ui(ui)
    , m_exceptionModel{QSharedPointer<ExceptionModel>::create()}
    , m_exceptionInfo{QSharedPointer<ExceptionInfo>::create()}
{
    m_ui->exceptionTableView->resizeColumnsToContents();
    connect(m_mainTableView, &QTableView::clicked, this, &ExceptionController::loadExceptionInfo);
}

void ExceptionController::loadExceptionInfo(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QVector<ExceptionInfo> l_exceptionInfo;
    QString filePath = PEUtils::getPEfilePath(m_ui->mainTable, index);
    if (filePath.isEmpty()) {
        qWarning() << "File path is empty";
        return;
    }
    qDebug() << "===============================";

    LIEF::PE::ParserConfig cfg;
    cfg.parse_exceptions = true;

    try {
    } catch (const std::exception &e) {
        qWarning() << "Test error:" << e.what();
    } catch (...) {
        qWarning() << "Unknown test error";
    }
}

void ExceptionController::updateModel(const QVector<ExceptionInfo> &ei)
{
    if (!m_exceptionModel)
        return;
    m_exceptionModel->loadExceptionDataToView(ei);
}

void ExceptionController::clear()
{
    if (!m_exceptionModel)
        return;
    m_exceptionModel->loadExceptionDataToView({});
}

QSharedPointer<ExceptionModel> ExceptionController::exceptionModel() const
{
    return m_exceptionModel;
}

QSharedPointer<ExceptionInfo> ExceptionController::exceptionInfo() const
{
    return m_exceptionInfo;
}
