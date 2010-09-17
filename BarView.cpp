#include <QPainter>
#include <QRect>
#include <QPaintEvent>
#include "barview.h"

void BarView::drawCube(QPainter& painter, int x, int y, int width, int height, int depth)
{
	QColor color = painter.brush().color();
	QPolygon top;
	top << QPoint(x, y) << QPoint(x+width, y) 
		<< QPoint(x+width+depth, y-depth) << QPoint(x+depth, y-depth);
	painter.setBrush(color.light());
	painter.drawPolygon(top);

	painter.setBrush(color);
	painter.drawRect(x, y, width, height);

	QPolygon right;
	right << QPoint(x+width, y) << QPoint(x+width+depth, y-depth) 
		  << QPoint(x+width+depth, y+height-depth) << QPoint(x+width, y+height);
	painter.setBrush(color.dark());
	painter.drawPolygon(right);
}

void BarView::drawData(QPainter& painter)
{
	const QColor colors[] = {
		Qt::red,
		Qt::blue,
		Qt::yellow,
		Qt::green,
		Qt::cyan,
		Qt::magenta,
		Qt::darkYellow,
		Qt::darkRed,
		Qt::darkGreen,
		Qt::darkBlue,
		Qt::darkCyan,
		Qt::darkMagenta,
	};
	painter.setPen(QPen(Qt::black));
	for(int i=0; i<pointCount; i++)
	{
		int amount = model()->data(model()->index(i, 1)).toInt();
		if(amount < 0)
		{
			int x = chartRect.left() + i * deltax;
			int height = amount / (maxAmount-minAmount) * chartRect.height();
			int y = chartRect.bottom() - horizontalHeight - height;
			painter.setBrush(QBrush(colors[i%12]));
			drawCube(painter, x+deltax/4, y-(BackGoundDepth-BarDepth)/2, 
					 2*deltax/3, height, BarDepth);
		}
	}

	drawGround(painter);

	painter.setPen(QPen(Qt::black));
	for(int i=0; i<pointCount; i++)
	{
		int amount = model()->data(model()->index(i, 1)).toInt();
		int x = chartRect.left() + i * deltax;

		// if amount<0, draw it's top only
		int height = (amount >= 0) ? amount / (maxAmount-minAmount) * chartRect.height() : 0;
		int y = chartRect.bottom() - horizontalHeight - height;
		painter.setBrush(QBrush(colors[i%12]));
		drawCube(painter, x+deltax/4, y-(BackGoundDepth-BarDepth)/2, 
				 2*deltax/3, height, BarDepth);
		painter.drawText(x+deltax/2, y-15, QString::number(amount));
	}
}