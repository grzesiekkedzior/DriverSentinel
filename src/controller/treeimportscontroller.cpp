#include "controller/treeimportscontroller.h"

TreeImportsController::TreeImportsController(QSharedPointer<TreeImportsItem> treeImportsItem,
                                             QTreeView *treeView,
                                             Ui::MainWindow *ui)
    : m_treeImportsItem(std::move(treeImportsItem))
    , m_treeView(treeView)
    , m_ui(ui)
{
    m_treeImportsModel = QSharedPointer<TreeImportsModel>::create();
}

void TreeImportsController::loadImportsDataToView(const QModelIndex &index)
{
    Q_UNUSED(index);

    auto rootItem = std::make_unique<TreeImportsItem>(QVariantList{"Root DLL"});

    auto child1 = std::make_unique<TreeImportsItem>(QVariantList{"FunctionA"});
    auto child2 = std::make_unique<TreeImportsItem>(QVariantList{"FunctionB"});
    rootItem->appendChild(std::move(child1));
    rootItem->appendChild(std::move(child2));

    updateModel(std::move(rootItem));
}

void TreeImportsController::clear()
{
    if (m_treeImportsModel)
        m_treeImportsModel->setRootItem({});
}

void TreeImportsController::updateModel(std::unique_ptr<TreeImportsItem> root)
{
    m_treeImportsModel->setRootItem(std::move(root));
}

QSharedPointer<TreeImportsModel> TreeImportsController::treeImportsModel() const
{
    return m_treeImportsModel;
}
