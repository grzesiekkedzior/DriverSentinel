#ifndef DISASSEMBLYCONTROLLER_H
#define DISASSEMBLYCONTROLLER_H

#include <QObject>
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

    QSharedPointer<DisassembyModel> disassemblyModel() const;

public slots:
    void loadAsemblyDataToView(const QModelIndex &index);
    void clear();

private:
    QSharedPointer<DisassemblyData> m_disassemblyData;
    QSharedPointer<DisassembyModel> m_disassemblyModel;
    QTableView *m_mainTableView{};
    Ui::MainWindow *m_ui;
};

#endif // DISASSEMBLYCONTROLLER_H
