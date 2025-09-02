#include "utils/disassemblydelegate.h"
#include <QApplication>
#include <QPainter>
#include <QRegularExpression>

DisassemblyDelegate::DisassemblyDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    QStringList mnem = {"mov",  "movzx", "movsx", "push",  "pop",   "add",  "sub",  "mul",  "imul",
                        "div",  "idiv",  "inc",   "dec",   "and",   "or",   "xor",  "not",  "neg",
                        "shl",  "shr",   "sal",   "sar",   "cmp",   "test", "jmp",  "je",   "jne",
                        "jg",   "jge",   "jl",    "jle",   "ja",    "jb",   "jo",   "js",   "jz",
                        "jnz",  "call",  "ret",   "leave", "enter", "nop",  "hlt",  "int",  "lea",
                        "xchg", "bswap", "cdq",   "cwd",   "cbw",   "cwde", "stos", "lods", "scas",
                        "cmps", "outs",  "ins"};

    for (const QString &s : mnem)
        mnemonics[s] = Qt::blue;

    QStringList regs = {"rax",  "rbx",  "rcx",  "rdx", "rsi",  "rdi",  "rsp",  "rbp",  "eax",
                        "ebx",  "ecx",  "edx",  "esi", "edi",  "esp",  "ebp",  "ax",   "bx",
                        "cx",   "dx",   "si",   "di",  "sp",   "bp",   "al",   "bl",   "cl",
                        "dl",   "ah",   "bh",   "ch",  "dh",   "r8",   "r9",   "r10",  "r11",
                        "r12",  "r13",  "r14",  "r15", "r8d",  "r9d",  "r10d", "r11d", "r12d",
                        "r13d", "r14d", "r15d", "r8w", "r9w",  "r10w", "r11w", "r12w", "r13w",
                        "r14w", "r15w", "r8b",  "r9b", "r10b", "r11b", "r12b", "r13b", "r14b",
                        "r15b", "cs",   "ds",   "es",  "fs",   "gs",   "ss"};

    for (const QString &s : regs)
        registers[s] = Qt::darkGreen;
}

QColor DisassemblyDelegate::tokenColor(const QString &token) const
{
    QString t = token.toLower();

    QString cleaned = t;
    cleaned.remove(QRegularExpression("^[\\[\\](%,)]+")); // remove [ ( %
    cleaned.remove(QRegularExpression("[\\[\\]()%,]+$")); // remove ] ) %

    for (auto it = mnemonics.begin(); it != mnemonics.end(); ++it) {
        if (cleaned.startsWith(it.key()))
            return it.value();
    }

    if (registers.contains(cleaned))
        return registers[cleaned];

    if (cleaned.startsWith("0x"))
        return Qt::red;

    return Qt::black;
}

void DisassemblyDelegate::setSyntaxColoring(bool on)
{
    m_syntax = on;
}

void DisassemblyDelegate::paint(QPainter *painter,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    if (!m_syntax) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    painter->save();

    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);

    opt.text.clear();
    opt.widget->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    QString text = index.data().toString();
    if (text.isEmpty()) {
        painter->restore();
        return;
    }

    QTextLayout layout(text, opt.font);

    QList<QTextLayout::FormatRange> formats;
    int pos = 0;
    QStringList tokens = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    for (const QString &token : tokens) {
        QTextLayout::FormatRange fr;
        fr.start = pos;
        fr.length = token.length();
        fr.format.setForeground(tokenColor(token));
        formats.append(fr);
        pos += token.length() + 1;
    }

    layout.setFormats(formats);

    QTextOption textOption;
    textOption.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layout.setTextOption(textOption);

    layout.beginLayout();
    QTextLine line = layout.createLine();
    line.setLineWidth(option.rect.width());
    layout.endLayout();

    QRect textRect = option.rect;
    textRect.adjust(3, 0, -2, 0);

    qreal height = layout.boundingRect().height();
    qreal y = textRect.top() + (textRect.height() - height) / 2;

    layout.draw(painter, QPointF(textRect.left(), y));

    painter->restore();
}
