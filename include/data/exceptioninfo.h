#ifndef EXCEPTIONINFO_H
#define EXCEPTIONINFO_H
#include <QString>
#include <QVector>

struct ExceptionInfo
{
    quint32 fileOffset = 0;
    quint32 beginRVA = 0;
    quint32 endRVA = 0;
    quint32 unwindRVA = 0;

    quint8 version = 0;
    quint8 flags = 0;
    quint8 sizeOfProlog = 0;
    quint8 countOfCodes = 0;
    quint16 frameRegister = 0;
    quint16 frameOffset = 0;
    quint32 exceptionHandlerRVA = 0;

    struct UnwindOp
    {
        quint8 opcode = 0;
        quint8 opinfo = 0;
        QString regName;
        quint32 offset = 0;
        quint32 fileOffset = 0;
    };

    QVector<UnwindOp> unwindOps;
};

#endif // EXCEPTIONINFO_H
