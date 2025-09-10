#ifndef EXCEPTIONCONTROLLER_H
#define EXCEPTIONCONTROLLER_H
#include <QObject>
#include <QTableView>

#include <LIEF/PE/Binary.hpp>

#include "data/exceptioninfo.h"
#include "model/exceptionmodel.h"
namespace Ui {
class MainWindow;
}

class ExceptionController : public QObject
{
    Q_OBJECT
public:
    ExceptionController(QTableView *mainTableView, Ui::MainWindow *ui, QObject *parent = nullptr);

    QSharedPointer<ExceptionInfo> exceptionInfo() const;

    QSharedPointer<ExceptionModel> exceptionModel() const;

public slots:
    void loadExceptionInfo(const QModelIndex &index);
    void updateModel(const QVector<ExceptionInfo> &ei);
    void clear();

private:
    QSharedPointer<ExceptionInfo> m_exceptionInfo;
    QSharedPointer<ExceptionModel> m_exceptionModel;
    QTableView *m_mainTableView{};
    Ui::MainWindow *m_ui;

    quint32 rvaToFileOffset(const LIEF::PE::Binary &binary, quint32 rva);
};

#endif // EXCEPTIONCONTROLLER_H
