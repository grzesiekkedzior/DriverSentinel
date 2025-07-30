#include "controller/functioninfocontroller.h"

FunctionInfoController::FunctionInfoController(QSharedPointer<FunctionInfo> functionInfo,
                                               Ui::MainWindow *ui,
                                               QObject *parent)
    : QObject(parent)
    , m_functionInfo(std::move(functionInfo))
    , m_ui(ui)
{
    m_functionInfoModel = QSharedPointer<FunctionInfoModel>::create();
}

void FunctionInfoController::loadFunctionInfoToView(const QModelIndex &index)
{
    QVector<FunctionInfo> functionInfoLocal;

    if (!index.isValid())
        return;
}

void FunctionInfoController::updateModel(const QVector<FunctionInfo> &fi)
{
    if (m_functionInfoModel)
        return;
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
    qDebug() << "Dll name: " << dllName;
}
