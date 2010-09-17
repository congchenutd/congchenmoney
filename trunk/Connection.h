#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlQuery>
#include <QVariant>
#include <QDate>
#include <QString>
#include <QSqlDatabase>
#include <QMessageBox>

inline int getNextID(const QString& tableName)
{
	QSqlQuery query;
	query.exec(QString("select max(id) from %1").arg(tableName));
	return query.next() ? query.value(0).toInt() + 1 : 1;
}

inline QString getTableName(bool outcome) {
	return outcome ? "Outcome" : "Income";
}

inline QString getCategoryTableName(bool outcome) {
	return outcome ? "OutcomeCategory" : "IncomeCategory";
}

inline QString getAutoTableName(bool outcome) {
	return outcome ? "OutcomeAuto" : "IncomeAuto";
}

inline QDate getFirstDay(const QString& table)
{
	QSqlQuery query;
	query.exec("select min(date) from " + table);
	return query.next() ? query.value(0).toDate() : QDate::currentDate();
}

inline QDate getLastDay(const QString& table)
{
	QSqlQuery query;
	query.exec("select max(date) from " + table);
	return query.next() ? query.value(0).toDate() : QDate::currentDate();
}

inline double getMinAmount(const QString& table)
{
	QSqlQuery query;
	query.exec("select min(amount) from " + table);
	return query.next() ? query.value(0).toDouble() : 0.0;
}

inline double getMaxAmount(const QString& table)
{
	QSqlQuery query;
	query.exec("select max(amount) from " + table);
	return query.next() ? query.value(0).toDouble() : 0.0;
}

inline bool openDB(const QString& name)
{
	QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
	database.setDatabaseName(name);
	if(!database.open())
	{
		QMessageBox::critical(0, "error", "can't open database");
		return false;
	}
	return true;
}

inline void createTables()
{
	QSqlQuery query;
	QString iocomeString = 
		"(                                 \
			id          int primary key,  \
			date        date,             \
			amount      decimal,          \
			category    int,              \
			userName    varchar,          \
			description varchar           \
		)";
	query.exec("create table Outcome" + iocomeString);
	query.exec("create table Income"  + iocomeString);

	QString categoryString = 
		"(                                \
			id     int primary key,       \
			name   varchar,               \
			color  unsigned int,          \
			parent int,                   \
			defaultUser varchar           \
		)";
	query.exec("create table OutcomeCategory" + categoryString);
	query.exec("create table IncomeCategory"  + categoryString);

	QString autoString = 
		"(                               \
			id        int primary key,  \
			start     date,             \
			interval  int,              \
			amount    decimal,          \
			category  int,              \
			user      varchar,          \
			auto      boolean,          \
			lastDate  date              \
		)";
	query.exec("create table OutcomeAuto" + autoString);
	query.exec("create table IncomeAuto"  + autoString);
	query.exec("create table TaxRate (rate double)");

	query.exec("create table HiddenSections(tableName varchar, section int)");
	query.exec("create table Users(userName varchar primary key)");
	query.exec(QObject::tr("insert into Users values(\'公共\')"));
	query.exec(QObject::tr("insert into OutcomeCategory values(1, \'未分类\', 4294901760, 1)"));
	query.exec(QObject::tr("insert into IncomeCategory  values(1, \'未分类\', 4294901760, 1)"));
}

#endif