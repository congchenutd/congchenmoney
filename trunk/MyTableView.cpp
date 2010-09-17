#include "MyTableView.h"
#include "MyHeaderView.h"
#include <QSqlQuery>
#include <QSqlTableModel>

MyTableView::MyTableView(QWidget* parent) : QTableView(parent) 
{
	setHorizontalHeader(new MyHeaderView(Qt::Horizontal, this));
}

void MyTableView::saveHiddenSections()
{
	QSqlQuery query;
	query.exec(tr("DELETE FROM HiddenSections WHERE tableName = \'%1\'").arg(tableName));
	for(int i=0; i<model()->columnCount(); ++i)
		if(isColumnHidden(i))
			query.exec(tr("INSERT INTO HiddenSections \
						   VALUES(\'%1\', %2)").arg(tableName).arg(i));
}

void MyTableView::setSectionSizeRatio(int section, double ratio) {
	sectionSizes.insert(section, ratio);
}

void MyTableView::resizeEvent(QResizeEvent* event)
{
	QTableView::resizeEvent(event);
	QMap<int, double>::const_iterator it = sectionSizes.constBegin();
	for(; it != sectionSizes.constEnd(); ++it) 
		setColumnWidth(it.key(), width() * it.value());
}

void MyTableView::setModel(QAbstractItemModel* model)
{
	QTableView::setModel(model);
	if(QSqlTableModel* tableModel = dynamic_cast<QSqlTableModel*>(model))
	{
		tableName = tableModel->tableName();
		QSqlQuery query;
		query.exec(tr("SELECT section FROM HiddenSections WHERE tableName = \'%1\'")
						.arg(tableName));
		while(query.next())
			hideColumn(query.value(0).toInt());
	}
}