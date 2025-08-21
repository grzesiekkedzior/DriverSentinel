#include "utils/pdftableexporter.h"
#include <QFileDialog>
#include <QFutureWatcher>
#include <QPainter>
#include <QPdfWriter>
#include <QTextDocument>
#include <QtConcurrent/QtConcurrent>
#include <qapplication.h>

PdfTableExporter::PdfTableExporter(QSharedPointer<QAbstractItemModel> model,
                                   const QString &fileName,
                                   QObject *parent)
    : QObject(parent)
    , m_model(std::move(model))
    , fileName(fileName)
{}

QProgressDialog *PdfTableExporter::showProgressDialog()
{
    auto *progressDialog = new QProgressDialog("Generating PDF...",
                                               "Cancel",
                                               0,
                                               m_model->rowCount());
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setMinimumDuration(0);
    return progressDialog;
}

void PdfTableExporter::exportPdf()
{
    if (fileName.isEmpty() || !m_model)
        return;

    QProgressDialog *progressDialog = showProgressDialog();
    progressDialog->show();
    QApplication::processEvents();

    QPdfWriter writer(fileName);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setResolution(300);

    QPainter painter(&writer);
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);

    renderPdf(writer, painter, progressDialog);

    painter.end();

    progressDialog->close();
    progressDialog->deleteLater();
}

void PdfTableExporter::renderPdf(QPdfWriter &writer,
                                 QPainter &painter,
                                 QProgressDialog *progressDialog)
{
    const int rowCount = m_model->rowCount();
    const int columnCount = m_model->columnCount();
    if (columnCount <= 0)
        return;

    int y = m_marginTop;
    const int pageHeight = writer.height() - m_marginTop;

    QVector<int> colWidths = calculateColumnWidths(painter, writer.width() - 2 * m_marginLeft);

    drawHeaders(painter, y, colWidths, painter.font());

    for (int row = 0; row < rowCount; ++row) {
        if (progressDialog->wasCanceled())
            break;

        QVector<int> colHeights(columnCount, m_rowHeightMin);
        for (int col = 0; col < columnCount; ++col) {
            QString text = m_model->data(m_model->index(row, col)).toString();

            QTextDocument doc;
            doc.setPlainText(text);
            doc.setTextWidth(colWidths[col]);
            doc.setDefaultFont(painter.font());
            QTextOption opt;
            opt.setWrapMode(QTextOption::WordWrap);
            doc.setDefaultTextOption(opt);

            colHeights[col] = std::max(m_rowHeightMin, static_cast<int>(doc.size().height()));
        }

        int maxHeight = *std::max_element(colHeights.begin(), colHeights.end());

        if (y + maxHeight > pageHeight) {
            writer.newPage();
            y = m_marginTop;
            drawHeaders(painter, y, colWidths, painter.font());
        }

        int curX = m_marginLeft;
        for (int col = 0; col < columnCount; ++col) {
            QString text = m_model->data(m_model->index(row, col)).toString();

            QTextDocument docCell;
            docCell.setPlainText(text);
            docCell.setTextWidth(colWidths[col]);
            docCell.setDefaultFont(painter.font());
            QTextOption opt;
            opt.setWrapMode(QTextOption::WordWrap);
            docCell.setDefaultTextOption(opt);

            painter.save();
            painter.translate(curX, y);
            QRectF rect(0, 0, colWidths[col], maxHeight);
            docCell.drawContents(&painter, rect);
            painter.restore();

            curX += colWidths[col] + m_columnSpacing;
        }

        y += maxHeight;
        progressDialog->setValue(row + 1);
        QApplication::processEvents();
    }
}

QVector<int> PdfTableExporter::calculateColumnWidths(QPainter &painter, int usableWidth)
{
    const int columnCount = m_model->columnCount();
    QVector<int> colWidths(columnCount, m_columnWidth);

    for (int col = 0; col < columnCount; ++col) {
        int maxWidth = 0;

        QString header = m_model->headerData(col, Qt::Horizontal).toString();
        QTextDocument docHeader;
        docHeader.setPlainText(header);
        docHeader.setDefaultFont(painter.font());
        docHeader.setTextWidth(10000);
        maxWidth = std::max(maxWidth, static_cast<int>(docHeader.idealWidth()));

        for (int row = 0; row < m_model->rowCount(); ++row) {
            QString text = m_model->data(m_model->index(row, col)).toString();
            QTextDocument doc;
            doc.setPlainText(text);
            doc.setDefaultFont(painter.font());
            doc.setTextWidth(10000);
            maxWidth = std::max(maxWidth, static_cast<int>(doc.idealWidth()));
        }

        colWidths[col] = maxWidth + 10;
    }

    int totalWidth = std::accumulate(colWidths.begin(), colWidths.end(), 0)
                     + m_columnSpacing * (columnCount - 1);
    if (totalWidth > usableWidth) {
        double scale = static_cast<double>(usableWidth) / totalWidth;
        for (int &w : colWidths)
            w = static_cast<int>(w * scale);
    }

    return colWidths;
}

void PdfTableExporter::drawHeaders(QPainter &painter,
                                   int &y,
                                   const QVector<int> &colWidths,
                                   const QFont &font)
{
    const int columnCount = colWidths.size();
    if (columnCount <= 0)
        return;

    int headerMaxHeight = m_rowHeightMin;

    for (int col = 0; col < columnCount; ++col) {
        QString header = m_model->headerData(col, Qt::Horizontal).toString();
        QTextDocument docHeader;
        docHeader.setPlainText(header);
        docHeader.setTextWidth(colWidths[col]);
        docHeader.setDefaultFont(font);
        QTextOption opt;
        opt.setWrapMode(QTextOption::WordWrap);
        docHeader.setDefaultTextOption(opt);

        headerMaxHeight = std::max(headerMaxHeight, static_cast<int>(docHeader.size().height()));
    }

    int curX = m_marginLeft;
    for (int col = 0; col < columnCount; ++col) {
        QString header = m_model->headerData(col, Qt::Horizontal).toString();

        QTextDocument doc;
        doc.setPlainText(header);
        doc.setTextWidth(colWidths[col]);
        doc.setDefaultFont(font);
        QTextOption opt;
        opt.setWrapMode(QTextOption::WordWrap);
        doc.setDefaultTextOption(opt);

        painter.save();
        painter.translate(curX, y);
        QRectF rect(0, 0, colWidths[col], headerMaxHeight);
        doc.drawContents(&painter, rect);
        painter.restore();

        curX += colWidths[col] + m_columnSpacing;
    }

    y += headerMaxHeight;
}
