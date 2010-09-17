#ifndef DAILYDIALOG_H
#define DAILYDIALOG_H

#include <QDate>
#include <QSqlQueryModel>
#include "ui_DailyDialog.h"

class QSqlTableModel;

struct DailyRecord
{
	QDate   date;
	double  amount;
	int     cateID;
	QString cateName;
	QString userName;
	QString description;
};

class DailyDialog : public QDialog
{
    Q_OBJECT

public:
    DailyDialog(bool outcome, QWidget *parent);
	void accept();

	DailyRecord getRecord() const;
	void        setRecord(const DailyRecord& record);

private:
	void initView();
	void initConnections();
	void clearData();
	double getAmount() const;
	void setDate       (const QDate& date);
	void setAmount     (double amount);
	void setDescription(const QString& str);
	void setCategory   (const QString& text);
	void setUserName   (const QString& name);
	void loadTaxRate();

private slots:
	void slotAddRecord();
	void slotEnableNewButton();
	void slotLaunchCalculator();
	void slotSetUser();
	void slotTax();

private:
    Ui::EditDialogClass ui;
	QString categoryTableName;
	QSqlTableModel* model;
};

#endif
