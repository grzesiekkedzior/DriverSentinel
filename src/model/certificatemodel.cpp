#include "model/certificatemodel.h"

CertificateModel::CertificateModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int CertificateModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_certificateInfo.size();
}

int CertificateModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(CertificateColumn::Count);
}

QVariant CertificateModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const auto &certificateInfo = m_certificateInfo.at(index.row());
    const auto certificateColumn = static_cast<CertificateColumn>(index.column());

    switch (certificateColumn) {
    case CertificateColumn::Subject:
        return certificateInfo.subject;
    case CertificateColumn::Issuer:
        return certificateInfo.issuer;
    case CertificateColumn::SerialNumber:
        return certificateInfo.serialNumber;
    case CertificateColumn::ValidFrom:
        return certificateInfo.validFrom;
    case CertificateColumn::ValidTo:
        return certificateInfo.validTo;
    case CertificateColumn::SignatureAlgorithm:
        return certificateInfo.signatureAlgorithm;
    case CertificateColumn::FingerprintSHA1:
        return certificateInfo.fingerprintSHA1;
    case CertificateColumn::FingerprintSHA256:
        return certificateInfo.fingerprintSHA256;
    case CertificateColumn::KeyUsage:
        return certificateInfo.keyUsage;
    case CertificateColumn::Version:
        return certificateInfo.version;
    case CertificateColumn::PublicKeyHex:
        return certificateInfo.publicKeyHex;
    default:
        return {};
    }
}

QVariant CertificateModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (static_cast<CertificateColumn>(section)) {
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
    case CertificateColumn::PublicKeyHex:
        return "Public Key (Hex)";
    default:
        return {};
    }
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
