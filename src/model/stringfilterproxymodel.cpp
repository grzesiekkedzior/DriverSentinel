#include "model/stringfilterproxymodel.h"
#include <QRegularExpression>

StringFilterProxyModel::StringFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseSensitive);
}

bool StringFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, 2, source_parent);
    QString value = sourceModel()->data(index).toString();

    if (m_filteringString.isEmpty())
        return true;

    if ((min && *min < 0) || (max && *max < 1))
        return false;

    if (min && max && *min >= *max)
        return false;

    if (isLengthOutOfRange())
        return false;

    QRegularExpression::PatternOption patternOption = caseSensitive
                                                          ? QRegularExpression::NoPatternOption
                                                          : QRegularExpression::CaseInsensitiveOption;

    if (regex) {
        QRegularExpression re(m_filteringString, patternOption);

        return re.match(value).hasMatch();
    } else {
        Qt::CaseSensitivity cs = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
        return value.contains(m_filteringString, cs);
    }
}

bool StringFilterProxyModel::getCaseSensitive() const
{
    return caseSensitive;
}

void StringFilterProxyModel::setCaseSensitive(bool newCaseSensitive)
{
    caseSensitive = newCaseSensitive;
    invalidateFilter();
}

bool StringFilterProxyModel::getRegex() const
{
    return regex;
}

void StringFilterProxyModel::setRegex(bool newRegex)
{
    regex = newRegex;
    invalidateFilter();
}

std::optional<int> StringFilterProxyModel::getMax() const
{
    return max;
}

void StringFilterProxyModel::setMax(std::optional<int> newMax)
{
    max = newMax;
    invalidateFilter();
}

std::optional<int> StringFilterProxyModel::getMin() const
{
    return min;
}

void StringFilterProxyModel::setMin(std::optional<int> newMin)
{
    min = newMin;
    invalidateFilter();
}

QString StringFilterProxyModel::filteringString() const
{
    return m_filteringString;
}

void StringFilterProxyModel::setFilteringString(const QString &newFilteringString)
{
    m_filteringString = newFilteringString;
    invalidateFilter();
}

bool StringFilterProxyModel::isLengthOutOfRange() const
{
    int length = m_filteringString.length();
    return length < min || length > max;
}

void StringFilterProxyModel::resetModel()
{
    this->setFilteringString("");
    this->setMin(std::nullopt);
    this->setMax(INT_MAX);
    this->setRegex(false);
    this->setCaseSensitive(false);
    this->invalidateFilter();
}
