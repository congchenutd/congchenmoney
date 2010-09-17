#ifndef PIEWIDGET_H
#define PIEWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include "ui_piewidget.h"
#include "PieModel.h"

class PieWidget : public QWidget
{
    Q_OBJECT

public:
    PieWidget(QWidget *parent = 0);
	~PieWidget();

	void setTable(const QString& table);
	void setSourceModel(QStandardItemModel* m) { srcModel = m; }
	void refresh();

private:
    Ui::PieWidgetClass ui;

	PieModel*           model;
	QStandardItemModel* srcModel;
	QString             tableName;
};

#endif // PIEWIDGET_H
