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
    void onTreeItemClicked(const QModelIndex &index);
    void exportModelToFile(const QString &fileName);
    void exportModelToJson(const QString &fileName);
signals:
    void dllSelected(const QString &dllName);
public slots:
    void loadImportsDataToView(const QModelIndex &index);
    void clear();

private:
    QSharedPointer<TreeImportsItem> m_treeImportsItem;
    QSharedPointer<TreeImportsModel> m_treeImportsModel;
    QTreeView *m_treeView{};
    Ui::MainWindow *m_ui;

    static constexpr const char *SystemRoot = "C:/Windows";
    static constexpr const char *System32 = "System32";
    static constexpr const char *SysWOW64 = "SysWOW64";
    static constexpr const char *DriversDir = "drivers";

    void buildImportsRecursive(const std::string &filePath,
                               std::set<std::string> &visited,
                               TreeImportsItem *parentItem);

    bool starts_with(const std::string &str, const std::string &prefix);
    std::string findDLLPath(const std::string &dllName, const std::string &filePath);
};

#endif // TREEIMPORTSCONTROLLER_H
