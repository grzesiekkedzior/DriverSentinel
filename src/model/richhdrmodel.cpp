#include "model/richhdrmodel.h"

RichHdrModel::RichHdrModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int RichHdrModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_richHdrData.size();
}

int RichHdrModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(RichHeaderColumn::ColumnCount);
}

QVariant RichHdrModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    auto column = static_cast<RichHeaderColumn>(index.column());
    const RichHeaderData &rhd = m_richHdrData.at(index.row());

    switch (column) {
    case RichHeaderColumn::Offset:
        return QString("0x%1").arg(rhd.offset, 0, 16).toUpper().replace('X', 'x');
    case RichHeaderColumn::Key:
        return QString("0x%1").arg(rhd.key, 0, 16).toUpper().replace('X', 'x');
    case RichHeaderColumn::ProductID:
        return rhd.product_id;
    case RichHeaderColumn::BuildID:
        return QString::number(rhd.buildId);
    case RichHeaderColumn::Count:
        return QString::number(rhd.count);
    // case RichHeaderColumn::VSVersion:
    //     return rhd.vsVersion;
    case RichHeaderColumn::RawValue:
        return QString("0x%1").arg(rhd.rawValue, 0, 16).toUpper().replace('X', 'x');
    case RichHeaderColumn::UnmaskedValue:
        return QString("0x%1").arg(QString::number(rhd.unmaskedValue, 16).toUpper());

    case RichHeaderColumn::RawData:
        return QString(rhd.rawData.toHex().toUpper());
    case RichHeaderColumn::UnmaskedData:
        return QString(rhd.unmaskedData.toHex().toUpper());
    }

    return {};
}

QVariant RichHdrModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (static_cast<RichHeaderColumn>(section)) {
        case RichHeaderColumn::Offset:
            return "Offset";
        case RichHeaderColumn::Key:
            return "Key";
        case RichHeaderColumn::ProductID:
            return "Product ID";
        case RichHeaderColumn::BuildID:
            return "Build ID";
        case RichHeaderColumn::Count:
            return "Count";
        // case RichHeaderColumn::VSVersion:
        //     return "VS Version";
        case RichHeaderColumn::RawValue:
            return "Raw Value";
        case RichHeaderColumn::UnmaskedValue:
            return "Unmasked Value";
        case RichHeaderColumn::RawData:
            return "Raw Data";
        case RichHeaderColumn::UnmaskedData:
            return "Unmasked Data";
        }
    }

    return QVariant();
}

void RichHdrModel::setRichHdrSection(const QVector<RichHeaderData> &richInfo)
{
    beginResetModel();
    m_richHdrData = richInfo;
    endResetModel();
}

void RichHdrModel::clear()
{
    beginResetModel();
    m_richHdrData.clear();
    endResetModel();
}
