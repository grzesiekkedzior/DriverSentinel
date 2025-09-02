#include "controller/disassemblycontroller.h"
#include <QtConcurrent>
#include "ui_mainwindow.h"
#include "utils/peutils.h"
#include <LIEF/PE.hpp>

DisassemblyController::DisassemblyController(QSharedPointer<DisassemblyData> disassemblyData,
                                             QTableView *mainTableView,
                                             Ui::MainWindow *ui,
                                             QObject *parent)
    : QObject(parent)
    , m_disassemblyData(std::move(disassemblyData))
    , m_mainTableView(mainTableView)
    , m_ui(ui)
{
    m_disassemblyModel = QSharedPointer<DisassemblyModel>::create();
    connect(m_mainTableView,
            &QTableView::clicked,
            this,
            &DisassemblyController::loadAssemblyDataToView);
    m_ui->tableViewAsm->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_ui->dialectComboBox->addItem("Intel", AsmDialect::DIALECT_INTEL);
    m_ui->dialectComboBox->addItem("AT&T", AsmDialect::DIALECT_ATT);
    m_ui->dialectComboBox->addItem("MASM", AsmDialect::DIALECT_MASM);
    m_ui->dialectComboBox->addItem("NoRegName", AsmDialect::DIALECT_NOREGNAME);
}

void DisassemblyController::loadAssemblyDataToView(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString filePath = PEUtils::getPEfilePath(m_ui->mainTable, index);
    if (filePath.isEmpty()) {
        qWarning() << "File path is empty";
        return;
    }

    auto *progress = new QProgressBar;
    progress->setRange(0, 100);
    progress->setValue(0);
    m_ui->statusbar->addPermanentWidget(progress);

    m_ui->statusbar->showMessage("Starting disassembly...");

    QFuture<QVector<DisassemblyData>> future
        = extractAsm(filePath,
                     progress,
                     static_cast<AsmDialect>(m_ui->dialectComboBox->currentData().toInt()));

    auto watcher = new QFutureWatcher<QVector<DisassemblyData>>(this);

    connect(watcher,
            &QFutureWatcher<QVector<DisassemblyData>>::finished,
            this,
            [this, watcher, progress]() {
                QVector<DisassemblyData> result = watcher->result();
                updateModel(result);

                m_ui->statusbar->showMessage("Disassembly finished.", 5000);
                m_ui->statusbar->removeWidget(progress);
                progress->deleteLater();

                watcher->deleteLater();
            });

    watcher->setFuture(future);
}

void DisassemblyController::clear()
{
    qDebug() << "Hello asm clear function";
    if (m_disassemblyModel)
        m_disassemblyModel->clearModel();
}

void DisassemblyController::updateModel(const QVector<DisassemblyData> &dd)
{
    if (m_disassemblyModel)
        m_disassemblyModel->setAsmDataView(dd);
}

void DisassemblyController::setDialect(csh handle, AsmDialect dialect)
{
    switch (dialect) {
    case DIALECT_INTEL:
        cs_option(handle, CS_OPT_SYNTAX, CS_OPT_SYNTAX_INTEL);
        break;
    case DIALECT_ATT:
        cs_option(handle, CS_OPT_SYNTAX, CS_OPT_SYNTAX_ATT);
        break;
    case DIALECT_MASM:
        cs_option(handle, CS_OPT_SYNTAX, CS_OPT_SYNTAX_MASM);
        break;
    case DIALECT_NOREGNAME:
        cs_option(handle, CS_OPT_SYNTAX, CS_OPT_SYNTAX_NOREGNAME);
        break;
    default:
        cs_option(handle, CS_OPT_SYNTAX, CS_OPT_SYNTAX_INTEL);
        break;
    }
}

QFuture<QVector<DisassemblyData>> DisassemblyController::extractAsm(QString filePath,
                                                                    QProgressBar *progress,
                                                                    AsmDialect dialect)
{
    return QtConcurrent::run([this, filePath, progress, dialect]() {
        QVector<DisassemblyData> l_disassemblerData;

        try {
            std::unique_ptr<LIEF::PE::Binary> binary = LIEF::PE::Parser::parse(
                filePath.toStdString());
            if (!binary)
                return QVector<DisassemblyData>{};

            LIEF::PE::Header::MACHINE_TYPES machine = binary->header().machine();
            cs_mode mode = (machine == LIEF::PE::Header::MACHINE_TYPES::AMD64) ? CS_MODE_64
                                                                               : CS_MODE_32;

            const auto &sections = binary->sections();
            int totalSections = sections.size();
            int sectionIndex = 0;

            for (const auto &section : sections) {
                auto content = section.content();
                if (content.empty())
                    continue;

                uint64_t address = section.virtual_address()
                                   + binary->optional_header().imagebase();

                csh handle;
                cs_open(CS_ARCH_X86, mode, &handle);

                setDialect(handle, dialect);

                cs_insn *insn = nullptr;
                size_t count = cs_disasm(handle, content.data(), content.size(), address, 0, &insn);

                if (count > 0) {
                    for (size_t i = 0; i < count; i++) {
                        DisassemblyData dasm;
                        dasm.section = QString::fromStdString(section.name());
                        dasm.address = insn[i].address;
                        dasm.bytes = QByteArray(reinterpret_cast<const char *>(insn[i].bytes),
                                                insn[i].size);
                        dasm.mnemonic = QString::fromUtf8(insn[i].mnemonic);
                        dasm.operands = QString::fromUtf8(insn[i].op_str);
                        dasm.comment = "";

                        l_disassemblerData.append(dasm);
                    }
                    cs_free(insn, count);
                }

                cs_close(&handle);

                // Progress actualization
                if (progress) {
                    int value = static_cast<int>((++sectionIndex * 100) / totalSections);
                    QMetaObject::invokeMethod(progress,
                                              "setValue",
                                              Qt::QueuedConnection,
                                              Q_ARG(int, value));
                }
            }

        } catch (const std::exception &e) {
            qWarning() << "LIEF error:" << e.what();
        } catch (...) {
            qWarning() << "Unknown error while parsing PE";
        }

        return l_disassemblerData;
    });
}

QSharedPointer<DisassemblyModel> DisassemblyController::disassemblyModel() const
{
    return m_disassemblyModel;
}
