#ifndef SECTIONINFOCONTROLLER_H
#define SECTIONINFOCONTROLLER_H
#include <QObject>
#include <QTableView>
#include "data/sectioninfo.h"
#include "model/sectioninfomodel.h"

namespace Ui {
class MainWindow;
}

class SectionInfoController : public QObject
{
    Q_OBJECT
public:
    SectionInfoController(QSharedPointer<SectionInfo> sectionInfo,
                          QTableView *sectionPEInfo,
                          Ui::MainWindow *ui,
                          QObject *parent = nullptr);

    QSharedPointer<SectionInfoModel> sectionInfoModel() const;

public slots:
    void loadPESectionDataToView(const QModelIndex &index);
    void clear();
    void updateModel(const QVector<SectionInfo> &sectionInfo);

private:
    QSharedPointer<SectionInfo> m_sectionInfo;
    QSharedPointer<SectionInfoModel> m_sectionInfoModel;
    QTableView *m_mainTableView{};
    Ui::MainWindow *m_ui;

    const QString SystemRoot = "\\SystemRoot";
    const QString C_WindowsPath = "C:\\Windows";

    QVariant extractFileNameFromRow(const QModelIndex &index, int column);
    QString getPEfilePath(const QModelIndex &index);
};

#endif // SECTIONINFOCONTROLLER_H
