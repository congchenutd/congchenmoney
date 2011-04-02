#include <QHeaderView>
#include "ComparePage.h"
#include "HistoryPage.h"
#include "Connection.h"

ComparePage::ComparePage(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	initModel();
	initView();
	initConnections();
}

ComparePage::~ComparePage()
{
	delete model;
	delete barviewModel;
}

void ComparePage::initModel()
{
	model = new QStandardItemModel(0, 4);
	model->setHeaderData(0, Qt::Horizontal, tr("Month"));
	model->setHeaderData(1, Qt::Horizontal, tr("Income"));
	model->setHeaderData(2, Qt::Horizontal, tr("Outcome"));
	model->setHeaderData(3, Qt::Horizontal, tr("Balance"));

	barviewModel = new QStandardItemModel(3, 2);
	barviewModel->setData(barviewModel->index(0, 0), tr("Income"));
	barviewModel->setData(barviewModel->index(1, 0), tr("Outcome"));
	barviewModel->setData(barviewModel->index(2, 0), tr("Balance"));
}

void ComparePage::initView()
{
	ui.tableView->setModel(model);
	ui.tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	ui.barView->setModel(barviewModel);

	slotByMonth(true);
	ui.splitter->setSizes(QList<int>() << 350 << 400);
}

void ComparePage::initConnections()
{
	connect(ui.dateEditStart,   SIGNAL(dateChanged(const QDate&)),
			this,               SLOT(slotSetDuration()));
	connect(ui.dateEditEnd,     SIGNAL(dateChanged(const QDate&)),
			this,               SLOT(slotSetDuration()));
	connect(ui.buttonByMonth,   SIGNAL(clicked(bool)), this, SLOT(slotByMonth(bool)));
	connect(ui.buttonByYear,    SIGNAL(clicked(bool)), this, SLOT(slotByYear (bool)));
	connect(ui.buttonPrevMonth, SIGNAL(clicked()), this, SLOT(slotPrevMonth()));
	connect(ui.buttonNextMonth, SIGNAL(clicked()), this, SLOT(slotNextMonth()));
	connect(ui.buttonPrevYear,  SIGNAL(clicked()), this, SLOT(slotPrevYear()));
	connect(ui.buttonNextYear,  SIGNAL(clicked()), this, SLOT(slotNextYear()));
}

void ComparePage::showEvent(QShowEvent*) {
	updateAll();
}

void ComparePage::updateAll()
{
	clearTable();
	getMonths();
	updateData();
	updateSum();
}

void ComparePage::getMonths()
{	
	months.clear();
	QSqlQuery query;
	query.exec(tr("select date from Outcome              \
				  where date between \"%1\" and \"%2\"   \
				  order by date")
				  .arg(startDate.toString(Qt::ISODate))
				  .arg(endDate  .toString(Qt::ISODate)));
	while(query.next())
		months.insert(std::make_pair(query.value(0).toDate().year(), 
									 query.value(0).toDate().month()));

	query.exec(tr("select date from Income               \
				  where date between \"%1\" and \"%2\"   \
				  order by date")
				  .arg(startDate.toString(Qt::ISODate))
				  .arg(endDate  .toString(Qt::ISODate)));
	while(query.next())
		months.insert(std::make_pair(query.value(0).toDate().year(), 
									 query.value(0).toDate().month()));
}

void ComparePage::clearTable() {
	model->removeRows(0, model->rowCount());
}

void ComparePage::updateData()
{
	for(Months::iterator it = months.begin(); it != months.end(); ++it)
	{
		QDate firstDayInMonth(it->first, it->second, 1);
		QDate lastDayInMonth (it->first, it->second, firstDayInMonth.daysInMonth());
		int lastRow = model->rowCount();
		model->insertRow(lastRow);
		model->setData(model->index(lastRow, 0), firstDayInMonth.toString("yyyy-MM"));

		QSqlQuery query;
		query.exec(tr("select sum(amount) from Income where date between \"%1\" and \"%2\"")
													.arg(firstDayInMonth.toString(Qt::ISODate))
													.arg(lastDayInMonth. toString(Qt::ISODate)));
		double income = query.next() ? query.value(0).toDouble() : 0.0;

		query.exec(tr("select sum(amount) from Outcome where date between \"%1\" and \"%2\"")
													.arg(firstDayInMonth.toString(Qt::ISODate))
													.arg(lastDayInMonth. toString(Qt::ISODate)));
		double outcome = query.next() ? query.value(0).toDouble() : 0.0;

		model->setData(model->index(lastRow, 1), income);	
		model->setData(model->index(lastRow, 2), outcome);	
		model->setData(model->index(lastRow, 3), income - outcome);
	}
}

void ComparePage::updateSum()
{
	int lastRow = model->rowCount();
	model->insertRow(lastRow);
	model->setData(model->index(lastRow, 0), tr("Sum"));
	for(int j=1; j<model->columnCount(); ++j)
	{
		double sum = 0.0;
		for(int i=0; i<model->rowCount(); ++i)
			sum += model->data(model->index(i, j)).toDouble();
		model->setData(model->index(model->rowCount()-1, j), sum);
		barviewModel->setData(barviewModel->index(j-1, 1), sum);
	}
}

void ComparePage::setDurationAll()
{
	ui.dateEditStart->setDate(getFirstDay());
	ui.dateEditEnd  ->setDate(getLastDay ());
}

QDate ComparePage::getFirstDay() {
	return qMin(::getFirstDay("Outcome"), ::getFirstDay("Income"));
}

QDate ComparePage::getLastDay() {
	return qMax(::getLastDay("Outcome"), ::getLastDay("Income"));
}

// slots
void ComparePage::slotSetDuration()
{
	startDate = ui.dateEditStart->date();
	endDate   = ui.dateEditEnd  ->date();
	if(startDate < endDate)
		updateAll();
}

void ComparePage::slotByMonth(bool checked)
{
	if(checked)
	{
		QDate today = QDate::currentDate();
		ui.dateEditStart->setDate(QDate(today.year(), today.month(), 1));
		ui.dateEditEnd  ->setDate(QDate(today.year(), today.month(), today.daysInMonth()));
		slotSetDuration();
		ui.buttonByYear->setChecked(false);
	}
	else
		setDurationAll();

	ui.buttonPrevMonth->setEnabled(checked);
	ui.buttonNextMonth->setEnabled(checked);
}

void ComparePage::slotByYear(bool checked)
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

void ComparePage::slotPrevMonth()
{
	QDate firstDayInPrevMonth = ui.dateEditStart->date().addMonths(-1);
	QDate lastDayInPrevMonth  = QDate(firstDayInPrevMonth.year(), 
									  firstDayInPrevMonth.month(), 
									  firstDayInPrevMonth.daysInMonth());
	if(lastDayInPrevMonth > getFirstDay())
	{
		ui.dateEditStart->setDate(firstDayInPrevMonth);
		ui.dateEditEnd  ->setDate(lastDayInPrevMonth);
		ui.buttonNextMonth->setEnabled(true);
	}
	if(firstDayInPrevMonth < getFirstDay())
		ui.buttonPrevMonth->setEnabled(false);
}

void ComparePage::slotNextMonth()
{
	QDate firstDayInNextMonth = ui.dateEditStart->date().addMonths(1);
	QDate lastDayInNextMonth  = QDate(firstDayInNextMonth.year(), 
									  firstDayInNextMonth.month(), 
									  firstDayInNextMonth.daysInMonth());
	if(firstDayInNextMonth < getLastDay())
	{
		ui.dateEditStart->setDate(firstDayInNextMonth);
		ui.dateEditEnd  ->setDate(lastDayInNextMonth);
		ui.buttonPrevMonth->setEnabled(true);
	}
	if(lastDayInNextMonth > getLastDay())
		ui.buttonNextMonth->setEnabled(false);
}

void ComparePage::slotPrevYear()
{
	int currentYear = ui.dateEditStart->date().year();
	QDate firstDayInPrevYear = QDate(currentYear-1, 1,  1);
	QDate lastDayInPrevYear  = QDate(currentYear-1, 12, 31);
	if(lastDayInPrevYear > getFirstDay())
	{
		ui.dateEditStart->setDate(firstDayInPrevYear);
		ui.dateEditEnd  ->setDate(lastDayInPrevYear);
		ui.buttonNextYear->setEnabled(true);
	}
	if(firstDayInPrevYear < getFirstDay())
		ui.buttonPrevYear->setEnabled(false);
}

void ComparePage::slotNextYear()
{
	int currentYear = ui.dateEditStart->date().year();
	QDate firstDayInNextYear = QDate(currentYear+1, 1,  1);
	QDate lastDayInNextYear  = QDate(currentYear+1, 12, 31);
	if(firstDayInNextYear < getLastDay())
	{
		ui.dateEditStart->setDate(firstDayInNextYear);
		ui.dateEditEnd  ->setDate(lastDayInNextYear);
		ui.buttonPrevYear->setEnabled(true);
	}
	if(lastDayInNextYear > getLastDay())
		ui.buttonNextYear->setEnabled(false);
}