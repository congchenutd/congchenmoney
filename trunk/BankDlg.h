#pragma once

#include <QDialog>
#include "ui_BankDlg.h"

class QSqlTableModel;

class BankDlg : public QDialog
{
	Q_OBJECT

public:
	BankDlg(QWidget* parent = 0);
	void accept();

private slots:
	void slotAdd();
	void slotEdit();
	void slotDel();
	void slotUpdateCurrentRow(const QModelIndex& idx);

private:
	Ui::BankDlgClass ui;
	int currentRow;
	QSqlTableModel* model;
};
