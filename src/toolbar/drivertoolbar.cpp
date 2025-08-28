#include "toolbar/drivertoolbar.h"

DriverToolbar::DriverToolbar(QWidget *parent)
{
    QIcon undoicon = QIcon::fromTheme(QIcon::ThemeIcon::ViewRefresh);
    QIcon clearicon = QIcon::fromTheme(QIcon::ThemeIcon::EditClear);
    QIcon openPE = QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen);
    refreshAction = new QAction(undoicon, tr("Refresh"), this);
    clearAction = new QAction(clearicon, tr("Clear"), this);
    openPEAction = new QAction(openPE, tr("Open"), this);

    addAction(refreshAction);
    addAction(clearAction);
    addAction(openPEAction);

    connect(refreshAction, &QAction::triggered, this, &DriverToolbar::refreshRequested);
    connect(clearAction, &QAction::triggered, this, &DriverToolbar::clearRequested);
    connect(openPEAction, &QAction::triggered, this, &DriverToolbar::openPERequested);
}
