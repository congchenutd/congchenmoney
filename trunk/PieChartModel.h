#ifndef PIEMODEL_H
#define PIEMODEL_H

#include <QStandardItemModel>
#include <QColor>

class PieChartModel : public QStandardItemModel
{
	Q_OBJECT

public:
    PieChartModel(QObject *parent = 0);
	void   insertRecord(int row, const QString& categoryName, double amount);
	double getTotalAmount() const;
	void   refreshPercentage();
	void   setTable(const QString& table) { tableName = table; }

private:
	enum {CATEGORY, AMOUNT, PERCENTAGE};
	QString tableName;
};

#endif // PIEMODEL_H
