#ifndef TREEIMPORTSCONTROLLER_H
#define TREEIMPORTSCONTROLLER_H
#include <QObject>
#include "data/treeimportsitem.h"
#include "model/treeimportsmodel.h"
#include <qtreeview.h>

namespace Ui {
class MainWindow;
}

class TreeImportsController : public QObject
{
    Q_OBJECT
public:
    TreeImportsController(QSharedPointer<TreeImportsItem> treeImportsItem,
                          QTreeView *treeView,
                          Ui::MainWindow *ui);

    QSharedPointer<TreeImportsModel> treeImportsModel() const;

    void updateModel(std::unique_ptr<TreeImportsItem> root);
public slots:
    void loadImportsDataToView(const QModelIndex &index);
    void clear();

private:
    QSharedPointer<TreeImportsItem> m_treeImportsItem;
    QSharedPointer<TreeImportsModel> m_treeImportsModel;
    QTreeView *m_treeView{};
    Ui::MainWindow *m_ui;
};

#endif // TREEIMPORTSCONTROLLER_H
