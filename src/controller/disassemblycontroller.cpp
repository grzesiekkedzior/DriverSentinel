#include "controller/disassemblycontroller.h"

DisassemblyController::DisassemblyController(QSharedPointer<DisassemblyData> disassemblyData,
                                             QTableView *mainTableView,
                                             Ui::MainWindow *ui,
                                             QObject *parent)
    : QObject(parent)
    , m_disassemblyData(std::move(disassemblyData))
    , m_mainTableView(mainTableView)
    , m_ui(ui)
{
    m_disassemblyModel = QSharedPointer<DisassembyModel>::create();
    connect(m_mainTableView,
            &QTableView::clicked,
            this,
            &DisassemblyController::loadAsemblyDataToView);
}

void DisassemblyController::loadAsemblyDataToView(const QModelIndex &index)
{
    qDebug() << "Hello asm";
}

void DisassemblyController::clear()
{
    qDebug() << "Hello asm clear function";
}

QSharedPointer<DisassembyModel> DisassemblyController::disassemblyModel() const
{
    return m_disassemblyModel;
}
