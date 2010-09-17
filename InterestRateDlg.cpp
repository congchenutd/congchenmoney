#include "InterestRateDlg.h"
#include <QSqlTableModel>
#include <QHeaderView>

InterestRateDlg::InterestRateDlg(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);

	model = new QSqlTableModel(this);
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->setTable("InterestRate");
	model->select();

	ui.tableView->setModel(model);
	ui.tableView->horizontalHeader()->setStretchLastSection(true);
}

void InterestRateDlg::accept()
{
	model->submitAll();
	QDialog::accept();
}