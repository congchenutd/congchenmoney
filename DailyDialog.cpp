#include <QDoubleValidator>
#include <QDesktopWidget>
#include <QSqlQuery>
#include <QProcess>
#include <QSqlTableModel>
#include <QMessageBox>
#include "DailyDialog.h"
#include "DailyPage.h"
#include "CategoryListDialog.h"
#include "Connection.h"
#include "MyThread.h"
#include "SettingDlg.h"

DailyDialog::DailyDialog(bool outcome, QWidget *parent) : QDialog(parent)
{
	categoryTableName = getCategoryTableName(outcome);
	initView();
	initConnections();
}

void DailyDialog::initView()
{
	ui.setupUi(this);
	setDate(QDate::currentDate());
	ui.comboBoxCategory->setMaintypeOnly(false);
	ui.comboBoxCategory->setTable(categoryTableName);
	ui.doubleSpinBox->setFocus();
	ui.doubleSpinBox->selectAll();
	loadTaxRate();

	model = new QSqlTableModel(this);
	model->setTable("Users");
	model->select();
	ui.comboBoxUser->setModel(model);
}

void DailyDialog::initConnections()
{
	connect(ui.buttonNew,        SIGNAL(clicked()), this, SLOT(slotAddRecord()));
	connect(ui.buttonOK,         SIGNAL(clicked()), this, SLOT(slotOK()));
	connect(ui.buttonCalculator, SIGNAL(clicked()),	this, SLOT(slotLaunchCalculator()));
	connect(ui.buttonTax,        SIGNAL(clicked()), this, SLOT(slotTax()));
	connect(ui.comboBoxCategory, SIGNAL(currentIndexChanged(int)),    this, SLOT(slotEnableNewButton()));
	connect(ui.comboBoxUser,     SIGNAL(currentIndexChanged(int)),    this, SLOT(slotEnableNewButton()));
	connect(ui.textEdit,         SIGNAL(textChanged(const QString&)), this, SLOT(slotEnableNewButton()));
	connect(ui.doubleSpinBox,    SIGNAL(valueChanged(double)),        this, SLOT(slotEnableNewButton()));
	connect(ui.comboBoxCategory, SIGNAL(currentIndexChanged(int)),    this, SLOT(slotSetUser()));
	connect(ui.doubleSpinBoxTax, SIGNAL(valueChanged(double)),        this, SLOT(slotSetTaxRate()));
}

void DailyDialog::clearData()
{
	setAmount(0);
	setCategory(tr("未分类"));
	setDescription("");
	ui.buttonNew->setEnabled(false);
	ui.doubleSpinBox->setFocus();
	ui.doubleSpinBox->selectAll();
}

void DailyDialog::slotAddRecord()
{
	if(getAmount() > 0)
	{
		static_cast<DailyPage*>(parent())->addRecord(getRecord());
		clearData();
	}
}

void DailyDialog::slotEnableNewButton() {
	ui.buttonNew->setEnabled(true);
}

void DailyDialog::slotLaunchCalculator()
{
	MyThread* thread = new MyThread;
	thread->start();
}

double  DailyDialog::getAmount() const { return ui.doubleSpinBox->value();  }
void DailyDialog::setDate       (const QDate& date)   { ui.dateEdit->setDate(date);             }
void DailyDialog::setCategory   (const QString& text) { ui.comboBoxCategory->setCategory(text); }
void DailyDialog::setDescription(const QString& str)  { ui.textEdit->setPlainText(str);         }
void DailyDialog::setAmount     (double amount)       { ui.doubleSpinBox->setValue(amount);     }
void DailyDialog::setUserName(const QString& name)
{
	int index = ui.comboBoxUser->findText(name);
	ui.comboBoxUser->setCurrentIndex(index == -1 ? 0 : index);
}

DailyRecord DailyDialog::getRecord() const
{
	DailyRecord result;
	result.date        = ui.dateEdit->date();
	result.amount      = getAmount();
	result.cateID      = ui.comboBoxCategory->getCategory();
	result.userName    = ui.comboBoxUser->currentText();
	result.description = ui.textEdit->toPlainText();
	return result;
}

void DailyDialog::setRecord(const DailyRecord& record)
{
	setDate       (record.date); 
	setCategory   (record.cateName);
	setAmount     (record.amount);
	setUserName   (record.userName);
	setDescription(record.description);
}

void DailyDialog::accept()
{
	if(getAmount() <= 0)
	{
		QMessageBox::critical(this, tr("错误"), tr("金额不能为0"));
		return;
	}
	QDialog::accept();
}

void DailyDialog::slotSetUser()
{
	QSqlQuery query;
	query.exec(tr("SELECT defaultUser FROM %1 WHERE name = \'%2\'")
			.arg(categoryTableName).arg(ui.comboBoxCategory->currentText()));
	if(query.next())
		ui.comboBoxUser->setCurrentText(query.value(0).toString());
}

void DailyDialog::slotTax()
{
	double tax = ui.doubleSpinBox->value() * ui.doubleSpinBoxTax->value() / 100.0;
	ui.doubleSpinBox->setValue(ui.doubleSpinBox->value() + tax);
}

void DailyDialog::loadTaxRate()
{
	UserSetting* setting = MySetting<UserSetting>::getInstance("Global");
	ui.doubleSpinBoxTax->setValue(setting->value("TaxRate").toDouble());
}