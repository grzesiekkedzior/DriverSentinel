#ifndef FUNCTIONINFOCONTROLLER_H
#define FUNCTIONINFOCONTROLLER_H
#include <QObject>
#include <QTableView>
#include "data/functioninfo.h"
#include "model/functioninfomodel.h"

namespace Ui {
class MainWindow;
}

class FunctionInfoController : public QObject
{
    Q_OBJECT
public:
    FunctionInfoController(QSharedPointer<FunctionInfo> functionInfo,
                           Ui::MainWindow *ui,
                           QObject *parent = nullptr);

    QSharedPointer<FunctionInfoModel> functionInfoModel() const;

public slots:
    void loadFunctionInfoToView(const QModelIndex &index);
    void updateModel(const QVector<FunctionInfo> &fi);
    void clear();
    void onDllSelected(const QString &dllName);

private:
    QSharedPointer<FunctionInfo> m_functionInfo;
    QSharedPointer<FunctionInfoModel> m_functionInfoModel;
    Ui::MainWindow *m_ui;
};

#endif // FUNCTIONINFOCONTROLLER_H
