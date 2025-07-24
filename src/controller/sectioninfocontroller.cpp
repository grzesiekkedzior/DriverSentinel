#include "controller/sectioninfocontroller.h"
#include "ui_mainwindow.h"
#include <LIEF/PE.hpp>

// The user selects a row in the main table
// → currentRowChanged(index) is emitted
// → loadPESectionDataToView(index) is called
// → the data is parsed and stored in m_sectionVectorInfo
// → updateModel() is called → the data is passed to the model → the view gets updated
SectionInfoController::SectionInfoController(QSharedPointer<SectionInfo> sectionInfo,
                                             QTableView *mainTableView,
                                             Ui::MainWindow *ui,
                                             QObject *parent)
    : m_sectionInfo{sectionInfo}
    , m_ui{ui}
    , m_mainTableView{mainTableView}
{
    m_sectionInfoModel = QSharedPointer<SectionInfoModel>::create();
    m_ui->peSectionView->setModel(this->sectionInfoModel().data());
    m_ui->peSectionView->resizeColumnsToContents();
}

void SectionInfoController::updateModel()
{
    if (!m_sectionInfoModel)
        return;
    m_sectionInfoModel->setPESecton(m_sectionVectorInfo);
}

QVariant SectionInfoController::extractFileNameFromRow(const QModelIndex &index, int column)
{
    if (!m_mainTableView || !m_mainTableView->model() || !index.isValid())
        return {};

    QModelIndex nameIndex = m_mainTableView->model()->index(index.row(), column);
    return m_mainTableView->model()->data(nameIndex, Qt::DisplayRole);
}

QString SectionInfoController::getPEfilePath(const QModelIndex &index)
{
    QVariant nameData = extractFileNameFromRow(index, 0);
    QString filePath = extractFileNameFromRow(index, 1).toString();
    filePath.replace(SystemRoot, C_WindowsPath);

    return filePath;
}

void SectionInfoController::loadPESectionDataToView(const QModelIndex &index)
{
    QVector<SectionInfo> sectionInfoLocal;
    if (!index.isValid())
        return;

    QString filePath = getPEfilePath(index);

    if (filePath.isEmpty()) {
        qWarning() << "File path is empty";
        return;
    }

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
    updateModel();
}

void SectionInfoController::refresh() {}

void SectionInfoController::clear()
{
    if (m_sectionInfoModel)
        m_sectionInfoModel->setPESecton({});
}

QSharedPointer<SectionInfoModel> SectionInfoController::sectionInfoModel() const
{
    return m_sectionInfoModel;
}
