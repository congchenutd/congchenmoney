#ifndef HISTORYPAGE_H
#define HISTORYPAGE_H

#include <QWidget>
#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QSqlQuery>
#include <QDate>
#include "ui_historypage.h"

class QSqlTableModel;

class HistoryPage : public QWidget
{
    Q_OBJECT

public:
    HistoryPage(bool outcome, QWidget *parent = 0);

protected:
	void showEvent(QShowEvent* event);

private slots:
	void slotEnableCategory(bool enable);
	void slotEnableUser    (bool enable);
	void slotSetCategory(const QString& cate);
	void slotSetUser    (const QString& user);
	void slotSetDuration();
	void slotByMonth(bool checked);
	void slotByYear (bool checked);
	void slotPrevMonth();
	void slotNextMonth();
	void slotPrevYear();
	void slotNextYear();

private:
	void initModels();
	void initViews();
	void initConnections();
	void resizeSplitters();
	void setDurationAll();

private:
    Ui::HistoryPageClass ui;

	QSqlTableModel* modelUser;
	QDate   startDate;
	QDate   endDate;
	QString tableName;
	QString categoryTableName;
};

#endif // HISTORYPAGE_H
