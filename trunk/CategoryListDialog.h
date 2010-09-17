#ifndef EDITCATEGORYDIALOG_H
#define EDITCATEGORYDIALOG_H

#include <QDialog>
#include "ui_CategoryListDialog.h"

class CategoryModel;

class CategoryListDialog : public QDialog
{
	Q_OBJECT

public:
	CategoryListDialog(bool out, QWidget *parent = 0);

private slots:
	void slotAdd();
	void slotEdit();
	void slotDelete();
	void slotUpdateCurrentRow(const QModelIndex& current);

private:
	void initModel();
	void initView();
	void initConnection();
	void changeParentTypeName (const QString& parentName, const QString& newName);
	void changeParentTypeColor(const QString& parentName, const QColor&  newColor);
	void deleteParentType     (const QString& parentName);

private:
	enum {ID, Name, Color, Parent, User};

	Ui::CategoryListDialogClass ui;
	CategoryModel* model;
	int            currentRow;
	QString        tableName;
	QString        categoryTableName;
	QString        autoTableName;
};

#endif // EDITCATEGORYDIALOG_H
