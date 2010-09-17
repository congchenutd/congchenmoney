#ifndef DETAILWINDOW_H
#define DETAILWINDOW_H

#include <QDialog>
#include <QDate>
#include <QSqlQueryModel>
#include "ui_detailwindow.h"

class DetailWindow : public QDialog
{
    Q_OBJECT

public:
	DetailWindow(bool outcome, QWidget* parent, 
				 const QString& filters, const QDate& startDate, const QDate& endDate);

public slots:
	int exec();

private slots:
	void slotSort(int column);

private:
	void centerWindow();
	void initView();
	void initModel();
	void initConnections();
	void executeQuery();
	QString getOrderFilter();

private:
	enum {Date,	Amount,	Category, User,	Description};
    Ui::DetailWindowClass ui;

	QSqlQueryModel model;
	Qt::SortOrder  order;
	QString        orderBy;
	QString        monthCategoryFilters;
	QDate          startDate;
	QDate          endDate;
	QString        tableName;
	QString        categoryTableName;
};

#endif // DETAILWINDOW_H
