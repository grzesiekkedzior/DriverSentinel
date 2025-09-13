#ifndef FILEINFOCONTROLLER_H
#define FILEINFOCONTROLLER_H
#include <QObject>
#include "model/filemodel.h"
#include <qtableview.h>

namespace Ui {
class MainWindow;
}

class FileInfoController : public QObject
{
    Q_OBJECT
public:
    FileInfoController(QTableView *mainTableView, Ui::MainWindow *ui, QObject *parent = nullptr);

    QSharedPointer<FileModel> fileModel() const;

public slots:
    void loadFileInfoToView(const QModelIndex &index);
    void update(const QVector<FileInfo> &fm);
    void clear();

private:
    QSharedPointer<FileInfo> m_fileInfo;
    QSharedPointer<FileModel> m_fileModel;

    QTableView *m_mainTableView{};
    Ui::MainWindow *m_ui;
};

#endif // FILEINFOCONTROLLER_H
