#ifndef DAILYMODEL_H
#define DAILYMODEL_H

#include <QSqlRelationalTableModel>

class DailyModel : public QSqlRelationalTableModel
{
	Q_OBJECT

public:
	DailyModel(QObject *parent = 0);

	virtual QVariant data(const QModelIndex& idx, int role = Qt::DisplayRole) const;

private:
	QColor getColor(const QString& cate) const;
};

#endif // DAILYMODEL_H
