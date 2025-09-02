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
    m_delegate = new DisassemblyDelegate(m_ui->tableViewAsm);
    m_ui->tableViewAsm->setItemDelegate(m_delegate);
    connect(m_mainTableView,
            &QTableView::clicked,
            this,
            &DisassemblyController::loadAssemblyDataToView);
    connect(m_ui->dialectComboBox,
            QOverload<int>::of(&QComboBox::activated),
            this,
            &DisassemblyController::onDialectChanged);
    connect(m_ui->radioButtonColorSyntax,
            &QRadioButton::clicked,
            this,
            &DisassemblyController::turnOnOffColorSyntax);

    //m_ui->tableViewAsm->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
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

void DisassemblyController::onDialectChanged(int index)
{
    Q_UNUSED(index);
    QModelIndex currentIndex = m_mainTableView->currentIndex();
    loadAssemblyDataToView(currentIndex);
}

void DisassemblyController::turnOnOffColorSyntax(bool syntax)
{
    qDebug() << "Syntax: " << syntax;
    if (m_delegate)
        m_delegate->setSyntaxColoring(syntax);

    m_ui->tableViewAsm->viewport()->update();
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

                cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);

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
                        QString comment;

                        if (insn[i].detail) {
                            cs_x86 *x86 = &insn[i].detail->x86;

                            switch (insn[i].id) {
                            case X86_INS_JMP:
                                comment += "Jump instruction; ";
                                break;
                            case X86_INS_JE:
                                comment += "Jump if equal; ";
                                break;
                            case X86_INS_JNE:
                                comment += "Jump if not equal; ";
                                break;
                            case X86_INS_JG:
                                comment += "Jump if greater; ";
                                break;
                            case X86_INS_JGE:
                                comment += "Jump if greater or equal; ";
                                break;
                            case X86_INS_JL:
                                comment += "Jump if less; ";
                                break;
                            case X86_INS_JLE:
                                comment += "Jump if less or equal; ";
                                break;
                            case X86_INS_JO:
                                comment += "Jump if overflow; ";
                                break;
                            case X86_INS_JS:
                                comment += "Jump if sign; ";
                                break;
                            case X86_INS_CALL:
                                comment += "Call instruction; ";
                                break;
                            case X86_INS_RET:
                                comment += "Return instruction; ";
                                break;
                            case X86_INS_INT:
                                comment += "Interrupt; ";
                                break;
                            case X86_INS_PUSH:
                                comment += "Push to stack; ";
                                break;
                            case X86_INS_POP:
                                comment += "Pop from stack; ";
                                break;
                            case X86_INS_CMP:
                                comment += "Compare; ";
                                break;
                            default:
                                comment += "Instruction; ";
                                break;
                            }

                            // jump addresses
                            if (insn[i].id == X86_INS_CALL || insn[i].id == X86_INS_JMP
                                || (insn[i].id >= X86_INS_JE && insn[i].id <= X86_INS_JS)) {
                                if (x86->op_count > 0) {
                                    const cs_x86_op &op = x86->operands[0];
                                    if (op.type == X86_OP_IMM) {
                                        comment += QString("Target: 0x%1; ").arg(op.imm, 0, 16);
                                    } else if (op.type == X86_OP_REG) {
                                        comment += QString("Register target: %1; ")
                                                       .arg(cs_reg_name(handle, op.reg));
                                    }
                                }
                            }

                            if (x86->op_count > 0) {
                                comment += "Operands: ";
                                for (int op_idx = 0; op_idx < x86->op_count; op_idx++) {
                                    const cs_x86_op &op = x86->operands[op_idx];
                                    switch (op.type) {
                                    case X86_OP_REG:
                                        comment += QString("%1 ").arg(cs_reg_name(handle, op.reg));
                                        break;
                                    case X86_OP_IMM:
                                        if (op.size == 1)
                                            comment += QString("0x%1(byte) ").arg(op.imm, 0, 16);
                                        else if (op.size == 2)
                                            comment += QString("0x%1(word) ").arg(op.imm, 0, 16);
                                        else if (op.size == 4)
                                            comment += QString("0x%1(dword) ").arg(op.imm, 0, 16);
                                        else if (op.size == 8)
                                            comment += QString("0x%1(qword) ").arg(op.imm, 0, 16);
                                        break;
                                    case X86_OP_MEM:
                                        comment += "[";
                                        if (op.mem.base != X86_REG_INVALID)
                                            comment += cs_reg_name(handle, op.mem.base);
                                        if (op.mem.index != X86_REG_INVALID)
                                            comment += QString("+%1*%2")
                                                           .arg(cs_reg_name(handle, op.mem.index))
                                                           .arg(op.mem.scale);
                                        if (op.mem.disp != 0)
                                            comment += QString("+0x%1").arg(op.mem.disp, 0, 16);
                                        comment += "] ";
                                        break;
                                    default:
                                        comment += "other ";
                                        break;
                                    }
                                }
                            }
                        }

                        dasm.comment = comment.trimmed();
                        l_disassemblerData.append(dasm);

                        // qDebug() << "Section:" << dasm.section
                        //          << "Address:" << QString::number(dasm.address, 16)
                        //          << "Mnemonic:" << dasm.mnemonic << "Operands:" << dasm.operands
                        //          << "Comment:" << dasm.comment << "Bytes:" << dasm.bytes.toHex();
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
