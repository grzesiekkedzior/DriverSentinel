#include "controller/debuginfocontroller.h"
#include <QDialog>
#include <QPlainTextEdit>
#include "ui_mainwindow.h"
#include <LIEF/PE.hpp>
#include <utils/peutils.h>

DebugInfoController::DebugInfoController(QTableView *mainTableView,
                                         Ui::MainWindow *ui,
                                         QWidget *parent)
    : QWidget(parent)
    , m_mainTableView(mainTableView)
    , m_ui(ui)
    , m_debugInfoModel{QSharedPointer<DebugInfoModel>::create()}
    , m_debugInfo{QSharedPointer<DebugInfo>::create()}
{
    m_ui->debugInfotableView->resizeColumnsToContents();
    connect(m_mainTableView, &QTableView::clicked, this, &DebugInfoController::loadDebugInfo);
}

void DebugInfoController::loadDebugInfo(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    QVector<DebugInfo> l_debugInfo;

    QString filePath = PEUtils::getPEfilePath(m_ui->mainTable, index);
    if (filePath.isEmpty()) {
        qWarning() << "File path is empty";
        return;
    }

    try {
        std::unique_ptr<LIEF::PE::Binary> binary = LIEF::PE::Parser::parse(filePath.toStdString());

        for (const auto &dbg : binary->debug()) {
            DebugInfo di;
            di.type = QString::fromStdString(LIEF::PE::to_string(dbg.type()));
            di.characteristics = dbg.characteristics();
            di.timestamp = dbg.timestamp();
            di.majorVersion = dbg.major_version();
            di.minorVersion = dbg.minor_version();
            di.sizeOfData = dbg.sizeof_data();
            di.addressOfRawData = dbg.addressof_rawdata();
            di.pointerToRawData = dbg.pointerto_rawdata();

            auto payloadSpan = dbg.payload();
            di.payload = QByteArray(reinterpret_cast<const char *>(payloadSpan.data()),
                                    static_cast<int>(payloadSpan.size()));

            l_debugInfo.append(di);
        }

        updateModel(l_debugInfo);

    } catch (const std::exception &e) {
        qWarning() << "LIEF error:" << e.what();
    } catch (...) {
        qWarning() << "Unknown error while parsing PE";
    }

    updateModel(l_debugInfo);
}

void DebugInfoController::updateModel(const QVector<DebugInfo> &di)
{
    if (!m_debugInfoModel)
        return;
    m_debugInfoModel->setDebugDataToView(di);
    for (int row = 0; row < m_debugInfoModel->rowCount(QModelIndex()); ++row) {
        auto idx = m_debugInfoModel->index(row, 8);
        QPushButton *btn = new QPushButton("Show", m_ui->debugInfotableView);

        m_ui->debugInfotableView->setIndexWidget(idx, btn);

        connect(btn, &QPushButton::clicked, this, [=, this]() { openPayloadDialog(row); });
    }

    m_ui->debugInfotableView->resizeColumnsToContents();
}

void DebugInfoController::openPayloadDialog(int row)
{
    const DebugInfo &di = m_debugInfoModel->debugInfo().at(row);
    QDialog payloadDialog(this);
    payloadDialog.setWindowIcon(QIcon::fromTheme(QIcon::ThemeIcon::DialogInformation));
    payloadDialog.setWindowTitle("Payload");

    QPlainTextEdit *payloadTextEdit = new QPlainTextEdit(&payloadDialog);
    payloadTextEdit->setFont(QFont("Courier", 12));
    payloadTextEdit->setPlainText(formatHexPayload(di.payload));
    payloadTextEdit->setReadOnly(true);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(payloadTextEdit);
    payloadDialog.setLayout(layout);

    QFontMetrics fm(payloadTextEdit->font());
    int charWidth = fm.horizontalAdvance('X');
    int totalWidth = charWidth * (16 * 2 + 15 + 16 + 4) + 16;
    payloadDialog.resize(totalWidth, 400);

    payloadDialog.exec();
}

QString DebugInfoController::formatHexPayload(const QByteArray &payload)
{
    QStringList lines;
    const int bytesPerLine = 16;

    for (int i = 0; i < payload.size(); i += bytesPerLine) {
        QByteArray chunk = payload.mid(i, bytesPerLine);
        QStringList hexBytes;
        for (char byte : chunk) {
            hexBytes << QString("%1")
                            .arg(static_cast<unsigned char>(byte), 2, 16, QLatin1Char('0'))
                            .toUpper();
        }
        lines << hexBytes.join(" ");
    }

    return lines.join("\n");
}

void DebugInfoController::clear()
{
    if (!m_debugInfoModel)
        return;
    m_debugInfoModel->setDebugDataToView({});
}

QSharedPointer<DebugInfoModel> DebugInfoController::debugInfoModel() const
{
    return m_debugInfoModel;
}

QSharedPointer<DebugInfo> DebugInfoController::debugInfo() const
{
    return m_debugInfo;
}
