#include "BankDlg.h"
#include <QInputDialog>
#include <QSqlTableModel>
#include <QHeaderView>
#include <QMessageBox>

BankDlg::BankDlg(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);
	currentRow = -1;

	model = new QSqlTableModel(this);
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->setTable("Banks");
	model->select();

	ui.tableView->setModel(model);
	ui.tableView->horizontalHeader()->setStretchLastSection(true);

	connect(ui.buttonAdd,  SIGNAL(clicked()), this, SLOT(slotAdd()));
	connect(ui.buttonEdit, SIGNAL(clicked()), this, SLOT(slotEdit()));
	connect(ui.buttonDel,  SIGNAL(clicked()), this, SLOT(slotDel()));
	connect(ui.tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
			this,                           SLOT(slotUpdateCurrentRow(QModelIndex)));
	connect(ui.tableView,                   SIGNAL(doubleClicked(const QModelIndex&)), 
			this,                           SLOT(slotEdit()));
}

void BankDlg::slotAdd()
{
	bool ok;
	QString bankName = QInputDialog::getText(this, tr("添加银行"),	tr("银行名称："),
											 QLineEdit::Normal,	QString(), &ok);
	if(ok && !bankName.isEmpty())
	{
		int lastRow = model->rowCount();
		model->insertRow(lastRow);
		model->setData(model->index(lastRow, 0), bankName);
	}
}

void BankDlg::slotEdit()
{
	bool ok;
	QString bankName = QInputDialog::getText(this, tr("编辑银行"),	tr("银行名称："),
			QLineEdit::Normal,	model->data(model->index(currentRow, 0)).toString(), &ok);
	if(ok && !bankName.isEmpty())
		model->setData(model->index(currentRow, 0), bankName);
}

void BankDlg::slotDel()
{
	if(QMessageBox::warning(this, tr("确认"), tr("真的要删除该记录么？"), 
		QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
		model->removeRow(currentRow);
}

void BankDlg::slotUpdateCurrentRow(const QModelIndex& idx)
{
	if(idx.isValid())
		currentRow = idx.row();
	ui.buttonEdit->setEnabled(idx.isValid());
	ui.buttonDel ->setEnabled(idx.isValid());
}

void BankDlg::accept()
{
	model->submitAll();
	QDialog::accept();
}