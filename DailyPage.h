#ifndef DAILYPAGE_H
#define DAILYPAGE_H

#include <QSqlRelationalTableModel>
#include "DailyDialog.h"
#include "ui_dailypage.h"

class DailyModel;

class DailyPage : public QWidget
{
    Q_OBJECT

public:
    DailyPage(bool out, QWidget *parent = 0);
	~DailyPage();
	void addRecord(const DailyRecord& record);

protected:
	virtual void showEvent(QShowEvent* event);

private slots:
	void slotAdd();
	void slotEdit();
	void slotDelete();
	void slotCategory();
	void slotAuto();
	void slotUpdateCurrentRow(const QModelIndex& idx);
	void slotCheckAuto();
	void slotEnableSearch(bool enable);
	void slotSearch();

private:
	void initModel();
	void initView();
	void initConnection();
	QDate getDueDay(const QDate& last, QString interval) const;
	void updateUser();
	void updateCategory();

	DailyRecord exportRecord(int row) const;
	void        importRecord(int row, const DailyRecord& record);

public:
	enum {ID, Date, Amount, Category, User, Description};
private:
    Ui::DailyPageClass ui;
	DailyModel* model;
	int     currentRow;
	bool    outcome;
	QString tableName;
	QString categoryTableName;
	bool    firstShow;
};

#endif // DAILYPAGE_H
