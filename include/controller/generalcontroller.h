#ifndef GENERALCONTROLLER_H
#define GENERALCONTROLLER_H
#include <QObject>
#include <QTableView>
#include "data/generalinfo.h"
#include <LIEF/PE.hpp>

namespace Ui {
class MainWindow;
}

class GeneralController : public QObject
{
    Q_OBJECT

public:
    explicit GeneralController(QSharedPointer<GeneralInfo> generalInfo,
                               QTableView *mainTableView,
                               Ui::MainWindow *ui,
                               QObject *parent = nullptr);
public slots:
    void loadGeneralInfo(const QModelIndex &index);
    void clear();

private:
    QSharedPointer<GeneralInfo> m_generalInfo;
    QTableView *m_mainTableView{};
    Ui::MainWindow *m_ui;

    QVariant extractFileNameFromRow(const QModelIndex &index, int column);
    QString extractFileVersion(const std::unique_ptr<LIEF::PE::Binary> &binary);
    QString getFileDescriptionFromWinAPI(const QString &filePath);
    QString getPEFormatString(LIEF::PE::PE_TYPE type);
    QString getPEFileExtention(const LIEF::PE::Binary &binary);
    QString getImageSize(QString filePath);
    void loadGeneralDataToLabel(GeneralInfo gi);

    const char *CompanyName = "CompanyName";
    const char *FileDescription = "FileDescription";
    const char *FileVersion = "FileVersion";
    const char *InternalName = "InternalName";
    const char *LegalCopyright = "LegalCopyright";
    const char *OriginalFilename = "OriginalFilename";
    const char *ProductName = "ProductName";
    const char *ProductVersion = "ProductVersion";
};

#endif // GENERALCONTROLLER_H
