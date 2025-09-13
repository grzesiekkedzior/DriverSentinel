#include "controller/fileinfocontroller.h"
#include "ui_mainwindow.h"

#include <LIEF/PE/Parser.hpp>
#include <utils/peutils.h>

#include <LIEF/PE/Binary.hpp>

FileInfoController::FileInfoController(QTableView *mainTableView,
                                       Ui::MainWindow *ui,
                                       QObject *parent)
    : QObject(parent)
    , m_mainTableView(mainTableView)
    , m_ui(ui)
    , m_fileInfo{QSharedPointer<FileInfo>::create()}
    , m_fileModel{QSharedPointer<FileModel>::create()}
{
    connect(m_mainTableView, &QTableView::clicked, this, &FileInfoController::loadFileInfoToView);
    // m_ui->fileHeaderTableView->horizontalHeader()->setSectionResizeMode(
    //     QHeaderView::ResizeToContents);
}

void FileInfoController::loadFileInfoToView(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString filePath = PEUtils::getPEfilePath(m_ui->mainTable, index);

    QVector<FileInfo> l_fileInfo;
    try {
        std::unique_ptr<LIEF::PE::Binary> binary = LIEF::PE::Parser::parse(filePath.toStdString());
        FileInfo fi;
        if (binary) {
            auto fileHdr = binary->header();

            fi.Machine = static_cast<quint16>(fileHdr.machine());
            fi.NumberOfSections = fileHdr.numberof_sections();
            fi.TimeDateStamp = fileHdr.time_date_stamp();
            fi.PointerToSymbolTable = fileHdr.pointerto_symbol_table();
            fi.NumberOfSymbols = fileHdr.numberof_symbols();
            fi.SizeOfOptionalHeader = fileHdr.sizeof_optional_header();
            fi.Characteristics = fileHdr.characteristics();
            l_fileInfo.append(fi);
        } else {
            qWarning() << "LIEF: failed to parse binary";
        }
        update(l_fileInfo);
    } catch (const std::exception &e) {
        qWarning() << "LIEF error:" << e.what();
    } catch (...) {
        qWarning() << "Unknown error while parsing PE";
    }
}

void FileInfoController::update(const QVector<FileInfo> &fm)
{
    if (m_fileModel)
        m_fileModel->loadFileHeaderDataToView(fm);
}

void FileInfoController::clear()
{
    if (m_fileModel)
        m_fileModel->clear();
}

QSharedPointer<FileModel> FileInfoController::fileModel() const
{
    return m_fileModel;
}
