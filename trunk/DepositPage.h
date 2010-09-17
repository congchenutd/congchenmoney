#pragma once

#include "ui_DepositPage.h"
#include "DepositDlg.h"
#include <QDialog>

class QSqlTableModel;

class DepositPage :	public QDialog
{
	Q_OBJECT

public:
	DepositPage(QWidget* parent = 0);
	~DepositPage();

private slots:
	void slotAdd();
	void slotEdit();
	void slotDel();
	void slotUpdateCurrentRow(const QModelIndex& idx);

private:
	void initModel();
	void initView();
	void initConnection();
	void setRecord(int row, const DepositInfo& record);
	DepositInfo getRecord(int row) const;
	void updateSummary();
	void submit();

public:
	enum {ID, Bank, Amount, StartDate, Period, EndDate, InterestRate, Interests, ReDeposit};
private:
	Ui::DepositPageClass ui;
	QSqlTableModel* model;
	int currentRow;
};
