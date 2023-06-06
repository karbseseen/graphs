#pragma once
#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>

template <uint Dim> class vec : public vec<Dim - 1> {
	double last;

public:
	inline auto begin() const { return (double*)this; }
	inline auto end() const { return begin() + Dim; }
	inline auto& operator[](size_t id) { return begin()[id]; }

private:
	template <class Func>
	inline auto& op_inl(vec<Dim>& b, Func func) {
		std::transform(begin(), end(), b.begin(), begin(), func);
		return *this;
	}
	template <class Func>
	inline auto op(vec<Dim>& b, Func func) const {
		vec<Dim> ret;
		std::transform(begin(), end(), b.begin(), ret.begin(), func);
		return ret;
	}

	template <class Func>
	inline auto& op_inl(double b, Func func) {
		for (auto& v : *this)
			v = func(v, b);
		return *this;
	}
	template <class Func>
	inline auto op(double b, Func func) const {
		vec<Dim> ret;
		std::transform(begin(), end(), ret.begin(),
			[&](double v) { return func(v, b); });
		return ret;
	}

public:
	inline vec() {}
	inline vec(std::initializer_list<double> list) {
		auto ptr = begin();
		for (auto value : list)
			*ptr++ = value;
	}

	inline bool operator==(vec<Dim> b) const {
		auto ptr = b.begin();
		for (auto v : *this)
			if (v != *ptr++)
				return false;
		return true;
	}
	inline bool operator!=(vec<Dim> b) const {
		auto ptr = b.begin();
		for (auto v : *this)
			if (v == *ptr++)
				return false;
		return true;
	}

	inline auto operator+(vec<Dim> b) const { return op(b, std::plus<double>()); }
	inline auto operator-(vec<Dim> b) const { return op(b, std::minus<double>()); }
	inline auto operator*(vec<Dim> b) const { return op(b, std::multiplies<double>()); }
	inline auto operator/(vec<Dim> b) const { return op(b, std::divides<double>()); }
	inline auto operator*(double b) const { return op(b, std::multiplies<double>()); }
	inline auto operator/(double b) const { return op(b, std::divides<double>()); }

	inline auto& operator+=(vec<Dim> b) { return op_inl(b, std::plus<double>()); }
	inline auto& operator-=(vec<Dim> b) { return op_inl(b, std::minus<double>()); }
	inline auto& operator*=(vec<Dim> b) { return op_inl(b, std::multiplies<double>()); }
	inline auto& operator/=(vec<Dim> b) { return op_inl(b, std::divides<double>()); }
	inline auto& operator*=(double b) { return op_inl(b, std::multiplies<double>()); }
	inline auto& operator/=(double b) { return op_inl(b, std::divides<double>()); }

	inline auto length2() const {
		return std::accumulate(begin(), end(), 0.0, [](double x, double v) { return x + v*v; });
	}
	inline auto length() const { return sqrt(length2()); }
	inline auto norm() const { return *this / length(); }
};
template <> class vec<0> {};
