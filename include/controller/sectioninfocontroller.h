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
    void refresh();
    void clear();
    void start();

private:
    QSharedPointer<SectionInfo> m_sectionInfo;
    QSharedPointer<SectionInfoModel> m_sectionInfoModel;
    QTableView *m_mainTableView{};
    Ui::MainWindow *m_ui;
    QVector<SectionInfo> m_sectionVectorInfo;

    QVariant extractFileNameFromRow(const QModelIndex &index, int column);
};

#endif // SECTIONINFOCONTROLLER_H
