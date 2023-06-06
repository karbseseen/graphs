#include "widgets.h"

constexpr bool DefaultText = true;

UintEdit::UintEdit(uint min_v, uint max_v, QWidget *parent) :
	QLineEdit(parent),
	min_v(min_v),
	max_v(max_v) {}
uint UintEdit::get() {
	auto l = text().toLongLong();
	l = std::max<long long>(l, min_v);
	l = std::min<long long>(l, max_v);
	setText(QString::number(l));
	return l;
}
DoubleEdit::DoubleEdit(double min_v, double max_v, QWidget *parent) :
	QLineEdit(parent),
	min_v(min_v),
	max_v(max_v) {}
double DoubleEdit::get() {
	auto d = text().toDouble();
	d = std::max(d, min_v);
	d = std::min(d, max_v);
	setText(QString::asprintf("%g", d));
	return d;
}


Methods::Methods(QWidget *parent) :
	QGroupBox("Method", parent),
	layout(this),
	nn("Nearest neighbours"),
	sa("Simulated annealing"),
	ac("Ant colony")
{
	layout.addWidget(&nn);
	layout.addWidget(&sa);
	layout.addWidget(&ac);
	nn.setChecked(true);
}
NewVerts::NewVerts(QWidget *parent) :
	QGroupBox(parent),
	layout(this),
	text("Vertex count:"),
	create("Create random vertexes")
{
	layout.addWidget(&text, 0,0);
	layout.addWidget(&vert_n, 0,1);
	layout.addWidget(&create, 1,0, 1,2);
	if constexpr (DefaultText) vert_n.setText("50");
}

Annealing::Annealing(QWidget *parent) :
	QGroupBox("Parameters", parent),
	layout(this),
	t0(0),
	c(0, 1)
{
	dep.addItems({
		"Linear",
		"Quadratic",
		"Exponential",
		"Logarithmical",
		"Linear multiplicative",
		"Quadratic multiplicative"
	});
	layout.addRow("Cooling:", &dep);
	layout.addRow("T0:", &t0);
	layout.addRow("Convergence rate:", &c);
	layout.addRow("Iteration num:", &iters);
	if constexpr (DefaultText)  {
		t0.setText("0.12");
		c.setText("0.25");
		iters.setText("100000000");
	}
}
Ants::Ants(QWidget *parent) :
	QGroupBox("Parameters", parent),
	layout(this)
{
	layout.addRow("A:", &a);
	layout.addRow("B:", &b);
	layout.addRow("P:", &p);
	layout.addRow("Ant num:", &ants);
	layout.addRow("Iteration num:", &iters);
	if constexpr (DefaultText)  {
		a.setText("1");
		b.setText("1");
		p.setText("0.15");
		ants.setText("20");
		iters.setText("10000");
	}
}

Input::Input(QWidget* parent) :
	QWidget(parent),
	layout(this),
	go("Calculate"),
	type(Nearest)
{
	layout.addWidget(&initializer);
	layout.addWidget(&method);
	layout.addWidget(&go);
	layout.addWidget(&progr);
	layout.addWidget(&score);
	layout.setAlignment(Qt::AlignTop);
	score.setAlignment(Qt::AlignCenter);
	auto w = method.sizeHint().width();
	w = std::max(w, annealing.sizeHint().width());
	w = std::max(w, ants.sizeHint().width());
	setMinimumWidth(w);

	connect(&method.nn, SIGNAL(clicked()), this, SLOT(set_nearest()));
	connect(&method.sa, SIGNAL(clicked()), this, SLOT(set_anneal()));
	connect(&method.ac, SIGNAL(clicked()), this, SLOT(set_ant()));
}
void Input::set_type(Type type_) {
	if (type != Nearest) {
		auto item = layout.takeAt(2);
		item->widget()->setParent(0);
		delete item;
	}
	if (type_ == Anneal) layout.insertWidget(2, &annealing);
	if (type_ == Ant) layout.insertWidget(2, &ants);
	type = type_;
}
void Input::set_nearest() { set_type(Nearest); }
void Input::set_anneal() { set_type(Anneal); }
void Input::set_ant() { set_type(Ant); }




