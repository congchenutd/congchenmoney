#include "PieWidget.h"

PieWidget::PieWidget(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);

	model = new PieChartModel(this);
	ui.pieView->setModel(model);
}

PieWidget::~PieWidget()
{
	delete model;
}

void PieWidget::setTable(const QString& table) 
{ 
	tableName = table; 
	model->setTable(tableName);
}

void PieWidget::refresh()
{
	model->removeRows(0, model->rowCount());
	for(int col = 1; col < srcModel->columnCount()-1; ++col)
	{
		model->insertRecord(
			col-1,
			srcModel->headerData(col, Qt::Horizontal).toString(),
			srcModel->data(srcModel->index(srcModel->rowCount()-1, col)).toDouble());
	}

	model->refreshPercentage();
}