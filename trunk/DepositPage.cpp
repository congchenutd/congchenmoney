#include "DepositPage.h"
#include "Connection.h"
#include <QSqlTableModel>
#include <QHeaderView>
#include <QMessageBox>

DepositPage::DepositPage(QWidget* parent) : QDialog(parent)
{
	initModel();
	initView();
	initConnection();
}

DepositPage::~DepositPage() {
	ui.tableView->saveHiddenSections();
}

void DepositPage::initModel()
{
	model = new QSqlTableModel(this);
	model->setTable("Deposits");
	model->select();
}

void DepositPage::initView()
{
	setWindowFlags(Qt::Dialog | Qt::WindowMaximizeButtonHint);
	ui.setupUi(this);
	ui.tableView->setModel(model);
	ui.tableView->sortByColumn(EndDate, Qt::DescendingOrder);
	ui.tableView->horizontalHeader()->setStretchLastSection(true);
	currentRow = -1;
	updateSummary();
}

void DepositPage::initConnection()
{
	connect(ui.buttonAdd,  SIGNAL(clicked()), this, SLOT(slotAdd()));
	connect(ui.buttonEdit, SIGNAL(clicked()), this, SLOT(slotEdit()));
	connect(ui.buttonDel,  SIGNAL(clicked()), this, SLOT(slotDel()));
	connect(ui.tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
			this,                           SLOT(slotUpdateCurrentRow(QModelIndex)));
	connect(ui.tableView,                   SIGNAL(doubleClicked(const QModelIndex&)), 
			this,                           SLOT(slotEdit()));
}

void DepositPage::slotAdd()
{
	DepositDlg dlg(this);
	dlg.setWindowTitle(tr("添加记录"));
	if(dlg.exec() == QDialog::Accepted)
	{
		int lastRow = model->rowCount();
		model->insertRow(lastRow);
		model->setData(model->index(lastRow, ID), getNextID("Deposits"));
		setRecord(lastRow, dlg.getDepositInfo());	
		submit();
	}
}

void DepositPage::slotEdit()
{
	DepositDlg dlg(this);
	dlg.setWindowTitle(tr("编辑记录"));
	dlg.setDepositInfo(getRecord(currentRow));
	if(dlg.exec() == QDialog::Accepted)
	{
		setRecord(currentRow, dlg.getDepositInfo());
		submit();
	}
}

void DepositPage::slotDel()
{
	if(QMessageBox::warning(this, tr("确认"), tr("真的要删除该记录么？"), 
		QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
	{
		model->removeRow(currentRow);
		submit();
	}
}

void DepositPage::slotUpdateCurrentRow(const QModelIndex& idx)
{
	if(idx.isValid())
		currentRow = idx.row();
	ui.buttonEdit->setEnabled(idx.isValid());
	ui.buttonDel ->setEnabled(idx.isValid());
}

void DepositPage::setRecord(int row, const DepositInfo& record)
{
	model->setData(model->index(row, Bank),         record.bank);
	model->setData(model->index(row, Amount),       record.amount);
	model->setData(model->index(row, StartDate),    record.start);
	model->setData(model->index(row, Period),       record.period);
	model->setData(model->index(row, EndDate),      record.end);
	model->setData(model->index(row, InterestRate), record.rate);
	model->setData(model->index(row, Interests),    record.interests);
	model->setData(model->index(row, ReDeposit),    record.reDeposit);
}

DepositInfo DepositPage::getRecord(int row) const
{
	DepositInfo result;
	result.bank   = model->data(model->index(row, Bank))        .toString();
	result.amount = model->data(model->index(row, Amount))      .toDouble();
	result.start  = model->data(model->index(row, StartDate))   .toDate();
	result.period = model->data(model->index(row, Period))      .toString();
	result.rate   = model->data(model->index(row, InterestRate)).toDouble();
	return result;
}

void DepositPage::updateSummary()
{
	double amount = 0.0;
	double interests = 0.0;
	for(int row=0; row<model->rowCount(); ++row)
	{
		amount    += model->data(model->index(row, Amount))   .toDouble();
		interests += model->data(model->index(row, Interests)).toDouble();
	}
	ui.spinBoxCount    ->setValue(model->rowCount());
	ui.spinBoxAmount   ->setValue(amount);
	ui.spinBoxInterests->setValue(interests);
	ui.spinBoxTotal    ->setValue(amount + interests);
}

void DepositPage::submit()
{
	model->submitAll();
	updateSummary();
}