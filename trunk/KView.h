#ifndef KVIEW_H
#define KVIEW_H

#include "ProgressChartView.h"

class KView : public ProgressChartView
{
public:
	KView(QWidget *parent = 0) : ProgressChartView(parent) {}

protected:
	virtual void drawData(QPainter& painter);

private:
	bool   sameSide  (const QPoint& start, const QPoint& end) const;
	QPoint jointPoint(const QPoint& start, const QPoint& end) const;
	QPoint upper     (const QPoint& start, const QPoint& end) const;
	QPoint lower     (const QPoint& start, const QPoint& end) const;
};

#endif