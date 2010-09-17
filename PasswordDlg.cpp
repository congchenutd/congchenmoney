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
		QMessageBox::critical(this, tr("����"), tr("ԭ�������"));
		return;
	}
	if(ui.lineEditNew->text().isEmpty())
	{
		QMessageBox::critical(this, tr("����"), tr("�����벻��Ϊ��"));
		return;
	}
	if(ui.lineEditNew->text() != ui.lineEditConfirm->text())
	{
		QMessageBox::critical(this, tr("����"), tr("�������벻һ��"));
		return;
	}
	setting->setPassword(ui.lineEditNew->text());
	QDialog::accept();
}
