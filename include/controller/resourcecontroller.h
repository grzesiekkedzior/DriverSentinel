#ifndef RESOURCECONTROLLER_H
#define RESOURCECONTROLLER_H
#include <QObject>
#include <QVector>
#include "data/resourcedata.h"
#include "model/resourcemodel.h"
#include <qtableview.h>
#include <qtreeview.h>

namespace Ui {
class MainWindow;
}

class TreeResourcesController : public QObject
{
    Q_OBJECT
public:
    TreeResourcesController(QTableView *mainTableView,
                            Ui::MainWindow *ui,
                            QObject *parent = nullptr);

    QSharedPointer<ResourceTreeModel> resourceTreeModel() const;

public slots:
    void loadResourcesToView(const QModelIndex &index);
    void clear();
    void onResourceClicked(const QModelIndex &index);

private:
    QSharedPointer<ResourceTreeEntry> m_resourceEntry;
    QSharedPointer<ResourceTreeModel> m_resourceTreeModel;
    QTableView *m_mainTableView{};
    Ui::MainWindow *m_ui;

    QString langIdToString(uint16_t langId);
};

#endif // RESOURCECONTROLLER_H
