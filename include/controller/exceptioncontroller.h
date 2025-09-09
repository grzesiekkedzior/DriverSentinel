#ifndef EXCEPTIONCONTROLLER_H
#define EXCEPTIONCONTROLLER_H
#include <QObject>
#include <QTableView>

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
};

#endif // EXCEPTIONCONTROLLER_H
