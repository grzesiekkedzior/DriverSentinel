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
    namespace fs = std::filesystem;

    // 1. exe folder
    fs::path baseDir = fs::path(filePath).parent_path();
    fs::path candidate = baseDir / dllName;
    if (fs::exists(candidate)) {
        return candidate.string();
    }

    // 2. SystemRoot
    const char *systemRoot = std::getenv("SystemRoot");
    if (!systemRoot) {
        systemRoot = SystemRoot; // fallback
    }

    // 2a. System32
    candidate = fs::path(systemRoot) / System32 / dllName;
    if (fs::exists(candidate)) {
        return candidate.string();
    }

    // 2b. SysWOW64
    candidate = fs::path(systemRoot) / SysWOW64 / dllName;
    if (fs::exists(candidate)) {
        return candidate.string();
    }

    // 2c. System
    candidate = fs::path(systemRoot) / System / dllName;
    if (fs::exists(candidate)) {
        return candidate.string();
    }

    // 2d. Windows root
    candidate = fs::path(systemRoot) / dllName;
    if (fs::exists(candidate)) {
        return candidate.string();
    }

    // 2e. System32/drivers
    candidate = fs::path(systemRoot) / DriversDir / dllName;
    if (fs::exists(candidate)) {
        return candidate.string();
    }

    // 3. Current
    candidate = fs::current_path() / dllName;
    if (fs::exists(candidate)) {
        return candidate.string();
    }

    // 4. PATH
    if (const char *envPath = std::getenv("PATH")) {
        std::stringstream ss(envPath);
        std::string dir;
        while (std::getline(ss, dir, ';')) {
            if (!dir.empty()) {
                candidate = fs::path(dir) / dllName;
                if (fs::exists(candidate)) {
                    return candidate.string();
                }
            }
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
