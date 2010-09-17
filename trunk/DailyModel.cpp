#include <QColor>
#include <QSqlQuery>
#include "DailyModel.h"
#include "DailyPage.h"

DailyModel::DailyModel(QObject *parent)
	: QSqlRelationalTableModel(parent) {}

QVariant DailyModel::data(const QModelIndex& idx, int role) const
{
	if(role == Qt::DecorationRole && idx.isValid() && idx.column() == DailyPage::Category)
		return getColor(data(idx, Qt::DisplayRole).toString());
	return QSqlRelationalTableModel::data(idx, role);
}

QColor DailyModel::getColor(const QString& cate) const
{
	QSqlQuery query;
	query.exec(tr("SELECT color FROM %1 WHERE name = \'%2\'")
						.arg(tableName() + "Category")
						.arg(cate));
	return query.next() ? query.value(0).toInt() : Qt::white;
}