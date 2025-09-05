#ifndef RICHHDRCONTROLLER_H
#define RICHHDRCONTROLLER_H
#include <QObject>
#include <QTableView>
#include "data/richhdrdata.h"
#include "model/richhdrmodel.h"

namespace Ui {
class MainWindow;
}

class RichHeaderController : public QObject
{
    Q_OBJECT
public:
    RichHeaderController(QTableView *mainTableView, Ui::MainWindow *ui, QObject *parent = nullptr);

    QSharedPointer<RichHdrModel> richHdrModel() const;

public slots:
    void loadRichHeaderDataToView(const QModelIndex &index);
    void clear();
    void updateModel(const QVector<RichHeaderData> &richHdrData);

private:
    QSharedPointer<RichHeaderData> m_richHeaderData;
    QSharedPointer<RichHdrModel> m_richHdrModel;
    QTableView *m_mainTableView{};
    Ui::MainWindow *m_ui;

    QString getCompilerVersion(quint16 productId);
};

#endif // RICHHDRCONTROLLER_H
