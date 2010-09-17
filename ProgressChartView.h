#ifndef PROGRESSCHARTVIEW_H
#define PROGRESSCHARTVIEW_H

#include <QAbstractItemView>
#include <QScrollBar>

class ProgressChartView : public QAbstractItemView
{
	Q_OBJECT

public:
	ProgressChartView(QWidget *parent=0);
	void update();

	// inherited virtual
	QRect       visualRect(const QModelIndex &) const    { return QRect();       }
	void        scrollTo(const QModelIndex&, ScrollHint) { update();             }
	QModelIndex indexAt(const QPoint &) const            { return QModelIndex(); }

protected:   // inherited virtual
	int horizontalOffset() const { return horizontalScrollBar()->value(); }
	int verticalOffset()   const { return verticalScrollBar()->value();   }
	bool isIndexHidden(const QModelIndex &) const { return false; }
	void setSelection(const QRect&, QItemSelectionModel::SelectionFlags) { update(); }
	QRegion visualRegionForSelection(const QItemSelection &) const { return QRegion(); }
	QModelIndex moveCursor(CursorAction, Qt::KeyboardModifiers) { return currentIndex(); }

	void updateGeometry();
	virtual void paintEvent (QPaintEvent*  event);
	virtual void resizeEvent(QResizeEvent* event);

protected slots:
	void dataChanged(const QModelIndex&, const QModelIndex&);

protected:
	void   updateFontMetrics();
	void   updatePointCount();
	void   updateAmount();
	double getMaxAmount() const;
	double getMinAmount() const;
	virtual void drawBack  (QPainter& painter);
	virtual void drawLeft  (QPainter& painter);
	virtual void drawGround(QPainter& painter);
	virtual void drawData  (QPainter& painter) = 0;

protected:
	enum {BackGoundDepth = 40, BarDepth = 20};

	QRect  chartRect;
	double deltax;
	double deltay;
	int    pointCount;
	double maxAmount;
	double minAmount;
	double deltaAmount;
	int    fontWidth;
	int    fontHeight;
	int    horizontalHeight;
};

#endif // PROGRESSCHARTVIEW_H
