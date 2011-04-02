#include <QMessageBox>
#include <QtSQL/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlDriver>
#include <QFile>
#include <QFileDialog>
#include <QDate>
#include <QTextStream>
#include <QInputDialog>
#include "MainWindow.h"
#include "SettingDlg.h"
#include "DailyPage.h"
#include "HistoryPage.h"
#include "ComparePage.h"
#include "Connection.h"
#include "DepositPage.h"

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
	initDB();
	initView();
	initConnections();
	loadSettings();
}

MainWindow::~MainWindow()
{
	UserSetting* setting = MySetting<UserSetting>::getInstance("Global");
	if(setting->value("AutoBackup").toBool())
	{
		delOldBackup();
		backup();
	}
	setting->destroySettingManager();
}

void MainWindow::initDB()
{
	loadDBFileName();
	openDB(fileName);
	createTables();
}

void MainWindow::initView()
{
	ui.setupUi(this);
	outcomePage        = new DailyPage  (true,  this);
	outcomeHistoryPage = new HistoryPage(true,  this);
	incomePage         = new DailyPage  (false, this);
	incomeHistoryPage  = new HistoryPage(false, this);
	comparePage        = new ComparePage(this);
	ui.stackedWidget->addWidget(outcomePage);
	ui.stackedWidget->addWidget(outcomeHistoryPage);
	ui.stackedWidget->addWidget(incomePage);
	ui.stackedWidget->addWidget(incomeHistoryPage);
	ui.stackedWidget->addWidget(comparePage);
	setWindowTitle("My Money");
	ui.listWidget->setMovement(QListView::Static);
}

void MainWindow::initConnections()
{
	connect(ui.listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
			this,		   SLOT  (slotChangePage    (QListWidgetItem*, QListWidgetItem*)));
	connect(ui.actionAbout,    SIGNAL(triggered()), this, SLOT(slotAbout()));
	connect(ui.actionSetting,  SIGNAL(triggered()), this, SLOT(slotSetting()));
	connect(ui.actionBackup,   SIGNAL(triggered()), this, SLOT(slotBackup()));
	connect(ui.actionDeposits, SIGNAL(triggered()), this, SLOT(slotDeposits()));
	connect(ui.actionQuit,     SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::slotChangePage(QListWidgetItem* current, QListWidgetItem*) 
{
	int row = ui.listWidget->row(current);
	ui.stackedWidget->setCurrentIndex(row);
}

void MainWindow::setFonts(const QFont& guiFont)
{
    QFont font = guiFont;
#ifdef Q_WS_MAC
	ui.listWidget->setFont(QFont("STHeiTi", 18, QFont::Black));
	font = QFont("STHeiTi", 15);
#endif

    QApplication::setFont(font);
    setFont(font);
}

void MainWindow::loadDBFileName()
{
	UserSetting* setting = MySetting<UserSetting>::getInstance("Global");
	fileName = setting->value("DBFileName").toString();
	if(fileName.isEmpty())
		fileName = "Money.db";
}

void MainWindow::loadSettings()
{
	UserSetting* setting = MySetting<UserSetting>::getInstance("Global");
	setFonts(setting->getFont("GuiFont"));
}

void MainWindow::slotSetting()
{
	SettingDlg dlg(this);
	dlg.importSettings();
	if(dlg.exec() == QDialog::Accepted)
	{
		dlg.exportSettings();
		loadSettings();
	}
}

void MainWindow::slotAbout() {
	QMessageBox::about(this, tr("About"), tr("<h3><b>MyMoney</b></h3>"
                                                                                         "<p>Build 2010.1.31</p>"
											 "<p>基于GPL协议发布</p>"
											 "<p>congchenutd@gmail.com</p>"));
}

void MainWindow::slotBackup()
{
	QString backupFileName = QFileDialog::getSaveFileName(this,
		tr("另存数据库文件"),
		tr("./Backup/%1.db").arg(QDate::currentDate().toString(Qt::ISODate)),
		"Database (*.db);;All Files (*.*)");

	if(!backupFileName.isEmpty())
		backup(backupFileName);
}

void MainWindow::delOldBackup()
{
	const QDate today = QDate::currentDate();
	const int   days  = MySetting<UserSetting>::getInstance("Global")->value("BackupDays").toInt();
	const QFileInfoList fileInfos = 
		QDir("Backup").entryInfoList(QStringList() << "*.db", QDir::Files);
	foreach(QFileInfo fileInfo, fileInfos)
		if(QDate::fromString(fileInfo.baseName(), Qt::ISODate).daysTo(today) > days)
			QFile::remove(fileInfo.filePath());
}

void MainWindow::backup(const QString& name)
{
	QDir::current().mkdir("Backup");
	QString backupFileName = name.isEmpty() 
		? "./Backup/" + QDate::currentDate().toString(Qt::ISODate) + ".db" 
		: name;

	if(QFile::exists(backupFileName))
		QFile::remove(backupFileName);

	QFile file(fileName);
	file.copy(backupFileName);
	file.close();
}

void MainWindow::slotDeposits()
{
	bool ok;
	QString pass = QInputDialog::getText(this, tr("密码"),
		tr("管理员密码："), QLineEdit::Password, QString(), &ok);
	if(!ok)
		return;
	if(UserSetting::getInstance("Global")->getPassword() != pass)
	{
		QMessageBox::critical(this, tr("错误"), tr("密码错误"));
		return;
	}
	DepositPage page(this);
	page.exec();
}
