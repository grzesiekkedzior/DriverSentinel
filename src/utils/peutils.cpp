#include "utils/peutils.h"

QVariant PEUtils::extractFileNameFromRow(QTableView *tableView, const QModelIndex &index, int column)
{
    if (!tableView || !tableView->model() || !index.isValid())
        return {};

    QModelIndex nameIndex = tableView->model()->index(index.row(), column);
    return tableView->model()->data(nameIndex, Qt::DisplayRole);
}

QString PEUtils::getPEfilePath(QTableView *tableView, const QModelIndex &index)
{
    QString filePath = extractFileNameFromRow(tableView, index, 1).toString();
    filePath.replace(SystemRoot, C_WindowsPath);

    return filePath;
}
