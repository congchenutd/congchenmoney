#ifndef CATEGORYDIALOG_H
#define CATEGORYDIALOG_H

#include <vector>
#include "ui_CategoryEditDialog.h"

class CategoryEditDialog : public QDialog
{
    Q_OBJECT

public:
	CategoryEditDialog(const QString& table, QWidget *parent = 0);

	QString getName  () const;
	QColor  getColor () const;
	int     getParent() const;
	QString getUser()   const;
	void    setName  (const QString& name);
	void    setColor (const QColor&  color);
	void    setParent(int parent);
	void    setUser  (const QString& user);

public slots:
	void slotChooseParent(int index);

private:
	void initColorComboBox();
	void initColors();

private:
    Ui::CategoryEditDialogClass ui;
	QString categoryTableName;
};

#endif // CATEGORYDIALOG_H
