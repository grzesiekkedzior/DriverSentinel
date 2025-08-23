#ifndef DOSHEADERCONTROLLER_H
#define DOSHEADERCONTROLLER_H
#include <QObject>
#include <QTableView>
#include "data/dosheader.h"

namespace Ui {
class MainWindow;
}

class DosHeaderController : public QObject
{
    Q_OBJECT
public:
    DosHeaderController(QSharedPointer<DosHeader> dosHeader,
                        QTableView *mainTableView,
                        Ui::MainWindow *ui,
                        QObject *parent = nullptr);

public slots:
    void loadDosHeaderInfo(const QModelIndex &index);
    void clear();

private:
    QSharedPointer<DosHeader> m_dosHeader;
    QTableView *m_mainTableView{};
    Ui::MainWindow *m_ui;

    void loadDosHeaderDataToLabel(DosHeader ds);
};

#endif // DOSHEADERCONTROLLER_H
