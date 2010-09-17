#include <QSqlRelationalTableModel>
#include <QHeaderView>
#include <QMessageBox>
#include "AutoListDialog.h"
#include "Connection.h"

AutoListDialog::AutoListDialog(bool out, QWidget *parent) : QDialog(parent), outcome(out)
{
	autoTableName     = getAutoTableName(out);
	categoryTableName = getCategoryTableName(out);
	initModel();
	initView();
	initConnections();
}

void AutoListDialog::initModel()
{
	model = new QSqlRelationalTableModel(this);
	model->setTable(autoTableName);
	model->setRelation(Category, QSqlRelation(categoryTableName, "id", "name"));
	model->select();
	model->setHeaderData(Date,     Qt::Horizontal, tr("��ʼ"));
	model->setHeaderData(Interval, Qt::Horizontal, tr("����"));
	model->setHeaderData(Amount,   Qt::Horizontal, tr("���"));
	model->setHeaderData(Category, Qt::Horizontal, tr("���"));
	model->setHeaderData(User,     Qt::Horizontal, tr("�û�"));
	model->setHeaderData(Auto,     Qt::Horizontal, tr("����ʾ"));
	model->setHeaderData(LastDate, Qt::Horizontal, tr("�ϴ���ʾʱ��"));
}

void AutoListDialog::initView()
{
	ui.setupUi(this);
	ui.tableView->setModel(model);
	ui.tableView->hideColumn(ID);
	ui.tableView->sortByColumn(Date, Qt::AscendingOrder);
	ui.tableView->setSectionSizeRatio(4, 0.2);
	ui.tableView->horizontalHeader()->setStretchLastSection(true);
}

void AutoListDialog::initConnections()
{
	connect(ui.buttonAdd,  SIGNAL(clicked()), this, SLOT(slotAdd()));
	connect(ui.buttonDel,  SIGNAL(clicked()), this, SLOT(slotDel()));
	connect(ui.tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
			this,                           SLOT(slotUpdateCurrentRow(QModelIndex)));
	connect(ui.tableView,                   SIGNAL(doubleClicked(const QModelIndex&)), 
			this,                           SLOT(slotEdit()));
}

void AutoListDialog::slotAdd()
{
	AutoDialog dlg(outcome, this);
	dlg.setWindowTitle(tr("��Ӽ�¼"));
	if(dlg.exec() == QDialog::Accepted)
	{
		int lastRow = model->rowCount();
		model->insertRow(lastRow);
		model->setData(model->index(lastRow, ID), getNextID(autoTableName));
		importRecord(lastRow, dlg.getRecord());
	}
}

void AutoListDialog::slotEdit()
{
	AutoDialog dlg(outcome, this);
	dlg.setWindowTitle(tr("�༭��¼"));
	dlg.setRecord(exportRecord(currentRow));
	if(dlg.exec() == QDialog::Accepted)
		importRecord(currentRow, dlg.getRecord());
}

void AutoListDialog::slotDel()
{
	if(QMessageBox::warning(this, tr("ȷ��"), tr("���Ҫɾ���ü�¼ô��"), 
							QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
	{
		model->removeRow(currentRow);
		model->submitAll();
	}
}

void AutoListDialog::slotUpdateCurrentRow(const QModelIndex& idx)
{
	if(idx.isValid())
		currentRow = idx.row();
	ui.buttonDel ->setEnabled(idx.isValid());
}

AutoRecord AutoListDialog::exportRecord(int row) const
{
	AutoRecord result;
	result.date     = model->data(model->index(row, Date))    .toDate();
	result.interval = model->data(model->index(row, Interval)).toString();
	result.amount   = model->data(model->index(row, Amount))  .toDouble();
	result.cateName = model->data(model->index(row, Category)).toString();
	result.user     = model->data(model->index(row, User))    .toString();
	result.autoAdd  = model->data(model->index(row, Auto))    .toBool();
	return result;
}

void AutoListDialog::importRecord(int row, const AutoRecord& record)
{
	model->setData(model->index(row, Date),     record.date);
	model->setData(model->index(row, Interval), record.interval);
	model->setData(model->index(row, Amount),   record.amount);
	model->setData(model->index(row, Category), record.cateID);
	model->setData(model->index(row, User),     record.user);
	model->setData(model->index(row, Auto),     record.autoAdd);
	model->setData(model->index(row, LastDate), record.date);
	model->submitAll();
}