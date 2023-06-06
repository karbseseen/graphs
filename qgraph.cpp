#include "qgraph.h"


QGraph::QGraph() {
	working = false;
	moveToThread(&thr);
	connect(this, &QGraph::sa_signal, this, &QGraph::sa_slot);
	connect(this, &QGraph::ac_signal, this, &QGraph::ac_slot);
	thr.start();
}
QGraph::~QGraph() {
	working = false;
	thr.quit();
	thr.wait();
}

void QGraph::sa_slot(uint dep, uint it_num, double t0, double c) {
	graph g = *this;
	auto callback = [&, this](uint p) {
		if ((it_num - p) & ((1 << 20) - 1)) return true;
		verts = g.verts;
		emit progress(p);
		return working;
	};
	switch(dep) {
	case 0:
		g.simulated_annealing(sequence::lin_add(t0, it_num), it_num, callback);
		break;
	case 1:
		g.simulated_annealing(sequence::quad_add(t0, it_num), it_num, callback);
		break;
	case 2:
		g.simulated_annealing(sequence::exp_mult(t0, it_num, c), it_num, callback);
		break;
	case 3:
		g.simulated_annealing(sequence::log_mult(t0, it_num, c), it_num, callback);
		break;
	case 4:
		g.simulated_annealing(sequence::lin_mult(t0, it_num, c), it_num, callback);
		break;
	case 5:
		g.simulated_annealing(sequence::quad_mult(t0, it_num, c), it_num, callback);
		break;
	}
	working = false;
	dre = g.dre;
}
void QGraph::ac_slot(double a, double b, double p, uint ant_num, uint it_num, edge_t* edges) {
	graph g = *this;
	auto progr = [this, &g](uint p, uint* path) {
		emit progress(p);
		return working;
	};
	if (edges)	g.ant_colony(a, b, p, ant_num, it_num, edges, progr);
	else		g.ant_colony(a, b, p, ant_num, it_num, progr);
	working = false;
	verts = g.verts;
	dre = g.dre;
}
