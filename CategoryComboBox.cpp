#include <QSqlQuery>
#include "CategoryComboBox.h"
#include "CategoryModel.h"

void CategoryComboBox::setTable(const QString& table)
{
	if(table.isEmpty())
		return;
	tableName = table;

	// load text and sort
	QSqlQuery query;
	query.exec(tr("SELECT name FROM %1 WHERE name != \'未分类\' ORDER By name").arg(table));
	while(query.next())
	{
		QString cateName = query.value(0).toString();
		if(maintypeOnly)
		{
			QString maintypeName = CategoryModel::getMaintypeName(cateName);
			if(findText(maintypeName) == -1)  // subtypes have the same maintype
				addItem(maintypeName);
		}
		else
			addItem(cateName);
	}
	sort();
	insertItem(0, tr("未分类"));

	// set color
	for(int i=0; i<count(); ++i)
	{
		QColor color = getCategoryColor(tableName, itemText(i));
		QPixmap pixmap(32, 32);
		pixmap.fill(color);
		setItemIcon(i, QIcon(pixmap));
	}
	setCurrentIndex(0);
}

void CategoryComboBox::refresh()
{
	clear();
	setTable(tableName);
}

int CategoryComboBox::getCategory() const
{
	QSqlQuery query;
	query.exec(tr("SELECT id FROM %1 WHERE name = \"%2\"").arg(tableName)
														  .arg(currentText()));
	return query.next() ? query.value(0).toInt() : 1;
}

void CategoryComboBox::setCategory(const QString &cate) {
	setCurrentIndex(findText(cate));
}

void CategoryComboBox::setCategory(int cateid)
{
	QSqlQuery query;
	query.exec(tr("SELECT name FROM %1 WHERE id = %2").arg(tableName)
													  .arg(cateid));
	if(query.next())
		setCategory(query.value(0).toString());
}

QString CategoryComboBox::getCategoryName(const QString& tableName, int id)
{
	QSqlQuery query;
	query.exec(tr("SELECT name FROM %1 WHERE id = %2").arg(tableName)
													  .arg(id));
	return query.next() ? query.value(0).toString() : tr("未分类");
}

QColor CategoryComboBox::getCategoryColor(const QString& tableName, const QString& cateName)
{
	QSqlQuery query;
	query.exec(tr("SELECT color FROM %1 WHERE name = \'%2\'").arg(tableName)
															 .arg(cateName));
	return query.next() ? query.value(0).toInt() : 0;
}