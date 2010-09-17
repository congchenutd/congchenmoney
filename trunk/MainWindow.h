#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QSqlDatabase>
#include "ui_mainwindow.h"

class DailyPage;
class HistoryPage;
class ComparePage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWindow();

private slots:
	void slotChangePage(QListWidgetItem*, QListWidgetItem*);
	void slotAbout();
	void slotSetting();
	void slotBackup();
	void slotDeposits();

private:
	void initView();
	void initDB();
	void initConnections();
	void loadDBFileName();
	void loadSettings();
	void setFonts(const QFont& guiFont);
	void backup(const QString& fileName = QString());
	void delOldBackup();

private:
    Ui::MainWindowClass ui;
	
	QString      fileName;
	DailyPage*   outcomePage;
	DailyPage*   incomePage;
	HistoryPage* outcomeHistoryPage;
	HistoryPage* incomeHistoryPage;
	ComparePage* comparePage;
};

#endif // MAINWINDOW_H
