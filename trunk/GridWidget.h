#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H

#include <QWidget>
#include <QDate>
#include <QStandardItemModel>
#include <QContextMenuEvent>
#include <map>
#include <set>
#include "ui_gridwidget.h"

class GridWidget : public QWidget
{
    Q_OBJECT

public:
	typedef std::map<int, QString>         Categories;
	typedef std::set<std::pair<int, int> > Months;
private:
	typedef enum{NoRule, CategoryOnly, MonthOnly, MonthCategory} RuleType;
	typedef std::set<std::pair<QString, QString> >               Rules;

public:
    GridWidget(QWidget *parent = 0);
	~GridWidget();
	void setOutcome(bool out);
	
	QStandardItemModel* getModel() { return model; }
	void setDuration(const QDate& start, const QDate& end);
	void refresh();
	int  findColumn(const QString& categoryName);
	void setUser(const QString& user);

protected:
	virtual void contextMenuEvent(QContextMenuEvent* event);

private:
	void initModel();
	void initView();
	void initConnections();
	void getMonths();
	void getData();
	void getSumForMonth();
	void getSumForCategory();
	int  findColumn(int categoryID);
	void refreshData();
	void refreshCategories();
	void removeZeroCategories();
	QString getUserClause() const;

	QString getMonthFilter   (const QString& monthRule);
	QString getCategoryFilter(const QString& categoryRule);
	QString getOneFilter     (const QString& monthRule, const QString& categoryRule);
	QString getMonthCategoryFilters(const Rules& rules);
	RuleType whichRule();
	Rules    getRules();

private slots:
	void slotShowDetail();

private:
    Ui::GridWidgetClass ui;

	QStandardItemModel* model;
	QDate      startDate;
	QDate      endDate;
	Months     months;
	bool       outcome;
	QString    tableName;
	QString    categoryTableName;
	QString    userName;
};

#endif // GRIDWIDGET_H
