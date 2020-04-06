#ifndef OCTOON_TRIANGLEPACKER_H_
#define OCTOON_TRIANGLEPACKER_H_

#include <vector> // std::vector
#include <memory> // std::unique_ptr
#include <string> // std::to_string
#include <algorithm> // std::sort

namespace octoon
{
	namespace detail
	{
		class exception : public std::exception
		{
		public:
			const int id;

			virtual const char* what() const noexcept override
			{
				return m.what();
			}

		protected:
			exception(int id_, const char* what_arg)
				: id(id_), m(what_arg)
			{}

			static std::string name(const std::string& ename, int id)
			{
				return "[uvmapper.exception." + ename + "." + std::to_string(id) + "] ";
			}

		private:
			std::runtime_error m;
		};

		class out_of_range : public exception
		{
		public:
			static out_of_range create(int id, const std::string& what_arg)
			{
				std::string w = exception::name("out_of_range", id) + what_arg;
				return out_of_range(id, w.c_str());
			}

		private:
			out_of_range(int id_, const char* what_arg)
				: exception(id_, what_arg)
			{
			}
		};

		template<typename T>
		struct Vector2
		{
			T x, y;

			Vector2() = default;
			Vector2(T xx, T yy) noexcept :x(xx), y(yy) {}

			Vector2& operator+=(T scale) noexcept { x += scale; y += scale; return *this; }
			Vector2& operator-=(T scale) noexcept { x -= scale; y -= scale; return *this; }
			Vector2& operator*=(T scale) noexcept { x *= scale; y *= scale; return *this; }
			Vector2& operator/=(T scale) noexcept { x /= scale; y /= scale; return *this; }

			Vector2& operator+=(const Vector2& r) noexcept { x += r.x; y += r.y; return *this; }
			Vector2& operator-=(const Vector2& r) noexcept { x -= r.x; y -= r.y; return *this; }
			Vector2& operator*=(const Vector2& r) noexcept { x *= r.x; y *= r.y; return *this; }
			Vector2& operator/=(const Vector2& r) noexcept { x /= r.x; y /= r.y; return *this; }
		};

		template<typename T>
		struct Vector3
		{
			T x, y, z;

			Vector3() = default;
			Vector3(T xx, T yy, T zz) noexcept :x(xx), y(yy), z(zz) {}

			Vector3& operator+=(const Vector3& r) noexcept { x += r.x; y += r.y; z += r.z; return *this; }
			Vector3& operator-=(const Vector3& r) noexcept { x -= r.x; y -= r.y; z -= r.z; return *this; }
			Vector3& operator*=(const Vector3& r) noexcept { x *= r.x; y *= r.y; z *= r.z; return *this; }
			Vector3& operator/=(const Vector3& r) noexcept { x /= r.x; y /= r.y; z /= r.z; return *this; }
		};

		template<typename T>
		struct Vector4
		{
			T x, y, z, w;

			Vector4() = default;
			Vector4(T xx, T yy, T zz, T ww) noexcept :x(xx), y(yy), z(zz), w(ww) {}

			Vector4& operator+=(const Vector4& r) noexcept { x += r.x; y += r.y; z += r.z; w += r.w; return *this; }
			Vector4& operator-=(const Vector4& r) noexcept { x -= r.x; y -= r.y; z -= r.z; w -= r.w; return *this; }
			Vector4& operator*=(const Vector4& r) noexcept { x *= r.x; y *= r.y; z *= r.z; w *= r.w; return *this; }
			Vector4& operator/=(const Vector4& r) noexcept { x /= r.x; y /= r.y; z /= r.z; w /= r.w; return *this; }
		};

		template<typename T>
		inline constexpr Vector2<T> operator+(const Vector2<T>& l, T scale) noexcept { return Vector2<T>(l.x + scale, l.y + scale); }
		template<typename T>
		inline constexpr Vector2<T> operator-(const Vector2<T>& l, T scale) noexcept { return Vector2<T>(l.x - scale, l.y - scale); }
		template<typename T>
		inline constexpr Vector2<T> operator*(const Vector2<T>& l, T scale) noexcept { return Vector2<T>(l.x * scale, l.y * scale); }
		template<typename T>
		inline constexpr Vector2<T> operator/(const Vector2<T>& l, T scale) noexcept { return Vector2<T>(l.x / scale, l.y / scale); }

		template<typename T>
		inline constexpr Vector2<T> operator+(const Vector2<T>& l, const Vector2<T>& r) noexcept { return Vector2<T>(l.x + r.x, l.y + r.y); }
		template<typename T>
		inline constexpr Vector2<T> operator-(const Vector2<T>& l, const Vector2<T>& r) noexcept { return Vector2<T>(l.x - r.x, l.y - r.y); }
		template<typename T>
		inline constexpr Vector2<T> operator*(const Vector2<T>& l, const Vector2<T>& r) noexcept { return Vector2<T>(l.x * r.x, l.y * r.y); }
		template<typename T>
		inline constexpr Vector2<T> operator/(const Vector2<T>& l, const Vector2<T>& r) noexcept { return Vector2<T>(l.x / r.x, l.y / r.y); }

		template<typename T>
		inline constexpr Vector3<T> operator+(const Vector3<T>& l, T scale) noexcept { return Vector3<T>(l.x + scale, l.y + scale, l.z + scale); }
		template<typename T>
		inline constexpr Vector3<T> operator-(const Vector3<T>& l, T scale) noexcept { return Vector3<T>(l.x - scale, l.y - scale, l.z - scale); }
		template<typename T>
		inline constexpr Vector3<T> operator*(const Vector3<T>& l, T scale) noexcept { return Vector3<T>(l.x * scale, l.y * scale, l.z * scale); }
		template<typename T>
		inline constexpr Vector3<T> operator/(const Vector3<T>& l, T scale) noexcept { return Vector3<T>(l.x / scale, l.y / scale, l.z / scale); }

		template<typename T>
		inline constexpr Vector3<T> operator+(const Vector3<T>& l, const Vector3<T>& r) noexcept { return Vector3<T>(l.x + r.x, l.y + r.y, l.z + r.z); }
		template<typename T>
		inline constexpr Vector3<T> operator-(const Vector3<T>& l, const Vector3<T>& r) noexcept { return Vector3<T>(l.x - r.x, l.y - r.y, l.z - r.z); }
		template<typename T>
		inline constexpr Vector3<T> operator*(const Vector3<T>& l, const Vector3<T>& r) noexcept { return Vector3<T>(l.x * r.x, l.y * r.y, l.z * r.z); }
		template<typename T>
		inline constexpr Vector3<T> operator/(const Vector3<T>& l, const Vector3<T>& r) noexcept { return Vector3<T>(l.x / r.x, l.y / r.y, l.z / r.z); }

		template<typename T>
		inline constexpr Vector4<T> operator+(const Vector4<T>& l, const Vector4<T>& r) noexcept { return Vector4<T>(l.x + r.x, l.y + r.y, l.z + r.z, l.w + r.w); }
		template<typename T>
		inline constexpr Vector4<T> operator-(const Vector4<T>& l, const Vector4<T>& r) noexcept { return Vector4<T>(l.x - r.x, l.y - r.y, l.z - r.z, l.w - r.w); }
		template<typename T>
		inline constexpr Vector4<T> operator*(const Vector4<T>& l, const Vector4<T>& r) noexcept { return Vector4<T>(l.x * r.x, l.y * r.y, l.z * r.z, l.w * r.w); }
		template<typename T>
		inline constexpr Vector4<T> operator/(const Vector4<T>& l, const Vector4<T>& r) noexcept { return Vector4<T>(l.x / r.x, l.y / r.y, l.z / r.z, l.w / r.w); }

		template<typename T>
		inline constexpr T dot(const Vector2<T>& v1, const Vector2<T>& v2) noexcept { return v1.x * v2.x + v1.y * v2.y; }
		template<typename T>
		inline constexpr T dot(const Vector3<T>& v1, const Vector3<T>& v2) noexcept { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

		template<typename T>
		inline constexpr T length2(const Vector2<T>& v) noexcept { return dot(v, v); }
		template<typename T>
		inline constexpr T length2(const Vector3<T>& v) noexcept { return dot(v, v); }

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline constexpr T length(const Vector2<T>& v) noexcept { return std::sqrt(length2(v)); }
		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline constexpr T length(const Vector3<T>& v) noexcept { return std::sqrt(length2(v)); }

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline constexpr Vector2<T> normalize(const Vector2<T>& v) noexcept
		{
			T magSq = length2(v);
			if (magSq > 0.0f)
			{
				T invSqrt = 1.0f / std::sqrt(magSq);
				return v * invSqrt;
			}

			return v;
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline constexpr Vector3<T> normalize(const Vector3<T>& v) noexcept
		{
			T magSq = length2(v);
			if (magSq > 0.0f)
			{
				T invSqrt = 1.0f / std::sqrt(magSq);
				return v * invSqrt;
			}

			return v;
		}

		template<typename _Tx, typename _Ty>
		struct Triangle
		{
			//       C           -
			//     * |  *        | h
			//   *   |     *     |
			// B-----+--------A  -
			// '-------w------'
			_Tx w, h;
			_Ty indices[3];

			Triangle() = default;
			Triangle(_Tx ww, _Tx hh, _Ty i1, _Ty i2, _Ty i3) noexcept
				: w(ww)
				, h(hh)
			{
				indices[0] = i1;
				indices[1] = i2;
				indices[2] = i3;
			}

			template<typename T>
			Triangle(const Vector3<T>& v1, const Vector3<T>& v2, const Vector3<T>& v3, _Ty i1, _Ty i2, _Ty i3) noexcept
			{
				indices[0] = i1;
				indices[1] = i2;
				indices[2] = i3;
				this->compute(v1, v2, v3);
			}

			constexpr auto area() const noexcept
			{
				return w * h;
			}

			template<typename T>
			void compute(const Vector3<T>& v1, const Vector3<T>& v2, const Vector3<T>& v3) noexcept
			{
				// https://github.com/ands/trianglepacker/blob/master/trianglepacker.h at 185 line
				Vector3<T> tv[3];
				tv[0] = v2 - v1;
				tv[1] = v3 - v2;
				tv[2] = v1 - v3;

				T len2[3];
				len2[0] = length2(tv[0]);
				len2[1] = length2(tv[1]);
				len2[2] = length2(tv[2]);

				std::uint8_t maxi; T maxl = len2[0]; maxi = 0;
				if (len2[1] > maxl) { maxl = len2[1]; maxi = 1; }
				if (len2[2] > maxl) { maxl = len2[2]; maxi = 2; }
				std::uint8_t nexti = (maxi + 1) % 3;

				auto ww = std::sqrt(maxl);
				auto xx = -dot(tv[maxi], tv[nexti]) / ww;
				auto hh = length((tv[maxi] + tv[nexti]) - normalize(tv[maxi]) * (ww - xx));

				this->w = static_cast<_Tx>(std::ceil(ww));
				this->h = static_cast<_Tx>(std::ceil(hh));
			}
		};

		template<typename _Tx, typename _Ty, typename std::enable_if_t<std::is_pointer_v<_Ty>>* = nullptr>
		struct Quad
		{
			_Ty t1, t2;

			Vector2<_Tx> edge;
			Vector2<_Tx> uv[6];
			Vector2<_Tx> margin;

			Quad() noexcept : t1(nullptr), t2(nullptr), margin(0, 0) { std::memset(uv, 0, sizeof(uv)); }
			Quad(_Ty _t1, _Ty _t2) noexcept : t1(_t1), t2(_t2), margin(0, 0) { std::memset(uv, 0, sizeof(uv)); this->computeEdge(); }
			Quad(_Ty _t1, _Ty _t2, const Vector2<_Tx>& _margin) noexcept : t1(_t1), t2(_t2), margin(_margin) { std::memset(uv, 0, sizeof(uv)); this->computeEdge(); }

			constexpr auto area() const noexcept
			{
				return edge.x * edge.y;
			}

			void translate(const Vector2<_Tx>& offset) noexcept
			{
				uv[0] += offset;
				uv[1] += offset;
				uv[2] += offset;
				uv[3] += offset;
				uv[4] += offset;
				uv[5] += offset;
			}

			void computeUV(const Vector2<_Tx>& border) noexcept
			{
				uv[0] = Vector2<_Tx>(0, margin.y) + border;
				uv[1] = Vector2<_Tx>(border.x, edge.y - border.x);
				uv[2] = Vector2<_Tx>(edge.x, edge.y) - border;

				uv[3] = Vector2<_Tx>(edge.x, edge.y - margin.y) - border;
				uv[4] = Vector2<_Tx>(edge.x - border.x, border.y);
				uv[5] = Vector2<_Tx>(0, 0) + border;
			}

			void computeEdge() noexcept
			{
				if (t1 && t2)
				{
					edge.x = _Tx(2 << ((int)std::round(std::log2((t1->w + t2->w) / 2))));
					edge.y = _Tx(2 << ((int)std::round(std::log2((t1->h + t2->h)))));
				}
				else
				{
					if (t1)
					{
						edge.x = _Tx(2 << ((int)std::round(std::log2(t1->w))));
						edge.y = _Tx(2 << ((int)std::round(std::log2(t1->h * 2))));
					}

					if (t2)
					{
						edge.x = _Tx(2 << ((int)std::round(std::log2(t1->w))));
						edge.y = _Tx(2 << ((int)std::round(std::log2(t1->h * 2))));
					}
				}
			}
		};

		template<typename _Tx, typename _Ty, typename std::enable_if_t<std::is_pointer_v<_Ty>>* = nullptr>
		class QuadNode
		{
		public:
			QuadNode() noexcept : _rect(0, 0, 1, 1) {}
			QuadNode(const Vector4<_Tx>& rect) noexcept : _rect(rect) {}

			QuadNode* insert(Quad<_Tx, _Ty>& q, bool write = true)
			{
				if (_child[0] && _child[1])
				{
					QuadNode<_Tx, _Ty>* c = _child[0]->insert(q);
					return c ? c : _child[1]->insert(q);
				}
				else
				{
					if (q.edge.x > _rect.z || q.edge.y > _rect.w)
						return nullptr;

					_child[0] = std::make_unique<QuadNode<_Tx, _Ty>>();
					_child[1] = std::make_unique<QuadNode<_Tx, _Ty>>();

					_Tx dw = _rect.z - q.edge.x;
					_Tx dh = _rect.w - q.edge.y;

					if (dw < dh)
					{
						_child[0]->_rect = Vector4<_Tx>(_rect.x + q.edge.x, _rect.y, _rect.z - q.edge.x, q.edge.y);
						_child[1]->_rect = Vector4<_Tx>(_rect.x, _rect.y + q.edge.y, _rect.z, _rect.w - q.edge.y);
					}
					else
					{
						_child[0]->_rect = Vector4<_Tx>(_rect.x, _rect.y + q.edge.y, q.edge.x, _rect.w - q.edge.y);
						_child[1]->_rect = Vector4<_Tx>(_rect.x + q.edge.x, _rect.y, _rect.z - q.edge.x, _rect.w);
					}

					if (write)
					{
						Vector2<_Tx> offset(_rect.x, _rect.y);
						q.translate(offset);
					}

					return this;
				}
			}

			QuadNode* insert(Quad<_Tx, _Ty>& q, const Vector2<_Tx>& margin, bool write = true)
			{
				if (_child[0] && _child[1])
				{
					QuadNode<_Tx, _Ty>* c = _child[0]->insert(q, margin);
					return c ? c : _child[1]->insert(q, margin);
				}
				else
				{
					if (q.edge.x > _rect.z || q.edge.y > _rect.w)
						return nullptr;

					_child[0] = std::make_unique<QuadNode<_Tx, _Ty>>();
					_child[1] = std::make_unique<QuadNode<_Tx, _Ty>>();

					_Tx dw = _rect.z - q.edge.x;
					_Tx dh = _rect.w - q.edge.y;

					if (dw < dh)
					{
						_child[0]->_rect = Vector4<_Tx>(_rect.x + q.edge.x, _rect.y, _rect.z - q.edge.x, q.edge.y);
						_child[1]->_rect = Vector4<_Tx>(_rect.x, _rect.y + q.edge.y, _rect.z, _rect.w - q.edge.y);

						_child[0]->_rect += Vector4<_Tx>(margin.x, 0.f, -margin.x, 0.f);
						_child[1]->_rect += Vector4<_Tx>(0.f, margin.y, 0.f, -margin.y);
					}
					else
					{
						_child[0]->_rect = Vector4<_Tx>(_rect.x, _rect.y + q.edge.y, q.edge.x, _rect.w - q.edge.y);
						_child[1]->_rect = Vector4<_Tx>(_rect.x + q.edge.x, _rect.y, _rect.z - q.edge.x, _rect.w);

						_child[0]->_rect += Vector4<_Tx>(0.f, margin.y, 0.f, -margin.y);
						_child[1]->_rect += Vector4<_Tx>(margin.x, 0.f, -margin.x, 0.f);
					}

					if (write)
					{
						Vector2<_Tx> offset(_rect.x, _rect.y);
						q.translate(offset);
					}

					return this;
				}
			}

		private:
			QuadNode(const QuadNode&) = delete;
			QuadNode& operator=(QuadNode&) = delete;

		private:
			Vector4<_Tx> _rect;
			std::unique_ptr<QuadNode<_Tx, _Ty>> _child[2];
		};
	}

	template<typename T, typename std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
	class basic_uvmapper
	{
	public:
		using value_t = T;

		using size_type = std::size_t;

		using vec2_t = detail::Vector2<value_t>;
		using vec3_t = detail::Vector3<value_t>;
		using vec4_t = detail::Vector4<value_t>;

		using triangle_t = detail::Triangle<int, int>;

		using quad_t = detail::Quad<value_t, triangle_t*>;
		using quad_node_t = detail::QuadNode<value_t, triangle_t*>;

		using exception = detail::exception;
		using out_of_range = detail::out_of_range;

		basic_uvmapper() = default;
		basic_uvmapper(const basic_uvmapper&) = delete;
		basic_uvmapper& operator=(basic_uvmapper&) = delete;

		static bool lightmappack(const value_t* vertices, size_type vertexCount, int width, int height, int margin, value_t* outUVs)
		{
			return lightmappack(vertices, vertexCount, width, height, margin, 0, outUVs);
		}

		static bool lightmappack(const value_t* vertices, size_type vertexCount, int width, int height, int margin, value_t scalefactor, value_t* outUVs)
		{
			const vec3_t* p = (const vec3_t*)vertices;

			std::vector<triangle_t> tris(vertexCount / 3);

			for (size_type i = 0; i < tris.size(); i++)
			{
				const vec3_t& v1 = p[i * 3 + 0];
				const vec3_t& v2 = p[i * 3 + 1];
				const vec3_t& v3 = p[i * 3 + 2];

				tris[i].indices[0] = i * 3 + 0;
				tris[i].indices[1] = i * 3 + 1;
				tris[i].indices[2] = i * 3 + 2;
				tris[i].compute(v1, v2, v3);
			}

			auto border = vec2_t((value_t)margin / width, (value_t)margin / height);

			if (scalefactor > 0)
				return lightmappack2(tris, width, height, border, scalefactor, outUVs) == tris.size();
			else
				return lightmappack2(tris, width, height, border, outUVs) == tris.size();
		}

		template<typename index_t = std::uint16_t, typename = std::enable_if_t<std::is_unsigned_v<index_t>>>
		static bool lightmappack(const value_t* vertices, const index_t* indices, size_type indexCount, int width, int height, int margin, index_t* outRemap, value_t* outUVs, index_t* outIndices, size_type& outVerticeCount)
		{
			auto p = (const vec3_t*)vertices;
			auto remap = outRemap;
			auto border = vec2_t((value_t)margin / width / 2, (value_t)margin / height / 2);

			std::vector<quad_t> quad(indexCount / 6);
			std::vector<triangle_t> tris(indexCount / 3);

			for (index_t index = 0, i = 0; i < quad.size(); i++)
			{
				auto v1 = *indices; indices++;
				auto v2 = *indices; indices++;
				auto v3 = *indices; indices++;

				auto v4 = *indices; indices++;
				auto v5 = *indices; indices++;
				auto v6 = *indices; indices++;

				if (v3 == v4 && v1 == v6)
				{
					*remap = v1; remap++;
					*remap = v2; remap++;
					*remap = v3; remap++;
					*remap = v5; remap++;
					*outIndices = index + 0; outIndices++;
					*outIndices = index + 1; outIndices++;
					*outIndices = index + 2; outIndices++;
					*outIndices = index + 2; outIndices++;
					*outIndices = index + 3; outIndices++;
					*outIndices = index + 0; outIndices++;

					vec3_t tp[4];
					tp[0] = p[v1];
					tp[1] = p[v2];
					tp[2] = p[v3];
					tp[3] = p[v5];

					tris[i * 2 + 0] = triangle_t(tp[0], tp[1], tp[2], index + 0, index + 1, index + 2);
					tris[i * 2 + 1] = triangle_t(tp[2], tp[3], tp[0], index + 2, index + 3, index + 0);

					index += 4;

					quad[i] = quad_t(&tris[i * 2], &tris[i * 2 + 1]);
				}
				else if (v1 == v4 && v3 == v5)
				{
					*remap = v1; remap++;
					*remap = v2; remap++;
					*remap = v3; remap++;
					*remap = v6; remap++;
					*outIndices = index + 0; outIndices++;
					*outIndices = index + 1; outIndices++;
					*outIndices = index + 2; outIndices++;
					*outIndices = index + 2; outIndices++;
					*outIndices = index + 3; outIndices++;
					*outIndices = index + 0; outIndices++;

					vec3_t tp[4];
					tp[0] = p[v1];
					tp[1] = p[v2];
					tp[2] = p[v3];
					tp[3] = p[v5];

					tris[i * 2 + 0] = triangle_t(tp[0], tp[1], tp[2], index + 0, index + 1, index + 2);
					tris[i * 2 + 1] = triangle_t(tp[2], tp[3], tp[0], index + 2, index + 3, index + 0);

					index += 4;

					quad[i] = quad_t(&tris[i * 2], &tris[i * 2 + 1]);
				}
				else
				{
					*remap = v1; remap++;
					*remap = v2; remap++;
					*remap = v3; remap++;
					*remap = v4; remap++;
					*remap = v5; remap++;
					*remap = v6; remap++;
					*outIndices = index + 0; outIndices++;
					*outIndices = index + 1; outIndices++;
					*outIndices = index + 2; outIndices++;
					*outIndices = index + 3; outIndices++;
					*outIndices = index + 4; outIndices++;
					*outIndices = index + 5; outIndices++;

					vec3_t tp[6];
					tp[0] = p[v1];
					tp[1] = p[v2];
					tp[2] = p[v3];
					tp[3] = p[v4];
					tp[4] = p[v5];
					tp[5] = p[v6];

					tris[i * 2 + 0] = triangle_t(tp[0], tp[1], tp[2], index + 0, index + 1, index + 2);
					tris[i * 2 + 1] = triangle_t(tp[3], tp[4], tp[5], index + 3, index + 4, index + 5);

					index += 6;

					quad[i] = quad_t(&tris[i * 2], &tris[i * 2 + 1], border);
				}
			}

			std::qsort(quad.data(), quad.size(), sizeof(quad_t), [](const void* a, const void* b) -> int
			{
				auto t1 = (quad_t*)a;
				auto t2 = (quad_t*)b;
				auto dh = t2->edge.y - t1->edge.y;
				return int(dh != 0 ? dh : t2->edge.x - t1->edge.x);
			});

			auto processed = lightmappack2(quad, width, height, border, outUVs);

			if (processed == quad.size())
				outVerticeCount = remap - outRemap;
			else
				outVerticeCount = 0;

			return processed == quad.size();
		}

	private:
		static size_type lightmappack2(std::vector<quad_t>& quad, int width, int height, const vec2_t& margin, value_t scalefactor, value_t* outUVs)
		{
			value_t area = 0;

			for (auto& it : quad)
				area += it.area();

			area = std::sqrt(area) * scalefactor;

			for (auto& it : quad)
			{
				it.edge /= area;
				it.computeUV(margin);
			}

			quad_node_t root;

			bool write = outUVs ? true : false;

			for (std::size_t i = 0; i < quad.size(); i++)
			{
				if (!root.insert(quad[i], write))
					return (i + 1);
			}

			if (outUVs)
			{
				vec2_t* uv = (vec2_t*)outUVs;

				for (auto& it : quad)
				{
					if (it.t1)
					{
						uv[it.t1->indices[0]] = it.uv[0];
						uv[it.t1->indices[1]] = it.uv[1];
						uv[it.t1->indices[2]] = it.uv[2];
					}

					if (it.t2)
					{
						uv[it.t2->indices[0]] = it.uv[3];
						uv[it.t2->indices[1]] = it.uv[4];
						uv[it.t2->indices[2]] = it.uv[5];
					}
				}
			}

			return quad.size();
		}

		static size_type lightmappack2(const std::vector<quad_t>& quad, int width, int height, const vec2_t& margin, value_t scalefactor, value_t* outUVs)
		{
			std::vector<quad_t> quad2(quad.size());
			std::memcpy(quad2.data(), quad.data(), quad.size() * sizeof(quad_t));
			return lightmappack2(quad2, width, height, margin, scalefactor, outUVs);
		}

		static size_type lightmappack2(const std::vector<triangle_t>& triangles, int width, int height, const vec2_t& margin, value_t scalefactor, value_t* outUVs)
		{
			std::vector<triangle_t> tris(triangles.size());
			std::memcpy(tris.data(), triangles.data(), triangles.size() * sizeof(triangle_t));

			std::qsort(tris.data(), tris.size(), sizeof(triangle_t), [](const void* a, const void* b) -> int
			{
				auto t1 = (triangle_t*)a;
				auto t2 = (triangle_t*)b;
				auto dh = t2->h - t1->h;
				return dh != 0 ? dh : (t2->w - t1->w);
			});

			std::vector<quad_t> quad(tris.size() >> 1);

			for (size_type i = 0; i < quad.size() - tris.size() % 2; i++)
			{
				quad[i].t1 = &tris[i * 2];
				quad[i].t2 = &tris[i * 2 + 1];
				quad[i].margin = margin;
				quad[i].computeEdge();
			}

			if (tris.size() % 2 > 0)
				quad.push_back(quad_t(&(*tris.rbegin()), nullptr));

			auto processed = lightmappack2(quad, width, height, margin, scalefactor, outUVs) * 2;

			return tris.size() % 2 > 0 ? processed-- : processed;
		}

		template<typename T, typename = std::enable_if_t<std::is_same_v<T, quad_t> || std::is_same_v<T, triangle_t>>>
		static size_type lightmappack2(const std::vector<T>& data, int width, int height, const vec2_t& margin, value_t* outUVs)
		{
			value_t testScale = 1.0f;
			value_t testScaleLast = 1.0f;

			auto processed = lightmappack2(data, width, height, margin, testScale, 0);

			if (processed >= data.size())
			{
				while (processed < data.size())
				{
					testScaleLast = testScale;
					testScale += 0.005;
					processed = lightmappack2(data, width, height, margin, testScale, 0);
				}

				testScale += 0.005;
			}
			else
			{
				while (processed < data.size())
				{
					testScaleLast = testScale;
					testScale += 0.005;
					processed = lightmappack2(data, width, height, margin, testScale, 0);
				}
			}

			auto testScaleDiff = (testScale - testScaleLast) / 16.0f;

			for (std::uint8_t i = 0; i < 16; i++)
			{
				testScale -= testScaleDiff;
				processed = lightmappack2(data, width, height, margin, testScale, 0);
				if (processed < data.size())
				{
					testScale += testScaleDiff;
					break;
				}
			}

			return lightmappack2(data, width, height, margin, testScale, outUVs);
		}
	};

	using uvmapper = basic_uvmapper<float>;
}

#endif