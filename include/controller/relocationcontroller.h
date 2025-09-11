#ifndef RELOCATIONCONTROLLER_H
#define RELOCATIONCONTROLLER_H
#include <QObject>
#include <QTableView>
#include "data/relocationinfo.h"
#include "model/relocationblocksmodel.h"
#include "model/relocationentriesmodel.h"

namespace Ui {
class MainWindow;
}

class RelocationController : public QObject
{
    Q_OBJECT

public:
    RelocationController(QTableView *mainTableView, Ui::MainWindow *ui, QObject *parent = nullptr);

    QSharedPointer<RelocationBlocksModel> relocationBlocksModel() const;

    QSharedPointer<RelocationEntriesModel> relocationEntriesModel() const;

public slots:
    void loadRelocationDataToView(const QModelIndex &index);
    void clear();
    void update(const QVector<LIEF_RELOCATION::RelocationBlockInfo> &ri);
    void onBlockRelocationClicked(const QModelIndex &index);

private:
    QVector<LIEF_RELOCATION::RelocationBlockInfo> m_relocationBlockInfo;
    QSharedPointer<RelocationBlocksModel> m_relocationBlocksModel;
    QSharedPointer<RelocationEntriesModel> m_relocationEntriesModel;
    QTableView *m_mainTableView{};
    Ui::MainWindow *m_ui;
};

#endif // RELOCATIONCONTROLLER_H
