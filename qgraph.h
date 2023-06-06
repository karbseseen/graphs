#pragma once
#include "graph.h"
#include <QThread>


struct QGraph : QObject, graph {
	Q_OBJECT
	QThread thr;

public slots:
	void sa_slot(uint dep, uint it_num, double t0, double c);
	void ac_slot(double a, double b, double p, uint ant_num, uint it_num, edge_t* edges);
signals:
	void sa_signal(uint dep, uint it_num, double t0, double c);
	void ac_signal(double a, double b, double p, uint ant_num, uint it_num, edge_t* edges);

public:
	bool working;

	QGraph();
	~QGraph();
	inline void simulated_annealing(uint dep, uint it_num, double t0, double c = 0) {
		working = true;
		emit sa_signal(dep, it_num, t0, c);
	}
	inline void ant_colony(double a, double b, double p, uint ant_num, uint it_num, edge_t* edges = 0) {
		working = true;
		emit ac_signal(a, b, p, ant_num, it_num, edges);
	}

signals:
	void progress(uint p);
};
