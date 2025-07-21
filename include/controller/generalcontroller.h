#ifndef GENERALCONTROLLER_H
#define GENERALCONTROLLER_H
#include <QObject>
#include <QTableView>
#include "data/driverinfo.h"
#include "data/generalinfo.h"

class GeneralController : public QObject
{
    Q_OBJECT

public:
    explicit GeneralController(QSharedPointer<GeneralInfo> generalInfo,
                               QTableView *mainTableView,
                               QObject *parent = nullptr);
public slots:
    GeneralInfo loadGeneralInfo(const QModelIndex &index);

private:
    QSharedPointer<GeneralInfo> m_generalInfo;
    QTableView *m_mainTableView{};
};

#endif // GENERALCONTROLLER_H
