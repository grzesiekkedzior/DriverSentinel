#ifndef STRINGFILTERPROXYMODEL_H
#define STRINGFILTERPROXYMODEL_H
#include <QSortFilterProxyModel>

namespace Ui {
class MainWindow;
}

class StringFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    StringFilterProxyModel(QObject *parent = nullptr);

    // QSortFilterProxyModel interface
    QString filteringString() const;
    void setFilteringString(const QString &newFilteringString);

    std::optional<int> getMin() const;
    void setMin(std::optional<int> newMin);

    std::optional<int> getMax() const;
    void setMax(std::optional<int> newMax);

    bool getRegex() const;
    void setRegex(bool newRegex);

    bool getCaseSensitive() const;
    void setCaseSensitive(bool newCaseSensitive);
    void resetModel();

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QString m_filteringString{};
    std::optional<int> min{};
    std::optional<int> max{INT_MAX};
    bool regex{};
    bool caseSensitive{};

    Ui::MainWindow *ui{};
    bool isLengthOutOfRange() const;
};

#endif // STRINGFILTERPROXYMODEL_H
