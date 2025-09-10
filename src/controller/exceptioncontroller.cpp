#include "controller/exceptioncontroller.h"
#include <QFutureWatcher>
#include <QtConcurrent>
#include "ui_mainwindow.h"
#include "utils/peutils.h"
#include <LIEF/PE.hpp>
namespace {

QString regToString(LIEF::PE::unwind_x64::Code::REG reg)
{
    switch (reg) {
    case LIEF::PE::unwind_x64::Code::REG::RAX:
        return "RAX";
    case LIEF::PE::unwind_x64::Code::REG::RCX:
        return "RCX";
    case LIEF::PE::unwind_x64::Code::REG::RDX:
        return "RDX";
    case LIEF::PE::unwind_x64::Code::REG::RBX:
        return "RBX";
    case LIEF::PE::unwind_x64::Code::REG::RSP:
        return "RSP";
    case LIEF::PE::unwind_x64::Code::REG::RBP:
        return "RBP";
    case LIEF::PE::unwind_x64::Code::REG::RSI:
        return "RSI";
    case LIEF::PE::unwind_x64::Code::REG::RDI:
        return "RDI";
    case LIEF::PE::unwind_x64::Code::REG::R8:
        return "R8";
    case LIEF::PE::unwind_x64::Code::REG::R9:
        return "R9";
    case LIEF::PE::unwind_x64::Code::REG::R10:
        return "R10";
    case LIEF::PE::unwind_x64::Code::REG::R11:
        return "R11";
    case LIEF::PE::unwind_x64::Code::REG::R12:
        return "R12";
    case LIEF::PE::unwind_x64::Code::REG::R13:
        return "R13";
    case LIEF::PE::unwind_x64::Code::REG::R14:
        return "R14";
    case LIEF::PE::unwind_x64::Code::REG::R15:
        return "R15";
    default:
        return "";
    }
}

ExceptionInfo::UnwindOp mapOpcode(const LIEF::PE::unwind_x64::Code *code)
{
    ExceptionInfo::UnwindOp uop;
    uop.opcode = static_cast<quint8>(code->opcode());
    uop.offset = code->position();

    using namespace LIEF::PE::unwind_x64;

    if (auto pnv = dynamic_cast<const PushNonVol *>(code)) {
        uop.regName = regToString(pnv->reg());
    } else if (auto alloc = dynamic_cast<const Alloc *>(code)) {
        uop.opinfo = alloc->size();
    } else if (auto snv = dynamic_cast<const SaveNonVolatile *>(code)) {
        uop.regName = regToString(snv->reg());
        uop.offset = snv->offset();
    } else if (auto setfp = dynamic_cast<const SetFPReg *>(code)) {
        uop.regName = regToString(setfp->reg());
    } else if (auto xmm = dynamic_cast<const SaveXMM128 *>(code)) {
        uop.regName = QString("XMM%1").arg(xmm->num());
        uop.offset = xmm->offset();
    } else if (auto pmf = dynamic_cast<const PushMachFrame *>(code)) {
        uop.opinfo = pmf->value();
    } else {
        uop.regName = "UNKNOWN";
    }

    return uop;
}

} // namespace
ExceptionController::ExceptionController(QTableView *mainTableView,
                                         Ui::MainWindow *ui,
                                         QObject *parent)
    : QObject(parent)
    , m_mainTableView(mainTableView)
    , m_ui(ui)
    , m_exceptionModel{QSharedPointer<ExceptionModel>::create()}
    , m_exceptionInfo{QSharedPointer<ExceptionInfo>::create()}
{
    m_ui->exceptionTableView->resizeColumnsToContents();
    connect(m_mainTableView, &QTableView::clicked, this, &ExceptionController::loadExceptionInfo);
}
QString unwindOpToString(const ExceptionInfo::UnwindOp &uop)
{
    return QString("Opcode=%1, Offset=%2, OpInfo=%3, Reg=%4")
        .arg(uop.opcode)
        .arg(uop.offset)
        .arg(uop.opinfo)
        .arg(uop.regName.isEmpty() ? "-" : uop.regName);
}

void ExceptionController::loadExceptionInfo(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString filePath = PEUtils::getPEfilePath(m_ui->mainTable, index);
    if (filePath.isEmpty()) {
        qWarning() << "File path is empty";
        return;
    }

    auto *watcher = new QFutureWatcher<QVector<ExceptionInfo>>(this);

    connect(watcher, &QFutureWatcher<QVector<ExceptionInfo>>::finished, this, [this, watcher]() {
        QVector<ExceptionInfo> result = watcher->result();
        updateModel(result);
        watcher->deleteLater();
    });

    QFuture<QVector<ExceptionInfo>> future = QtConcurrent::run(
        [filePath]() -> QVector<ExceptionInfo> {
            QVector<ExceptionInfo> vec;

            try {
                LIEF::PE::ParserConfig cfg;
                cfg.parse_exceptions = true;
                std::unique_ptr<LIEF::PE::Binary> binary
                    = LIEF::PE::Parser::parse(filePath.toStdString(), cfg);

                if (!binary)
                    return vec;

                for (auto &ex : binary->exceptions()) {
                    if (auto rf = ex.as<LIEF::PE::RuntimeFunctionX64>()) {
                        ExceptionInfo exInfo;
                        exInfo.beginRVA = rf->rva_start();
                        exInfo.endRVA = rf->rva_end();
                        exInfo.unwindRVA = rf->unwind_rva();

                        if (auto ui = rf->unwind_info()) {
                            exInfo.version = ui->version;
                            exInfo.flags = ui->flags;
                            exInfo.sizeOfProlog = ui->sizeof_prologue;
                            exInfo.countOfCodes = ui->count_opcodes;
                            exInfo.frameRegister = ui->frame_reg;
                            exInfo.frameOffset = ui->frame_reg_offset;
                            exInfo.exceptionHandlerRVA = ui->handler.value_or(0);

                            for (const auto &opcode : ui->opcodes()) {
                                if (auto epilog = dynamic_cast<const LIEF::PE::unwind_x64::Epilog *>(
                                        opcode.get())) {
                                    exInfo.hasEpilog = true;
                                    exInfo.epilogFlags = epilog->flags();
                                    exInfo.epilogSize = epilog->size();
                                } else {
                                    auto uop = mapOpcode(opcode.get());
                                    exInfo.unwindOps.append(uop);
                                }
                            }
                        }

                        vec.append(exInfo);
                    }
                }
            } catch (const std::exception &e) {
                qWarning() << "Exception parsing PE:" << e.what();
            } catch (...) {
                qWarning() << "Unknown exception parsing PE";
            }

            return vec;
        });

    watcher->setFuture(future);
}

void ExceptionController::updateModel(const QVector<ExceptionInfo> &ei)
{
    if (!m_exceptionModel)
        return;
    m_exceptionModel->loadExceptionDataToView(ei);
}

void ExceptionController::clear()
{
    if (!m_exceptionModel)
        return;
    m_exceptionModel->loadExceptionDataToView({});
}

quint32 ExceptionController::rvaToFileOffset(const LIEF::PE::Binary &binary, quint32 rva)
{
    auto sect = binary.section_from_rva(rva);
    if (sect) {
        qDebug() << "RVA:" << QString::number(rva, 16)
                 << "-> Section:" << QString::fromStdString(sect->name())
                 << "VA:" << QString::number(sect->virtual_address(), 16)
                 << "Raw:" << QString::number(sect->pointerto_raw_data(), 16);
        return rva - sect->virtual_address() + sect->pointerto_raw_data();
    }
    return 0;
}

QSharedPointer<ExceptionModel> ExceptionController::exceptionModel() const
{
    return m_exceptionModel;
}

QSharedPointer<ExceptionInfo> ExceptionController::exceptionInfo() const
{
    return m_exceptionInfo;
}
