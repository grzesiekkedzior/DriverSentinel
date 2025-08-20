#include "controller/stringtoolboxcontroller.h"
#include <QFileDialog>
#include <QPainter>
#include <QPdfWriter>
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
    qDebug() << "saveAsPdf";

    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save as PDF", "", "*.pdf");
    if (fileName.isEmpty())
        return;

    QPdfWriter writer(fileName);
    writer.setPageSize(QPageSize::A4);
    writer.setResolution(300);

    QPainter painter(&writer);

    if (!m_proxyModel)
        return;

    auto *model = m_proxyModel.data();
    if (!model)
        return;

    int rowCount = model->rowCount();
    int columnCount = model->columnCount();

    int x = 20;
    int y = 60;
    int rowHeight = 25;
    int columnWidth = 250;

    for (int col = 0; col < columnCount; ++col) {
        QString header = model->headerData(col, Qt::Horizontal).toString();
        painter.drawText(x + col * columnWidth, y, header);
    }

    y += rowHeight;

    for (int row = 0; row < rowCount; ++row) {
        int maxHeight = rowHeight;

        for (int col = 0; col < columnCount; ++col) {
            QString text = model->data(model->index(row, col)).toString();
            QRect rect(x + col * columnWidth, y, columnWidth, INT_MAX);

            QFontMetrics fm(painter.font());
            QRect needed = fm.boundingRect(rect, Qt::TextWordWrap, text);

            painter.drawText(needed, Qt::TextWordWrap, text);

            if (needed.height() > maxHeight)
                maxHeight = needed.height();
        }

        y += maxHeight;
    }

    painter.end();
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
