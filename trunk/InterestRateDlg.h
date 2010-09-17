#pragma once
#include <QDialog>
#include "ui_InterestRateDlg.h"

class QSqlTableModel;

class InterestRateDlg : public QDialog
{
public:
	InterestRateDlg(QWidget* parent = 0);
	void accept();

private:
	Ui::InterestRateDlgClass ui;
	QSqlTableModel* model;
};
