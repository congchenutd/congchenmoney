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
	connect(ui.checkBoxAutoBackup, SIGNAL(toggled(bool)), ui.spinBoxBackupDays, SLOT(setEnabled(bool)));
}

void SettingDlg::slotGuiFont() {
	setGuiFont(QFontDialog::getFont(0, getGuiFont(), this));
}

void SettingDlg::slotSetDBFileName()
{
	QString fileName = QFileDialog::getOpenFileName(
		this, tr("Database file"), "money.db", "Database (*.db);;All Files (*.*)");
	if(!fileName.isEmpty())
	{
		setDBFileName(QFileInfo(fileName).filePath());
		QMessageBox::information(this, tr("Warning"), tr("Activate next time"));
	}
}

void SettingDlg::slotAddUser()
{
	bool ok;
	QString name = QInputDialog::getText(this, tr("Add user"), 
		tr("User name"), QLineEdit::Normal, tr("New user"), &ok);
	if (ok && !name.isEmpty())
	{
		if(ui.comboBoxUser->findText(name) != -1)
		{
			QMessageBox::warning(this, tr("Error"), tr("User exists"));
			return;
		}
		ui.comboBoxUser->addItem(name);
		model->submitAll();
	}
}

void SettingDlg::slotDelUser()
{
	if(QMessageBox::warning(this, tr("Confirm"), tr("Really delete?"), 
						QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
	{
		ui.comboBoxUser->removeItem(ui.comboBoxUser->currentIndex());
		model->submitAll();
	}
}

void SettingDlg::exportSettings() const
{
	UserSetting* setting = MySetting<UserSetting>::getInstance();
	setting->setValue("DBFileName", getDBFileName());
	setting->setValue("Calculator", getCalculator());
	setting->setValue("GuiFont",    getGuiFont().toString());
	setting->setValue("AutoBackup", getAutoBackup());
	setting->setValue("BackupDays", getBackupDays());
	setting->setValue("TaxRate",    getTaxRate());
}

void SettingDlg::importSettings()
{
	UserSetting* setting = MySetting<UserSetting>::getInstance();
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

void SettingDlg::slotSetCalc()
{
	QString fileName = QFileDialog::getOpenFileName(
		this, tr("Set calculator path"));
	if(!fileName.isEmpty())
		setCalculator(QFileInfo(fileName).filePath());
}
