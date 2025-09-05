#ifndef RICHHDRDATA_H
#define RICHHDRDATA_H
#pragma once
#include <QByteArray>
#include <QString>
#include <QtGlobal>

struct RichHeaderData
{
    quint32 offset{};
    quint32 key{};

    QString product_id{};
    quint16 buildId{};
    quint32 count{};
    //QString vsVersion;

    quint64 rawValue{};
    quint64 unmaskedValue{};

    QByteArray rawData;
    QByteArray unmaskedData;
};

#endif // RICHHDRDATA_H
