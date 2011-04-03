#ifndef BARVIEW_H
#define BARVIEW_H

#include "ProgressChartView.h"

class BarChartView : public ProgressChartView
{
public:
	BarChartView(QWidget *parent = 0) : ProgressChartView(parent) {}

protected:
	virtual void drawData(QPainter& painter);

private:
	void drawCube(QPainter& painter, int x, int y, int width, int height, int depth);
};

#endif // BARVIEW_H
