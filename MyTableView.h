#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H

#include <QMap>
#include <QTableView>

class MyTableView : public QTableView
{
public:
	MyTableView(QWidget *parent = 0);
	void saveHiddenSections();

	void setSectionSizeRatio(int section, double ratio);
	virtual void setModel(QAbstractItemModel* model);

protected:
	virtual void resizeEvent(QResizeEvent*);

private:
	QMap<int, double> sectionSizes;
	QString tableName;
};

#endif // MYTABLEVIEW_H
