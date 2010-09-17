#define _USE_MATH_DEFINES
#include <cmath>
#include <QScrollBar>
#include <QFontMetrics>
#include <QPainter>
#include <QPaintEvent>
#include "PieView.h"
#include "PieModel.h"

PieView::PieView(QWidget *parent)
    : QAbstractItemView(parent) {}

int PieView::getValidItems() const
{
	int result = 0;
	for(int row = 0; row < model()->rowCount(); ++row)
		if(model()->data(model()->index(row, AMOUNT)).toDouble() > 0.0)
			result++;
	return result;
}

void PieView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	QAbstractItemView::dataChanged(topLeft, bottomRight);
	validItems  = getValidItems();
	totalAmount = static_cast<PieModel*>(model())->getTotalAmount();
	viewport()->update();
}

QString PieView::getText(int row)
{
	return tr("%1(%3)").arg(model()->data(model()->index(row, NAME)).toString())
					   .arg(model()->data(model()->index(row, PERCENTAGE)).toString());
}

double PieView::getDeltaAngle(int row)
{
	const double thisRowAmount = model()->data(model()->index(row, AMOUNT)).toDouble();
	return 360 * thisRowAmount / totalAmount;
}

QColor PieView::getColor(int row)
{
	return QColor(
		model()->data(model()->index(row, NAME), Qt::DecorationRole).toString());
}

void PieView::drawText(QPainter& painter, double startAngle, 
					   double deltaAngle, const QString& text)
{
	if(deltaAngle > 0.0)
	{
		int width = QFontMetrics(painter.font()).width(text);
		int r = pieSize / 3;
		int x = r*cos((startAngle+deltaAngle/2)*M_PI/180);
		int y = r*sin((startAngle+deltaAngle/2)*M_PI/180);
		painter.drawText(pieSize/2 + x - width/2, pieSize/2 - y, text);
	}
}

void PieView::drawTexts(QPainter& painter)
{
	double startAngle = 0.0;
	for(int row=0; row<model()->rowCount(); startAngle+=getDeltaAngle(row), ++row)
		drawText(painter, startAngle, getDeltaAngle(row), getText(row));
}

void PieView::drawPie(QPainter& painter, double startAngle, 
					  double deltaAngle, const QColor& color)
{
	if(deltaAngle > 0)
	{
		painter.setBrush(color);
		painter.drawPie(0, 0, pieSize, pieSize, startAngle*16, deltaAngle*16);
	}
}

void PieView::drawPies(QPainter& painter)
{
	double startAngle = 0.0;
	for(int row=0; row<model()->rowCount(); startAngle+=getDeltaAngle(row), ++row)
		drawPie(painter, startAngle, getDeltaAngle(row), getColor(row));
}

void PieView::paintEvent(QPaintEvent*)
{
	if(validItems > 0)
	{
		QPainter painter(viewport());
		painter.setRenderHint(QPainter::Antialiasing);
		painter.translate(Margin, Margin);
		painter.drawEllipse(0, 0, pieSize, pieSize);
		drawPies (painter);
		drawTexts(painter);
	}
}

void PieView::resizeEvent(QResizeEvent*)
{
	int totalSize = qMin(viewport()->width(), viewport()->height());
	pieSize = totalSize - 2 * Margin;
}


// inherited
QModelIndex PieView::indexAt(const QPoint&) const {
    return QModelIndex();
}

bool PieView::isIndexHidden(const QModelIndex&) const {
    return false;
}

int PieView::horizontalOffset() const {
    return horizontalScrollBar()->value();
}

QModelIndex PieView::moveCursor(QAbstractItemView::CursorAction, Qt::KeyboardModifiers)
{
    viewport()->update();
    return currentIndex();
}

void PieView::scrollTo(const QModelIndex&, ScrollHint) {
    update();
}

void PieView::setSelection(const QRect&, QItemSelectionModel::SelectionFlags) {
    update();
}

int PieView::verticalOffset() const {
    return verticalScrollBar()->value();
}

QRegion PieView::visualRegionForSelection(const QItemSelection&) const {
	return QRegion();
}

QRect PieView::visualRect(const QModelIndex&) const {
	return QRect();
}