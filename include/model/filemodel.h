#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include "data/fileheader.h"

class FileModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    FileModel(QObject *parent = nullptr);

    enum class FileInfoColumn {
        Machine,
        NumberOfSections,
        TimeDateStamp,
        PointerToSymbolTable,
        NumberOfSymbols,
        SizeOfOptionalHeader,
        Characteristics,
        ColumnCount
    };

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

public slots:
    void loadFileHeaderDataToView(const QVector<FileInfo> &fh);
    void clear();

private:
    QVector<FileInfo> m_fileInfo;

    static QString machineToString(quint16 machine);
    static QStringList characteristicsToStrings(quint16 characteristics);
};

#endif // FILEMODEL_H
