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
    std::string absPath = std::filesystem::absolute(filePath).string();
    if (visited.count(absPath)) {
        qDebug() << "Already visited:" << QString::fromStdString(absPath);
        return;
    }
    visited.insert(absPath);

    qDebug() << "Processing:" << QString::fromStdString(absPath);

    auto binary = LIEF::PE::Parser::parse(filePath);
    if (!binary) {
        qWarning() << "Cant parse file:" << QString::fromStdString(filePath);
        return;
    }

    for (const auto &import : binary->imports()) {
        std::string dllName = import.name();

        if (starts_with(dllName, "ext-ms-win-") || starts_with(dllName, "api-ms-win-")) {
            qDebug() << "  Skipping:" << QString::fromStdString(dllName);
            continue;
        }

        qDebug() << "  +-- DLL:" << QString::fromStdString(dllName);

        auto dllItem = std::make_unique<TreeImportsItem>(QVariantList{QString::fromStdString(
                                                             dllName)},
                                                         parentItem);

        for (const auto &entry : import.entries()) {
            QString funcName = entry.is_ordinal() ? QString("Ordinal: %1").arg(entry.ordinal())
                                                  : QString::fromStdString(entry.name());

            auto funcItem = std::make_unique<TreeImportsItem>(QVariantList{funcName}, dllItem.get());
            dllItem->appendChild(std::move(funcItem));

            qDebug() << "  |     Function:" << funcName;
        }

        TreeImportsItem *dllRawPtr = dllItem.get();
        parentItem->appendChild(std::move(dllItem));

        std::string dllPath = findDLLPath(dllName, filePath);
        if (!dllPath.empty() && std::filesystem::exists(dllPath)) {
            qDebug() << "  |-- Recursing into:" << QString::fromStdString(dllPath);
            buildImportsRecursive(dllPath, visited, dllRawPtr);
        } else {
            qDebug() << "  |-- DLL not found:" << QString::fromStdString(dllName);
        }
    }

    qDebug() << "Finished:" << QString::fromStdString(absPath);
}

bool TreeImportsController::starts_with(const std::string &str, const std::string &prefix)
{
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

std::string TreeImportsController::findDLLPath(const std::string &dllName,
                                               const std::string &filePath)
{
    namespace fs = std::filesystem;
    std::vector<fs::path> searchPaths;

    searchPaths.push_back(fs::path(filePath).parent_path());
    searchPaths.push_back(fs::path(SystemRoot) / System32);
    searchPaths.push_back(fs::path(SystemRoot) / SysWOW64);
    searchPaths.push_back(fs::path(SystemRoot) / DriversDir);

    searchPaths.push_back(fs::current_path());

    if (const char *envPath = std::getenv("PATH")) {
        std::stringstream ss(envPath);
        std::string dir;
        while (std::getline(ss, dir, ';')) {
            if (!dir.empty())
                searchPaths.push_back(fs::path(dir));
        }
    }

    for (const auto &dir : searchPaths) {
        fs::path candidate = dir / dllName;
        if (fs::exists(candidate)) {
            return candidate.string();
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

void TreeImportsController::onTreeItemClicked(const QModelIndex &index)
{
    auto item = static_cast<TreeImportsItem *>(index.parent().internalPointer());
    if (!item)
        return;

    QString dllName = item->data(0).toString();
    //qDebug() << "[Tree] Emitting signal with:" << dllName;
    emit dllSelected(dllName);
}
