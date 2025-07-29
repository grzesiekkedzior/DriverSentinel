#include "controller/treeimportscontroller.h"
#include "ui_mainwindow.h"
#include <filesystem>
#include <utils/peutils.h>

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
    QString filePath = PEUtils::getPEfilePath(m_ui->mainTable, index);

    std::set<std::string> visited;
    auto rootItem = std::make_unique<TreeImportsItem>(QVariantList{"Imports for: " + filePath});
    buildImportsRecursive(filePath.toStdString(), visited, rootItem.get());
    updateModel(std::move(rootItem));
}

void TreeImportsController::clear()
{
    if (m_treeImportsModel)
        m_treeImportsModel->setRootItem({});
}

void TreeImportsController::buildImportsRecursive(const std::string &filePath,
                                                  std::set<std::string> &visited,
                                                  TreeImportsItem *parentItem)
{
    auto binary = LIEF::PE::Parser::parse(filePath);
    if (!binary) {
        qWarning() << "Cant parse file:" << QString::fromStdString(filePath);
        return;
    }

    for (const auto &import : binary->imports()) {
        std::string dllName = import.name();

        if (starts_with(dllName, "ext-ms-win-") || starts_with(dllName, "api-ms-win-"))
            continue;

        if (visited.count(dllName))
            continue;

        visited.insert(dllName);

        auto dllItem = std::make_unique<TreeImportsItem>(QVariantList{QString::fromStdString(
                                                             dllName)},
                                                         parentItem);

        for (const auto &entry : import.entries()) {
            QString funcName = entry.is_ordinal() ? QString("Ordinal: %1").arg(entry.ordinal())
                                                  : QString::fromStdString(entry.name());

            auto funcItem = std::make_unique<TreeImportsItem>(QVariantList{funcName}, dllItem.get());
            dllItem->appendChild(std::move(funcItem));
        }

        TreeImportsItem *dllRawPtr = dllItem.get();
        parentItem->appendChild(std::move(dllItem));

        std::string dllPath = findDLLPath(dllName, filePath);
        if (std::filesystem::exists(dllPath)) {
            buildImportsRecursive(dllPath, visited, dllRawPtr);
        }
    }
}

bool TreeImportsController::starts_with(const std::string &str, const std::string &prefix)
{
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

std::string TreeImportsController::findDLLPath(const std::string &dllName,
                                               const std::string &filePath)
{
    // Filesystem library (since C++17)
    namespace fs = std::filesystem;

    fs::path baseDir = fs::path(filePath).parent_path();

    fs::path candidate = baseDir / dllName;
    if (fs::exists(candidate)) {
        return candidate.string();
    }

    // unsafe getenv
    const char *systemRoot = std::getenv(SystemRoot);
    if (systemRoot) {
        fs::path system32Path = fs::path(systemRoot) / System32 / dllName;
        if (fs::exists(system32Path)) {
            return system32Path.string();
        }
    }

    return "";
}

void TreeImportsController::updateModel(std::unique_ptr<TreeImportsItem> root)
{
    m_treeImportsModel->setRootItem(std::move(root));
}

QSharedPointer<TreeImportsModel> TreeImportsController::treeImportsModel() const
{
    return m_treeImportsModel;
}
