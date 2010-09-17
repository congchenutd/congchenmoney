#ifndef KWIDGET_H
#define KWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include "ui_ProgressWidget.h"

class ProgressWidget : public QWidget
{
    Q_OBJECT

public:
    ProgressWidget(QWidget *parent = 0);
	~ProgressWidget();

	void setSourceModel(QStandardItemModel* m) { srcModel = m; }
	void setDataColumn(int col);
	void refresh();

private:
    Ui::ProgressWidgetClass ui;
	QStandardItemModel* model;
	QStandardItemModel* srcModel;
	int                 dataColumn;
};

#endif // KWIDGET_H
