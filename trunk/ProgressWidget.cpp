#include <QTableView>
#include <QDate>
#include "ProgressWidget.h"

ProgressWidget::ProgressWidget(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);

	model = new QStandardItemModel(0, 2);
	ui.kView  ->setModel(model);
	ui.barView->setModel(model);
}

ProgressWidget::~ProgressWidget() {
	delete model;
}

void ProgressWidget::refresh()
{
	model->removeRows(0, model->rowCount());
	int rowCount = srcModel->rowCount()-1;
	if(rowCount == 0)
	{
		model->insertRows(0, 1);
		model->setData(model->index(0, 0), "no data");
		model->setData(model->index(0, 1), 0);
	}
	else
	{
		model->insertRows(0, rowCount);
		for(int i=0; i<srcModel->rowCount()-1; ++i)
		{
			QDate date = QDate::fromString(
				srcModel->data(srcModel->index(i, 0)).toString(), "yyyy-MM");
			model->setData(model->index(i, 0), date.toString("yyMM"));
			model->setData(model->index(i, 1), srcModel->data(srcModel->index(i, dataColumn)));
		}
	}
}

void ProgressWidget::setDataColumn(int col)
{
	dataColumn = col;
	refresh();
}