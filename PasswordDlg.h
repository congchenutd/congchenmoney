#ifndef PASSWORDDLG_H
#define PASSWORDDLG_H

#include <QDialog>
#include "ui_PasswordDlg.h"

class PasswordDlg : public QDialog
{
	Q_OBJECT

public:
	PasswordDlg(QWidget *parent = 0);

	void accept();

private:
	Ui::PasswordDlgClass ui;
};

#endif // PASSWORDDLG_H
