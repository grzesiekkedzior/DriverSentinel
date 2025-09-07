#ifndef DEBUGINFOCONTROLLER_H
#define DEBUGINFOCONTROLLER_H
#include <QObject>
#include <QTableView>
#include "model/debuginfomodel.h"
#include <data/debuginfo.h>

namespace Ui {
class MainWindow;
}

class DebugInfoController : public QWidget
{
    Q_OBJECT

public:
    DebugInfoController(QTableView *mainTableView, Ui::MainWindow *ui, QWidget *parent = nullptr);

    QSharedPointer<DebugInfo> debugInfo() const;

    QSharedPointer<DebugInfoModel> debugInfoModel() const;

public slots:
    void loadDebugInfo(const QModelIndex &index);
    void updateModel(const QVector<DebugInfo> &di);
    void clear();

private:
    QSharedPointer<DebugInfo> m_debugInfo;
    QSharedPointer<DebugInfoModel> m_debugInfoModel;
    QTableView *m_mainTableView{};
    Ui::MainWindow *m_ui;
    void openPayloadDialog(int row);
    QString formatHexPayload(const QByteArray &payload);
};

#endif // DEBUGINFOCONTROLLER_H
