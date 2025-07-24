#include "controller/generalcontroller.h"
#include <QDir>
#include <QFileInfo>
#include "data/sectioninfo.h"
#include "ui_mainwindow.h"
#include <Windows.h>

GeneralController::GeneralController(QSharedPointer<GeneralInfo> generalInfo,
                                     QTableView *mainTableView,
                                     Ui::MainWindow *ui,
                                     QObject *parent)
    : m_generalInfo{generalInfo}
    , m_mainTableView(mainTableView)
    , m_ui{ui}
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

//Unused
QString GeneralController::extractFileVersion(const std::unique_ptr<LIEF::PE::Binary> &binary)
{
    // Example
    // file_version_ms = 0x000A0000; // (10 << 16) | 0
    // file_version_ls = 0x4A170001; // (19095 << 16) | 1
    if (binary->has_resources() && binary->resources_manager()->has_version()) {
        auto version = binary->resources_manager()->version();
        auto file_info = version[0].file_info();

        uint32_t ms = file_info.file_version_ms;
        uint32_t ls = file_info.file_version_ls;

        int major = (ms >> 16) & 0xFFFF;
        int minor = ms & 0xFFFF;
        int build = (ls >> 16) & 0xFFFF;
        int revision = ls & 0xFFFF;

        QString versionStr = QString("%1.%2.%3.%4").arg(major).arg(minor).arg(build).arg(revision);
        return versionStr;
    }
    return {};
}

//WinAPI unused
QString GeneralController::getFileDescriptionFromWinAPI(const QString &filePath)
{
    DWORD dummy;
    DWORD size = GetFileVersionInfoSizeW((LPCWSTR) filePath.unicode(), &dummy);
    if (size == 0)
        return {};

    std::vector<BYTE> data(size);
    if (!GetFileVersionInfoW((LPCWSTR) filePath.utf16(), 0, size, data.data()))
        return {};

    void *buffer = nullptr;
    UINT sizeBuf = 0;
    if (VerQueryValueW(
            data.data(),
            // https://learn.microsoft.com/en-us/windows/win32/menurc/stringfileinfo-block
            // BLOCK "StringFileInfo" { BLOCK "lang-charset" {VALUE "string-name", "value" . . . }}
            L"\\StringFileInfo\\040904b0\\FileDescription",
            &buffer,
            &sizeBuf)) {
        // -1 to remove \u0
        return QString::fromWCharArray(static_cast<wchar_t *>(buffer), sizeBuf - 1);
    }

    return {};
}

QString GeneralController::getPEFormatString(LIEF::PE::PE_TYPE type)
{
    switch (type) {
    case LIEF::PE::PE_TYPE::PE32:
        return "PE32 (32-bit)";
    case LIEF::PE::PE_TYPE::PE32_PLUS:
        return "PE32+ (64-bit)";
    default:
        return "?";
    }
}

QString GeneralController::getPEFileExtention(const LIEF::PE::Binary &binary)
{
    // enum FORMATS {
    //     UNKNOWN = 0,
    //     ELF,
    //     PE,
    //     MACHO,
    //     OAT,
    // };
    LIEF::Binary::FORMATS FileFormat = binary.format();
    switch (FileFormat) {
    case LIEF::Binary::FORMATS::ELF:
        return "ELF";
        break;
    case LIEF::Binary::FORMATS::PE:
        return "PE";
        break;
    case LIEF::Binary::FORMATS::MACHO:
        return "MACHO";
        break;
    case LIEF::Binary::FORMATS::OAT:
        return "OAT";
        break;
    default:
        return "UNKNOWN";
        break;
    }
}

QString GeneralController::getImageSize(QString filePath)
{
    QFileInfo info{filePath};
    qint64 size = info.size();

    return QString("%1 KB").arg(size / 1024);
}

void GeneralController::loadGeneralDataToLabel(GeneralInfo gi)
{
    m_ui->originalFilename_e->setText(gi.originalFilename);
    m_ui->productName_e->setText(gi.productName);
    m_ui->legalCopyright_e->setText(gi.legalCopyright);
    m_ui->path_e->setText(gi.path);
    m_ui->baseAddress_e->setText(QString::number(gi.baseAddress, 16).toUpper());
    m_ui->virtualSize_e->setText(QString::number(gi.virtualSize));
    m_ui->imageSize_e->setText(gi.imageSize);
    m_ui->checksum_e->setText(gi.checksum);
    m_ui->productVersion_e->setText(gi.productVersion);
    m_ui->fileDescription_e->setText(gi.fileDescription);
    m_ui->fileType_e->setText(gi.fileType);
    m_ui->fileExtention_e->setText(gi.fileExtention);
    m_ui->companyName_e->setText(gi.companyName);
}

//For now this is the only expample function
void GeneralController::loadGeneralInfo(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QVariant nameData = extractFileNameFromRow(index, 0);
    QString filePath = extractFileNameFromRow(index, 1).toString();
    filePath.replace("\\SystemRoot", "C:\\Windows");

    GeneralInfo info;

    try {
        std::unique_ptr<LIEF::PE::Binary> binary = LIEF::PE::Parser::parse(filePath.toStdString());
        if (binary) {
            if (binary->has_resources()) {
                const auto &rm = binary->resources_manager();
                if (rm->has_version()) {
                    const auto &version = rm->version();
                    auto map = version.data()->string_file_info();
                    auto c = map->children();

                    auto getValueOrEmpty = [&](const auto &key) -> QString {
                        if (c.empty())
                            return QString{};
                        if (auto node = c->get(key); node && node->data()) {
                            try {
                                return QString::fromStdString(node->data());
                            } catch (...) {
                                return QString{};
                            }
                        }
                        return QString{};
                    };

                    info.internalName = getValueOrEmpty(InternalName);
                    info.originalFilename = getValueOrEmpty(OriginalFilename);
                    info.productName = getValueOrEmpty(ProductName);
                    info.legalCopyright = getValueOrEmpty(LegalCopyright);
                    info.path = filePath;
                    info.productVersion = getValueOrEmpty(ProductVersion);
                    info.fileDescription = getValueOrEmpty(FileDescription);
                    info.companyName = getValueOrEmpty(CompanyName);
                }
            }
            info.baseAddress = binary->imagebase();
            info.virtualSize = binary->virtual_size();
            info.imageSize = getImageSize(filePath);
            info.checksum = QString("0x") + QString::number(binary->compute_checksum(), 16);
            info.fileType = getPEFormatString(binary->type());
            info.fileExtention = getPEFileExtention(*binary);

            qDebug() << "==== PE File Info ====";
            qDebug() << "Internal Name:     " << info.internalName;
            qDebug() << "Original Filename: " << info.originalFilename;
            qDebug() << "Product Name:      " << info.productName;
            qDebug() << "Product Version:   " << info.productVersion;
            qDebug() << "File Description:  " << info.fileDescription;
            qDebug() << "Company Name:      " << info.companyName;
            qDebug() << "Legal Copyright:   " << info.legalCopyright;
            qDebug() << "Path:              " << info.path;
            qDebug() << "Base Address:      " << QString("0x%1").arg(info.baseAddress, 0, 16);
            qDebug() << "Virtual Size:      " << QString("0x%1").arg(info.virtualSize, 0, 16);
            qDebug() << "Image Size:        " << info.imageSize;
            qDebug() << "Checksum:          " << info.checksum;
            qDebug() << "File Type:         " << info.fileType;
            qDebug() << "File Extension:    " << info.fileExtention;

            loadGeneralDataToLabel(info);
        } else {
            qWarning() << "LIEF: failed to parse binary";
        }
    } catch (const std::exception &e) {
        qWarning() << "LIEF error:" << e.what();
    } catch (...) {
        qWarning() << "Unknown error while parsing PE";
    }
}

void GeneralController::clear()
{
    m_ui->originalFilename_e->clear();
    m_ui->productName_e->clear();
    m_ui->legalCopyright_e->clear();
    m_ui->path_e->clear();
    m_ui->baseAddress_e->clear();
    m_ui->virtualSize_e->clear();
    m_ui->imageSize_e->clear();
    m_ui->checksum_e->clear();
    m_ui->productVersion_e->clear();
    m_ui->fileDescription_e->clear();
    m_ui->fileType_e->clear();
    m_ui->fileExtention_e->clear();
    m_ui->companyName_e->clear();
}
