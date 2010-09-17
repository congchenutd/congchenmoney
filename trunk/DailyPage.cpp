#include <QSqlQuery>
#include <QHeaderView>
#include <QMessageBox>
#include <QVector>
#include <QTimer>
#include <QString>
#include <QDate>
#include "DailyPage.h"
#include "CategoryListDialog.h"
#include "AutoListDialog.h"
#include "Connection.h"
#include "DailyModel.h"
#include "AutoListDialog.h"
#include "CategoryModel.h"

DailyPage::DailyPage(bool out, QWidget *parent) : QWidget(parent), outcome(out)
{
	tableName         = getTableName(outcome);
	categoryTableName = getCategoryTableName(outcome);
	firstShow = true;
	model = new DailyModel(this);
	initModel();
	initView();
	initConnection();
}

DailyPage::~DailyPage() {
	ui.tableView->saveHiddenSections();
}

void DailyPage::initModel()
{
	model->setTable(tableName);
	model->setRelation(Category, QSqlRelation(categoryTableName, "id", "name"));
	model->select();
	model->setHeaderData(ID,          Qt::Horizontal, tr("编号"));
	model->setHeaderData(Date,        Qt::Horizontal, tr("日期"));
	model->setHeaderData(Amount,      Qt::Horizontal, tr("金额"));
	model->setHeaderData(Category,    Qt::Horizontal, tr("类别"));
	model->setHeaderData(User,        Qt::Horizontal, tr("用户"));
	model->setHeaderData(Description, Qt::Horizontal, tr("备注"));
}

void DailyPage::initView()
{
	ui.setupUi(this);
	ui.tableView->setModel(model);
	ui.tableView->sortByColumn(Date, Qt::DescendingOrder);
	ui.tableView->horizontalHeader()->setStretchLastSection(true);
	ui.tableView->setSectionSizeRatio(Category, 0.25);
	slotEnableSearch(false);
	ui.comboCategory->setMaintypeOnly(false);
}

void DailyPage::initConnection()
{
	connect(ui.buttonAdd,      SIGNAL(clicked()), this, SLOT(slotAdd()));
	connect(ui.buttonEdit,     SIGNAL(clicked()), this, SLOT(slotEdit()));
	connect(ui.buttonDel,      SIGNAL(clicked()), this, SLOT(slotDelete()));
	connect(ui.buttonCategory, SIGNAL(clicked()), this, SLOT(slotCategory()));
	connect(ui.buttonAuto,     SIGNAL(clicked()), this, SLOT(slotAuto()));
	connect(ui.buttonSearch,   SIGNAL(clicked(bool)), this, SLOT(slotEnableSearch(bool)));
	connect(ui.tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
			this,                           SLOT(slotUpdateCurrentRow(QModelIndex)));
	connect(ui.tableView,                   SIGNAL(doubleClicked(const QModelIndex&)), 
			this,                           SLOT(slotEdit()));
	connect(ui.dateEditStart,   SIGNAL(dateChanged(const QDate&)), this, SLOT(slotSearch()));
	connect(ui.dateEditEnd,     SIGNAL(dateChanged(const QDate&)), this, SLOT(slotSearch()));
	connect(ui.dblSpinBoxStart, SIGNAL(valueChanged(double)),      this, SLOT(slotSearch()));
	connect(ui.dblSpinBoxEnd,   SIGNAL(valueChanged(double)),      this, SLOT(slotSearch()));
	connect(ui.comboCategory,   SIGNAL(currentIndexChanged(int)),  this, SLOT(slotSearch()));
	connect(ui.comboUser,       SIGNAL(currentIndexChanged(int)),  this, SLOT(slotSearch()));
	connect(ui.lineEditNote,    SIGNAL(textEdited(QString)),       this, SLOT(slotSearch()));
}

void DailyPage::addRecord(const DailyRecord& record)
{
	int lastRow = model->rowCount();
	model->insertRow(lastRow);
	model->setData(model->index(lastRow, ID), getNextID(tableName));
	importRecord(lastRow, record);
}

QDate DailyPage::getDueDay(const QDate& last, QString interval) const
{
	if(interval == tr("每天"))
		return last.addDays(1);
	if(interval == tr("每周"))
		return last.addDays(7);
	if(interval == tr("每月"))
		return last.addMonths(1);
	if(interval == tr("每年"))
		return last.addYears(1);
	if(interval.endsWith(tr("个月")))
	{
		QString sub = interval.left(interval.length() - 2);
		return last.addMonths(sub.toInt());
	}

	qFatal("DailyPage::dueDay no such case");
	return last;
}

void DailyPage::slotCheckAuto()
{
	QVector<int>   ids;
	QVector<QDate> dueDays;
	QString autoTable = getAutoTableName(outcome);
	QDate   today     = QDate::currentDate();
	QSqlQuery query;
	query.exec(tr("SELECT * FROM %1").arg(autoTable));
	while(query.next())
	{
		QDate   lastDate = query.value(AutoListDialog::LastDate).toDate();
		QString interval = query.value(AutoListDialog::Interval).toString();
		QDate dueDay = getDueDay(lastDate, interval);
		if(today >= dueDay)
		{
			double  amount   = query.value(AutoListDialog::Amount).toDouble();
			int     category = query.value(AutoListDialog::Category).toInt();
			QString user     = query.value(AutoListDialog::User).toString();
			bool    aut      = query.value(AutoListDialog::Auto).toBool();
			DailyRecord record;
			record.date        = today;
			record.amount      = amount;
			record.cateID      = category;
			record.cateName    = CategoryComboBox::getCategoryName(categoryTableName, category);
			record.userName    = user;
			record.description = tr("自动添加");
			if(aut)
				addRecord(record);
			else
			{
				DailyDialog dlg(outcome, this);
				dlg.setWindowTitle(tr("自动添加记录"));
				dlg.setRecord(record);
				if(dlg.exec() == QDialog::Accepted)
					addRecord(dlg.getRecord());
			}

			ids     << query.value(AutoListDialog::ID).toInt();
			dueDays << dueDay;
		}
	}

	// set lastDate to dueday
	for(int i=0; i<ids.size(); ++i)
		query.exec(tr("UPDATE %1 SET lastDate = \'%2\' WHERE id = %3")
					.arg(autoTable)
					.arg(dueDays[i].toString(Qt::ISODate))
					.arg(ids[i]));
}

void DailyPage::showEvent(QShowEvent*)
{
	if(firstShow)
	{
		firstShow = false;
		// wait until page is displayed completely, because dlg is modal
		QTimer::singleShot(2000, this, SLOT(slotCheckAuto()));
	}
}

void DailyPage::slotAdd()
{
	DailyDialog dlg(outcome, this);
	dlg.setWindowTitle(tr("添加记录"));
	if(dlg.exec() == QDialog::Accepted)
		addRecord(dlg.getRecord());
}

void DailyPage::slotEdit()
{
	DailyDialog dlg(outcome, this);
	dlg.setWindowTitle(tr("编辑记录"));
	dlg.setRecord(exportRecord(currentRow));
	if(dlg.exec() == QDialog::Accepted)
		importRecord(currentRow, dlg.getRecord());
}

void DailyPage::slotDelete()
{
	if(QMessageBox::warning(this, tr("确认"), tr("真的要删除该记录么？"), 
							QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
	{
		model->removeRow(currentRow);
		model->submitAll();
	}
}

void DailyPage::slotCategory()
{
	CategoryListDialog dlg(outcome, this);
	dlg.exec();
	model->select();  // dlg may delete/edit category, so refresh
}

void DailyPage::slotUpdateCurrentRow(const QModelIndex& idx)
{
	if(idx.isValid())
		currentRow = idx.row();
	ui.buttonEdit->setEnabled(idx.isValid());
	ui.buttonDel ->setEnabled(idx.isValid());
}

void DailyPage::slotAuto()
{
	AutoListDialog dlg(outcome, this);
	dlg.exec();
}

void DailyPage::slotSearch()
{
	QString filter;
	QStringList filters;
	filters << tr("(Date between \'%1\' and \'%2\')")
								.arg(ui.dateEditStart->date().toString(Qt::ISODate))
								.arg(ui.dateEditEnd  ->date().toString(Qt::ISODate));
	filters << tr("(Amount between %1 and %2)").arg(ui.dblSpinBoxStart->value())
											   .arg(ui.dblSpinBoxEnd  ->value());

	if(ui.comboCategory->currentText() != tr("所有分类"))
		filters << tr("(Category = %1)").arg(ui.comboCategory->getCategory());

	QString userFilter = ui.comboUser->currentText();
	if(userFilter != tr("所有用户"))
		filters << tr("(UserName = \'%1\')").arg(userFilter);

	QString descriptionFilter = ui.lineEditNote->text();
	if(!descriptionFilter.isEmpty())
		filters << tr("(Description like \'%%1%\')").arg(descriptionFilter);

	filter = filters.join(" AND ");
	model->setFilter(filters.join(" AND "));
}

void DailyPage::slotEnableSearch(bool enable)
{
	if(enable)
	{
		ui.groupBoxSearch->show();
		ui.dateEditStart->setDate(getFirstDay(tableName));
		ui.dateEditEnd  ->setDate(getLastDay (tableName));
		ui.dblSpinBoxStart->setValue(getMinAmount(tableName));
		ui.dblSpinBoxEnd  ->setValue(getMaxAmount(tableName));
		ui.lineEditNote->clear();

		updateUser();
		updateCategory();
	}
	else
	{
		ui.groupBoxSearch->hide();
		model->setFilter(QString());
	}
}

DailyRecord DailyPage::exportRecord(int row) const
{
	DailyRecord result;
	result.date        = model->data(model->index(row, Date))       .toDate();
	result.amount      = model->data(model->index(row, Amount))     .toDouble();
	result.cateName    = model->data(model->index(row, Category))   .toString();
	result.userName    = model->data(model->index(row, User))       .toString();
	result.description = model->data(model->index(row, Description)).toString();
	return result;
}

void DailyPage::importRecord(int row, const DailyRecord& record)
{
	model->setData(model->index(row, Date),        record.date);
	model->setData(model->index(row, Amount),      record.amount);
	model->setData(model->index(row, Category),    record.cateID);
	model->setData(model->index(row, User),        record.userName);
	model->setData(model->index(row, Description), record.description);
	model->submitAll();
}

void DailyPage::updateUser()
{
	ui.comboUser->clear();
	ui.comboUser->addItem(tr("所有用户"));
	QSqlQuery query;
	query.exec(tr("select * from Users"));
	while(query.next())
		ui.comboUser->addItem(query.value(0).toString());
}

void DailyPage::updateCategory() 
{
	ui.comboCategory->clear();
	ui.comboCategory->setTable(categoryTableName);
	ui.comboCategory->insertItem(0, tr("所有分类"));
	ui.comboCategory->setCurrentIndex(0);
}