#include <QPainter>
#include <QApplication>
#include "ProgressChartView.h"

ProgressChartView::ProgressChartView(QWidget *parent)
	: QAbstractItemView(parent)
{
	deltax     = 0;
	deltay     = 0;
	pointCount = 0;
	horizontalHeight = 0;
	maxAmount   = 0.0;
	minAmount   = 0.0;
	deltaAmount = 1.0;
}

void ProgressChartView::resizeEvent(QResizeEvent*) {
	updateGeometry();
}

void ProgressChartView::paintEvent(QPaintEvent*)
{
	QPainter painter(viewport());
	updateFontMetrics();
	drawBack(painter);
	drawLeft(painter);
	drawData(painter);
}

void ProgressChartView::drawBack(QPainter& painter)
{
	// panel
	painter.setPen(Qt::black);
	painter.setBrush(Qt::lightGray);
	painter.drawRect(chartRect.translated(BackGoundDepth, -BackGoundDepth));

	// grid lines
	painter.setPen(QPen(Qt::DashLine));
	double y = 0.0;
	double amount = 0.0;
	while(amount <= maxAmount)
	{
		painter.drawLine(chartRect.left()   + BackGoundDepth, 
						 chartRect.bottom() - BackGoundDepth - horizontalHeight - y,
						 chartRect.right()  + BackGoundDepth, 
						 chartRect.bottom() - BackGoundDepth - horizontalHeight - y);
		y += deltay;
		amount += deltaAmount;
	}
	y = 0.0;
	amount = 0.0;
	while(amount > minAmount)
	{
		painter.drawLine(chartRect.left()   + BackGoundDepth, 
						 chartRect.bottom() - BackGoundDepth - horizontalHeight - y,
						 chartRect.right()  + BackGoundDepth, 
						 chartRect.bottom() - BackGoundDepth - horizontalHeight - y);
		y -= deltay;
		amount -= deltaAmount;
	}
}

void ProgressChartView::drawLeft(QPainter& painter)
{
	// panel
	painter.setPen(Qt::black);
	painter.setBrush(Qt::gray);
	QPolygon left;
	left << QPoint(chartRect.left(), chartRect.bottom()) 
		<< QPoint(chartRect.left(), chartRect.top())
		<< QPoint(chartRect.left()+BackGoundDepth, chartRect.top()-BackGoundDepth)
		<< QPoint(chartRect.left()+BackGoundDepth, chartRect.bottom()-BackGoundDepth);
	painter.drawPolygon(left);

	// grid lines and marks
	painter.setPen(QPen(Qt::DashLine));
	double y = 0.0;
	double amount = 0.0;
	while(amount <= maxAmount)
	{
		painter.drawLine(chartRect.left(),				  
						 chartRect.bottom() - horizontalHeight - y,
						 chartRect.left()   + BackGoundDepth, 
						 chartRect.bottom() - BackGoundDepth - horizontalHeight - y);
		QRect rect(chartRect.left()-fontWidth-10, 
				   chartRect.bottom()-horizontalHeight-y-fontHeight/2, 
				   fontWidth, fontHeight);
		painter.drawText(rect, Qt::AlignRight, QString::number((int)(amount)));
		y += deltay;
		amount += deltaAmount;
	}
	y = 0.0;
	amount = 0.0;
	while(amount >= minAmount)
	{
		painter.drawLine(chartRect.left(),				  
						 chartRect.bottom() - horizontalHeight - y,
						 chartRect.left()   + BackGoundDepth, 
						 chartRect.bottom() - BackGoundDepth - horizontalHeight - y);
		QRect rect(chartRect.left()-fontWidth-10, 
				   chartRect.bottom()-horizontalHeight-y-fontHeight/2, 
				   fontWidth, fontHeight);
		painter.drawText(rect, Qt::AlignRight, QString::number((int)(amount)));
		y -= deltay;
		amount -= deltaAmount;
	}

	// y-axis
	painter.setPen(QPen(QBrush(Qt::black), 3));    
	painter.drawLine(chartRect.topLeft(), chartRect.bottomLeft());
}

void ProgressChartView::drawGround(QPainter& painter)
{
	// panel
	painter.setPen(Qt::black);
	painter.setBrush(Qt::darkGray);
	QPolygon bottom;
	bottom << QPoint(chartRect.left(), chartRect.bottom()) 
		   << QPoint(chartRect.left() +BackGoundDepth, chartRect.bottom()-BackGoundDepth)
		   << QPoint(chartRect.right()+BackGoundDepth, chartRect.bottom()-BackGoundDepth)
		   << QPoint(chartRect.right(), chartRect.bottom());
	bottom.translate(0, -horizontalHeight);
	painter.drawPolygon(bottom);
  
	// x-axis
	painter.setPen(QPen(QBrush(Qt::black), 3));
	painter.drawLine(chartRect.bottomLeft()  - QPoint(0, horizontalHeight), 
					 chartRect.bottomRight() - QPoint(0, horizontalHeight));

	// grid lines
	painter.setPen(QPen(Qt::DashLine));
	for(int i=0; i<pointCount; i++)
		painter.drawLine(chartRect.left() + i*deltax, 
						 chartRect.bottom() - horizontalHeight,
						 chartRect.left() + i*deltax + BackGoundDepth, 
						 chartRect.bottom() - BackGoundDepth - horizontalHeight);

	// marks
	painter.setPen(QPen(Qt::black, 3));
	for(int i=0; i<pointCount; i++)
	{
		painter.drawLine(chartRect.left() + i*deltax, chartRect.bottom() - horizontalHeight,
						 chartRect.left() + i*deltax, chartRect.bottom() - horizontalHeight - 5);
		painter.drawText(chartRect.left() + i*deltax + (deltax-fontWidth)/2, 
						 chartRect.bottom() + fontHeight - horizontalHeight,
						 model()->data(model()->index(i, 0)).toString());
	}
}

void ProgressChartView::updateFontMetrics()
{
	QFontMetrics fm(QApplication::font());
	fontWidth  = qMax(fm.width(QString::number(maxAmount)), fm.width("07-00"));
	fontHeight = fm.height();
}

void ProgressChartView::updateGeometry()
{
	const int leftMargin   = 70;
	const int rightMargin  = 60;
	const int topMargin    = 60;
	const int bottomMargin = 35;
	double h = viewport()->height() - topMargin  - bottomMargin;
	int height = h/10 * 10;   // has factor 10
	chartRect = QRect(leftMargin, topMargin,
					  viewport()->width()  - leftMargin - rightMargin,
					  height);
	deltax = (pointCount <= 1) ? chartRect.width() : chartRect.width()/pointCount;
	deltay = chartRect.height() / 10;

	if(minAmount < 0)
		horizontalHeight = chartRect.height() * (-minAmount)/(maxAmount-minAmount);
	else
		horizontalHeight = 0;
}

void ProgressChartView::updatePointCount() {
	pointCount = model()->rowCount();
}

double ProgressChartView::getMaxAmount() const
{
	double result = 0.0;
	for(int row=0; row<pointCount; row++)
	{
		double amount = model()->data(model()->index(row, 1)).toDouble();
		if(amount > result)
			result = amount;
	}
	return (result < 10) ? 10 : result;
}

double ProgressChartView::getMinAmount() const
{
	double result = 0.0;
	for(int row=0; row<pointCount; row++)
	{
		double amount = model()->data(model()->index(row, 1)).toDouble();
		if(amount < result)
			result = amount;
	}
	if(result >= 0.0)
		return 0.0;
	if(result > -10.0)
		return -10.0;
	return result;
}

void ProgressChartView::updateAmount()
{
	maxAmount = getMaxAmount();
	minAmount = getMinAmount();
	deltaAmount = (maxAmount-minAmount) / 10;
	if(deltaAmount == 0.0)
		deltaAmount = 1.0;
}

void ProgressChartView::update()
{
	updatePointCount();
	updateAmount();
	updateGeometry();
	viewport()->update();    // call paintEvent()
}

// slots
void ProgressChartView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	QAbstractItemView::dataChanged(topLeft, bottomRight);
	update();
}