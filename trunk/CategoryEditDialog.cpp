#include <QColorDialog>
#include <QSqlQuery>
#include <algorithm>
#include "CategoryEditDialog.h"
#include <QSqlTableModel>

CategoryEditDialog::CategoryEditDialog(const QString& table, QWidget *parent) 
	: QDialog(parent), categoryTableName(table)
{
	ui.setupUi(this);
	initColors();
	ui.comboBoxParent->setTable(table);

	QSqlTableModel* model = new QSqlTableModel(this);
	model->setTable("Users");
	model->select();
	ui.comboBoxUsers->setModel(model);

	connect(ui.comboBoxParent, SIGNAL(activated(int)), this, SLOT(slotChooseParent(int)));
}

void CategoryEditDialog::initColorComboBox()
{
	QSqlQuery query;
	query.exec(tr("SELECT color FROM %1 GROUP BY color").arg(categoryTableName));
	while(query.next())
		ui.comboBoxColors->addColor((query.value(0).toInt()), "");
	ui.comboBoxColors->setColorDialogEnabled();
}

void CategoryEditDialog::initColors()
{
	initColorComboBox();
	setColor(Qt::red);    // default color
}

QString CategoryEditDialog::getName() const
{
	QString subtypeName = ui.lineEdit->text();
	QString parentName  = ui.comboBoxParent->currentText();
	if(parentName == tr("NoCategory"))
		return subtypeName;
	else
		return parentName + " - " + subtypeName;
}

void CategoryEditDialog::setName(const QString& name) 
{
	ui.lineEdit->setText(name);
	ui.lineEdit->selectAll();
	int index = ui.comboBoxParent->findText(name);
	if(index != -1)   // can't be itself's parent
		ui.comboBoxParent->removeItem(ui.comboBoxParent->findText(name));
}

QColor CategoryEditDialog::getColor() const {
	return ui.comboBoxColors->currentColor();
}

void CategoryEditDialog::setColor(const QColor& color) {
	ui.comboBoxColors->setCurrentColor(color);
}

int CategoryEditDialog::getParent() const
{
	QSqlQuery query;
	query.exec(tr("SELECT id FROM %1 WHERE name = \"%2\"")
					.arg(categoryTableName)
					.arg(ui.comboBoxParent->currentText()));
	return query.next() ? query.value(0).toInt() : 1;   // 1 for NoCategory
}

void CategoryEditDialog::setParent(int parent)
{
	QSqlQuery query;
	query.exec(tr("SELECT name FROM %1 WHERE id = \"%2\"").arg(categoryTableName).arg(parent));
	if(query.next())
		ui.comboBoxParent->setCurrentIndex(ui.comboBoxParent->findText(query.value(0).toString()));
	else
		ui.comboBoxParent->setCurrentIndex(0);		
}

void CategoryEditDialog::slotChooseParent(int)
{
	QSqlQuery query;
	query.exec(tr("SELECT color FROM %1 WHERE name = \"%2\"")
										.arg(categoryTableName)
										.arg(ui.comboBoxParent->currentText()));
	if(query.next())  // same color as parent's
		ui.comboBoxColors->setCurrentColor(QColor(query.value(0).toInt()));
}

QString CategoryEditDialog::getUser() const {
	return ui.comboBoxUsers->currentText();
}

void CategoryEditDialog::setUser(const QString& user) {
	ui.comboBoxUsers->setCurrentText(user);
}