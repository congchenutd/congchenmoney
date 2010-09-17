#pragma once
#include "QDialog.h"
#include "ui_DepositDlg.h"
#include <QVector>

struct DepositInfo
{
	QString bank;
	double  amount;
	QDate   start;
	QString period;
	QDate   end;
	double  rate;
	double  interests;
	bool    reDeposit;
};

class QSqlTableModel;

class DepositDlg : public QDialog
{
	Q_OBJECT

public:
	DepositDlg(QWidget* parent = 0);
	DepositInfo getDepositInfo() const;
	void        setDepositInfo(const DepositInfo& info);

private slots:
	void slotCalcEndDate();
	void slotCalcRate();
	void slotCalcInterests();
	void slotInterestDlg();
	void slotBank();

private:
	int getPeriod() const;

private:
	QVector<int> months;
	int monthsDeposited;
	Ui::DepositDlgClass ui;
	QSqlTableModel* model;
};
