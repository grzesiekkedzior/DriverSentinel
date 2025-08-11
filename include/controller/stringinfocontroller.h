#ifndef STRINGINFOCONTROLLER_H
#define STRINGINFOCONTROLLER_H

#include <QObject>
#include <QSharedPointer>
#include <QTableView>
#include "data/stringinfo.h"
#include "model/stringinfomodel.h"

namespace Ui {
class MainWindow;
}

class StringInfoController : public QObject
{
    Q_OBJECT
public:
    StringInfoController(QSharedPointer<StringInfo> stringInfo,
                         QTableView *mainTableView,
                         Ui::MainWindow *ui,
                         QObject *parent = nullptr);

    QSharedPointer<StringInfoModel> stringInfoModel() const;

public slots:
    void loadStringDataToView(const QModelIndex &index);
    void updateModel(const QVector<StringInfo> &si);
    void clear();

private:
    QSharedPointer<StringInfo> m_stringInfo;
    QSharedPointer<StringInfoModel> m_stringInfoModel;
    QTableView *m_mainTableView{};
    Ui::MainWindow *m_ui;
};

#endif // STRINGINFOCONTROLLER_H
