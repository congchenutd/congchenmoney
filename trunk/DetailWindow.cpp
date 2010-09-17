#include <QHeaderView>
#include <QDesktopWidget>
#include "DetailWindow.h"
#include "Connection.h"

DetailWindow::DetailWindow(bool outcome, QWidget* parent, const QString& filters, 
						   const QDate& start, const QDate& end)
    : QDialog(parent)
{
	tableName         = getTableName(outcome);
	categoryTableName = getCategoryTableName(outcome);
	order     = Qt::AscendingOrder;
	orderBy   = tr("date");
	startDate = start;
	endDate   = end;
	monthCategoryFilters = filters;

	initModel();
	initView();
	initConnections();
}

void DetailWindow::centerWindow()
{
	move((QApplication::desktop()->width()  - width())  / 2,
		 (QApplication::desktop()->height() - height()) / 2);
}

void DetailWindow::initView()
{
	ui.setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::WindowMaximizeButtonHint);
	centerWindow();
	ui.tableView->setModel(&model);
	ui.tableView->horizontalHeader()->setStretchLastSection(true);
	ui.tableView->setSectionSizeRatio(Date,     0.2);
	ui.tableView->setSectionSizeRatio(Category, 0.25);
}

void DetailWindow::initModel()
{
	model.setHeaderData(Date,        Qt::Horizontal, tr("日期"));
	model.setHeaderData(Amount,      Qt::Horizontal, tr("金额"));
	model.setHeaderData(Category,    Qt::Horizontal, tr("类别"));
	model.setHeaderData(User,        Qt::Horizontal, tr("用户"));
	model.setHeaderData(Description, Qt::Horizontal, tr("备注"));
}

void DetailWindow::initConnections()
{
	connect(ui.tableView->horizontalHeader(), SIGNAL(sectionClicked(int)),
			this,                             SLOT(slotSort(int)));
}

int DetailWindow::exec()
{	
	executeQuery();
	return QDialog::exec();
}

QString DetailWindow::getOrderFilter()
{
	// descending because slotSort() reversed order
	return order == Qt::DescendingOrder ? QString(" order by %1").arg(orderBy) 
										: QString(" order by %1 desc").arg(orderBy);
}

void DetailWindow::executeQuery()
{
	QString queryString = tr("\
		select * from (                             \
			select date, amount, name, userName, description  \
			from %1, %2                             \
			where category = %3.id and              \
			date between \'%4\' and \'%5\')         \
		").arg(tableName)
		  .arg(categoryTableName)
		  .arg(categoryTableName)
		  .arg(startDate.toString(Qt::ISODate))
		  .arg(endDate  .toString(Qt::ISODate));
	queryString.append(monthCategoryFilters + getOrderFilter());
	model.setQuery(queryString);
}

void DetailWindow::slotSort(int column)
{
	order = (order == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
	QString fields[] = {"date", "amount", "name", "userName", "description"};
	orderBy = fields[column];
	executeQuery();
}