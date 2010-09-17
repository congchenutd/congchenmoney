#include "PasswordDlg.h"
#include "QMessageBox.h"
#include "SettingDlg.h"
#include <QByteArray>

PasswordDlg::PasswordDlg(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
}

void PasswordDlg::accept()
{
	UserSetting* setting = UserSetting::getInstance("Global");
	if(ui.lineEditOld->text() != setting->getPassword())
	{
		QMessageBox::critical(this, tr("´íÎó"), tr("Ô­ÃÜÂë´íÎó"));
		return;
	}
	if(ui.lineEditNew->text().isEmpty())
	{
		QMessageBox::critical(this, tr("´íÎó"), tr("ÐÂÃÜÂë²»ÄÜÎª¿Õ"));
		return;
	}
	if(ui.lineEditNew->text() != ui.lineEditConfirm->text())
	{
		QMessageBox::critical(this, tr("´íÎó"), tr("Á½´ÎÃÜÂë²»Ò»ÖÂ"));
		return;
	}
	setting->setPassword(ui.lineEditNew->text());
	QDialog::accept();
}
