#include "toolbar/drivertoolbar.h"

DriverToolbar::DriverToolbar(QWidget *parent)
{
    QIcon undoicon = QIcon::fromTheme(QIcon::ThemeIcon::ViewRefresh);
    QIcon clearicon = QIcon::fromTheme(QIcon::ThemeIcon::EditClear);
    refreshAction = new QAction(undoicon, tr("Refresh"), this);
    clearAction = new QAction(clearicon, tr("Clear"), this);

    addAction(refreshAction);
    addAction(clearAction);

    connect(refreshAction, &QAction::triggered, this, &DriverToolbar::refreshRequested);
    connect(clearAction, &QAction::triggered, this, &DriverToolbar::clearRequested);
}
