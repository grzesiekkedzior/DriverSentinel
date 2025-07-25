#ifndef CERTIFICATECONTROLLER_H
#define CERTIFICATECONTROLLER_H
#include <QObject>
#include <QTableView>
#include "data/certificateinfo.h"
#include "model/certificatemodel.h"
#include <LIEF/PE.hpp>

namespace Ui {
class MainWindow;
}

class CertificateController : public QObject
{
    Q_OBJECT

public:
    CertificateController(QSharedPointer<CertificateInfo> certificateInfo,
                          QTableView *mainTableView,
                          Ui::MainWindow *ui,
                          QObject *parent = nullptr);

    QSharedPointer<CertificateModel> certificateModel() const;

public slots:
    void loadCertificateDataToView(const QModelIndex &index);
    void updateModel(const QVector<CertificateInfo> &certificateInfo);
    void clear();

private:
    QSharedPointer<CertificateInfo> m_certificateInfo;
    QSharedPointer<CertificateModel> m_certificateModel;
    QTableView *m_mainTableView{};
    Ui::MainWindow *m_ui;

    QString toHexString(const std::vector<uint8_t> &data);
    QString toHexColonString(const std::vector<uint8_t> &data);
    QString formatKeyUsage(const LIEF::PE::x509 &cert);
    QString formatKeyType(const LIEF::PE::x509 &cert);
    QString formatExtendedKeyUsage(const LIEF::PE::x509 &cert);
};

#endif // CERTIFICATECONTROLLER_H
