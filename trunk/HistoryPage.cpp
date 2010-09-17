#include <QHeaderView>
#include <QList>
#include <QContextMenuEvent>
#include <QMenu>
#include <QItemSelectionModel>
#include <QSqlTableModel>
#include "HistoryPage.h"
#include "Connection.h"

HistoryPage::HistoryPage(bool outcome, QWidget *parent)
    : QWidget(parent)
{
	tableName         = getTableName(outcome);
	categoryTableName = getCategoryTableName(outcome);
	ui.setupUi(this);
	initModels();
	initViews();
	initConnections();
	ui.gridWidget->setOutcome(outcome);
}

void HistoryPage::initModels()
{
	ui.pieWidget     ->setSourceModel(ui.gridWidget->getModel());
	ui.progressWidget->setSourceModel(ui.gridWidget->getModel());
	ui.pieWidget     ->setTable(categoryTableName);

	modelUser = new QSqlTableModel(this);
	modelUser->setTable("Users");
	modelUser->select();
}

void HistoryPage::resizeSplitters()
{
	ui.splitterHorizontal->setSizes(QList<int>() << 250 << 400);
	ui.splitterVertical  ->setSizes(QList<int>() << 500 << 250);
}

void HistoryPage::initViews()
{
	resizeSplitters();
	ui.comboBoxCategory->setTable(categoryTableName);
	slotEnableCategory(false);
	ui.comboBoxUser->setModel(modelUser);
	slotEnableUser(false);
	slotByYear(true);
}

void HistoryPage::initConnections()
{
	connect(ui.checkBoxByCategory, SIGNAL(clicked(bool)),
			this,                  SLOT(slotEnableCategory(bool)));
	connect(ui.checkBoxByUser,     SIGNAL(clicked(bool)),
			this,                  SLOT(slotEnableUser(bool)));
	connect(ui.comboBoxCategory,   SIGNAL(currentIndexChanged(const QString&)),
			this,                  SLOT(slotSetCategory(const QString&)));
	connect(ui.comboBoxUser,       SIGNAL(currentIndexChanged(const QString&)),
			this,                  SLOT(slotSetUser(const QString&)));
	connect(ui.dateEditStart,      SIGNAL(dateChanged(const QDate&)),
			this,                  SLOT(slotSetDuration()));
	connect(ui.dateEditEnd,        SIGNAL(dateChanged(const QDate&)),
			this,                  SLOT(slotSetDuration()));
	connect(ui.buttonByMonth,   SIGNAL(clicked(bool)), this, SLOT(slotByMonth(bool)));
	connect(ui.buttonByYear,    SIGNAL(clicked(bool)), this, SLOT(slotByYear (bool)));
	connect(ui.buttonPrevMonth, SIGNAL(clicked()), this, SLOT(slotPrevMonth()));
	connect(ui.buttonNextMonth, SIGNAL(clicked()), this, SLOT(slotNextMonth()));
	connect(ui.buttonPrevYear,  SIGNAL(clicked()), this, SLOT(slotPrevYear()));
	connect(ui.buttonNextYear,  SIGNAL(clicked()), this, SLOT(slotNextYear()));
}

void HistoryPage::showEvent(QShowEvent*)
{
	ui.comboBoxCategory->refresh();
	ui.gridWidget    ->refresh();
	ui.progressWidget->refresh();
	ui.pieWidget     ->refresh();
	slotEnableCategory(ui.checkBoxByCategory->isChecked());
}

void HistoryPage::setDurationAll()
{
	ui.dateEditStart->setDate(getFirstDay(tableName));
	ui.dateEditEnd  ->setDate(getLastDay (tableName));
}

// slots
void HistoryPage::slotEnableCategory(bool enable)
{
	ui.comboBoxCategory->setEnabled(enable);
	slotSetCategory(enable ? ui.comboBoxCategory->currentText() : QString());
}

void HistoryPage::slotSetCategory(const QString& cate) 
{
	ui.progressWidget->setDataColumn(
		cate.isEmpty() ? ui.gridWidget->getModel()->columnCount() - 1  // total amount
					   : ui.gridWidget->findColumn(cate));
}

void HistoryPage::slotEnableUser(bool enable) 
{
	ui.comboBoxUser->setEnabled(enable);
	slotSetUser(enable ? ui.comboBoxUser->currentText() : QString());
}

void HistoryPage::slotSetUser(const QString& user) 
{
	ui.gridWidget->setUser(user);
	ui.progressWidget->refresh();
	ui.pieWidget->refresh();
}

void HistoryPage::slotSetDuration()
{
	startDate = ui.dateEditStart->date();
	endDate   = ui.dateEditEnd  ->date();
	if(startDate < endDate)
	{
		ui.gridWidget->setDuration(startDate, endDate);
		ui.progressWidget->refresh();
		ui.pieWidget->refresh();
	}
}

void HistoryPage::slotByMonth(bool checked)
{
	if(checked)
	{
		QDate today = QDate::currentDate();
		ui.dateEditStart->setDate(QDate(today.year(), today.month(), 1));
		ui.dateEditEnd  ->setDate(QDate(today.year(), today.month(), 
										today.daysInMonth()));
		slotSetDuration();
		ui.buttonByYear->setChecked(false);
	}
	else
		setDurationAll();

	ui.buttonPrevMonth->setEnabled(checked);
	ui.buttonNextMonth->setEnabled(checked);
}

void HistoryPage::slotByYear(bool checked)
{
	if(checked)
	{
		ui.dateEditStart->setDate(QDate(QDate::currentDate().year(), 1, 1));
		ui.dateEditEnd  ->setDate(QDate(QDate::currentDate().year(), 12, 31));
		slotSetDuration();
		ui.buttonByMonth->setChecked(false);
	}
	else
		setDurationAll();

	ui.buttonPrevYear->setEnabled(checked);
	ui.buttonNextYear->setEnabled(checked);
}

void HistoryPage::slotPrevMonth()
{
	QDate firstDayInPrevMonth = ui.dateEditStart->date().addMonths(-1);
	QDate lastDayInPrevMonth  = QDate(firstDayInPrevMonth.year(), 
									  firstDayInPrevMonth.month(), 
									  firstDayInPrevMonth.daysInMonth());
	if(lastDayInPrevMonth > getFirstDay(tableName))
	{
		ui.dateEditStart->setDate(firstDayInPrevMonth);
		ui.dateEditEnd  ->setDate(lastDayInPrevMonth);
		ui.buttonNextMonth->setEnabled(true);
	}
	if(firstDayInPrevMonth < getFirstDay(tableName))
		ui.buttonPrevMonth->setEnabled(false);
}

void HistoryPage::slotNextMonth()
{
	QDate firstDayInNextMonth = ui.dateEditStart->date().addMonths(1);
	QDate lastDayInNextMonth  = QDate(firstDayInNextMonth.year(), 
									  firstDayInNextMonth.month(), 
									  firstDayInNextMonth.daysInMonth());
	if(firstDayInNextMonth < getLastDay(tableName))
	{
		ui.dateEditStart->setDate(firstDayInNextMonth);
		ui.dateEditEnd  ->setDate(lastDayInNextMonth);
		ui.buttonPrevMonth->setEnabled(true);
	}
	if(lastDayInNextMonth > getLastDay(tableName))
		ui.buttonNextMonth->setEnabled(false);
}

void HistoryPage::slotPrevYear()
{
	int currentYear = ui.dateEditStart->date().year();
	QDate firstDayInPrevYear = QDate(currentYear-1, 1,  1);
	QDate lastDayInPrevYear  = QDate(currentYear-1, 12, 31);
	if(lastDayInPrevYear > getFirstDay(tableName))
	{
		ui.dateEditStart->setDate(firstDayInPrevYear);
		ui.dateEditEnd  ->setDate(lastDayInPrevYear);
		ui.buttonNextYear->setEnabled(true);
	}
	if(firstDayInPrevYear < getFirstDay(tableName))
		ui.buttonPrevYear->setEnabled(false);
}

void HistoryPage::slotNextYear()
{
	int currentYear = ui.dateEditStart->date().year();
	QDate firstDayInNextYear = QDate(currentYear+1, 1,  1);
	QDate lastDayInNextYear  = QDate(currentYear+1, 12, 31);
	if(firstDayInNextYear < getLastDay(tableName))
	{
		ui.dateEditStart->setDate(firstDayInNextYear);
		ui.dateEditEnd  ->setDate(lastDayInNextYear);
		ui.buttonPrevYear->setEnabled(true);
	}
	if(lastDayInNextYear > getLastDay(tableName))
		ui.buttonNextYear->setEnabled(false);
}