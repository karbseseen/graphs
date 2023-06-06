#pragma once
#include "qgraph.h"
#include <QWidget>


struct GraphEdit : QWidget {
	QGraph g;
	int sel_id;
	bool moved;
	bool draw_edges;
	graph::edge_t* edges;
	double max_w;

	vec<2> tr;
	double sc;
	vec<2> pos;

	GraphEdit(QWidget *parent = 0);
	//QPointF get_coord(QMouseEvent* me);

	void resizeEvent(QResizeEvent* re) override;
	void paintEvent(QPaintEvent* pe) override;
	void mousePressEvent(QMouseEvent* me) override;
	void mouseMoveEvent(QMouseEvent* me) override;
	void mouseReleaseEvent(QMouseEvent* me) override;
	void wheelEvent(QWheelEvent* we) override;
};

