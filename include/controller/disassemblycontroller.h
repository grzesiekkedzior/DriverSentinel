#ifndef DISASSEMBLYCONTROLLER_H
#define DISASSEMBLYCONTROLLER_H

#include <QFuture>
#include <QObject>
#include <QProgressBar>
#include <QSharedPointer>
#include <QTableView>
#include "data/disassemblydata.h"
#include "model/disassemblymodel.h"

namespace Ui {
class MainWindow;
}

class DisassemblyController : public QObject
{
    Q_OBJECT
public:
    DisassemblyController(QSharedPointer<DisassemblyData> disassemblyData,
                          QTableView *mainTableView,
                          Ui::MainWindow *ui,
                          QObject *parent = nullptr);

    QSharedPointer<DisassemblyModel> disassemblyModel() const;

public slots:
    void loadAssemblyDataToView(const QModelIndex &index);
    void clear();
    void updateModel(const QVector<DisassemblyData> &dd);

private:
    QSharedPointer<DisassemblyData> m_disassemblyData;
    QSharedPointer<DisassemblyModel> m_disassemblyModel;
    QTableView *m_mainTableView{};
    Ui::MainWindow *m_ui;

    void loadAsemblyDataTo(const QModelIndex &index);
    QFuture<QVector<DisassemblyData>> extractAsm(QString filePath, QProgressBar *progress);
};

#endif // DISASSEMBLYCONTROLLER_H
