#ifndef OPTIONALHEADERCONTROLLER_H
#define OPTIONALHEADERCONTROLLER_H
#include <QObject>
#include <QTableView>
#include "data/optionalheaderinfo.h"
#include <LIEF/PE.hpp>
namespace Ui {
class MainWindow;
}

class OptionalHeaderController : public QObject
{
    Q_OBJECT

public:
    OptionalHeaderController(QSharedPointer<OptionalHeaderInfo> optionalHeader,
                             QTableView *mainTableView,
                             Ui::MainWindow *ui,
                             QObject *parent = nullptr);

    uint32_t getOptionalOffsetAddress(const std::unique_ptr<LIEF::PE::Binary> &binary);

public slots:
    void loadOptionalHeaderInfo(const QModelIndex &index);
    void clear();

private:
    QSharedPointer<OptionalHeaderInfo> m_optionalHeader;
    QTableView *m_mainTableView{};
    Ui::MainWindow *m_ui;

    void loadOptionalHeaderDataToLabel(OptionalHeaderInfo oh);
    QString subsystemToString(LIEF::PE::OptionalHeader::SUBSYSTEM s) const;
};

#endif // OPTIONALHEADERCONTROLLER_H
