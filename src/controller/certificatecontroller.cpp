#include "controller/certificatecontroller.h"
#include "ui_mainwindow.h"
#include <LIEF/PE/signature/RsaInfo.hpp>
#include <LIEF/PE/signature/x509.hpp>
#include <utils/peutils.h>

CertificateController::CertificateController(QSharedPointer<CertificateInfo> certificateInfo,
                                             QTableView *mainTableView,
                                             Ui::MainWindow *ui,
                                             QObject *parent)
    : QObject(parent)
    , m_certificateInfo(std::move(certificateInfo))
    , m_mainTableView(mainTableView)
    , m_ui(ui)
{
    m_certificateModel = QSharedPointer<CertificateModel>::create();
}

QSharedPointer<CertificateModel> CertificateController::certificateModel() const
{
    return m_certificateModel;
}

auto dateToQString = [](const LIEF::PE::x509::date_t &date) -> QString {
    return QString("%1-%2-%3 %4:%5:%6")
        .arg(date[0])                     // year
        .arg(date[1], 2, 10, QChar('0'))  // month
        .arg(date[2], 2, 10, QChar('0'))  // day
        .arg(date[3], 2, 10, QChar('0'))  // hour
        .arg(date[4], 2, 10, QChar('0'))  // minute
        .arg(date[5], 2, 10, QChar('0')); // second
};

void CertificateController::loadCertificateDataToView(const QModelIndex &index)
{
    QVector<CertificateInfo> certificateInfoLocal;

    if (!index.isValid())
        return;

    QString filePath = PEUtils::getPEfilePath(m_ui->mainTable, index);

    if (filePath.isEmpty()) {
        qWarning() << "File path is empty";
        return;
    }

    try {
        std::unique_ptr<LIEF::PE::Binary> binary = LIEF::PE::Parser::parse(filePath.toStdString());

        if (!binary->has_signatures()) {
            qWarning() << "No digital signature found in file.";
            updateModel({});
            return;
        }

        const auto &signatures = binary->signatures();

        for (const auto &sig : signatures) {
            for (const auto &cert : sig.certificates()) {
                CertificateInfo ci;

                ci.subject = QString::fromStdString(cert.subject());
                ci.issuer = QString::fromStdString(cert.issuer());
                ci.serialNumber = toHexString(cert.serial_number());

                auto from = cert.valid_from();
                ci.validFrom = dateToQString(cert.valid_from());
                ci.validTo = dateToQString(cert.valid_to());
                ci.signatureAlgorithm = QString::fromStdString(cert.signature_algorithm());
                auto cert_data = cert.raw(); // raw data from certificate
                std::vector<uint8_t> sha1_hash
                    = LIEF::PE::Signature::hash(cert_data, LIEF::PE::ALGORITHMS::SHA_1);
                ci.fingerprintSHA1 = toHexColonString(sha1_hash);

                std::vector<uint8_t> sha256_hash
                    = LIEF::PE::Signature::hash(cert_data, LIEF::PE::ALGORITHMS::SHA_256);
                ci.fingerprintSHA256 = toHexColonString(sha256_hash);
                ci.keyUsage = formatKeyUsage(cert);

                ci.version = static_cast<int>(cert.version());
                ci.keyType = formatKeyType(cert);
                ci.extendedKeyUsage = formatExtendedKeyUsage(cert);
                certificateInfoLocal.append(ci);
            }
        }

    } catch (const std::exception &e) {
        qWarning() << "LIEF error:" << e.what();
    } catch (...) {
        qWarning() << "Unknown error while parsing PE";
    }

    updateModel(certificateInfoLocal);
}

void CertificateController::updateModel(const QVector<CertificateInfo> &certificateInfo)
{
    if (!m_certificateModel)
        return;
    m_certificateModel->setCertificateInfo(certificateInfo);
}

void CertificateController::clear()
{
    if (m_certificateModel)
        m_certificateModel->setCertificateInfo({});
}

QString CertificateController::toHexString(const std::vector<uint8_t> &data)
{
    QString result;
    for (uint8_t byte : data) {
        result += QString("%1").arg(byte, 2, 16, QLatin1Char('0')).toUpper();
    }
    return result;
}

QString CertificateController::toHexColonString(const std::vector<uint8_t> &data)
{
    QString result;
    for (size_t i = 0; i < data.size(); ++i) {
        if (i != 0) {
            result += ":";
        }
        result += QString("%1").arg(data[i], 2, 16, QChar('0')).toUpper();
    }
    return result;
}

QString CertificateController::formatKeyUsage(const LIEF::PE::x509 &cert)
{
    qDebug() << "Format Key Usage";
    QStringList usages;
    for (auto usage : cert.key_usage()) {
        switch (usage) {
        case LIEF::PE::x509::KEY_USAGE::DIGITAL_SIGNATURE:
            usages << "Digital Signature";
            break;
        case LIEF::PE::x509::KEY_USAGE::NON_REPUDIATION:
            usages << "Non Repudiation";
            break;
        case LIEF::PE::x509::KEY_USAGE::KEY_ENCIPHERMENT:
            usages << "Key Encipherment";
            break;
        case LIEF::PE::x509::KEY_USAGE::DATA_ENCIPHERMENT:
            usages << "Data Encipherment";
            break;
        case LIEF::PE::x509::KEY_USAGE::KEY_AGREEMENT:
            usages << "Key Agreement";
            break;
        case LIEF::PE::x509::KEY_USAGE::KEY_CERT_SIGN:
            usages << "Certificate Signing";
            break;
        case LIEF::PE::x509::KEY_USAGE::CRL_SIGN:
            usages << "CRL Signing";
            break;
        case LIEF::PE::x509::KEY_USAGE::ENCIPHER_ONLY:
            usages << "Encipher Only";
            break;
        case LIEF::PE::x509::KEY_USAGE::DECIPHER_ONLY:
            usages << "Decipher Only";
            break;
        default:
            usages << "Unknown";
            break;
        }
    }

    if (usages.isEmpty()) {
        return "Empty";
    }

    return usages.join(", ");
}

QString CertificateController::formatKeyType(const LIEF::PE::x509 &cert)
{
    using KEY_TYPES = LIEF::PE::x509::KEY_TYPES;

    switch (cert.key_type()) {
    case KEY_TYPES::NONE:
        return "None";
    case KEY_TYPES::RSA:
        return "RSA";
    case KEY_TYPES::ECKEY:
        return "Elliptic Curve";
    case KEY_TYPES::ECKEY_DH:
        return "Elliptic Curve DH";
    case KEY_TYPES::ECDSA:
        return "ECDSA";
    case KEY_TYPES::RSA_ALT:
        return "RSA (Alt)";
    case KEY_TYPES::RSASSA_PSS:
        return "RSASSA-PSS";
    default:
        return "Unknown";
    }
}

QString CertificateController::formatExtendedKeyUsage(const LIEF::PE::x509 &cert)
{
    QStringList oids;
    for (const auto &oid : cert.ext_key_usage()) {
        oids << QString::fromStdString(oid);
    }
    return oids.join(", ");
}
