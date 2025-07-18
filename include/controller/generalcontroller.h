#ifndef GENERALCONTROLLER_H
#define GENERALCONTROLLER_H
#include <QObject>
#include "data/driverinfo.h"
#include "data/generalinfo.h"

class GeneralController : public QObject
{
    Q_OBJECT

public:
    explicit GeneralController(QSharedPointer<GeneralInfo> generalInfo, QObject *parent = nullptr);
    GeneralInfo loadGeneralInfo();

private:
    QSharedPointer<GeneralInfo> m_generalInfo;
};

#endif // GENERALCONTROLLER_H
