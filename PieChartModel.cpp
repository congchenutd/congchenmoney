#include <QSqlQuery>
#include "PieChartModel.h"
#include "CategoryModel.h"

PieChartModel::PieChartModel(QObject *parent)
	: QStandardItemModel(parent)
{
	insertColumns(0, 3);
	setHeaderData(CATEGORY,   Qt::Horizontal, tr("Category"));
	setHeaderData(AMOUNT,     Qt::Horizontal, tr("Amount"));
	setHeaderData(PERCENTAGE, Qt::Horizontal, tr("Percentage"));
}

void PieChartModel::insertRecord(int row, const QString& categoryName, double amount)
{
	insertRow(row);
	QColor color = CategoryModel::getColor(tableName, categoryName);
	setData(index(row, CATEGORY), categoryName);
	setData(index(row, CATEGORY), color, Qt::DecorationRole);
	setData(index(row, AMOUNT),   amount);
}

double PieChartModel::getTotalAmount() const
{
	double result = 0.0;
	for(int row = 0; row < rowCount(); ++row)
		result += data(index(row, AMOUNT)).toDouble();
	return result;
}

void PieChartModel::refreshPercentage()
{
	double total = getTotalAmount();
	for(int row = 0; row < rowCount(); ++row)
	{
		double thisAmount = data(index(row, AMOUNT)).toDouble();
		QString percentage = 
			(total == 0) ? "100%" : QString::number((int)(thisAmount/total*100)) + "%";
		setData(index(row, PERCENTAGE), percentage);
	}
}