#ifndef DEBUGINFOMODEL_H
#define DEBUGINFOMODEL_H
#include <QAbstractTableModel>
#include <QObject>
#include "data/debuginfo.h"

class DebugInfoModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    DebugInfoModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
    enum class DebugColumn {
        Type,
        Characteristics,
        TimeDateStamp,
        MajorVersion,
        MinorVersion,
        SizeOfData,
        AddressOfRawData,
        PointerToRawData,
        PayloadButton,
        ColumnCount
    };

public:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

public slots:
    void setDebugDataToView(const QVector<DebugInfo> &di);
    void clear();

private:
    QVector<DebugInfo> m_debugInfo;

    // QAbstractItemModel interface
public:
};

#endif // DEBUGINFOMODEL_H
