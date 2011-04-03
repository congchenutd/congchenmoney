#include <QPainter>
#include <QScrollBar>
#include <QFontMetrics>
#include <QFont>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QPaintEvent>
#include <deque>
#include "LineChartView.h"

void LineChartView::drawData(QPainter& painter)
{
	// get points
	std::deque<QPoint> points;
	for(int i=0; i<pointCount; i++)
	{
		int expense = model()->data(model()->index(i, 1)).toInt();
		int x = chartRect.left() + i * deltax + deltax/2 + BackGoundDepth/2;
		int y = chartRect.bottom() - horizontalHeight - BackGoundDepth/2
								   - expense/(maxAmount-minAmount)*chartRect.height();
		points.push_back(QPoint(x, y));
	}

	// draw lines
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(QPen(QBrush(Qt::blue), 3));
	std::deque<QPoint> remainedPoints;
	for(int i=0; i<pointCount-1; ++i)
	{
		QPoint start = points[i];
		QPoint end   = points[i+1];
		if(sameSide(start, end))
			painter.drawLine(start, end);
		else
		{
			painter.drawLine(lower(start, end), jointPoint(start, end));
			remainedPoints.push_back(upper(start, end));
			remainedPoints.push_back(jointPoint(start, end));
		}
	}

	// ground
	painter.setRenderHint(QPainter::Antialiasing, false);
	drawGround(painter);

	// hidden lines
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(QPen(QBrush(Qt::blue), 3));
	while(!remainedPoints.empty())
	{
		QPoint start = remainedPoints.front();
		remainedPoints.pop_front();
		QPoint end   = remainedPoints.front();
		remainedPoints.pop_front();
		painter.drawLine(start, end);
	}

	// draw points
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setPen(QPen(Qt::DashLine));         // drop line
	for(int i=0; i<pointCount; ++i)
		painter.drawLine(points[i].x(), points[i].y(), 
						 points[i].x(), chartRect.bottom() - horizontalHeight - BackGoundDepth/2);

	painter.setPen(QPen(QBrush(Qt::red), 10));  // dots
	for(int i=0; i<pointCount; ++i)
	{
		painter.drawPoint(points[i]);
		double expense = model()->data(model()->index(i, 1)).toDouble();
		painter.drawText(points[i].x()+5, points[i].y()-5, QString::number(expense));
	}
}

bool LineChartView::sameSide(const QPoint& start, const QPoint& end) const
{
	const double highbound = chartRect.bottom() - BackGoundDepth - horizontalHeight;
	const double lowbound  = chartRect.bottom() - horizontalHeight;
	return (start.y() <= highbound && end.y() <= highbound) ||
		   (start.y() >= lowbound  && end.y() >= lowbound);
}

QPoint LineChartView::jointPoint(const QPoint& start, const QPoint& end) const
{
	const double horizontal = chartRect.bottom() - BackGoundDepth/2 - horizontalHeight;
	double dy = horizontal - start.y();
	double dx = ((double)end.x()-start.x()) / ((double)end.y()-start.y()) * dy;
	if(dx > end.x()-start.x())
		return abs(start.y()-horizontal) < abs(end.y()-horizontal) ? start : end;
	return QPoint(start.x() + dx, horizontal);
}

QPoint LineChartView::upper(const QPoint& start, const QPoint& end) const {
	return start.y() < end.y() ? start : end;
}

QPoint LineChartView::lower(const QPoint& start, const QPoint& end) const {
	return start.y() > end.y() ? start : end;
}