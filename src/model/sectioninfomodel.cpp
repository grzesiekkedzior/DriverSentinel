#include "model/sectioninfomodel.h"

SectionInfoModel::SectionInfoModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int SectionInfoModel::rowCount(const QModelIndex &parent) const {}

int SectionInfoModel::columnCount(const QModelIndex &parent) const {}

QVariant SectionInfoModel::data(const QModelIndex &index, int role) const {}

QVariant SectionInfoModel::headerData(int section, Qt::Orientation orientation, int role) const {}

void SectionInfoModel::setPESecton(const QVector<SectionInfo> &sectionInfo) {}

void SectionInfoModel::clear() {}
