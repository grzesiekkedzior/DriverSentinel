#include "controller/resourcecontroller.h"
#include "ui_mainwindow.h"

#include <LIEF/PE/Parser.hpp>
#include <LIEF/PE/ResourceData.hpp>
#include <LIEF/PE/ResourceDirectory.hpp>
#include <LIEF/PE/ResourceNode.hpp>
#include <utils/peutils.h>

#include <LIEF/PE/Binary.hpp>

TreeResourcesController::TreeResourcesController(QTableView *mainTableView,
                                                 Ui::MainWindow *ui,
                                                 QObject *parent)
    : QObject(parent)
    , m_mainTableView(mainTableView)
    , m_ui(ui)
    , m_resourceEntry{QSharedPointer<ResourceTreeEntry>::create()}
    , m_resourceTreeModel{QSharedPointer<ResourceTreeModel>::create()}

{
    m_ui->resourcesTreeView->setModel(m_resourceTreeModel.data());
    connect(m_mainTableView,
            &QTableView::clicked,
            this,
            &TreeResourcesController::loadResourcesToView);
    connect(m_ui->resourcesTreeView,
            &QTreeView::clicked,
            this,
            &TreeResourcesController::onResourceClicked);
    m_ui->hexViewer->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
}

// The two horrible functions within this project
void TreeResourcesController::loadResourcesToView(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString filePath = PEUtils::getPEfilePath(m_ui->mainTable, index);

    try {
        std::unique_ptr<LIEF::PE::Binary> binary = LIEF::PE::Parser::parse(filePath.toStdString());
        if (!binary) {
            qWarning() << "LIEF: failed to parse binary";
            return;
        }

        m_resourceTreeModel->clear();
        ResourceTreeEntry rootEntry{"DIR", "Resources", "", ""};
        TreeNode *rootNode = new TreeNode(rootEntry);

        if (binary->has_resources()) {
            const auto resMgr = binary->resources_manager();

            auto processNode = [&](auto &&self,
                                   const LIEF::PE::ResourceNode *node,
                                   TreeNode *parentNode,
                                   uint16_t langId = 0) -> void {
                if (!node)
                    return;

                QString nameOrId = node->has_name() ? QString::fromStdString(node->utf8_name())
                                                    : QString("ID=%1").arg(node->id());

                if (node->is_directory()) {
                    const auto *dir = node->cast<LIEF::PE::ResourceDirectory>();
                    if (!dir)
                        return;

                    uint16_t newLangId = langId;

                    if (parentNode && parentNode->entry().type == "DIR" && parentNode->parent()
                        && parentNode->parent()->entry().type == "DIR") {
                        bool ok = false;
                        uint16_t id = nameOrId.startsWith("ID=") ? nameOrId.mid(3).toUShort(&ok)
                                                                 : 0;
                        if (ok)
                            newLangId = id;
                    }

                    bool isLanguageNode = (newLangId != langId && newLangId != 0);
                    TreeNode *dirNode = parentNode;
                    if (!isLanguageNode) {
                        ResourceTreeEntry dirEntry{"DIR", nameOrId, "", ""};
                        dirNode = new TreeNode(dirEntry, parentNode);
                        parentNode->appendChild(dirNode);
                    }

                    for (const auto &child : dir->childs()) {
                        self(self, &child, dirNode, newLangId);
                    }
                } else if (node->is_data()) {
                    const auto *data = node->cast<LIEF::PE::ResourceData>();
                    if (!data)
                        return;

                    QString langStr = langId != 0 ? QString("ID=%1").arg(langId) : "";

                    QString resIdStr = nameOrId;

                    bool ok = false;
                    uint16_t resId = 0;
                    if (resIdStr.startsWith("ID=")) {
                        resId = resIdStr.mid(3).toUShort(&ok);
                    }

                    //QString resNameStr = ok ? QString("ID=%1").arg(resId) : resIdStr;

                    QString size = QString("Size: %1 bytes").arg(data->content().size());

                    ResourceTreeEntry dataEntry{"DATA", langStr, langIdToString(resId), size};
                    auto spanContent = data->content();
                    dataEntry.rawContent.assign(spanContent.begin(), spanContent.end());
                    TreeNode *dataNode = new TreeNode(dataEntry, parentNode);
                    parentNode->appendChild(dataNode);
                }
            };

            for (auto type : resMgr->get_types()) {
                const LIEF::PE::ResourceNode *typeNode = resMgr->get_node_type(type);
                if (!typeNode)
                    continue;

                QString typeName = QString::fromStdString(LIEF::PE::to_string(type));
                ResourceTreeEntry typeEntry{"DIR", typeName, "", ""};
                TreeNode *typeTreeNode = new TreeNode(typeEntry, rootNode);
                rootNode->appendChild(typeTreeNode);

                const auto *dir = typeNode->cast<LIEF::PE::ResourceDirectory>();
                if (!dir)
                    continue;

                for (const auto &child : dir->childs()) {
                    processNode(processNode, &child, typeTreeNode, 0);
                }
            }
        }

        m_resourceTreeModel->setRoot(rootNode);
        m_ui->resourcesTreeView->expandAll();

    } catch (const std::exception &e) {
        qWarning() << "LIEF error:" << e.what();
    } catch (...) {
        qWarning() << "Unknown error while parsing PE";
    }
}

void TreeResourcesController::clear()
{
    if (m_resourceTreeModel) {
        m_resourceTreeModel->clear();
        m_ui->hexViewer->clear();
    }
}

QString TreeResourcesController::langIdToString(uint16_t langId)
{
    switch (langId) {
    case 0x0401:
        return "Arabic";
    case 0x0402:
        return "Bulgarian";
    case 0x0403:
        return "Catalan";
    case 0x0404:
        return "Chinese (Traditional)";
    case 0x0804:
        return "Chinese (Simplified)";
    case 0x0405:
        return "Czech";
    case 0x0406:
        return "Danish";
    case 0x0407:
        return "German";
    case 0x0408:
        return "Greek";
    case 0x0409:
        return "English (US)";
    case 0x040B:
        return "Finnish";
    case 0x040C:
        return "French";
    case 0x040D:
        return "Hebrew";
    case 0x040E:
        return "Hungarian";
    case 0x0410:
        return "Italian";
    case 0x0411:
        return "Japanese";
    case 0x0412:
        return "Korean";
    case 0x0413:
        return "Dutch";
    case 0x0414:
        return "Norwegian";
    case 0x0415:
        return "Polish";
    case 0x0416:
        return "Portuguese (Brazil)";
    case 0x0418:
        return "Romanian";
    case 0x0419:
        return "Russian";
    case 0x041A:
        return "Croatian";
    case 0x041B:
        return "Slovak";
    case 0x041D:
        return "Swedish";
    case 0x041F:
        return "Turkish";
    case 0x0422:
        return "Ukrainian";
    case 0x043E:
        return "Malay";
    case 0x0C00:
        return "Neutral";
    default:
        return QString("Unknown (0x%1)").arg(langId, 4, 16, QChar('0'));
    }
}

void TreeResourcesController::onResourceClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    auto *node = static_cast<TreeNode *>(index.internalPointer());
    if (!node)
        return;

    const ResourceTreeEntry &entry = node->entry();

    if (!entry.rawContent.empty()) {
        QString hexStr;
        hexStr.reserve(entry.rawContent.size() * 3);

        for (size_t i = 0; i < entry.rawContent.size(); ++i) {
            hexStr += QString("%1 ").arg(static_cast<unsigned char>(entry.rawContent[i]),
                                         2,
                                         16,
                                         QChar('0'));
            if ((i + 1) % 16 == 0)
                hexStr += "\n";
        }

        m_ui->hexViewer->setPlainText(hexStr);
        QFont monoFont("Courier New");
        monoFont.setStyleHint(QFont::Monospace);
        monoFont.setPointSize(12);
        m_ui->hexViewer->setFont(monoFont);
    } else {
        m_ui->hexViewer->clear();
    }
}

QSharedPointer<ResourceTreeModel> TreeResourcesController::resourceTreeModel() const
{
    return m_resourceTreeModel;
}
