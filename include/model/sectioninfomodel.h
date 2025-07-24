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
        Name,                 // QString name
        VirtualAddress,       // QString virtualAddress
        VirtualSize,          // QString virtualSize
        RawSize,              // QString rawSize
        PointerToRawData,     // QString pointerToRawData
        Characteristics,      // QString characteristics (hex)
        Entropy,              // QString entropy
        SectionAlignment,     // QString sectionAlignment
        PointerToRelocations, // QString pointerToRelocations
        NumberOfRelocations,  // QString numberOfRelocations
        CharacteristicsDesc,  // QString characteristicsDesc (decoded description)
        Count
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
