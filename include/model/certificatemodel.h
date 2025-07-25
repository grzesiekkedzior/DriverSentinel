#ifndef CERTIFICATEMODEL_H
#define CERTIFICATEMODEL_H
#include <QAbstractTableModel>
#include <QObject>
#include "data/certificateinfo.h"

class CertificateModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    CertificateModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
    enum class CertificateColumn {
        Subject,
        Issuer,
        SerialNumber,
        ValidFrom,
        ValidTo,
        SignatureAlgorithm,
        FingerprintSHA1,
        FingerprintSHA256,
        KeyUsage,
        Version,
        KeyType,
        ExtendedKeyUsage,
        Count
    };

public:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setCertificateInfo(const QVector<CertificateInfo> &certificateInfo);
    void clear();

private:
    QVector<CertificateInfo> m_certificateInfo;
};

#endif // CERTIFICATEMODEL_H
