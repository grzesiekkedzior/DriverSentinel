#ifndef SECTIONINFOMODEL_H
#define SECTIONINFOMODEL_H
#include <QAbstractTableModel>
#include <QObject>
#include "data/sectioninfo.h"

class SectionInfoModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    SectionInfoModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
    enum class SectionColumn {
        Name,                // Section name, e.g., ".text"
        VirtualAddress,      // RVA (Relative Virtual Address)
        VirtualSize,         // Virtual size
        RawSize,             // Raw size in file
        PointerToRawData,    // Offset in file
        Characteristics,     // Hex flags
        CharacteristicsDesc, // Decoded characteristics (optional, readable form)
        Count                // Used to get total number of columns
    };

public:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setPESecton(const QVector<SectionInfo> &sectionInfo);
    void clear();

private:
    QVector<SectionInfo> m_sectionInfo;
};

#endif // SECTIONINFOMODEL_H
