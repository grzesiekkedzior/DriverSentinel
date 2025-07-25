#include "model/certificatemodel.h"

CertificateModel::CertificateModel(QObject *parent)
    : QAbstractTableModel(parent)
{}
int CertificateModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(CertificateColumn::Count);
}

int CertificateModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 2;
}

QVariant CertificateModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    if (m_certificateInfo.isEmpty())
        return {};

    const CertificateInfo &ci = m_certificateInfo.first();
    int row = index.row();
    int col = index.column();

    auto certificateColumn = static_cast<CertificateColumn>(row);

    if (col == 0) {
        switch (certificateColumn) {
        case CertificateColumn::Subject:
            return "Subject";
        case CertificateColumn::Issuer:
            return "Issuer";
        case CertificateColumn::SerialNumber:
            return "Serial Number";
        case CertificateColumn::ValidFrom:
            return "Valid From";
        case CertificateColumn::ValidTo:
            return "Valid To";
        case CertificateColumn::SignatureAlgorithm:
            return "Signature Algorithm";
        case CertificateColumn::FingerprintSHA1:
            return "SHA-1 Fingerprint";
        case CertificateColumn::FingerprintSHA256:
            return "SHA-256 Fingerprint";
        case CertificateColumn::KeyUsage:
            return "Key Usage";
        case CertificateColumn::Version:
            return "Version";
        case CertificateColumn::KeyType:
            return "Key Type";
        case CertificateColumn::ExtendedKeyUsage:
            return "Extended Key Usage";
        default:
            return {};
        }
    } else if (col == 1) {
        switch (certificateColumn) {
        case CertificateColumn::Subject:
            return ci.subject;
        case CertificateColumn::Issuer:
            return ci.issuer;
        case CertificateColumn::SerialNumber:
            return ci.serialNumber;
        case CertificateColumn::ValidFrom:
            return ci.validFrom;
        case CertificateColumn::ValidTo:
            return ci.validTo;
        case CertificateColumn::SignatureAlgorithm:
            return ci.signatureAlgorithm;
        case CertificateColumn::FingerprintSHA1:
            return ci.fingerprintSHA1;
        case CertificateColumn::FingerprintSHA256:
            return ci.fingerprintSHA256;
        case CertificateColumn::KeyUsage:
            return ci.keyUsage;
        case CertificateColumn::Version:
            return ci.version;
        case CertificateColumn::KeyType:
            return ci.keyType;
        case CertificateColumn::ExtendedKeyUsage:
            return ci.extendedKeyUsage;
        default:
            return {};
        }
    }

    return {};
}

QVariant CertificateModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    if (section == 0)
        return "Field";
    else if (section == 1)
        return "Value";

    return {};
}

void CertificateModel::setCertificateInfo(const QVector<CertificateInfo> &certificateInfo)
{
    beginResetModel();
    m_certificateInfo = certificateInfo;
    endResetModel();
}

void CertificateModel::clear()
{
    beginResetModel();
    m_certificateInfo.clear();
    endResetModel();
}
