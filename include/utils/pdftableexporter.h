#ifndef PDFTABLEEXPORTER_H
#define PDFTABLEEXPORTER_H
#include <QAbstractItemModel>
#include <QObject>
#include <QProgressDialog>
#include <qpdfwriter.h>

class PdfTableExporter : public QObject
{
    Q_OBJECT
public:
    PdfTableExporter(QSharedPointer<QAbstractItemModel> model,
                     const QString &fileName,
                     QObject *parent = nullptr);

    QProgressDialog *showProgressDialog();
    void drawHeaders(QPainter &painter, int &y, const QVector<int> &colWidths, const QFont &font);
    void exportPdf();
    void renderPdf(QPdfWriter &writer, QPainter &painter, QProgressDialog *progressDialog);
    QVector<int> calculateColumnWidths(QPainter &painter, int usableWidth);

private:
    int m_marginLeft = 20;
    int m_marginTop = 60;
    int m_rowHeightMin = 20;
    int m_columnWidth = 150;
    int m_columnSpacing = 20;

    QSharedPointer<QAbstractItemModel> m_model;
    QString fileName{};
};

#endif // PDFTABLEEXPORTER_H
