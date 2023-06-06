#pragma once

#include <QWidget>
#include "graph.h"

struct graph_view : QWidget {
	graph g;

	graph_view(QWidget *parent = 0);
	void paintEvent(QPaintEvent* pe) override;
};
