#ifndef PEUTILS_H
#define PEUTILS_H
#include <QModelIndex>
#include <QString>
#include <QTableView>

class PEUtils
{
public:
    static QVariant extractFileNameFromRow(QTableView *tableView,
                                           const QModelIndex &index,
                                           int column);
    static QString getPEfilePath(QTableView *tableView, const QModelIndex &index);

    inline static const QString SystemRoot = R"(\SystemRoot\)";
    inline static const QString C_WindowsPath = R"(C:\Windows\)";
};
#endif // PEUTILS_H
