#ifndef AUTOLISTDIALOG_H
#define AUTOLISTDIALOG_H

#include <QDialog>
#include "ui_AutoListDialog.h"
#include "AutoDialog.h"

class QSqlRelationalTableModel;

class AutoListDialog : public QDialog
{
	Q_OBJECT

public:
	AutoListDialog(bool out, QWidget *parent = 0);

private:
	void initModel();
	void initView();
	void initConnections();

	AutoRecord exportRecord(int row) const;
	void       importRecord(int row, const AutoRecord& record);

private slots:
	void slotAdd();
	void slotDel();
	void slotEdit();
	void slotUpdateCurrentRow(const QModelIndex& idx);

public:
	enum {ID, Date, Interval, Amount, Category, User, Auto, LastDate};
private:
	Ui::AutoListDialogClass ui;
	QSqlRelationalTableModel* model;
	QString autoTableName;
	QString categoryTableName;
	int     currentRow;
	bool    outcome;
};

#endif // AUTOLISTDIALOG_H
