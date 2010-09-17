#ifndef COMPAREPAGE_H
#define COMPAREPAGE_H

#include <QWidget>
#include <QStandardItemModel>
#include <set>
#include "ui_ComparePage.h"

class ComparePage : public QWidget
{
	Q_OBJECT

	typedef std::set<std::pair<int, int> > Months;

public:
	ComparePage(QWidget *parent = 0);
	~ComparePage();

protected:
	void showEvent(QShowEvent* event);

private slots:
	void slotSetDuration();
	void slotByMonth(bool checked);
	void slotByYear (bool checked);
	void slotPrevMonth();
	void slotNextMonth();
	void slotPrevYear();
	void slotNextYear();

private:
	void initModel();
	void initView();
	void initConnections();
	void setDurationAll();
	QDate getFirstDay();
	QDate getLastDay();
	void  getMonths();
	void  updateAll();
	void  clearTable();
	void  updateData();
	void  updateSum();

private:
	Ui::ComparePageClass ui;
	QDate  startDate;
	QDate  endDate;
	Months months;
	QStandardItemModel* model;
	QStandardItemModel* barviewModel;
};

#endif // COMPAREPAGE_H
