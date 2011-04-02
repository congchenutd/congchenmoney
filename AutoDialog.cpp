#include <QSqlTableModel>
#include <QTableView>
#include <QDate>
#include "AutoDialog.h"
#include "Connection.h"

AutoDialog::AutoDialog(bool outcome, QWidget *parent) : QDialog(parent)
{
	categoryTableName = getCategoryTableName(outcome);
	initModel();
	initView();

	connect(ui.comboBoxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(slotIntervalChanged(int)));
}

void AutoDialog::initModel()
{
	userModel = new QSqlTableModel(this);
	userModel->setTable("Users");
	userModel->select();
}

void AutoDialog::initView()
{	
	ui.setupUi(this);

	ui.comboBoxUser->setModel(userModel);
	ui.comboBoxCategory->setMaintypeOnly(false);
	ui.comboBoxCategory->setTable(categoryTableName);

	ui.dateEdit->setDate(QDate::currentDate());
	setInterval(tr("Every Month"));
	ui.doubleSpinBox->setFocus();
	ui.spinBoxMonths->hide();
}

QString AutoDialog::getInterval() const 
{
	int index = ui.comboBoxInterval->currentIndex();
	if(index < 4)
		return ui.comboBoxInterval->currentText();
	return tr("%1 Month(s)").arg(ui.spinBoxMonths->value());
}

double AutoDialog::getAmount() const {
	return ui.doubleSpinBox->value();
}

void AutoDialog::setInterval(const QString& interval) 
{
	int index = ui.comboBoxInterval->findText(interval);
	if(index > 0)
		ui.comboBoxInterval->setCurrentIndex(index);
	else
	{
		ui.comboBoxInterval->setCurrentIndex(4);  // custom
		QString sub = interval.left(interval.length() - 2);
		ui.spinBoxMonths->setValue(sub.toInt());
	}
}

void AutoDialog::setUser(const QString& user){
	int index = ui.comboBoxUser->findText(user);
	ui.comboBoxUser->setCurrentIndex(index == -1 ? 0 : index);
}

void AutoDialog::slotIntervalChanged(int index)
{
	if(index == 4)  // custom
		ui.spinBoxMonths->show();
	else
		ui.spinBoxMonths->hide();
}

AutoRecord AutoDialog::getRecord() const
{
	AutoRecord result;
	result.date     = ui.dateEdit->date();
	result.interval = getInterval();
	result.amount   = getAmount();
	result.cateID   = ui.comboBoxCategory->getCategory();
	result.user     = ui.comboBoxUser->currentText();
	result.autoAdd  = ui.checkBox->isChecked();
	return result;
}

void AutoDialog::setRecord(const AutoRecord& record)
{
	ui.dateEdit->setDate(record.date);
	setInterval(record.interval);
	ui.doubleSpinBox->setValue(record.amount);
	ui.comboBoxCategory->setCategory(record.cateName);
	setUser(record.user);
	ui.checkBox->setChecked(record.autoAdd);
}

void AutoDialog::accept()
{
	if (getAmount() <= 0)
	{
		QMessageBox::critical(this, tr("Error"), tr("Amount can not be zero"));
		return;
	}
	QDialog::accept();
}
