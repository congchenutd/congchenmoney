#include <QSqlQuery>
#include <QHeaderView>
#include <QMessageBox>
#include "Connection.h"
#include "CategoryModel.h"
#include "CategoryListDialog.h"
#include "CategoryEditDialog.h"

CategoryListDialog::CategoryListDialog(bool out, QWidget *parent)
	: QDialog(parent)
{
	tableName         = getTableName(out);
	categoryTableName = getCategoryTableName(out);
	autoTableName     = getAutoTableName(out);
	ui.setupUi(this);
	initModel();
	initView();
	initConnection();
}

void CategoryListDialog::initModel()
{
	model = new CategoryModel(this);
	model->setTable(categoryTableName);
	model->select();
	model->setHeaderData(Name, Qt::Horizontal, tr("类别名称"));
	model->setHeaderData(User, Qt::Horizontal, tr("缺省用户"));
}

void CategoryListDialog::initView()
{
	ui.tableView->setModel(model);
	ui.tableView->sortByColumn(Name, Qt::AscendingOrder);
	ui.tableView->resizeColumnsToContents();
	ui.tableView->horizontalHeader()->setStretchLastSection(true);
	ui.tableView->hideColumn(ID);
	ui.tableView->hideColumn(Color);
	ui.tableView->hideColumn(Parent);
}

void CategoryListDialog::initConnection()
{
	connect(ui.buttonAdd,  SIGNAL(clicked()), this, SLOT(slotAdd()));
	connect(ui.buttonEdit, SIGNAL(clicked()), this, SLOT(slotEdit()));
	connect(ui.buttonDel,  SIGNAL(clicked()), this, SLOT(slotDelete()));
	connect(ui.tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
			this,                          	SLOT(slotUpdateCurrentRow(QModelIndex)));
	connect(ui.tableView, SIGNAL(doubleClicked(const QModelIndex&)), 
			this,         SLOT(slotEdit()));
}

void CategoryListDialog::slotAdd()
{
	CategoryEditDialog dlg(categoryTableName);
	if(dlg.exec() == QDialog::Accepted && !dlg.getName().isEmpty())
	{
		int lastRow = model->rowCount();
		model->insertRow(lastRow);
		model->setData(model->index(lastRow, ID),     getNextID(categoryTableName));
		model->setData(model->index(lastRow, Name),   dlg.getName());
		model->setData(model->index(lastRow, Color),  dlg.getColor().rgb());
		model->setData(model->index(lastRow, Parent), dlg.getParent());
		model->setData(model->index(lastRow, User),   dlg.getUser());
		model->submitAll();
	}
}

void CategoryListDialog::slotEdit()
{
	CategoryEditDialog dlg(categoryTableName);
	const QString oldFullName = model->getFullName(currentRow);
	const QColor  oldColor    = model->getColor   (currentRow);
	const int     oldParent   = model->getParent  (currentRow);
	dlg.setName  (model->getSubtypeName(currentRow));
	dlg.setColor (oldColor);
	dlg.setParent(oldParent);
	dlg.setUser(model->data(model->index(currentRow, User)).toString());
	if(dlg.exec() == QDialog::Accepted && !dlg.getName().isEmpty())
	{
		const QString newFullName = dlg.getName();
		const QColor  newColor    = dlg.getColor();
		const int     newParent   = dlg.getParent();
		// if parent type changes, synchronize all subtypes
		if(CategoryModel::hasSubtypes(categoryTableName, oldFullName))
		{
			const QString newSubtypeName = CategoryModel::getSubtypeName(newFullName);
			if(newSubtypeName != oldFullName)
				changeParentTypeName(oldFullName, newSubtypeName);
			if(newColor != oldColor)
				changeParentTypeColor(newSubtypeName, newColor);
		}
		model->setData(model->index(currentRow, Name),   newFullName);
		model->setData(model->index(currentRow, Color),  newColor.rgb());
		model->setData(model->index(currentRow, Parent), newParent);
		model->setData(model->index(currentRow, User), dlg.getUser());
		model->submitAll();
	}
}

void CategoryListDialog::changeParentTypeName(const QString& parentName, const QString& newName)
{
	// subtypes
	QSqlQuery query;
	QStringList subtypes = CategoryModel::getSubtypes(categoryTableName, parentName);
	foreach(QString oldFullName, subtypes)
	{
		const QString newFullName = tr("%1 - %2").arg(newName)
												 .arg(CategoryModel::getSubtypeName(oldFullName));
		query.exec(tr("UPDATE %1 SET name = \'%2\' WHERE name = \'%3\'").arg(categoryTableName)
																		.arg(newFullName)
																		.arg(oldFullName));
	}
	// parent type
	query.exec(tr("UPDATE %1 SET name = \'%2\' WHERE name = \'%3\'").arg(categoryTableName)
																	.arg(newName)			
																	.arg(parentName));
	model->select();
}

void CategoryListDialog::changeParentTypeColor(const QString& parentName, const QColor& newColor)
{
	QSqlQuery query;
	QStringList tobeChanged = CategoryModel::getSubtypes(categoryTableName, parentName);
	tobeChanged << parentName;
	foreach(QString subtype, tobeChanged)
		query.exec(tr("UPDATE %1 SET color = \'%2\' WHERE name = \'%3\'").arg(categoryTableName)
																		 .arg(newColor.rgb())
																		 .arg(subtype));
	model->select();
}

void CategoryListDialog::deleteParentType(const QString& parentName)
{
	QSqlQuery query;
	QStringList tobeDeleted = CategoryModel::getSubtypes(categoryTableName, parentName);
	tobeDeleted << parentName;
	foreach(QString typeName, tobeDeleted)
	{
		int id = CategoryModel::getCategoryID(categoryTableName, typeName);
		query.exec(tr("UPDATE %1 SET Category=1 WHERE Category = %2").arg(tableName)    .arg(id));
		query.exec(tr("UPDATE %1 SET Category=1 WHERE Category = %2").arg(autoTableName).arg(id));
		query.exec(tr("DELETE FROM %1 WHERE id = %2").arg(categoryTableName).arg(id));
	}
	model->select();
}

void CategoryListDialog::slotDelete()
{
	if(QMessageBox::warning(this, tr("警告"), 
							tr("删除类别会删除其下所有子类，并将所含记录归为未分类"),
							QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
		deleteParentType(model->data(model->index(currentRow, 1)).toString());
}

void CategoryListDialog::slotUpdateCurrentRow(const QModelIndex& idx)
{
	if(idx.isValid())
		currentRow = idx.row();
	ui.buttonEdit  ->setEnabled(idx.isValid());
	ui.buttonDel->setEnabled(idx.isValid());
}