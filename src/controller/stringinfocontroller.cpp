#include "controller/stringinfocontroller.h"
#include "ui_mainwindow.h"
#include <LIEF/PE.hpp>
#include <utils/peutils.h>

StringInfoController::StringInfoController(QSharedPointer<StringInfo> stringInfo,
                                           QTableView *mainTableView,
                                           Ui::MainWindow *ui,
                                           QObject *parent)
    : QObject(parent)
    , m_stringInfo(std::move(stringInfo))
    , m_mainTableView(mainTableView)
    , m_ui(ui)
{
    m_stringInfoModel = QSharedPointer<StringInfoModel>::create();
}

void StringInfoController::loadStringDataToView(const QModelIndex &index)
{
    QVector<StringInfo> l_stringInfoVector;
    StringInfo l_stringInfo;

    if (!index.isValid())
        return;

    QString filePath = PEUtils::getPEfilePath(m_ui->mainTable, index);
    if (filePath.isEmpty()) {
        qWarning() << "File path is empty";
        return;
    }

    try {
        std::unique_ptr<LIEF::PE::Binary> binary = LIEF::PE::Parser::parse(filePath.toStdString());
        const auto &sections = binary->sections();

        for (const auto &section : sections) {
            auto vec = section.content();
            std::string ascii;
            const size_t minLength = 4;
            size_t startPos = 0;
            bool inString = false;

            for (size_t i = 0; i < vec.size(); ++i) {
                if (std::isprint(static_cast<unsigned char>(vec[i]))) {
                    if (!inString) {
                        inString = true;
                        startPos = i;
                    }
                    ascii += static_cast<char>(vec[i]);
                } else {
                    if (inString && ascii.length() >= minLength) {
                        l_stringInfo.section = QString::fromStdString(section.name());
                        l_stringInfo.offset
                            = QString("0x%1").arg(section.offset() + startPos, 0, 16).toUpper();
                        l_stringInfo.sectionStrng = QString::fromStdString(ascii);
                        l_stringInfoVector.append(l_stringInfo);
                    }
                    ascii.clear();
                    inString = false;
                }
            }

            if (inString && ascii.length() >= minLength) {
                l_stringInfo.section = QString::fromStdString(section.name());
                l_stringInfo.offset
                    = QString("0x%1").arg(section.offset() + startPos, 0, 16).toUpper();
                l_stringInfo.sectionStrng = QString::fromStdString(ascii);
                l_stringInfoVector.append(l_stringInfo);
            }
        }
    } catch (const std::exception &e) {
        qWarning() << "LIEF error:" << e.what();
    } catch (...) {
        qWarning() << "Unknown error while parsing PE";
    }

    updateModel(l_stringInfoVector);
}

void StringInfoController::updateModel(const QVector<StringInfo> &si)
{
    if (!m_stringInfoModel)
        return;
    m_stringInfoModel->setStringInfo(si);
}

void StringInfoController::clear() {}

QSharedPointer<StringInfoModel> StringInfoController::stringInfoModel() const
{
    return m_stringInfoModel;
}
