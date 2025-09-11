#include "controller/relocationcontroller.h"
#include "ui_mainwindow.h"

#include "utils/peutils.h"
#include <LIEF/PE/Binary.hpp>
#include <LIEF/PE/Parser.hpp>
#include <LIEF/PE/Relocation.hpp>
#include <LIEF/PE/RelocationEntry.hpp>

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
    connect(m_ui->blockRelocationTableView,
            &QTableView::pressed,
            this,
            &RelocationController::onBlockRelocationClicked);
}

void RelocationController::loadRelocationDataToView(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString filePath = PEUtils::getPEfilePath(m_ui->mainTable, index);
    if (filePath.isEmpty()) {
        qWarning() << "File path is empty";
        return;
    }

    QVector<LIEF_RELOCATION::RelocationBlockInfo> l_relocationBlockInfo;

    try {
        std::unique_ptr<LIEF::PE::Binary> binary = LIEF::PE::Parser::parse(filePath.toStdString());

        if (!binary->has_relocations()) {
            qWarning() << "No relocations found in binary";
            clear();
            return;
        }

        for (auto &reloc : binary->relocations()) {
            LIEF_RELOCATION::RelocationBlockInfo blockInfo;
            blockInfo.pageRVA = reloc.virtual_address();
            blockInfo.blockSize = reloc.block_size();
            blockInfo.entriesCount = reloc.entries().size();

            LIEF_RELOCATION::RelocationEntryInfo entryInfo;
            for (auto &entry : reloc.entries()) {
                entryInfo.offsetInPage = entry.position();
                switch (entry.type()) {
                case LIEF::PE::RelocationEntry::BASE_TYPES::ABS:
                    entryInfo.type = "ABS";
                    break;
                case LIEF::PE::RelocationEntry::BASE_TYPES::HIGH:
                    entryInfo.type = "HIGH";
                    break;
                case LIEF::PE::RelocationEntry::BASE_TYPES::LOW:
                    entryInfo.type = "LOW";
                    break;
                case LIEF::PE::RelocationEntry::BASE_TYPES::HIGHLOW:
                    entryInfo.type = "HIGHLOW";
                    break;
                case LIEF::PE::RelocationEntry::BASE_TYPES::HIGHADJ:
                    entryInfo.type = "HIGHADJ";
                    break;
                case LIEF::PE::RelocationEntry::BASE_TYPES::DIR64:
                    entryInfo.type = "DIR64";
                    break;
                default:
                    entryInfo.type = "UNKNOWN";
                    break;
                }

                entryInfo.relocRVA = reloc.virtual_address() + entryInfo.offsetInPage;
                entryInfo.value = entry.data();
                blockInfo.entries.append(entryInfo);
            }

            l_relocationBlockInfo.push_back(blockInfo);
        }
        m_relocationBlockInfo = l_relocationBlockInfo;
        update(l_relocationBlockInfo);

    } catch (const std::exception &e) {
        qWarning() << "LIEF error:" << e.what();
        return;
    } catch (...) {
        qWarning() << "Unknown error while parsing PE";
        return;
    }

    update(l_relocationBlockInfo);
}

void RelocationController::clear()
{
    if (!m_relocationBlocksModel || !m_relocationEntriesModel)
        return;
    m_relocationBlocksModel->setRelocationBlocksToView({});
    m_relocationEntriesModel->setRelocationEntriesToView({});
    m_relocationBlockInfo.clear();
}

void RelocationController::update(const QVector<LIEF_RELOCATION::RelocationBlockInfo> &ri)
{
    if (ri.isEmpty())
        return;

    m_relocationBlocksModel->setRelocationBlocksToView(ri);
    m_relocationEntriesModel->setRelocationEntriesToView(ri.first().entries);
}

void RelocationController::onBlockRelocationClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    int row = index.row();
    qDebug() << "Pressed row:" << row;

    if (row < m_relocationBlockInfo.size()) {
        const auto &entries = m_relocationBlockInfo.at(row).entries;
        if (!entries.isEmpty()) {
            m_relocationEntriesModel->setRelocationEntriesToView(entries);
        } else {
            m_relocationEntriesModel->setRelocationEntriesToView({});
        }
    } else {
        clear();
    }
}

QSharedPointer<RelocationBlocksModel> RelocationController::relocationBlocksModel() const
{
    return m_relocationBlocksModel;
}

QSharedPointer<RelocationEntriesModel> RelocationController::relocationEntriesModel() const
{
    return m_relocationEntriesModel;
}
