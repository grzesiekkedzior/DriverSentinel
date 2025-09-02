#ifndef DISASSEMBLYDELEGATE_H
#define DISASSEMBLYDELEGATE_H

#include <QStyledItemDelegate>
#include <QTextLayout>

class DisassemblyDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DisassemblyDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    void setSyntaxColoring(bool on);

private:
    QColor tokenColor(const QString &token) const;

    QHash<QString, QColor> mnemonics;
    QHash<QString, QColor> registers;

    bool m_syntax = true;
};
#endif // DISASSEMBLYDELEGATE_H
