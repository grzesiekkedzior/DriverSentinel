#ifndef STRINGINFOMODEL_H
#define STRINGINFOMODEL_H

#include <QAbstractTableModel>
#include "data/stringinfo.h"

class StringInfoModel : public QAbstractTableModel
{
    Q_OBJECT
    // QAbstractItemModel interface
    enum StringsColumn { Section, Offset, String, Count };

public:
    StringInfoModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setStringInfo(const QVector<StringInfo> &si);
    void clear();

private:
    QVector<StringInfo> m_stringInfo;
};

#endif // STRINGINFOMODEL_H
