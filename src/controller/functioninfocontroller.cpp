#include "controller/functioninfocontroller.h"
#include "ui_mainwindow.h"
#include <LIEF/PE.hpp>
#include <filesystem>

FunctionInfoController::FunctionInfoController(QSharedPointer<FunctionInfo> functionInfo,
                                               Ui::MainWindow *ui,
                                               QObject *parent)
    : QObject(parent)
    , m_functionInfo(std::move(functionInfo))
    , m_ui(ui)
{
    m_functionInfoModel = QSharedPointer<FunctionInfoModel>::create();
}

/**
 * Loads the list of functions (both imports and exports) from the specified DLL file.
 * 
 * For imports:
 *   - Retrieves functions that this DLL depends on, i.e., functions implemented in other DLLs.
 * For exports:
 *   - Retrieves functions that are implemented and exposed by this DLL itself.
 * 
 * The function populates a local container with FunctionInfo entries representing these functions,
 * which can then be used to update the UI model.
 * 
 * @param index QModelIndex used to verify the context of the selection (must be valid).
 */
void FunctionInfoController::loadFunctionInfoToView(const QModelIndex &index)
{
    clear();
    m_ui->treeTableView->update();
    QVector<FunctionInfo> functionInfoLocal;

    namespace fs = std::filesystem;
    fs::path fp = m_system32.toStdString() + m_dllName.toStdString();
    if (!fs::exists(fp)) {
        qWarning() << "File path: " << QString::fromStdString(fp.string()) << " does NOT exist";
        fp = m_drivers.toStdString() + m_dllName.toStdString();
    }

    if (!fs::exists(fp)) {
        qWarning() << "File path in drivers: " << QString::fromStdString(fp.string())
                   << " does NOT exist";
    }
    QString filePath = QString::fromStdString(fp.string());
    try {
        std::unique_ptr<LIEF::PE::Binary> binary = LIEF::PE::Parser::parse(filePath.toStdString());
        if (!binary)
            return;

        for (const auto &import : binary->imports()) {
            for (const auto &entry : import.entries()) {
                FunctionInfo info;
                info.dllName = QString::fromStdString(import.name());
                info.functionName = entry.is_ordinal() ? QString("Ordinal: %1").arg(entry.ordinal())
                                                       : QString::fromStdString(entry.name());

                info.isImport = true;
                info.isExport = false;
                info.isForwarded = false;

                info.iatAddress = entry.iat_address();
                info.iatValue = entry.iat_value();
                info.iltValue = entry.ilt_value();
                info.isOrdinal = entry.is_ordinal();
                info.hint = entry.hint();
                info.hintNameRVA = entry.hint_name_rva();
                info.entryData = entry.data();
                info.importOrdinal = entry.ordinal();

                functionInfoLocal.append(info);
            }
        }

        if (binary->has_exports()) {
            for (const auto &entry : binary->get_export()->entries()) {
                FunctionInfo info;
                info.dllName = m_dllName;
                info.functionName = entry.name().empty()
                                        ? QString("Ordinal: %1").arg(entry.ordinal())
                                        : QString::fromStdString(entry.name());
                if (entry.is_forwarded()) {
                    qDebug() << "Forwarded to:"
                             << QString::fromStdString(entry.forward_information().function);
                }

                info.isImport = false;
                info.isExport = true;
                info.isForwarded = entry.is_forwarded();

                info.functionRVA = entry.function_rva();
                info.address = entry.address();
                info.isExtern = entry.is_extern();
                info.exportOrdinal = entry.ordinal();

                if (entry.is_forwarded()) {
                    info.forwardInfo = QString::fromStdString(entry.forward_information().function);
                }

                functionInfoLocal.append(info);
            }
        }

    } catch (const std::exception &e) {
        qWarning() << "LIEF error:" << e.what();
    } catch (...) {
        qWarning() << "Unknown error while parsing PE";
    }

    if (!index.isValid())
        return;
    updateModel(functionInfoLocal);
}

void FunctionInfoController::updateModel(const QVector<FunctionInfo> &fi)
{
    qDebug() << "FunctionInfo count:" << fi.size();

    for (const auto &f : fi) {
        qDebug() << "Function:" << f.functionName << " Import:" << f.isImport
                 << " Export:" << f.isExport;
    }

    if (m_functionInfoModel)
        m_functionInfoModel->setFunInfoSection(fi);
}

void FunctionInfoController::clear()
{
    if (m_functionInfoModel)
        m_functionInfoModel->setFunInfoSection({});
}

QSharedPointer<FunctionInfoModel> FunctionInfoController::functionInfoModel() const
{
    return m_functionInfoModel;
}

void FunctionInfoController::onDllSelected(const QString &dllName)
{
    qDebug() << "Selected DLL:" << dllName;

    m_dllName = dllName;
}
