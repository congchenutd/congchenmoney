#ifndef CATEGORYMODEL_H
#define CATEGORYMODEL_H

#include <QSqlQuery>
#include <QSqlTableModel>
#include <QColor>
#include "Connection.h"

class CategoryModel : public QSqlTableModel
{
	Q_OBJECT

public:
	CategoryModel(QObject *parent = 0);
	virtual QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;
	virtual void     setTable(const QString& tableName);

	QString getFullName   (int row) const;
	QString getSubtypeName(int row) const;
	QColor  getColor      (int row) const;
	int     getParent     (int row) const;

	static int     getParentID    (const QString& table, int id);
	static QColor  getColor       (const QString& table, const QString& categoryName);
	static bool    isSubtype      (const QString& categoryName);
	static QString getSubtypeName (const QString& fullName);
	static QString getMaintypeName(const QString& fullName);
	static QString getCategoryName(const QString& table, int id);
	static int     getCategoryID  (const QString& table, const QString& categoryName);
	static QStringList getSubtypes(const QString& table, const QString& categoryName);
	static bool        hasSubtypes(const QString& table, const QString& categoryName);
	static QList<int>  categoryFamily(const QString& table, int parent);

private:
	enum {ID, Name, Color, Parent};
};

#endif // CATEGORYMODEL_H
