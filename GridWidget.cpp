#include <QSqlQuery>
#include <QHeaderView>
#include <QMenu>
#include <QItemSelectionModel>
#include "GridWidget.h"
#include "DetailWindow.h"
#include "CategoryModel.h"
#include "Connection.h"

GridWidget::GridWidget(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	initModel();
	initView();
	initConnections();
}

GridWidget::~GridWidget() {
	delete model;
}

void GridWidget::setOutcome(bool out)
{
	outcome           = out;
	tableName         = getTableName(out);
	categoryTableName = getCategoryTableName(out);
}

void GridWidget::initModel() {
	model = new QStandardItemModel(0, 1);
}

void GridWidget::initView()
{
	ui.tableView->setModel(model);
	ui.tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
}

void GridWidget::initConnections()
{
	connect(ui.tableView, SIGNAL(doubleClicked(const QModelIndex&)),
			this,         SLOT(slotShowDetail()));
	connect(ui.actionDetail, SIGNAL(triggered()), this, SLOT(slotShowDetail()));
}

void GridWidget::refreshCategories()
{
	model->clear();
	model->insertColumn(0);
	model->setHeaderData(0, Qt::Horizontal, tr("月份"));

	QSqlQuery query;
	query.exec(tr("select name from %1").arg(categoryTableName));
	while(query.next())
	{
		QString cateName = query.value(0).toString();
		if(!CategoryModel::isSubtype(cateName))  // not subtype
		{
			int lastCol = model->columnCount();
			model->insertColumn(lastCol);
			model->setHeaderData(lastCol, Qt::Horizontal, cateName);
		}
	}

	int lastCol = model->columnCount();
	model->insertColumn(lastCol);
	model->setHeaderData(lastCol, Qt::Horizontal, tr("当月总和"));
}

void GridWidget::refreshData()
{
	getMonths();
	getData();
	getSumForMonth();
	getSumForCategory();
	removeZeroCategories();
}

void GridWidget::removeZeroCategories()
{
	initView();
	for(int i = 1; i < model->columnCount()-1; ++i)
		if(model->data(model->index(model->rowCount()-1, i)).toDouble() == 0.0)
			ui.tableView->hideColumn(i);    // hide 0-data column
}

void GridWidget::refresh()
{
	refreshCategories();
	refreshData();
}

void GridWidget::setDuration(const QDate& start, const QDate& end)
{
	startDate = start;
	endDate   = end;
	refresh();
}

int GridWidget::findColumn(int cateID) {
	return findColumn(CategoryModel::getCategoryName(categoryTableName, cateID));
}

int GridWidget::findColumn(const QString& cateName)
{
	for(int i=1; i<model->columnCount()-1; ++i)
		if(model->headerData(i, Qt::Horizontal).toString() == cateName)
			return i;
	return -1;
}

void GridWidget::getMonths()
{	
	QSqlQuery query;
	query.exec(tr("select date from %1                       \
				   where date between \"%2\" and \"%3\" %4   \
				   order by date").arg(tableName)
								  .arg(startDate.toString(Qt::ISODate))
								  .arg(endDate  .toString(Qt::ISODate))
								  .arg(getUserClause()));
	months.clear();
	while(query.next())
		months.insert(std::make_pair(query.value(0).toDate().year(), 
									 query.value(0).toDate().month()));
}

void GridWidget::getData()
{
	model->removeRows(0, model->rowCount());
	for(Months::iterator it = months.begin(); it != months.end(); ++it)  // for all months
	{
		// get data within this month(+duration) for each category
		QDate firstDayInMonth(it->first, it->second, 1);
		QDate lastDayInMonth (it->first, it->second, firstDayInMonth.daysInMonth());

		QString monthClause = tr("select * from %1 \
								  where date between \'%2\' and \'%3\' %4")
									.arg(tableName)
									.arg(firstDayInMonth.toString(Qt::ISODate))
									.arg(lastDayInMonth. toString(Qt::ISODate))
									.arg(getUserClause());

		QSqlQuery query;
		query.exec(tr("select category, sum(amount)           \
					   from(%1)                               \
					   where date between \"%2\" and \"%3\"   \
					   group by category").arg(monthClause)
										  .arg(startDate.toString(Qt::ISODate))
										  .arg(endDate.  toString(Qt::ISODate)));

		// add to model
		int lastRow = model->rowCount();
		model->insertRow(lastRow);
		model->setData(model->index(lastRow, 0), firstDayInMonth.toString("yyyy-MM"));
		while(query.next())   // each category
		{
			// merge data from different subtypes into 1
			int parentID = CategoryModel::getParentID(categoryTableName, query.value(0).toInt());
			int col      = findColumn(parentID);
			double oldAmount = model->data(model->index(lastRow, col)).toDouble();
			double newAmount = oldAmount + query.value(1).toDouble();
			model->setData(model->index(lastRow, col), newAmount);
		}
	}
}

void GridWidget::getSumForMonth()
{
	for(int i=0; i<model->rowCount(); ++i)
	{
		double sum = 0.0;
		for(int j=1; j<model->columnCount()-1; ++j)
			sum += model->data(model->index(i, j)).toDouble();
		model->setData(model->index(i, model->columnCount()-1), sum);
	}
}

void GridWidget::getSumForCategory()
{
	int lastRow = model->rowCount();
	model->insertRow(lastRow);
	model->setData(model->index(lastRow, 0), tr("分类总和"));
	for(int j=1; j<model->columnCount(); ++j)
	{
		double sum = 0.0;
		for(int i=0; i<model->rowCount(); ++i)
			sum += model->data(model->index(i, j)).toDouble();
		model->setData(model->index(model->rowCount()-1, j), sum);
	}
}

void GridWidget::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu contextMenu(this);
	contextMenu.addAction(ui.actionDetail);
	contextMenu.exec(event->globalPos());
}

QString GridWidget::getMonthFilter(const QString& monthRule)
{
	QString firstMonth = "\'" + monthRule + "-01\'";
	QString lastMonth  = "\'" + monthRule + "-31\'";
	return QString("date between %1 and %2").arg(firstMonth).arg(lastMonth);
}

QString GridWidget::getCategoryFilter(const QString& categoryRule) {
	return tr("NAME LIKE \'%1%%\'").arg(categoryRule);
}

QString GridWidget::getOneFilter(const QString& monthRule, const QString& categoryRule)
{
	QString result;
	if(!monthRule.isEmpty() && !categoryRule.isEmpty())
		result = getMonthFilter(monthRule) + " and " + getCategoryFilter(categoryRule);
	else if(!monthRule.isEmpty() && categoryRule.isEmpty())
		result = getMonthFilter(monthRule);
	else if(monthRule.isEmpty() && !categoryRule.isEmpty())
		result = getCategoryFilter(categoryRule);
	
	return "(" + result + ")";
}

GridWidget::RuleType GridWidget::whichRule()
{
	int center = 0, left = 0, right = 0, bottom = 0;
	QModelIndexList indexes = ui.tableView->selectionModel()->selection().indexes();
	foreach(QModelIndex index, indexes)
	{
		int row = index.row();
		int col = index.column();
		if(0 < col && col < model->columnCount() - 1 && row < model->rowCount() - 1)
			center ++;
		if(row == model->rowCount()-1 && 0 < col && col < model->columnCount()-1)
			bottom ++;
		if(row < model->rowCount()-1 && col == 0)
			left ++;
		if(row < model->rowCount()-1 && col == model->columnCount()-1)
			right ++;
	}
	if(center > 0)		           return MonthCategory;
	else if(left > 0 || right > 0) return MonthOnly;
	else if(bottom > 0)            return CategoryOnly;
	else                           return NoRule;
}

GridWidget::Rules GridWidget::getRules()
{
	using std::make_pair;
	Rules rules;
	QModelIndexList indexes = ui.tableView->selectionModel()->selection().indexes();
	switch(whichRule())
	{
	case MonthOnly:
		foreach(QModelIndex index, indexes)
			rules.insert(make_pair(
				model->data(model->index(index.row(), 0)).toString(), ""));
		break;
	case CategoryOnly:
		foreach(QModelIndex index, indexes)  // for each category all months in duration
			for(Months::iterator it = months.begin(); it!= months.end(); ++it)
			{
				rules.insert(make_pair(
					QDate(it->first, it->second, 1).toString("yyyy-MM"), 
					model->headerData(index.column(), Qt::Horizontal).toString()));
			}
		break;
	case MonthCategory:
		foreach(QModelIndex index, indexes)
		{
			int row = index.row();
			int col = index.column();
			if(0 < col && col < model->columnCount()-1 &&
			   row < model->rowCount()-1)
			{
				rules.insert(make_pair(
					model->data(model->index(row, 0)).toString(), 
					model->headerData(col, Qt::Horizontal).toString()));
			}
		}
		break;
	default:
		break;
	}
	return rules;
}

QString GridWidget::getMonthCategoryFilters(const Rules& rules)
{
	if(!rules.empty())
	{
		QStringList filters;
		for(Rules::const_iterator it = rules.begin(); it != rules.end(); ++it)
			filters << getOneFilter(it->first, it->second);
		return "where (" + filters.join(" OR ") + ")" + getUserClause();
	}
	else
		return userName.isEmpty() ? QString() 
								  : tr("where userName = \'%1\'").arg(userName);
}

void GridWidget::slotShowDetail()
{
	DetailWindow detailWindow(outcome, this, getMonthCategoryFilters(getRules()), 
							  startDate, endDate);
	detailWindow.exec();
}

void GridWidget::setUser(const QString& user)
{
	userName = user;
	refresh();
}

QString GridWidget::getUserClause() const {
	return userName.isEmpty() ? QString() : tr(" and userName = \'%1\'").arg(userName);
}