#ifndef DRIVERTOOLBAR_H
#define DRIVERTOOLBAR_H

#include <QAction>
#include <QToolBar>

class DriverToolbar : public QToolBar
{
    Q_OBJECT
public:
    explicit DriverToolbar(QWidget *parent = nullptr);

signals:
    void refreshRequested();
    void clearRequested();

private:
    QAction *refreshAction;
    QAction *clearAction;
};

#endif // DRIVERTOOLBAR_H
