#include "controller/relocationcontroller.h"
#include "ui_mainwindow.h"

#include <QFutureWatcher>
#include <QtConcurrent>
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
        clear();
        return;
    }

    auto watcher = new QFutureWatcher<QVector<LIEF_RELOCATION::RelocationBlockInfo>>(this);

    connect(watcher,
            &QFutureWatcher<QVector<LIEF_RELOCATION::RelocationBlockInfo>>::finished,
            this,
            [this, watcher]() {
                QVector<LIEF_RELOCATION::RelocationBlockInfo> blocks = watcher->result();
                m_relocationBlockInfo = blocks;

                if (blocks.isEmpty()) {
                    clear();
                } else {
                    update(blocks);
                }

                watcher->deleteLater();
            });

    watcher->setFuture(
        QtConcurrent::run([filePath]() -> QVector<LIEF_RELOCATION::RelocationBlockInfo> {
            QVector<LIEF_RELOCATION::RelocationBlockInfo> blocks;

            try {
                std::unique_ptr<LIEF::PE::Binary> binary(LIEF::PE::Parser::parse(
                    filePath.toStdString())); // = LIEF::PE::Parser::parse(filePath.toStdString());
                if (!binary->has_relocations())
                    return blocks;

                for (auto &reloc : binary->relocations()) {
                    LIEF_RELOCATION::RelocationBlockInfo blockInfo;
                    blockInfo.pageRVA = reloc.virtual_address();
                    blockInfo.blockSize = reloc.block_size();
                    blockInfo.entriesCount = reloc.entries().size();

                    for (auto &entry : reloc.entries()) {
                        LIEF_RELOCATION::RelocationEntryInfo e;
                        e.offsetInPage = entry.position();

                        switch (entry.type()) {
                        case LIEF::PE::RelocationEntry::BASE_TYPES::ABS:
                            e.type = "ABS";
                            break;
                        case LIEF::PE::RelocationEntry::BASE_TYPES::HIGH:
                            e.type = "HIGH";
                            break;
                        case LIEF::PE::RelocationEntry::BASE_TYPES::LOW:
                            e.type = "LOW";
                            break;
                        case LIEF::PE::RelocationEntry::BASE_TYPES::HIGHLOW:
                            e.type = "HIGHLOW";
                            break;
                        case LIEF::PE::RelocationEntry::BASE_TYPES::HIGHADJ:
                            e.type = "HIGHADJ";
                            break;
                        case LIEF::PE::RelocationEntry::BASE_TYPES::DIR64:
                            e.type = "DIR64";
                            break;
                        default:
                            e.type = "UNKNOWN";
                            break;
                        }

                        e.relocRVA = reloc.virtual_address() + e.offsetInPage;
                        e.value = entry.data();
                        blockInfo.entries.append(e);
                    }

                    blocks.push_back(blockInfo);
                }
            } catch (const std::exception &e) {
                qWarning() << "Exception parsing PE:" << e.what();
            } catch (...) {
                qWarning() << "Unknown exception parsing PE";
            }

            return blocks;
        }));
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
