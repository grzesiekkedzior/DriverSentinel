#include "controller/generalcontroller.h"

#include <QDir>
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

QVariant GeneralController::extractFileNameFromRow(const QModelIndex &index, int column)
{
    int row = index.row();

    QModelIndex nameIndex = m_mainTableView->model()->index(row, column);
    QVariant nameData = m_mainTableView->model()->data(nameIndex, Qt::DisplayRole);

    return nameData;
}

//For now this is the only expample function
GeneralInfo GeneralController::loadGeneralInfo(const QModelIndex &index)
{
    if (!index.isValid())
        return {};

    QVariant nameData = extractFileNameFromRow(index, 0);
    QString filePath = extractFileNameFromRow(index, 1).toString();
    filePath.replace("\\SystemRoot", "C:\\Windows");

    qDebug() << "File path: " << filePath;
    qDebug() << "First column:" << nameData.toString();

    GeneralInfo info;

    try {
        std::unique_ptr<LIEF::PE::Binary> binary = LIEF::PE::Parser::parse(filePath.toStdString());
        if (binary) {
            info.name = nameData.toString();
            info.path = filePath;
            info.baseAddress = binary->imagebase();
            info.imageSize = binary->virtual_size();
            info.checksum = QString::number(binary->compute_checksum(), 16);
            std::time_t timestamp = binary->header().time_date_stamp();
            info.compileTime = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(timestamp));

            qDebug() << "File:" << info.name;
            qDebug() << "Base Address:" << QString::number(info.baseAddress, 16);
            qDebug() << "Image Size:" << QString::number(info.imageSize);
            qDebug() << "File Version:" << info.fileVersion;
            qDebug() << "Company:" << info.companyName;
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
