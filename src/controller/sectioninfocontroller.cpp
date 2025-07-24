#include "controller/sectioninfocontroller.h"
#include <LIEF/PE.hpp>

SectionInfoController::SectionInfoController(QSharedPointer<SectionInfo> sectionInfo,
                                             QTableView *mainTableView,
                                             Ui::MainWindow *ui,
                                             QObject *parent)
    : m_sectionInfo{sectionInfo}
    , m_mainTableView{mainTableView}
    , m_ui{ui}
{
    m_sectionInfoModel = QSharedPointer<SectionInfoModel>::create();
}

void SectionInfoController::start()
{
    if (!m_sectionInfoModel)
        return;
    m_sectionInfoModel->setPESecton(m_sectionVectorInfo);
}

QVariant SectionInfoController::SectionInfoController::extractFileNameFromRow(
    const QModelIndex &index, int column)
{
    int row = index.row();

    QModelIndex nameIndex = m_mainTableView->model()->index(row, column);
    QVariant nameData = m_mainTableView->model()->data(nameIndex, Qt::DisplayRole);

    return nameData;
}

void SectionInfoController::loadPESectionDataToView(const QModelIndex &index)
{
    QVector<SectionInfo> sectionInfoLocal;
    if (!index.isValid())
        return;

    QVariant nameData = extractFileNameFromRow(index, 0);
    QString filePath = extractFileNameFromRow(index, 1).toString();
    filePath.replace("\\SystemRoot", "C:\\Windows");

    try {
        std::unique_ptr<LIEF::PE::Binary> binary = LIEF::PE::Parser::parse(filePath.toStdString());
        const auto &sections = binary->sections();

        for (const auto &section : sections) {
            SectionInfo si;
            si.name = QString::fromStdString(section.name());
            si.virtualAddress = QString("0x%1").arg(section.virtual_address(), 0, 16);
            si.virtualSize = QString("0x%1").arg(section.virtual_size(), 0, 16);
            si.rawSize = QString("0x%1").arg(section.size(), 0, 16);
            si.pointerToRawData = QString("0x%1").arg(section.offset(), 0, 16);
            si.characteristics = QString("0x%1").arg(section.characteristics(), 8, 16).toUpper();
            si.entropy = QString::number(section.entropy(), 'f', 2);
            si.sectionAlignment = QString("0x%1").arg(binary->optional_header().section_alignment(),
                                                      0,
                                                      16);
            si.pointerToRelocations = QString("0x%1").arg(section.pointerto_relocation(), 0, 16);
            si.numberOfRelocations = QString::number(section.numberof_relocations());
            si.characteristicsDesc = si.characteristicsDescription();
            sectionInfoLocal.append(si);
        }

    } catch (const std::exception &e) {
        qWarning() << "LIEF error:" << e.what();
    } catch (...) {
        qWarning() << "Unknown error while parsing PE";
    }

    m_sectionVectorInfo = sectionInfoLocal;
    start();
}

void SectionInfoController::refresh() {}

void SectionInfoController::clear() {}

QSharedPointer<SectionInfoModel> SectionInfoController::sectionInfoModel() const
{
    return m_sectionInfoModel;
}
