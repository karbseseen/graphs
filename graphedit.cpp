#include "graphedit.h"
#include <QMouseEvent>
#include <QPainter>
#include <QString>
#include <cmath>
#include <QDebug>

auto length2(QPointF p) {
	return p.x() * p.x() + p.y() * p.y();
}
auto length(QPointF p) {
	return sqrt(length2(p));
}
QPointF conv(vec<2> v) {
	return { v[0],v[1] };
};
vec<2> conv(QPointF p) {
	return { p.x(),p.y() };
};
/*QPointF GraphEdit::get_coord(QMouseEvent* me) {
	return (me->pos() - tr) / sc;
}*/

void GraphEdit::resizeEvent(QResizeEvent *re) {
	auto d = re->size() - re->oldSize();
	tr += { d.width()*0.5, d.height()*0.5 };
}

GraphEdit::GraphEdit(QWidget* parent) :
	QWidget{parent},
	sel_id(-1),
	draw_edges(false),
	edges(0),
	max_w(INFINITY),
	tr { 0,0 },
	sc(1)
{
	float a = 0;
	for (int i = 5; i; i--) {
		a -= M_PI * 2 / 5;
		g.verts.push_back({ cosf(a)*150, sinf(a)*150 });
	}
	g.verts.push_back({ 0,0 });
}

constexpr double VertR = 4;
constexpr double VertSelR = 20;
constexpr uint VertC = 0xff202020;
constexpr uint VertSelC = 0xff18ff18;
void GraphEdit::paintEvent(QPaintEvent* pe) {
	QPainter p(this);
	if(x())	p.drawLine(0,0, 0,height());
	else	p.drawLine(0,0, width(),0);

	p.setRenderHint(QPainter::Antialiasing);
	QBrush br(VertC, Qt::BrushStyle::SolidPattern);
	p.setBrush(br);

	if (draw_edges) {
		if (edges) {
			auto pen = p.pen();
			auto e_ptr = edges;
			auto add = 0ul;
			auto new_max = 0.0;
			const auto k = 6 / max_w;
			for (auto& ref1 : g.verts) {
				auto ptr1 = &ref1;
				auto v1 = ref1 * sc + tr;
				e_ptr += ++add;
				for (auto ptr2 = ptr1; ++ptr2 < &*g.verts.end();) {
					new_max = std::max(new_max, e_ptr->pheromone);
					auto w = e_ptr->pheromone * k;
					e_ptr++;
					if (w < 0.15) continue;
					pen.setWidthF(std::min(w, 6.0));
					p.setPen(pen);
					auto v2 = *ptr2 * sc + tr;
					p.drawLine(conv(v1), conv(v2));
				}
			}
			max_w = new_max;
		} else {
			auto v0 = g.verts.back() * sc + tr;
			for (auto v1 : g.verts) {
				v1 = v1 * sc + tr;
				p.drawLine(conv(v0), conv(v1));
				v0 = v1;
			}
		}
	}

	p.setPen(Qt::NoPen);
	int i = -1;
	for (auto& v : g.verts) {
		if (++i == sel_id)
			p.setBrush(QBrush(VertSelC, Qt::BrushStyle::SolidPattern));
		p.drawEllipse(conv(v * sc + tr), VertR, VertR);
		if (i == sel_id) p.setBrush(br);
	}
}

void GraphEdit::mousePressEvent(QMouseEvent* me) {
	pos = conv(me->pos()) - tr;
	moved = false;
}
void GraphEdit::mouseMoveEvent(QMouseEvent* me) {
	moved = true;
	tr = conv(me->pos()) - pos;
	repaint();
}
void GraphEdit::mouseReleaseEvent(QMouseEvent* me) {
	if (moved) return;

	auto p = pos / sc;
	std::pair min_v { double(INFINITY), (vec<2>*)0 };
	min_v = std::accumulate(g.verts.begin(), g.verts.end(), min_v, [p](decltype(min_v) init, vec<2>& v) {
		auto len = (p - v).length2();
		return init.first < len ? init : std::pair { len, &v };
	});

	auto r2 = VertSelR / sc;
	r2 *= r2;
	vec<2>* ptr = min_v.first <= r2 ? min_v.second : 0;
	int id = ptr ? ptr - g.verts.data() : -1;

	switch (me->button()) {
	case Qt::LeftButton:
		if (ptr) g.remove(ptr);
		else g.verts.push_back(p);
		if (sel_id == id) sel_id = -1;
		else if (sel_id == g.verts.size()) sel_id = id;
		draw_edges = false;
		break;
	case Qt::RightButton:
		sel_id = ((sel_id == id) ? -1 : id);
		break;
	default:
		return;
	}
	repaint();
}
void GraphEdit::wheelEvent(QWheelEvent* we) {
	auto d = we->angleDelta().y();
	if (!d) return;
	auto dsc = pow(1.002, d);
	sc *= dsc;
	tr = tr * dsc + conv(we->position()) * (1 - dsc);
	repaint();
}
