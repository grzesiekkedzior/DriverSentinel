#include "controller/generalcontroller.h"

#include <QFileInfo>
#include <LIEF/PE.hpp>

GeneralController::GeneralController(QSharedPointer<GeneralInfo> generalInfo,
                                     QTableView *mainTableView,
                                     QObject *parent)
    : m_generalInfo{generalInfo}
    , m_mainTableView(mainTableView)
    , QObject{parent}
{
    connect(m_mainTableView, &QTableView::clicked, this, &GeneralController::loadGeneralInfo);
}

//For now this is the only expample function
GeneralInfo GeneralController::loadGeneralInfo(const QModelIndex &index)
{
    if (!index.isValid())
        return {};

    int row = index.row();

    QModelIndex nameIndex = m_mainTableView->model()->index(row, 0);
    QVariant nameData = m_mainTableView->model()->data(nameIndex, Qt::DisplayRole);

    qDebug() << "First column:" << nameData.toString();

    QString filePath = "C:\\Windows\\System32\\hal.dll";
    GeneralInfo info;
    info.path = filePath;
    info.name = QFileInfo(filePath).fileName();

    try {
        std::unique_ptr<LIEF::PE::Binary> binary = LIEF::PE::Parser::parse(filePath.toStdString());
        if (binary) {
            info.baseAddress = binary->imagebase();

            qDebug() << "File:" << info.name;
            qDebug() << "Base Address:" << QString::number(info.baseAddress, 16);
        } else {
            qWarning() << "LIEF: failed to parse binary";
        }
    } catch (const std::exception &e) {
        qWarning() << "LIEF error:" << e.what();
    } catch (...) {
        qWarning() << "Unknown error while parsing PE";
    }

    return info;
}
