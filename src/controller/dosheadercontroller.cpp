#include "controller/dosheadercontroller.h"
#include "ui_mainwindow.h"
#include <LIEF/PE.hpp>
#include <utils/peutils.h>

DosHeaderController::DosHeaderController(QSharedPointer<DosHeader> dosHeader,
                                         QTableView *mainTableView,
                                         Ui::MainWindow *ui,
                                         QObject *parent)
    : QObject(parent)
    , m_dosHeader(std::move(dosHeader))
    , m_mainTableView(mainTableView)
    , m_ui(ui)
{
    connect(m_mainTableView, &QTableView::clicked, this, &DosHeaderController::loadDosHeaderInfo);
}

void DosHeaderController::loadDosHeaderInfo(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString filePath = PEUtils::getPEfilePath(m_ui->mainTable, index);

    DosHeader ds;

    try {
        std::unique_ptr<LIEF::PE::Binary> binary = LIEF::PE::Parser::parse(filePath.toStdString());

        if (binary) {
            const auto &dosHeader = binary->dos_header();

            ds.magic = dosHeader.MAGIC;
            ds.usedBytesInLastPage = dosHeader.used_bytes_in_last_page();
            ds.fileSizeInPages = dosHeader.file_size_in_pages();
            ds.numberOfRelocations = dosHeader.numberof_relocation();
            ds.headerSizeInParagraphs = dosHeader.header_size_in_paragraphs();
            ds.minExtraParagraphs = dosHeader.minimum_extra_paragraphs();
            ds.maxExtraParagraphs = dosHeader.maximum_extra_paragraphs();
            ds.initialRelativeSs = dosHeader.initial_relative_ss();
            ds.initialSp = dosHeader.initial_sp();
            ds.checksum = dosHeader.checksum();
            ds.initialIp = dosHeader.initial_ip();
            ds.initialRelativeCs = dosHeader.initial_relative_cs();
            ds.addressRelocationTable = dosHeader.addressof_relocation_table();
            ds.overlayNumber = dosHeader.overlay_number();
            std::copy_n(dosHeader.reserved().begin(), ds.reserved.size(), ds.reserved.begin());
            ds.oemId = dosHeader.oem_id();
            ds.oemInfo = dosHeader.oem_info();
            std::copy_n(dosHeader.reserved2().begin(), ds.reserved2.size(), ds.reserved2.begin());
            ds.addressNewExeHeader = dosHeader.addressof_new_exeheader();
            loadDosHeaderDataToLabel(ds);
        } else {
            qWarning() << "LIEF: failed to parse binary";
        }
    } catch (const std::exception &e) {
        qWarning() << "LIEF error:" << e.what();
    } catch (...) {
        qWarning() << "Unknown error while parsing PE";
    }
}

void DosHeaderController::clear()
{
    const QString dash = "-";

    m_ui->magic->setText(dash);
    m_ui->bytesInlastPage->setText(dash);
    m_ui->sizeInPage->setText(dash);
    m_ui->numOfRelocation->setText(dash);
    m_ui->sizeInParagraphs->setText(dash);
    m_ui->minExtraParagraphs->setText(dash);
    m_ui->maxExtraParagraphs->setText(dash);
    m_ui->initialSS->setText(dash);
    m_ui->initialSP->setText(dash);
    m_ui->checksum_2->setText(dash);
    m_ui->initialIP->setText(dash);
    m_ui->initialCS->setText(dash);
    m_ui->addressRelocationTable->setText(dash);
    m_ui->overlayNumber->setText(dash);
    m_ui->reservedWorlds4x->setText(dash);
    m_ui->reservedWorld10x->setText(dash);
    m_ui->addressNewExeHeader->setText(dash);
}

void DosHeaderController::loadDosHeaderDataToLabel(DosHeader ds)
{
    auto formatWord = [](quint32 value, int width = 4) {
        return QString("0x%1").arg(value, width, 16, QChar('0')).toUpper().replace("X", "x");
    };

    m_ui->magic->setText(formatWord(ds.magic));
    m_ui->bytesInlastPage->setText(QString::number(ds.usedBytesInLastPage));
    m_ui->sizeInPage->setText(QString::number(ds.fileSizeInPages));
    m_ui->numOfRelocation->setText(QString::number(ds.numberOfRelocations));
    m_ui->sizeInParagraphs->setText(QString::number(ds.headerSizeInParagraphs));
    m_ui->minExtraParagraphs->setText(QString::number(ds.minExtraParagraphs)); // zmiana
    m_ui->maxExtraParagraphs->setText(QString::number(ds.maxExtraParagraphs));
    m_ui->initialSS->setText(formatWord(ds.initialRelativeSs));
    m_ui->initialSP->setText(formatWord(ds.initialSp));
    m_ui->checksum_2->setText(formatWord(ds.checksum));
    m_ui->initialIP->setText(formatWord(ds.initialIp));
    m_ui->initialCS->setText(formatWord(ds.initialRelativeCs));
    m_ui->addressRelocationTable->setText(formatWord(ds.addressRelocationTable));
    m_ui->overlayNumber->setText(QString::number(ds.overlayNumber));

    QString reservedStr;
    for (auto val : ds.reserved) {
        reservedStr += formatWord(val) + " ";
    }
    m_ui->reservedWorlds4x->setText(reservedStr.trimmed());

    QString reserved2Str;
    for (auto val : ds.reserved2) {
        reserved2Str += formatWord(val) + " ";
    }
    m_ui->reservedWorld10x->setText(reserved2Str.trimmed());
    m_ui->addressNewExeHeader->setText(formatWord(ds.addressNewExeHeader));
}
