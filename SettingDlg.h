#ifndef SETTINGDLG_H
#define SETTINGDLG_H

#include <QDialog>
#include <QByteArray>
#include "../MySetting/MySetting.h"
#include "ui_SettingDlg.h"

class UserSetting : public MySetting<UserSetting>
{
public:
	UserSetting(const QString& userName) : MySetting<UserSetting>(userName) {
		if(QFile(userName + ".ini").size() == 0)   // no setting
			loadDefaults();
	}

	QFont getFont(const QString& section) const
	{
		QFont font;
		font.fromString(value(section).toString());
		return font;
	}

	QString getPassword() const
	{
		QByteArray bytes = value("Password").toByteArray();
		return QByteArray::fromBase64(bytes);
	}

	void setPassword(const QString& pass) {
		setValue("Password", pass.toAscii().toBase64());
	}

protected:
	virtual void loadDefaults()
	{
		setValue("DBFileName", "Money.db");
		setValue("AutoBackup", true);
		setValue("BackupDays", 7);
		setValue("TaxRate", 8.25);
		setValue("GuiFont", QFont("Arial", 10));
	}
};

class QSqlTableModel;

class SettingDlg : public QDialog
{
	Q_OBJECT

public:
	SettingDlg(QWidget *parent = 0);
	void exportSettings() const;
	void importSettings();

private:
	QFont   getGuiFont   () const { return guiFont;   }
	QString getDBFileName() const { return ui.lineEditDBFile->text();          }
	QString getCalculator() const { return ui.lineEditCalc->text();            }
	bool    getAutoBackup() const { return ui.checkBoxAutoBackup->isChecked(); }
	int     getBackupDays() const { return ui.spinBoxBackupDays->value();      }
	double  getTaxRate   () const { return ui.doubleSpinBoxTax->value();       }
	void setGuiFont   (const QFont& font)   { guiFont   = font; }
	void setDBFileName(const QString& name) { ui.lineEditDBFile->setText(name);       }
	void setBackupDays(int days)            { ui.spinBoxBackupDays->setValue(days);   }
	void setTaxRate   (double rate)         { ui.doubleSpinBoxTax->setValue(rate);    }
	void setCalculator(const QString& calc) { ui.lineEditCalc->setText(calc);         }
	void setAutoBackup(bool aut);

private slots:
	void slotGuiFont();
	void slotSetDBFileName();
	void slotAddUser();
	void slotDelUser();
	void slotSetCalc();

private:
	Ui::SettingDlgClass ui;
	QFont guiFont;
	QSqlTableModel* model;
};

#endif // SETTINGDLG_H
