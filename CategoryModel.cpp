#include <QSqlQuery>
#include <QColor>
#include <QStandardItemModel>
#include <QTableView>
#include "CategoryModel.h"

CategoryModel::CategoryModel(QObject *parent) 
	: QSqlTableModel(parent) {}

void CategoryModel::setTable(const QString& tableName)
{
	QSqlTableModel::setTable(tableName);
	setFilter(tr("name != \'未分类\'"));
}

QVariant CategoryModel::data(const QModelIndex& idx, int role) const
{
	if(!idx.isValid())
		return QVariant();
	if(idx.column() == Name && role == Qt::DecorationRole)
		return getColor(idx.row());
	return QSqlTableModel::data(idx, role);
}

QString CategoryModel::getFullName(int row) const {
	return QSqlTableModel::data(index(row, Name), Qt::DisplayRole).toString();
}

QString CategoryModel::getSubtypeName(int row) const {
	return getSubtypeName(getFullName(row));
}

QColor CategoryModel::getColor(int row) const {
	return QColor(data(index(row, Color)).toInt());
}

int CategoryModel::getParent(int row) const {
	return data(index(row, Parent)).toInt();
}

// static 
int CategoryModel::getParentID(const QString& table, int id)
{
	QSqlQuery query;
	query.exec(tr("SELECT parent FROM %1 WHERE id = %2").arg(table).arg(id));
	if(query.next())
		if(query.value(0).toInt() == 1)  // 1 refers to 'nocategory', namely no parent
			return id;
		else
			return query.value(0).toInt();
	return 1;  // 'nocategory'
}

bool CategoryModel::isSubtype(const QString& categoryName) {
	return getSubtypeName(categoryName) != categoryName;
}

QColor CategoryModel::getColor(const QString& table, const QString& categoryName)
{
	QSqlQuery query;
	query.exec(tr("select color from %1 where name = \"%2\"").arg(table).arg(categoryName));
	return query.next() ? QColor(query.value(0).toUInt()) : Qt::white;
}

QString CategoryModel::getSubtypeName(const QString& fullName) {
	QString result = fullName.section(" - ", 1, 1);
	return result.isEmpty() ? fullName : result;   // if no subtype, return fullname
}

QString CategoryModel::getMaintypeName(const QString& fullName)
{
	QString result = fullName.section(" - ", 0, 0);
	return result.isEmpty() ? fullName : result;   // if no subtype, return fullname
}

QString CategoryModel::getCategoryName(const QString& table, int id)
{
	QSqlQuery query;
	query.exec(tr("SELECT name FROM %1 WHERE id = %2").arg(table).arg(id));
	return query.next() ? query.value(0).toString() : tr("未分类");
}

int CategoryModel::getCategoryID(const QString& table, const QString& categoryName)
{
	QSqlQuery query;
	query.exec(tr("SELECT id FROM %1 WHERE name=\'%2\'").arg(table).arg(categoryName));
	return query.next() ? query.value(0).toInt() : 1;
}

QStringList CategoryModel::getSubtypes(const QString& table, const QString& categoryName)
{
	QStringList result;
	if(categoryName == tr("未分类"))
		return result;

	QSqlQuery query;
	query.exec(tr("SELECT id FROM %1 WHERE name = \"%2\"").arg(table).arg(categoryName));  // get id
	if(query.next())
	{
		query.exec(tr("SELECT name from %1 WHERE parent=%2")  // get subtypes
						.arg(table)
						.arg(query.value(0).toInt()));
		while(query.next())
			result << query.value(0).toString();
	}
	return result;
}

bool CategoryModel::hasSubtypes(const QString& table, const QString& categoryName) {
	return !getSubtypes(table, categoryName).isEmpty();
}

QList<int> CategoryModel::categoryFamily(const QString& table, int parent)
{
	QList<int> result;
	QSqlQuery query;
	query.exec(tr("select id from %1 where parent = %2").arg(table).arg(parent));
	while(query.next())
		result << query.value(0).toInt();
	return result;
}