#ifndef RICHHDRMODEL_H
#define RICHHDRMODEL_H
#include <QAbstractTableModel>
#include <QObject>
#include "data/richhdrdata.h"

class RichHdrModel : public QAbstractTableModel
{
    Q_OBJECT
    // QAbstractItemModel interface
public:
    RichHdrModel(QObject *parent = nullptr);

    enum class RichHeaderColumn {
        Offset = 0, // Offset of the Rich Header in the file
        Key,        // XOR key used to encode the Rich Header
        ProductID,  // Tool ID
        BuildID,    // Build number of the tool
        Count,      // How many times the tool was used
        //VSVersion,     // Corresponding Visual Studio version
        RawValue,      // Raw encoded value from the entry
        UnmaskedValue, // Decoded value from the entry
        RawData,       // Raw bytes of the entire Rich Header (optional)
        UnmaskedData,  // Decoded bytes of the entire Rich Header (optional)
        ColumnCount
    };

public:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setRichHdrSection(const QVector<RichHeaderData> &richInfo);
    void clear();

private:
    QVector<RichHeaderData> m_richHdrData;
};

#endif // RICHHDRMODEL_H
