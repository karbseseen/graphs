#pragma once
#include "sequence.h"
#include "vec.h"
#include <initializer_list>
#include <limits>
#include <vector>
#include <random>
#include <chrono>


class graph {
	struct no_value {};
public:
	struct edge_t {
		double len;
		double pheromone;
		double count;
	};

	std::vector<vec<2>> verts;
	std::mt19937 dre;

	inline vec<2> random_vec()
		{ return { double(dre()), double(dre()) }; }

	inline graph() :
		dre(std::chrono::high_resolution_clock::now().time_since_epoch().count()) {}

	inline void remove(vec<2>* vert) {
		std::swap(*vert, verts.back());
		verts.pop_back();
	}

	inline void fill_random_verts(vec<2> min_p, vec<2> max_p, uint num) {
		auto range = (max_p - min_p) / dre.max();
		verts.clear();
		while (num--)
			verts.push_back(min_p + range * random_vec());
	}

	template <class Callback>
	void for_edges(Callback callback) {
		auto ptr0 = &verts.back();
		for (auto& v1 : verts) {
			callback(*ptr0, v1);
			ptr0 = &v1;
		}
	}
	auto path_length() {
		auto len = 0.0;
		for_edges([&](vec<2> a, vec<2> b) { len += (a - b).length(); });
		return len;
	}
	auto temp_norm_k() {
		auto c = std::accumulate(verts.begin(), verts.end(), vec<2> { 0,0 });
		c /= verts.size();
		auto c_dist = std::accumulate(verts.begin(), verts.end(), 0.0,
			[c](double init, vec<2> v) { return init + (v - c).length(); });
		c_dist /= verts.size();
		return c_dist;
	}

	static constexpr auto Inf = std::numeric_limits<double>::infinity();
	inline auto nearest_neighbours(vec<2>* first) {
		auto len = 0.0;
		auto begin = verts.data(), end = begin + verts.size();
		while (true) {
			std::swap(*begin, *first);
			auto current = *begin;
			if (++begin == end) break;
			auto p = std::accumulate(begin, end, std::pair((vec<2>*)0, Inf), [current](std::pair<vec<2>*, double> init, vec<2>& e) {
				auto p = std::pair(&e, (e - current).length2());
				return (init.second < p.second) ? init : p;
			});
			first = p.first;
			len += sqrt(p.second);
		}
		return len + (verts.front() - verts.back()).length();
	}
	inline auto nearest_neighbours() {
		auto verts2 = verts;
		auto min_len = Inf;
		auto min_i = 0;
		for (uint i = 0; i < verts.size(); i++) {
			auto len = nearest_neighbours(&verts[i]);
			if (min_len > len) {
				min_len = len;
				min_i = i;
			}
			verts = verts2;
		}
		return nearest_neighbours(&verts[min_i]);
	}

	template <class Seq, class progr_t = no_value>
	inline void simulated_annealing(
		Seq seq, uint64_t it_num, progr_t progr = {}
	) {
		constexpr bool Progr = !std::is_same_v<progr_t, no_value>;

		const auto r_k = 1.0 / dre.max();
		const auto size = verts.size();
		const auto size_1 = verts.size() - 1;
		const auto size_3 = verts.size() - 3;
		const auto size_2 = verts.size() / 2;
		for (uint i = 0; i < it_num;) {
			auto id1 = dre() % size;
			auto id2 = id1 + 2 + dre() % size_3;
			if (id2 >= size)
				id2 -= size;
			auto id1prev = id1 ? id1-1 : size_1;
			auto id2prev = id2 ? id2-1 : size_1;

			auto ds =
				+ (verts[id1prev] - verts[id2prev]).length()
				- (verts[id1prev] - verts[id1]).length()
				+ (verts[id1] - verts[id2]).length()
				- (verts[id2prev] - verts[id2]).length();

			if (ds <= 0 || dre() * r_k < exp(-ds / seq())) {
				long l = id2 - id1;
				if (l < 0) l += size;
				if (l > size_2) {
					std::swap(id1, id2);
					std::swap(id1prev, id2prev);
				}
				auto ptr1 = &verts[id1];
				auto ptr2 = &verts[id2prev];
				if (ptr1 > ptr2) {
					auto l1 = &*verts.end() - ptr1;
					auto l2 = ptr2 - verts.data();
					if (l1 <= l2) {
						do std::swap(*ptr1++, *ptr2--);
						while (ptr1 < &*verts.end());
						ptr1 = verts.data();
					} else {
						do std::swap(*ptr1++, *ptr2--);
						while (ptr2 >= verts.data());
						ptr2 += size;
					}
				}
				while (ptr1 < ptr2)
					std::swap(*ptr1++, *ptr2--);
			}

			i++;
			if constexpr (Progr) {
				if (!progr(i)) break;
			}
		}
	}

	//a >= 0
	//b >= 1
	template <class progr_t = no_value, class edge_c = no_value>
	inline void ant_colony(
		double a, double b,
		double p,
		uint ant_num,
		uint it_num,
		edge_t* const edges,
		progr_t progr = {}
	) {
		constexpr bool Progr = !std::is_same_v<progr_t, no_value>;
		constexpr bool EdgeCallback = !std::is_same_v<edge_c, no_value>;
		b = -b;
		p = 1 - p;

		const auto size = verts.size();
		const auto probability = new double[size];
		const auto path = new uint[size * 2];

		const auto best_path = path + size;
		const auto path_end = best_path;
		const auto edge_end = edges + size * size;

		auto p_ptr = path;
		auto e_ptr = edges;
		uint i = 0;
		for (auto v1 : verts) {
			*p_ptr++ = i++;
			for (auto v2 : verts)
				*e_ptr++ = { (v1 - v2).length(), 1, 0 };
		}

		const auto r_k = 1.0 / (dre.max() + 1);
		double min_len = INFINITY;
		for (i = 0; i < it_num; i++) {
			for (auto j = ant_num; j; j--) {
				auto path_len = 0.0;
				std::swap(path[0], path[dre() % size]);
				p_ptr = path;
				auto e0ptr = e_ptr = edges + path[0] * size;
				uint id0;
				while (true) {
					id0 = *p_ptr;
					if (++p_ptr >= path_end) break;
					auto pr = probability;
					auto sum = 0.0;
					auto next = p_ptr;
					do {
						auto edge = e_ptr[*next];
						sum += pow(edge.pheromone, a) * pow(edge.len, b);
						*pr++ = sum;
					} while (++next < path_end);
					pr = std::lower_bound(probability, pr, dre() * r_k * sum);
					std::swap(p_ptr[0], p_ptr[pr - probability]);

					auto& edge = e_ptr[*p_ptr];
					path_len += edge.len;
					auto count = ++edge.count;
					e_ptr = edges + *p_ptr * size;
					e_ptr[id0].count = count;
				}
				auto& edge = e_ptr[p_ptr[-size]];
				path_len += edge.len;
				auto count = ++edge.count;
				e0ptr[id0].count = count;
				if (min_len > path_len) {
					min_len = path_len;
					std::copy(path, path_end, best_path);
				}
			}

			for (e_ptr = edges; e_ptr < edge_end; e_ptr++) {
				e_ptr->pheromone *= p;
				e_ptr->pheromone += e_ptr->count / e_ptr->len;
				e_ptr->count = 0;
			}

			if constexpr (Progr) {
				if (!(i % (1 << 9)) && !progr(i, best_path)) break;
			}
		}
		progr(it_num, best_path);

		std::vector<vec<2>> src(size);
		src.swap(verts);
		p_ptr = best_path;
		for (auto& dst : verts)
			dst = src[*p_ptr++];

		delete[] probability;
		delete[] path;
	}
	//a >= 0
	//b >= 1
	template <class progr_t = no_value, class edge_c = no_value>
	inline void ant_colony(
		double a, double b,
		double p,
		uint ant_num,
		uint it_num,
		progr_t progr = {}
	) {
		const auto edges = new edge_t[verts.size() * verts.size()];
		ant_colony(a, b, p, ant_num, it_num, edges, progr);
		delete[] edges;
	}
};
