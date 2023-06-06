#pragma once
#include <cstdio>
#include <initializer_list>
#include <cstdint>
#include <cmath>


namespace sequence {

template <class Func>
auto golden_section(Func f0, double x1, double x2, double y, double e = 1e-8) {
	e *= y;
	auto f = [&](double x){ return std::abs(f0(x) - y); };
	const double resphi = 2 - (1 + sqrt(5)) / 2;
	auto d = resphi * (x2 - x1);
	double l = x1;
	double r = x2;
	x1 = l + d;
	x2 = r - d;
	double f1 = f(x1);
	double f2 = f(x2);
	while (true)
		if (f1 < f2) {
			r = x2;
			x2 = x1;
			f2 = f1;
			x1 = l + resphi * (r - l);
			f1 = f(x1);
			if (f1 < e) break;
		} else {
			l = x1;
			x1 = x2;
			f1 = f2;
			x2 = r - resphi * (r - l);
			f2 = f(x2);
			if (f2 < e) break;
		}
	return (x1 + x2) / 2;
}

template <typename T> auto sqr(T x) { return x * x; }


template <class T, class Body>
void for_each(T interpolator, uint it_num, Body body) {
	for (uint i = 0; i < it_num; i++)
		body(i, interpolator());
}

class lin_add {
	double t;
	double dt;
public:
	inline lin_add(double t0, double it_num) :
		t(t0), dt(-t0 / it_num) {}
	inline auto operator()() {
		auto old = t;
		t += dt;
		return old;
	}
};
class quad_add {
	double k;
	double i;
public:
	inline quad_add(double t0, double it_num) :
		k(t0 / sqr(it_num)), i(it_num) {}
	inline auto operator()() {
		return k * sqr(i--);
	}
};
class exp_mult {
	double t;
	double a;
public:
	inline exp_mult(double t0, double it_num, double c) :
		t(t0)
	{
		auto f = [=](double x) { return (pow(x, it_num) - 1) / log(x); };
		a = golden_section(f, 0, 1, c * it_num);
		printf("exp_mult: a = %.18f, c = %.18f\n", a, f(a) / it_num);
	}
	inline auto operator()() {
		auto old = t;
		t *= a;
		return old;
	}
};
class log_mult {
	double t0;
	double a;
	double k;
public:
	inline log_mult(double t0, double it_num, double c) :
		t0(t0)
	{
		auto ei1 = std::expint(1);
		auto f = [=](double x)
			 { return (std::expint(1 + log(1 + x * it_num)) - ei1) / (x * M_E); };
		a = golden_section(f, 0, 100, c * it_num);
		k = -a;
		printf("log_mult: a = %.18f, c = %.18f\n", a, f(a) / it_num);
	}
	inline auto operator()()
		{ return t0 / (1 + log(1 + (k += a))); }
};
class lin_mult {
	double t0;
	double a;
	double k;
public:
	inline lin_mult(double t0, double it_num, double c) :
		t0(t0), k(0)
	{
		auto f = [=](double x) { return log(x * it_num + 1) / x; };
		a = golden_section(f, 0, 100, c * it_num);
		printf("lin_mult: a = %.18f, c = %.18f\n", a, f(a) / it_num);
	}
	inline auto operator()()
		{ return t0 / (1 + a * k++); }
};
class quad_mult {
	double t0;
	double a;
	double k;
public:
	inline quad_mult(double t0, double it_num, double c) :
		t0(t0), k(0)
	{
		auto f = [=](double x) {
			x = sqrt(x);
			return atan(x * it_num) / x;
		};
		a = golden_section(f, 0, 100, c * it_num, 1e-20);
		printf("quad_mult: a = %.18f, c = %.18f\n", a, f(a) / it_num);
	}
	inline auto operator()()
		{ return t0 / (1 + a * sqr(k++)); }
};

}
