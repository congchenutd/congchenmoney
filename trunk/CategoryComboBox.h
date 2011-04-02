#ifndef CATEGORYCOMBOBOX_H
#define CATEGORYCOMBOBOX_H

#include "../MyComboBox/SetTextComboBox.h"
#include <QComboBox>

class CategoryComboBox : public SetTextComboBox
{
	Q_OBJECT

public:
	CategoryComboBox(QWidget *parent=0) : SetTextComboBox(parent), maintypeOnly(true) {}

	int  getCategory() const;
	void setTable   (const QString& table);
	void setCategory(const QString& cate);
	void setCategory(int cateid);
	void setMaintypeOnly(bool show) { maintypeOnly = show; }
	void refresh();

	static QString getCategoryName(const QString& tableName, int id);
	static QColor  getCategoryColor(const QString& tableName, const QString& cateName);

private:
	QString tableName;
	bool    maintypeOnly;
};

#endif // CATEGORYCOMBOBOX_H
