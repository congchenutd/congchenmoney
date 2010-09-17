#include "DepositDlg.h"
#include "InterestRateDlg.h"
#include "BankDlg.h"
#include <QDate>
#include <QSqlTableModel>
#include <QSqlQuery>

DepositDlg::DepositDlg(QWidget* parent) : QDialog(parent)
{
	months << 3 << 6 << 12 << 24 << 36 << 60;

	ui.setupUi(this);

	model = new QSqlTableModel(this);
	model->setTable("Banks");
	model->select();
	ui.comboBoxBank->setModel(model);
		
	connect(ui.buttonRate,        SIGNAL(clicked()),                this, SLOT(slotInterestDlg()));
	connect(ui.buttonBank,        SIGNAL(clicked()),                this, SLOT(slotBank()));
	connect(ui.checkBoxReDeposit, SIGNAL(clicked()),                this, SLOT(slotCalcEndDate()));
	connect(ui.dateEditStart,     SIGNAL(dateChanged(QDate)),       this, SLOT(slotCalcEndDate()));
	connect(ui.comboBoxPeriod,    SIGNAL(currentIndexChanged(int)), this, SLOT(slotCalcEndDate()));
	connect(ui.comboBoxPeriod,    SIGNAL(currentIndexChanged(int)), this, SLOT(slotCalcRate()));
	connect(ui.dateEditStart,     SIGNAL(dateChanged(QDate)),       this, SLOT(slotCalcInterests()));
	connect(ui.dSpinBoxAmount,    SIGNAL(valueChanged(double)),     this, SLOT(slotCalcInterests()));
	connect(ui.dSpinBoxRate,      SIGNAL(valueChanged(double)),     this, SLOT(slotCalcInterests()));

	ui.dateEditStart->setDate(QDate::currentDate());
	ui.dSpinBoxAmount->selectAll();
	slotCalcRate();
}

DepositInfo DepositDlg::getDepositInfo() const
{
	DepositInfo result;
	result.bank      = ui.comboBoxBank->currentText();
	result.amount    = ui.dSpinBoxAmount->value();
	result.start     = ui.dateEditStart->date();
	result.period    = ui.comboBoxPeriod->currentText();
	result.end       = ui.dateEditEnd->date();
	result.rate      = ui.dSpinBoxRate->value();
	result.interests = ui.dSpinBoxInterestsReal->value();
	return result;
}

void DepositDlg::setDepositInfo(const DepositInfo& info)
{
	int bankRow = ui.comboBoxBank->findText(info.bank);
	ui.comboBoxBank->setCurrentIndex(bankRow == -1 ? 0 : bankRow);

	int periodRow = ui.comboBoxPeriod->findText(info.period);
	ui.comboBoxPeriod->setCurrentIndex(periodRow == -1 ? 0 : periodRow);

	ui.dSpinBoxAmount->setValue(info.amount);
	ui.dateEditStart->setDate(info.start);
	ui.dSpinBoxRate->setValue(info.rate);
}

void DepositDlg::slotCalcEndDate()
{
	QDate startDate = ui.dateEditStart->date();
	int   period    = getPeriod();
	QDate endDate   = startDate.addMonths(months[period]);
	monthsDeposited = months[period];

	if(ui.checkBoxReDeposit->isChecked())
	{
		QDate today = QDate::currentDate();
		while(endDate < today)
		{
			endDate = endDate.addMonths(months[period]);
			monthsDeposited += months[period];
		}
	}

	ui.dateEditEnd->setDate(endDate);
}

void DepositDlg::slotCalcRate()
{
	QSqlTableModel model;
	model.setTable("InterestRate");
	model.select();
	ui.dSpinBoxRate->setValue(model.data(model.index(getPeriod(), 1)).toDouble());
}

void DepositDlg::slotCalcInterests()
{
	double rate = ui.dSpinBoxRate->value() / 100.0;
	double amount = ui.dSpinBoxAmount->value();
	int interval = months[getPeriod()];
	double intervalRate = interval / 12.0 * rate;
	for(int month = 0; month < monthsDeposited; month += interval)
		amount += amount * intervalRate;
	double interests = amount - ui.dSpinBoxAmount->value();
	ui.dSpinBoxInterests->setValue(interests);

	QSqlQuery query;
	query.exec(tr("select 利率 from InterestRate where 存期 = \'利息税率\'"));
	double taxRate = query.next() ? query.value(0).toDouble() : 0;
	ui.dSpinBoxInterestsReal->setValue(interests * (1-taxRate/100));
}

void DepositDlg::slotInterestDlg()
{
	InterestRateDlg dlg(this);
	dlg.exec();
}

int DepositDlg::getPeriod() const {
	return ui.comboBoxPeriod->currentIndex();
}

void DepositDlg::slotBank()
{
	BankDlg dlg(this);
	if(dlg.exec() == QDialog::Accepted)
		model->select();
}