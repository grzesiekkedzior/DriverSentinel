#ifndef CERTIFICATEINFO_H
#define CERTIFICATEINFO_H
#include <QString>

struct CertificateInfo
{
    QString subject;
    QString issuer;
    QString serialNumber;
    QString validFrom;
    QString validTo;
    QString signatureAlgorithm;
    QString fingerprintSHA1;
    QString fingerprintSHA256;
    QString keyUsage;
    int version;
    QString publicKeyHex;
};

#endif // CERTIFICATEINFO_H
