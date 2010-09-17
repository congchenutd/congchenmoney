#ifndef AUTODIALOG_H
#define AUTODIALOG_H

#include <QDialog>
#include "ui_AutoDialog.h"

class QSqlTableModel;

struct AutoRecord
{
	QDate   date;
	QString interval;
	double  amount;
	int     cateID;
	QString cateName;
	QString user;
	bool    autoAdd;
};

class AutoDialog : public QDialog
{
	Q_OBJECT

public:
	AutoDialog(bool outcome, QWidget *parent = 0);
	void accept();

	AutoRecord getRecord() const;
	void       setRecord(const AutoRecord& record);

private:
	void initModel();
	void initView();
	QString getInterval() const;
	double  getAmount()   const;
	void setInterval(const QString& interval);
	void setUser    (const QString& user);

private slots:
	void slotIntervalChanged(int);

private:
	Ui::AutoDialogClass ui;
	QSqlTableModel* userModel;
	QString         categoryTableName;
};

#endif // AUTODIALOG_H
