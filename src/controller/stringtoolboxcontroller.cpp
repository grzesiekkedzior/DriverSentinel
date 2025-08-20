#include "controller/stringtoolboxcontroller.h"
#include <QFileDialog>
#include <QFutureWatcher>
#include <QPainter>
#include <QPdfWriter>
#include <QProgressDialog>
#include <QtConcurrent/QtConcurrent>
#include "ui_mainwindow.h"

StringToolBoxController::StringToolBoxController(StringFilterProxyModel *model,
                                                 Ui::MainWindow *ui,
                                                 QObject *parent)
    : m_proxyModel{model}
    , m_ui{ui}
    , QObject{parent}
{
    pdfAction = new QAction{"PDF", this};
    txtAction = new QAction{"TXT", this};
    csvAction = new QAction{"CSV", this};
    actionMenu = new QMenu;
    actionMenu->addAction(pdfAction);
    actionMenu->addAction(txtAction);
    actionMenu->addAction(csvAction);
    m_ui->saveButton->setMenu(actionMenu);
    connect(pdfAction, &QAction::triggered, this, &StringToolBoxController::saveAsPdf);
    connect(txtAction, &QAction::triggered, this, &StringToolBoxController::saveAsTxt);
    connect(csvAction, &QAction::triggered, this, &StringToolBoxController::saveAsCsv);
}

void StringToolBoxController::saveAsPdf()
{
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save as PDF", "", "*.pdf");
    if (fileName.isEmpty())
        return;

    if (!m_proxyModel)
        return;

    auto *model = m_proxyModel.data();
    if (!model)
        return;

    QProgressDialog *progressDialog = new QProgressDialog("Generating PDF...",
                                                          "Cancel",
                                                          0,
                                                          model->rowCount());
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setMinimumDuration(0);
    progressDialog->show();

    QFuture<void> future = QtConcurrent::run([=]() {
        QPdfWriter writer(fileName);
        writer.setPageSize(QPageSize(QPageSize::A4));
        writer.setResolution(300);

        QPainter painter(&writer);
        QFont font = painter.font();
        font.setPointSize(8);
        painter.setFont(font);

        int rowCount = model->rowCount();
        int columnCount = model->columnCount();

        int marginLeft = 20;
        int marginTop = 60;
        int rowHeightMin = 20;
        int columnWidth = 150;
        int columnSpacing = 20;
        int usableWidth = writer.width() - 2 * marginLeft;
        int pageHeight = writer.height() - marginTop;

        int x = marginLeft;
        int y = marginTop;

        auto drawHeaders = [&]() {
            int headerMaxHeight = rowHeightMin;
            int lastColWidth = usableWidth - (columnCount - 1) * (columnWidth + columnSpacing);

            QTextDocument docHeader;
            docHeader.setPlainText(model->headerData(columnCount - 1, Qt::Horizontal).toString());
            docHeader.setTextWidth(lastColWidth);
            docHeader.setDefaultFont(font);
            QTextOption opt;
            opt.setWrapMode(QTextOption::WordWrap);
            docHeader.setDefaultTextOption(opt);
            int lastHeight = static_cast<int>(docHeader.size().height());
            if (lastHeight > headerMaxHeight)
                headerMaxHeight = lastHeight;

            int curX = x;
            for (int col = 0; col < columnCount; ++col) {
                int w = (col == columnCount - 1) ? lastColWidth : columnWidth;
                QString header = model->headerData(col, Qt::Horizontal).toString();

                if (col == columnCount - 1) {
                    QTextDocument doc;
                    doc.setPlainText(header);
                    doc.setTextWidth(w);
                    doc.setDefaultFont(font);
                    QTextOption option;
                    option.setWrapMode(QTextOption::WordWrap);
                    doc.setDefaultTextOption(option);

                    painter.save();
                    painter.translate(curX, y);
                    QRectF rect(0, 0, w, headerMaxHeight);
                    doc.drawContents(&painter, rect);
                    painter.restore();
                } else {
                    QRect rect(curX, y, w, headerMaxHeight);
                    painter.drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, header);
                }
                curX += w + columnSpacing;
            }

            y += headerMaxHeight;
        };

        drawHeaders();

        for (int row = 0; row < rowCount; ++row) {
            int maxHeight = rowHeightMin;
            int lastColWidth = usableWidth - (columnCount - 1) * (columnWidth + columnSpacing);

            QString lastText = model->data(model->index(row, columnCount - 1)).toString();
            QTextDocument doc;
            doc.setPlainText(lastText);
            doc.setTextWidth(lastColWidth);
            doc.setDefaultFont(font);
            QTextOption option;
            option.setWrapMode(QTextOption::WordWrap);
            doc.setDefaultTextOption(option);
            int lastHeight = static_cast<int>(doc.size().height());
            if (lastHeight > maxHeight)
                maxHeight = lastHeight;

            if (y + maxHeight > pageHeight) {
                writer.newPage();
                y = marginTop;
                drawHeaders();
            }

            int curX = x;
            for (int col = 0; col < columnCount; ++col) {
                int w = (col == columnCount - 1) ? lastColWidth : columnWidth;
                QString text = model->data(model->index(row, col)).toString();

                if (col == columnCount - 1) {
                    QTextDocument docCell;
                    docCell.setPlainText(text);
                    docCell.setTextWidth(w);
                    docCell.setDefaultFont(font);
                    QTextOption opt;
                    opt.setWrapMode(QTextOption::WordWrap);
                    docCell.setDefaultTextOption(opt);

                    painter.save();
                    painter.translate(curX, y);
                    QRectF rect(0, 0, w, maxHeight);
                    docCell.drawContents(&painter, rect);
                    painter.restore();
                } else {
                    QRect rect(curX, y, w, maxHeight);
                    painter.drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text);
                }

                curX += w + columnSpacing;
            }

            y += maxHeight;

            QMetaObject::invokeMethod(progressDialog,
                                      "setValue",
                                      Qt::QueuedConnection,
                                      Q_ARG(int, row + 1));
        }

        painter.end();
    });

    QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
    watcher->setFuture(future);
    QObject::connect(watcher, &QFutureWatcher<void>::finished, [progressDialog, watcher]() {
        progressDialog->close();
        watcher->deleteLater();
        progressDialog->deleteLater();
    });
}

void StringToolBoxController::saveAsTxt()
{
    qDebug() << "saveAsTxt";

    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save as TXT", "", "*.txt");
    if (fileName.isEmpty())
        return;

    if (!m_proxyModel)
        return;

    auto *model = m_proxyModel.data();
    if (!model)
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file for writing:" << file.errorString();
        return;
    }

    QTextStream out(&file);

    int rowCount = model->rowCount();
    int columnCount = model->columnCount();

    // columns width
    QVector<int> columnWidths(columnCount, 0);

    for (int col = 0; col < columnCount; ++col) {
        int maxWidth = model->headerData(col, Qt::Horizontal).toString().length();
        for (int row = 0; row < rowCount; ++row) {
            QString text = model->data(model->index(row, col)).toString();
            if (text.length() > maxWidth)
                maxWidth = text.length();
        }
        columnWidths[col] = maxWidth + 2;
    }

    // headers
    for (int col = 0; col < columnCount; ++col) {
        QString header = model->headerData(col, Qt::Horizontal).toString();
        out << header.leftJustified(columnWidths[col]);
    }
    out << "\n";

    // data
    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < columnCount; ++col) {
            QString text = model->data(model->index(row, col)).toString();
            out << text.leftJustified(columnWidths[col]);
        }
        out << "\n";
    }

    file.close();
}

void StringToolBoxController::saveAsCsv()
{
    qDebug() << "saveAsCsv";

    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save as CSV", "", "*.csv");
    if (fileName.isEmpty())
        return;

    if (!m_proxyModel)
        return;

    auto *model = m_proxyModel.data();
    if (!model)
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file for writing:" << file.errorString();
        return;
    }

    QTextStream out(&file);

    int rowCount = model->rowCount();
    int columnCount = model->columnCount();

    const QString sep = ";";

    for (int col = 0; col < columnCount; ++col) {
        QString header = model->headerData(col, Qt::Horizontal).toString();
        if (header.contains(sep) || header.contains("\"") || header.contains("\n")) {
            header.replace("\"", "\"\"");
            header = "\"" + header + "\"";
        }
        out << header;
        if (col < columnCount - 1)
            out << sep;
    }
    out << "\n";

    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < columnCount; ++col) {
            QString text = model->data(model->index(row, col)).toString();
            if (text.contains(sep) || text.contains("\"") || text.contains("\n")) {
                text.replace("\"", "\"\"");
                text = "\"" + text + "\"";
            }
            out << text;
            if (col < columnCount - 1)
                out << sep;
        }
        out << "\n";
    }

    file.close();
}
