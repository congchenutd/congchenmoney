#include <QFontDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSqlQuery>
#include <QTextStream>
#include <QInputDialog>
#include <QSqlTableModel>
#include "SettingDlg.h"
#include "PasswordDlg.h"

SettingDlg::SettingDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	model = new QSqlTableModel(this);
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->setTable("Users");
	model->select();
	ui.comboBoxUser->setModel(model);

	connect(ui.buttonGuiFont,       SIGNAL(clicked()), this, SLOT(slotGuiFont()));
	connect(ui.buttonSetDBFileName, SIGNAL(clicked()), this, SLOT(slotSetDBFileName()));
	connect(ui.buttonCalc,          SIGNAL(clicked()), this, SLOT(slotSetCalc()));
	connect(ui.buttonAddUser,       SIGNAL(clicked()), this, SLOT(slotAddUser()));
	connect(ui.buttonDelUser,       SIGNAL(clicked()), this, SLOT(slotDelUser()));
	connect(ui.buttonPassword,      SIGNAL(clicked()), this, SLOT(slotPassword()));
	connect(ui.checkBoxAutoBackup, SIGNAL(toggled(bool)), ui.spinBoxBackupDays, SLOT(setEnabled(bool)));
}

void SettingDlg::slotGuiFont() {
	setGuiFont(QFontDialog::getFont(0, getGuiFont(), this));
}

void SettingDlg::slotSetDBFileName()
{
	QString fileName = QFileDialog::getOpenFileName(
		this, tr("设置数据库文件"),	"money.db",	"Database (*.db);;All Files (*.*)");
	if(!fileName.isEmpty())
	{
		setDBFileName(QFileInfo(fileName).filePath());
		QMessageBox::information(this, tr("注意"), tr("本设置将在下次启动生效"));
	}
}

void SettingDlg::slotAddUser()
{
	bool ok;
	QString name = QInputDialog::getText(this, tr("添加用户"), 
		tr("输入用户名"), QLineEdit::Normal, tr("新建用户"), &ok);
	if (ok && !name.isEmpty())
	{
		if(ui.comboBoxUser->findText(name) != -1)
		{
			QMessageBox::warning(this, tr("错误"), tr("该用户名已经存在"));
			return;
		}
		ui.comboBoxUser->addItem(name);
		model->submitAll();
	}
}

void SettingDlg::slotDelUser()
{
	if(QMessageBox::warning(this, tr("确定删除"), tr("真的要删除该用户么？"), 
						QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
	{
		ui.comboBoxUser->removeItem(ui.comboBoxUser->currentIndex());
		model->submitAll();
	}
}

void SettingDlg::exportSettings() const
{
	UserSetting* setting = MySetting<UserSetting>::getInstance("Global");
	setting->setValue("DBFileName", getDBFileName());
	setting->setValue("Calculator", getCalculator());
        setting->setValue("GuiFont",    getGuiFont().toString());
	setting->setValue("AutoBackup", getAutoBackup());
	setting->setValue("BackupDays", getBackupDays());
        setting->setValue("TaxRate",    getTaxRate());
}

void SettingDlg::importSettings()
{
	UserSetting* setting = MySetting<UserSetting>::getInstance("Global");
	setDBFileName(setting->value("DBFileName").toString());
	setCalculator(setting->value("Calculator").toString());
        setGuiFont   (setting->getFont("GuiFont"));
	setAutoBackup(setting->value("AutoBackup").toBool());
	setBackupDays(setting->value("BackupDays").toInt());
	setTaxRate(setting->value("TaxRate").toDouble());
}

void SettingDlg::setAutoBackup(bool aut)
{
	ui.checkBoxAutoBackup->setChecked(aut);
	ui.spinBoxBackupDays->setEnabled(aut);
}

void SettingDlg::slotPassword()
{
	PasswordDlg dlg(this);
	dlg.exec();
}

void SettingDlg::slotSetCalc()
{
	QString fileName = QFileDialog::getOpenFileName(
		this, tr("设置计算器路径"));
	if(!fileName.isEmpty())
		setCalculator(QFileInfo(fileName).filePath());
}
