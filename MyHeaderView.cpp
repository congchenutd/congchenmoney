#include "MyHeaderView.h"
#include <QAbstractItemModel>
#include <QMouseEvent>
#include <QTableView>
#include <QMenu>

MyHeaderView::MyHeaderView(Qt::Orientation orientation, QWidget *parent)
	: QHeaderView(orientation, parent)
{
	setClickable(true);
}

void MyHeaderView::mousePressEvent(QMouseEvent* event)
{
	if(event->button() == Qt::RightButton)
	{
		QTableView* view = static_cast<QTableView*>(parent());
		QAbstractItemModel* model = view->model();
		sections.clear();
		QMenu menu(this);
		for(int section=0; section<model->columnCount(); ++section)
		{
			const QString secName = model->headerData(section, Qt::Horizontal).toString();
			QAction* action = new QAction(secName, &menu);
			action->setCheckable(true);
			action->setChecked(!view->isColumnHidden(section));
			sections.insert(section, action);
			connect(action, SIGNAL(triggered()), this, SLOT(slotRefreshSections()));
			menu.addAction(action);
		}	
		menu.exec(event->globalPos());
	}
	return QHeaderView::mousePressEvent(event);
}

void MyHeaderView::slotRefreshSections()
{
	QMap<int, QAction*>::const_iterator it = sections.constBegin();
	for(; it != sections.constEnd(); ++it) 
		setSectionHidden(it.key(), !it.value()->isChecked());
}