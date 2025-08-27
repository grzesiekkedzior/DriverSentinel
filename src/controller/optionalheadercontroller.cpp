#include "controller/optionalheadercontroller.h"
#include "ui_mainwindow.h"
#include <LIEF/PE.hpp>
#include <utils/peutils.h>

OptionalHeaderController::OptionalHeaderController(QSharedPointer<OptionalHeaderInfo> optionalHeader,
                                                   QTableView *mainTableView,
                                                   Ui::MainWindow *ui,
                                                   QObject *parent)
    : QObject(parent)
    , m_optionalHeader(std::move(optionalHeader))
    , m_mainTableView(mainTableView)
    , m_ui(ui)
{
    connect(m_mainTableView,
            &QTableView::clicked,
            this,
            &OptionalHeaderController::loadOptionalHeaderInfo);
}

uint32_t OptionalHeaderController::getOptionalOffsetAddress(
    const std::unique_ptr<LIEF::PE::Binary> &binary)
{
    auto &dos = binary->dos_header();
    uint32_t e_lfanew = dos.addressof_new_exeheader();
    uint32_t optionalHeaderOffset = e_lfanew + 4 + 20;

    return optionalHeaderOffset;
}

void OptionalHeaderController::loadOptionalHeaderInfo(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString filePath = PEUtils::getPEfilePath(m_ui->mainTable, index);

    OptionalHeaderInfo oh;

    try {
        std::unique_ptr<LIEF::PE::Binary> binary = LIEF::PE::Parser::parse(filePath.toStdString());

        if (binary) {
            uint32_t optionalHeaderOffset = getOptionalOffsetAddress(binary);
            auto optionalHeader = binary->optional_header();

            auto magicEnum = binary->optional_header().magic();
            std::vector<int> fieldSizes;

            if (magicEnum == LIEF::PE::PE_TYPE::PE32) {
                qDebug() << "PE32 (32-bit)";
                fieldSizes = {
                    2, // magic
                    1, // majorLinkerVersion
                    1, // minorLinkerVersion
                    4, // sizeOfCode
                    4, // sizeOfInitializedData
                    4, // sizeOfUninitializedData
                    4, // entryPoint
                    4, // baseOfCode
                    4, // baseOfData (ONLY IN PE32)
                    4, // imageBase (32-bit)
                    4, // sectionAlign
                    4, // fileAlign
                    2, // majorOSVersion
                    2, // minorOSVersion
                    2, // majorImageVersion
                    2, // minorImageVersion
                    2, // majorSubsystemVersion
                    2, // minorSubsystemVersion
                    4, // win32VersionValue
                    4, // sizeOfImage
                    4, // sizeOfHeaders
                    4, // checksum
                    2, // subsystem
                    2, // dllCharacteristics
                    4, // sizeOfStackReserve (32-bit)
                    4, // sizeOfStackCommit (32-bit)
                    4, // sizeOfHeapReserve (32-bit)
                    4, // sizeOfHeapCommit (32-bit)
                    4, // loaderFlags
                    4  // numberOfRvaAndSizes
                };
            } else { // PE32+ (64-bit)
                qDebug() << "PE32+ (64-bit)";
                fieldSizes = {
                    2, // magic
                    1, // majorLinkerVersion
                    1, // minorLinkerVersion
                    4, // sizeOfCode
                    4, // sizeOfInitializedData
                    4, // sizeOfUninitializedData
                    4, // entryPoint
                    4, // baseOfCode
                    // NO baseOfData IN PE32+
                    8, // imageBase (64-bit)
                    4, // sectionAlign
                    4, // fileAlign
                    2, // majorOSVersion
                    2, // minorOSVersion
                    2, // majorImageVersion
                    2, // minorImageVersion
                    2, // majorSubsystemVersion
                    2, // minorSubsystemVersion
                    4, // win32VersionValue
                    4, // sizeOfImage
                    4, // sizeOfHeaders
                    4, // checksum
                    2, // subsystem
                    2, // dllCharacteristics
                    8, // sizeOfStackReserve (64-bit)
                    8, // sizeOfStackCommit (64-bit)
                    8, // sizeOfHeapReserve (64-bit)
                    8, // sizeOfHeapCommit (64-bit)
                    4, // loaderFlags
                    4  // numberOfRvaAndSizes
                };
            }
            int offset = optionalHeaderOffset;
            int fieldIndex = 0;

            int totalFields = fieldSizes.size();
            bool skipBaseOfData = (magicEnum == LIEF::PE::PE_TYPE::PE32_PLUS);

            for (int guiIndex = 0; guiIndex < totalFields + (skipBaseOfData ? 1 : 0); ++guiIndex) {
                if (skipBaseOfData && guiIndex == 8) {
                    continue;
                }

                QString objectName = QString("off_%1").arg(guiIndex + 1);
                QLabel *label = m_ui->tabWidget->findChild<QLabel *>(objectName);
                if (label) {
                    label->setText(QString("0x%1").arg(offset, 4, 16, QLatin1Char('0')));
                    // qDebug() << "Field" << (guiIndex + 1) << "Offset: 0x"
                    //          << QString::number(offset, 16);
                }

                offset += fieldSizes[fieldIndex++];
            }

            oh.magic = static_cast<quint16>(optionalHeader.magic());
            oh.majorLinkerVersion = optionalHeader.major_linker_version();
            oh.minorLinkerVersion = optionalHeader.minor_linker_version();
            oh.sizeOfCode = optionalHeader.sizeof_code();
            oh.sizeOfInitializedData = optionalHeader.sizeof_initialized_data();
            oh.sizeOfUninitializedData = optionalHeader.sizeof_uninitialized_data();
            oh.entryPoint = optionalHeader.addressof_entrypoint();
            oh.baseOfCode = optionalHeader.baseof_code();
            if (magicEnum == LIEF::PE::PE_TYPE::PE32_PLUS) {
                oh.baseOfData = static_cast<quint32>(-1);
            } else {
                oh.baseOfData = optionalHeader.baseof_data();
            }
            oh.imageBase = optionalHeader.imagebase();
            oh.sectionAlign = optionalHeader.section_alignment();
            oh.fileAlign = optionalHeader.file_alignment();
            oh.majorOSVersion = optionalHeader.major_operating_system_version();
            oh.minorOSVersion = optionalHeader.minor_operating_system_version();
            oh.majorImageVersion = optionalHeader.major_image_version();
            oh.minorImageVersion = optionalHeader.minor_image_version();
            oh.majorSubsystemVersion = optionalHeader.major_subsystem_version();
            oh.minorSubsystemVersion = optionalHeader.minor_subsystem_version();
            oh.win32VersionValue = optionalHeader.win32_version_value();
            oh.sizeOfImage = optionalHeader.sizeof_image();
            oh.sizeOfHeaders = optionalHeader.sizeof_headers();
            oh.checksum = optionalHeader.checksum();
            oh.subsystem = optionalHeader.subsystem();
            oh.dllCharacteristics = optionalHeader.dll_characteristics();
            oh.sizeOfStackReserve = optionalHeader.sizeof_stack_reserve();
            oh.sizeOfStackCommit = optionalHeader.sizeof_stack_commit();
            oh.sizeOfHeapReserve = optionalHeader.sizeof_heap_reserve();
            oh.sizeOfHeapCommit = optionalHeader.sizeof_heap_commit();
            oh.loaderFlags = optionalHeader.loader_flags();
            oh.numberOfRvaAndSizes = optionalHeader.numberof_rva_and_size();

            loadOptionalHeaderDataToLabel(oh);
        } else {
            qWarning() << "LIEF: failed to parse binary";
        }
    } catch (const std::exception &e) {
        qWarning() << "LIEF error:" << e.what();
    } catch (...) {
        qWarning() << "Unknown error while parsing PE";
    }
}

void OptionalHeaderController::clear()
{
    m_ui->optionalMagic->setText("-");
    m_ui->majorLinkerVersion->setText("-");
    m_ui->minorLinkerVersion->setText("-");
    m_ui->sizeOfCode->setText("-");
    m_ui->sizeOfInitializedData->setText("-");
    m_ui->sizeOfUninitializedData->setText("-");
    m_ui->entryPoint->setText("-");
    m_ui->baseOfCode->setText("-");
    m_ui->baseOfData->setText("-");
    m_ui->imageBase->setText("-");
    m_ui->sectionAlign->setText("-");
    m_ui->fileAlign->setText("-");
    m_ui->majorOSVersion->setText("-");
    m_ui->minorOSVersion->setText("-");
    m_ui->majorImageVersion->setText("-");
    m_ui->minorImageVersion->setText("-");
    m_ui->majorSubsystemVersion->setText("-");
    m_ui->minorSubsystemVersion->setText("-");
    m_ui->win32VersionValue->setText("-");
    m_ui->sizeOfImage->setText("-");
    m_ui->sizeOfHeaders->setText("-");
    m_ui->checksumOptional->setText("-");
    m_ui->subsystem->setText("-");
    m_ui->dllCharacteristics->setText("-");
    m_ui->sizeOfStackReserve->setText("-");
    m_ui->sizeOfStackCommit->setText("-");
    m_ui->sizeOfHeapReserve_2->setText("-");
    m_ui->sizeOfHeapCommit->setText("-");
    m_ui->loaderFlags->setText("-");
    m_ui->numberOfRvaAndSizes->setText("-");

    for (int i = 0; i < 30; ++i) {
        QString objectName = QString("off_%1").arg(i + 1);
        QLabel *label = m_ui->tabWidget->findChild<QLabel *>(objectName);
        if (label) {
            label->setText("-");
        }
    }
}

void OptionalHeaderController::loadOptionalHeaderDataToLabel(OptionalHeaderInfo oh)
{
    auto formatWord = [](quint64 value, int width = 8) {
        return QString("0x%1").arg(value, width, 16, QChar('0')).toUpper().replace("X", "x");
    };

    m_ui->optionalMagic->setText(formatWord(oh.magic, 4));

    m_ui->majorLinkerVersion->setText(formatWord(oh.majorLinkerVersion, 2));
    m_ui->minorLinkerVersion->setText(formatWord(oh.minorLinkerVersion, 2));

    m_ui->sizeOfCode->setText(formatWord(oh.sizeOfCode));
    m_ui->sizeOfInitializedData->setText(formatWord(oh.sizeOfInitializedData));
    m_ui->sizeOfUninitializedData->setText(formatWord(oh.sizeOfUninitializedData));
    m_ui->entryPoint->setText(formatWord(oh.entryPoint));
    m_ui->baseOfCode->setText(formatWord(oh.baseOfCode));

    if (oh.baseOfData == static_cast<quint32>(-1)) {
        m_ui->baseOfData->setText("-");
    } else {
        m_ui->baseOfData->setText(formatWord(oh.baseOfData));
    }

    m_ui->imageBase->setText(formatWord(oh.imageBase, 16));
    m_ui->sectionAlign->setText(formatWord(oh.sectionAlign));
    m_ui->fileAlign->setText(formatWord(oh.fileAlign));

    m_ui->majorOSVersion->setText(formatWord(oh.majorOSVersion, 2));
    m_ui->minorOSVersion->setText(formatWord(oh.minorOSVersion, 2));
    m_ui->majorImageVersion->setText(formatWord(oh.majorImageVersion, 2));
    m_ui->minorImageVersion->setText(formatWord(oh.minorImageVersion, 2));
    m_ui->majorSubsystemVersion->setText(formatWord(oh.majorSubsystemVersion, 2));
    m_ui->minorSubsystemVersion->setText(formatWord(oh.minorSubsystemVersion, 2));

    m_ui->win32VersionValue->setText(formatWord(oh.win32VersionValue));
    m_ui->sizeOfImage->setText(formatWord(oh.sizeOfImage));
    m_ui->sizeOfHeaders->setText(formatWord(oh.sizeOfHeaders));
    m_ui->checksumOptional->setText(formatWord(oh.checksum, 8));

    m_ui->subsystem->setText(subsystemToString(oh.subsystem));

    m_ui->dllCharacteristics->setText(formatWord(oh.dllCharacteristics, 4));

    m_ui->sizeOfStackReserve->setText(formatWord(oh.sizeOfStackReserve, 16));
    m_ui->sizeOfStackCommit->setText(formatWord(oh.sizeOfStackCommit, 16));
    m_ui->sizeOfHeapReserve_2->setText(formatWord(oh.sizeOfHeapReserve, 16));
    m_ui->sizeOfHeapCommit->setText(formatWord(oh.sizeOfHeapCommit, 16));

    m_ui->loaderFlags->setText(formatWord(oh.loaderFlags));
    m_ui->numberOfRvaAndSizes->setText(formatWord(oh.numberOfRvaAndSizes));
}

QString OptionalHeaderController::subsystemToString(LIEF::PE::OptionalHeader::SUBSYSTEM s) const
{
    switch (s) {
    case LIEF::PE::OptionalHeader::SUBSYSTEM::UNKNOWN:
        return "Unknown";
    case LIEF::PE::OptionalHeader::SUBSYSTEM::NATIVE:
        return "Native";
    case LIEF::PE::OptionalHeader::SUBSYSTEM::WINDOWS_GUI:
        return "Windows GUI";
    case LIEF::PE::OptionalHeader::SUBSYSTEM::WINDOWS_CUI:
        return "Windows CUI";
    case LIEF::PE::OptionalHeader::SUBSYSTEM::OS2_CUI:
        return "OS/2 CUI";
    case LIEF::PE::OptionalHeader::SUBSYSTEM::POSIX_CUI:
        return "POSIX CUI";
    case LIEF::PE::OptionalHeader::SUBSYSTEM::NATIVE_WINDOWS:
        return "Native Windows 9x driver";
    case LIEF::PE::OptionalHeader::SUBSYSTEM::WINDOWS_CE_GUI:
        return "Windows CE GUI";
    case LIEF::PE::OptionalHeader::SUBSYSTEM::EFI_APPLICATION:
        return "EFI Application";
    case LIEF::PE::OptionalHeader::SUBSYSTEM::EFI_BOOT_SERVICE_DRIVER:
        return "EFI Boot Service Driver";
    case LIEF::PE::OptionalHeader::SUBSYSTEM::EFI_RUNTIME_DRIVER:
        return "EFI Runtime Driver";
    case LIEF::PE::OptionalHeader::SUBSYSTEM::EFI_ROM:
        return "EFI ROM";
    case LIEF::PE::OptionalHeader::SUBSYSTEM::XBOX:
        return "XBOX";
    case LIEF::PE::OptionalHeader::SUBSYSTEM::WINDOWS_BOOT_APPLICATION:
        return "Windows Boot Application";
    default:
        return QString("Unknown (%1)").arg(static_cast<size_t>(s));
    }
}
