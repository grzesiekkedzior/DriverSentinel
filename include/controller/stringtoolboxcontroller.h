#ifndef STRINGTOOLBOXCONTROLLER_H
#define STRINGTOOLBOXCONTROLLER_H
#include <QAction>
#include <QObject>
#include "model/stringfilterproxymodel.h"

namespace Ui {
class MainWindow;
}

class StringToolBoxController : public QObject
{
    Q_OBJECT
public:
    explicit StringToolBoxController(StringFilterProxyModel *model,
                                     Ui::MainWindow *ui,
                                     QObject *parent = nullptr);

public slots:
    void saveAsPdf();
    void saveAsTxt();
    void saveAsCsv();

private:
    QSharedPointer<StringFilterProxyModel> m_proxyModel;
    Ui::MainWindow *m_ui;

    QMenu *actionMenu;
    QAction *pdfAction;
    QAction *txtAction;
    QAction *csvAction;
};

#endif // STRINGTOOLBOXCONTROLLER_H
