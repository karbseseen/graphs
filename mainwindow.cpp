#include "mainwindow.h"
#include "qdebug.h"

#include <QMessageBox>
#include <QResizeEvent>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	input_scroll(this),
	ge(this)
{
	input_scroll.setWidget(&input);
	input_scroll.setWidgetResizable(true);
	input_scroll.setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	input_scroll.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	setMinimumSize(input.minimumWidth() + input_scroll.verticalScrollBar()->sizeHint().width(), 400);
	setPalette(QPalette(Qt::white));

	connect(&input.initializer.create, SIGNAL(clicked()), this, SLOT(init()));
	connect(&input.go, SIGNAL(clicked()), this, SLOT(calc()));
	connect(&ge.g, SIGNAL(progress(uint)), this, SLOT(update(uint)));
}

void MainWindow::resizeEvent(QResizeEvent *event) {
	auto size = event->size();
	int w = size.width();
	int h = size.height();
	if (w/2 < minimumWidth()) {
		h /= 2;
		int h2 = size.height() - h;
		ge.resize(w, h2);
		ge.move(0, h);
	} else {
		w = minimumWidth();
		int w2 = size.width() - w;
		ge.resize(w2, h);
		ge.move(w, 0);
	}
	input_scroll.resize(w, h);
}

void MainWindow::init () {
	ge.tr = { 0,0 };
	ge.sc = 1;
	ge.g.fill_random_verts(
		{ 0,0 },
		{ double(ge.width()), double(ge.height()) },
		input.initializer.vert_n.get());
	ge.draw_edges = false;
	ge.sel_id = -1;
	ge.repaint();
}
void MainWindow::calc() {
	if (ge.g.working) {
		ge.g.working = false;
		input.go.setText("Calculate");
		return;
	}

	ge.draw_edges = true;
	auto& g = ge.g;
	switch (input.type) {
	case Input::Nearest:
		if (ge.sel_id == -1)
			g.nearest_neighbours();
		else
			g.nearest_neighbours(&g.verts[ge.sel_id]);
		ge.sel_id = 0;
		update();
		break;
	case Input::Anneal: {
		auto it_num = input.annealing.iters.get();
		input.progr.setMaximum(it_num);
		input.go.setText("Stop");
		ge.sel_id = -1;

		g.simulated_annealing(
			input.annealing.dep.currentIndex(),
			it_num,
			input.annealing.t0.get() * g.temp_norm_k(),
			input.annealing.c.get());
		break; }
	case Input::Ant:
		auto it_num = input.ants.iters.get();
		input.progr.setMaximum(it_num);
		input.go.setText("Stop");
		ge.sel_id = -1;
		ge.edges = new graph::edge_t[g.verts.size() * g.verts.size()];

		g.ant_colony(
			input.ants.a.get(),
			input.ants.b.get(),
			input.ants.p.get(),
			input.ants.ants.get(),
			it_num,
			ge.edges);
		break;
	}
}
void MainWindow::update(uint p) {
	auto len = 0.0;
	ge.g.for_edges([&](vec<2> v1, vec<2> v2) {
		len += (v1 - v2).length();
	});
	input.score.setText("Path length: " + QString::number(len));
	input.progr.setValue(p);
	if (p == input.progr.maximum()) {
		input.go.setText("Calculate");
		if (ge.edges) {
			delete[] ge.edges;
			ge.edges = 0;
		}
	}
	ge.repaint();
}

