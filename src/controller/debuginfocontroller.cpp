#include "controller/debuginfocontroller.h"
#include "ui_mainwindow.h"
#include <LIEF/PE.hpp>
#include <utils/peutils.h>

DebugInfoController::DebugInfoController(QTableView *mainTableView,
                                         Ui::MainWindow *ui,
                                         QObject *parent)
    : QObject(parent)
    , m_mainTableView(mainTableView)
    , m_ui(ui)
    , m_debugInfoModel{QSharedPointer<DebugInfoModel>::create()}
    , m_debugInfo{QSharedPointer<DebugInfo>::create()}
{
    m_ui->debugInfotableView->resizeColumnsToContents();
    connect(m_mainTableView, &QTableView::clicked, this, &DebugInfoController::loadDebugInfo);
}

void DebugInfoController::loadDebugInfo(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    QVector<DebugInfo> l_debugInfo;

    QString filePath = PEUtils::getPEfilePath(m_ui->mainTable, index);
    if (filePath.isEmpty()) {
        qWarning() << "File path is empty";
        return;
    }

    try {
        std::unique_ptr<LIEF::PE::Binary> binary = LIEF::PE::Parser::parse(filePath.toStdString());

        for (const auto &dbg : binary->debug()) {
            DebugInfo di;
            di.type = QString::fromStdString(LIEF::PE::to_string(dbg.type()));
            di.characteristics = dbg.characteristics();
            di.timestamp = dbg.timestamp();
            di.majorVersion = dbg.major_version();
            di.minorVersion = dbg.minor_version();
            di.sizeOfData = dbg.sizeof_data();
            di.addressOfRawData = dbg.addressof_rawdata();
            di.pointerToRawData = dbg.pointerto_rawdata();

            auto payloadSpan = dbg.payload();
            di.payload = QByteArray(reinterpret_cast<const char *>(payloadSpan.data()),
                                    static_cast<int>(payloadSpan.size()));

            l_debugInfo.append(di);
        }

        updateModel(l_debugInfo);

    } catch (const std::exception &e) {
        qWarning() << "LIEF error:" << e.what();
    } catch (...) {
        qWarning() << "Unknown error while parsing PE";
    }

    updateModel(l_debugInfo);
}

void DebugInfoController::updateModel(const QVector<DebugInfo> &di)
{
    if (!m_debugInfoModel)
        return;
    m_debugInfoModel->setDebugDataToView(di);
}

void DebugInfoController::clear()
{
    if (!m_debugInfoModel)
        return;
    m_debugInfoModel->setDebugDataToView({});
}

QSharedPointer<DebugInfoModel> DebugInfoController::debugInfoModel() const
{
    return m_debugInfoModel;
}

QSharedPointer<DebugInfo> DebugInfoController::debugInfo() const
{
    return m_debugInfo;
}
