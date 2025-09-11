#include "controller/resourcecontroller.h"

TreeResourcesController::TreeResourcesController(QTableView *mainTableView,
                                                 Ui::MainWindow *ui,
                                                 QObject *parent)
    : QObject(parent)
    , m_mainTableView(mainTableView)
    , m_ui(ui)
    , m_resourceEntry{QSharedPointer<ResourceTreeEntry>::create()}
    , m_resourceTreeModel{QSharedPointer<ResourceTreeModel>::create()}

{
    connect(m_mainTableView,
            &QTableView::clicked,
            this,
            &TreeResourcesController::loadResourcesToView);
}

void TreeResourcesController::loadResourcesToView(const QModelIndex & /*index*/)
{
    if (!m_resourceTreeModel || !m_resourceEntry)
        return;

    ResourceTreeEntry rootEntry{"DIR", "Resources", "", ""};
    TreeNode *rootNode = new TreeNode(rootEntry);

    ResourceTreeEntry iconEntry{"DIR", "ICON", "", ""};
    TreeNode *iconNode = new TreeNode(iconEntry, rootNode);

    ResourceTreeEntry idEntry{"DIR", "ID=1", "", ""};
    TreeNode *idNode = new TreeNode(idEntry, iconNode);

    ResourceTreeEntry langEntry{"DIR", "", "Lang=1033", ""};
    TreeNode *langNode = new TreeNode(langEntry, idNode);

    ResourceTreeEntry dataEntry{"DATA", "", "Lang=1033", "Rozmiar: 512 bytes"};
    TreeNode *dataNode = new TreeNode(dataEntry, langNode);

    langNode->appendChild(dataNode);
    idNode->appendChild(langNode);
    iconNode->appendChild(idNode);
    rootNode->appendChild(iconNode);

    m_resourceTreeModel->setRoot(rootNode);
}

void TreeResourcesController::clear()
{
    if (m_resourceTreeModel) {
        m_resourceTreeModel->clear();
    }
}

QSharedPointer<ResourceTreeModel> TreeResourcesController::resourceTreeModel() const
{
    return m_resourceTreeModel;
}
