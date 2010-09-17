#ifndef MYHEADERVIEW_H
#define MYHEADERVIEW_H

#include <QHeaderView>
#include <QMap>

class QAction;

class MyHeaderView : public QHeaderView
{
	Q_OBJECT

public:
	MyHeaderView(Qt::Orientation, QWidget *parent = 0);

protected:
	virtual void mousePressEvent(QMouseEvent* event);

private slots:
	void slotRefreshSections();

private:
	QMap<int, QAction*> sections;
};

#endif // MYHEADERVIEW_H
