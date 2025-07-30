#ifndef FUNCTIONINFOMODEL_H
#define FUNCTIONINFOMODEL_H
#include <QAbstractTableModel>
#include <QObject>
#include "data/functioninfo.h"

class FunctionInfoModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    FunctionInfoModel(QObject *parent = nullptr);

    enum class FunctionColumn {
        DllName,
        FunctionName,
        IsImport,
        IsExport,
        IsForwarded,
        ForwardInfo,

        // Import fields
        IAT_Address,
        IAT_Value,
        ILT_Value,
        IsOrdinal,
        Hint,
        HintNameRVA,
        EntryData,
        ImportOrdinal,

        // Export fields
        FunctionRVA,
        Address,
        IsExtern,
        ExportOrdinal,

        Count
    };

    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setFunInfoSection(const QVector<FunctionInfo> &fi);
    void clear();

private:
    QVector<FunctionInfo> m_functionInfo;
};

#endif // FUNCTIONINFOMODEL_H
