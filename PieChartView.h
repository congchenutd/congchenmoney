#ifndef PIEVIEW_H
#define PIEVIEW_H

#include <QAbstractItemView>

class PieChartView : public QAbstractItemView
{
    Q_OBJECT

public:
    PieChartView(QWidget *parent = 0);

    virtual QRect       visualRect(const QModelIndex &) const;
    virtual void        scrollTo  (const QModelIndex &, ScrollHint hint = EnsureVisible);
    virtual QModelIndex indexAt   (const QPoint &) const;

public slots:
    void dataChanged(const QModelIndex &, const QModelIndex &);

protected:
    virtual int  horizontalOffset() const;
    virtual int  verticalOffset()  const;
    virtual bool isIndexHidden(const QModelIndex &) const;
    virtual void setSelection(const QRect&, QItemSelectionModel::SelectionFlags);
	virtual QRegion visualRegionForSelection(const QItemSelection &) const;
	virtual QModelIndex moveCursor(QAbstractItemView::CursorAction, Qt::KeyboardModifiers);

    void paintEvent (QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);

private:
	int     getValidItems()  const;
	QString getText      (int row);
	double  getDeltaAngle(int row);
	QColor  getColor     (int row);
	void    drawText (QPainter& painter, double start, double delta, const QString&);
	void    drawPie  (QPainter& painter, double start, double delta, const QColor&);
	void    drawPies (QPainter& painter);
	void    drawTexts(QPainter& painter);

private:
	enum {NAME, AMOUNT, PERCENTAGE, Margin = 20};

    int    pieSize;
    int    validItems;
    double totalAmount;
};

#endif
