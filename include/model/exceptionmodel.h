#ifndef EXCEPTIONMODEL_H
#define EXCEPTIONMODEL_H
#include <QAbstractTableModel>
#include <QObject>
#include "data/exceptioninfo.h"

class ExceptionModel : public QAbstractTableModel
{
    // QAbstractItemModel interface
    Q_OBJECT
public:
    ExceptionModel(QObject *parent = nullptr);

    enum class ExceptionColumn {
        //FileOffset,
        BeginRVA,
        EndRVA,
        UnwindRVA,
        Version,
        Flags,
        SizeOfProlog,
        CountOfCodes,
        RegistersSaved,
        OpcodesDetail,
        HasEpilog,
        EpilogFlags,
        EpilogSize,
        ColumnCount
    };

public:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

public slots:
    void loadExceptionDataToView(const QVector<ExceptionInfo> &ei);
    void clear();

private:
    QVector<ExceptionInfo> m_exceptionInfo;
};

#endif // EXCEPTIONMODEL_H
