#ifndef OCTOON_IMAGE_LUT_H_
#define OCTOON_IMAGE_LUT_H_

#include <limits>
#include <sstream>

namespace octoon
{
	namespace image
	{
		namespace lut
		{
			namespace detail
			{
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

					friend Vector2 operator+(const Vector2& l, const Vector2& r) noexcept { return Vector2(l.x + r.x, l.y + r.y); }
					friend Vector2 operator-(const Vector2& l, const Vector2& r) noexcept { return Vector2(l.x - r.x, l.y - r.y); }
					friend Vector2 operator*(const Vector2& l, const Vector2& r) noexcept { return Vector2(l.x * r.x, l.y * r.y); }
					friend Vector2 operator/(const Vector2& l, const Vector2& r) noexcept { return Vector2(l.x / r.x, l.y / r.y); }

					template<typename S>
					friend Vector2 operator+(const Vector2& l, S scale) noexcept { return Vector2(l.x + scale, l.y + scale); }
					template<typename S>
					friend Vector2 operator-(const Vector2& l, S scale) noexcept { return Vector2(l.x - scale, l.y - scale); }
					template<typename S>
					friend Vector2 operator*(const Vector2& l, S scale) noexcept { return Vector2(l.x * scale, l.y * scale); }
					template<typename S>
					friend Vector2 operator/(const Vector2& l, S scale) noexcept { return Vector2(l.x / scale, l.y / scale); }
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

					template<typename S>
					friend Vector3 operator+(const Vector3& l, S scale) noexcept { return Vector3(l.x + scale, l.y + scale, l.z + scale); }
					template<typename S>
					friend Vector3 operator-(const Vector3& l, S scale) noexcept { return Vector3(l.x - scale, l.y - scale, l.z - scale); }
					template<typename S>
					friend Vector3 operator*(const Vector3& l, S scale) noexcept { return Vector3(l.x * scale, l.y * scale, l.z * scale); }
					template<typename S>
					friend Vector3 operator/(const Vector3& l, S scale) noexcept { return Vector3(l.x / scale, l.y / scale, l.z / scale); }

					friend Vector3 operator+(const Vector3& l, const Vector3& r) noexcept { return Vector3(l.x + r.x, l.y + r.y, l.z + r.z); }
					friend Vector3 operator-(const Vector3& l, const Vector3& r) noexcept { return Vector3(l.x - r.x, l.y - r.y, l.z - r.z); }
					friend Vector3 operator*(const Vector3& l, const Vector3& r) noexcept { return Vector3(l.x * r.x, l.y * r.y, l.z * r.z); }
					friend Vector3 operator/(const Vector3& l, const Vector3& r) noexcept { return Vector3(l.x / r.x, l.y / r.y, l.z / r.z); }
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

					friend Vector4 operator+(const Vector4& l, const Vector4& r) noexcept { return Vector4(l.x + r.x, l.y + r.y, l.z + r.z, l.w + r.w); }
					friend Vector4 operator-(const Vector4& l, const Vector4& r) noexcept { return Vector4(l.x - r.x, l.y - r.y, l.z - r.z, l.w - r.w); }
					friend Vector4 operator*(const Vector4& l, const Vector4& r) noexcept { return Vector4(l.x * r.x, l.y * r.y, l.z * r.z, l.w * r.w); }
					friend Vector4 operator/(const Vector4& l, const Vector4& r) noexcept { return Vector4(l.x / r.x, l.y / r.y, l.z / r.z, l.w / r.w); }
				};

				template<typename T>
				inline static T frac(const T x) noexcept
				{
					return x - std::floor(x);
				}

				template<typename _Tx, typename _Ty>
				inline static _Tx lerp(const _Tx t1, const _Tx t2, const _Ty t3) noexcept
				{
					if (t3 == 0) return t1; // float-precision
					if (t3 == 1) return t2; // float-precision
					return t1 + (t2 - t1) * t3;
				}

				template<typename T>
				inline static T dot(const Vector2<T>& v1, const Vector2<T>& v2) noexcept { return v1.x * v2.x + v1.y * v2.y; }
				template<typename T>
				inline static T dot(const Vector3<T>& v1, const Vector3<T>& v2) noexcept { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

				template<typename T>
				inline static T length2(const Vector2<T>& v) noexcept { return dot(v, v); }
				template<typename T>
				inline static T length2(const Vector3<T>& v) noexcept { return dot(v, v); }

				template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
				inline static T length(const Vector2<T>& v) noexcept { return std::sqrt(length2(v)); }
				template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
				inline static T length(const Vector3<T>& v) noexcept { return std::sqrt(length2(v)); }

				template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
				inline static Vector2<T> normalize(const Vector2<T>& v) noexcept
				{
					T magSq = length2(v);
					if (magSq > 0.0f)
					{
						T invSqrt = 1.0f / std::sqrt(magSq);
						return v * invSqrt;
					}

					return v;
				}

				template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
				inline static Vector3<T> normalize(const Vector3<T>& v) noexcept
				{
					T magSq = length2(v);
					if (magSq > 0.0f)
					{
						T invSqrt = 1.0f / std::sqrt(magSq);
						return v * invSqrt;
					}

					return v;
				}

				template<typename T, typename = std::enable_if_t<std::is_integral<T>::value | std::is_unsigned<T>::value | std::is_floating_point<T>::value>>
				class sampler
				{
				public:
					std::uint32_t width;
					std::uint32_t height;
					std::uint8_t channel;
					std::unique_ptr<T[]> data;

					sampler() noexcept {};
					sampler(std::unique_ptr<T>&& _data, std::uint32_t w, std::uint32_t h, std::uint8_t c) noexcept : data(std::move(_data)), width(w), height(h), channel(c) {}
					explicit sampler(std::uint32_t size, std::uint8_t channel_ = 3) noexcept { this->create(size, channel_); }
					explicit sampler(std::istream& stream) noexcept { this->create(stream); }

					void create(std::uint32_t size, std::uint8_t channel_) noexcept
					{
						width = size * size;
						height = size;
						channel = channel_;
						data = std::make_unique<T[]>(width * height * channel);

						for (std::uint32_t x = 0; x < width; x++)
						{
							for (std::uint32_t y = 0; y < height; y++)
							{
								float u = float(x) / (width - 1) * ((width - 1.0f) / width);
								float v = float(y) / (height - 1) * ((height - 1.0f) / height);

								float rgb[3];
								rgb[0] = frac(u * size);
								rgb[1] = v;
								rgb[2] = u - rgb[0] / size;

								rgb[0] *= size / float(size - 1);
								rgb[1] *= size / float(size - 1);
								rgb[2] *= size / float(size - 1);

								std::size_t n = (width * y + x) * 3;
								data[n + 0] = cast<T>(rgb[0]);
								data[n + 1] = cast<T>(rgb[1]);
								data[n + 2] = cast<T>(rgb[2]);
							}
						}
					}
					
					void create(std::istream& stream) noexcept(false)
					{
						std::string line;
						std::uint32_t size = 0;

						std::vector<float> values;

						while (std::getline(stream, line))
						{
							if (line.empty())
								continue;

							if (line[0] == '#')
								continue;

							if (line.compare(0, 5, "TITLE") == 0)
								continue;

							if (line.compare(0, 10, "DOMAIN_MIN") == 0 || line.compare(0, 10, "DOMAIN_MAX") == 0)
								continue;

							if (line.compare(0, 11, "LUT_3D_SIZE") == 0)
							{
								std::string token;
								std::istringstream sin(line);

								sin >> token >> size;
								continue;
							}

							float r, g, b;

							std::istringstream sin(line);
							sin >> r >> g >> b;

							values.push_back(r);
							values.push_back(g);
							values.push_back(b);
						}

						if (values.size() == size * size * size * 3)
						{
							this->width = size * size;
							this->height = size;
							this->channel = 3;
							this->data = std::make_unique<T[]>(this->width * this->height * this->channel);

							for (std::size_t y = 0; y < size; y++)
							{
								for (std::size_t z = 0; z < size; z++)
								{
									for (std::size_t x = 0; x < size; x++)
									{
										auto dst = (y * width + (z * size + x)) * this->channel;
										auto src = (z * width + (y * size + x)) * this->channel;

										this->data[dst + 0] = cast<T>(values[src + 0]);
										this->data[dst + 1] = cast<T>(values[src + 1]);
										this->data[dst + 2] = cast<T>(values[src + 2]);
									}
								}
							}								
						}
					}

				private:
					template<typename S>
					static std::enable_if_t<std::is_floating_point<S>::value, S> cast(float x) noexcept
					{
						return std::min(std::max(x, 0.0f), 1.0f);
					}

					template<typename S>
					static std::enable_if_t<std::is_integral<S>::value | std::is_unsigned<S>::value, S> cast(float x) noexcept
					{
						return static_cast<S>(std::min(std::max(x, 0.0f), 1.0f) * std::numeric_limits<S>::max());
					}
				};

				template<typename _Tx, typename _Ty, typename = std::enable_if_t<std::is_floating_point<_Ty>::value>>
				inline Vector3<_Ty> tex2D(const sampler<_Tx>& tex, _Ty u, _Ty v) noexcept
				{
					_Ty x = std::min(std::max(0.0f, u), 1.0f) * tex.width;
					_Ty y = std::min(std::max(0.0f, v), 1.0f) * tex.height;
					_Ty x2 = x >= tex.width - 1 ? tex.width - 1 : x + 1;
					_Ty y2 = y >= tex.height - 1 ? tex.height - 1 : y + 1;

					_Ty xw = frac(x);
					_Ty yw = frac(y);

					// linear interpolation
					_Ty xw1 = 1.0f - xw;
					_Ty xw2 = xw;
					_Ty yw1 = 1.0f - yw;
					_Ty yw2 = yw;

					auto n1 = (tex.width * (int)y + (int)x) * tex.channel;
					auto n2 = (tex.width * (int)y + (int)x2) * tex.channel;
					auto n3 = (tex.width * (int)y2 + (int)x) * tex.channel;
					auto n4 = (tex.width * (int)y2 + (int)x2) * tex.channel;

					auto v1 = Vector3<_Ty>(tex.data[n1], tex.data[n1 + 1], tex.data[n1 + 2]);
					auto v2 = Vector3<_Ty>(tex.data[n2], tex.data[n2 + 1], tex.data[n2 + 2]);
					auto v3 = Vector3<_Ty>(tex.data[n3], tex.data[n3 + 1], tex.data[n3 + 2]);
					auto v4 = Vector3<_Ty>(tex.data[n4], tex.data[n4 + 1], tex.data[n4 + 2]);

 					v1 = v1 * xw1 * yw1;
 					v2 = v2 * xw2 * yw1;
 					v3 = v3 * xw1 * yw2;
 					v4 = v4 * xw2 * yw2;

					return v1 + v2 + v3 + v4;
				}

				template<typename _Tx, typename _Ty, typename = std::enable_if_t<std::is_floating_point<_Ty>::value>>
				inline Vector3<_Ty> lookup(const sampler<_Tx>& tex, const _Ty color[3])
				{
					assert(color[0] >= 0.0f && color[0] <= 1.0f);
					assert(color[1] >= 0.0f && color[1] <= 1.0f);
					assert(color[2] >= 0.0f && color[2] <= 1.0f);
					assert(tex.width == tex.height * tex.height);

					_Ty c[3];
					c[0] = color[0] * ((tex.height - 1.0f) / tex.height);
					c[1] = color[1] * ((tex.height - 1.0f) / tex.height);
					c[2] = color[2] * ((tex.height - 1.0f) / tex.height);

					_Ty slice = c[2] * tex.height - 0.5f;
					_Ty s = frac(slice);
					slice -= s;

					_Ty u = std::max<_Ty>(0.0f, c[0] + slice) / tex.height;
					_Ty v = std::max<_Ty>(0.0f, c[1]);

					_Ty uv0[] = { u, v };
					_Ty uv1[] = { u + 1.0f / tex.height, v };

					auto col0 = tex2D(tex, uv0[0], uv0[1]);
					auto col1 = tex2D(tex, uv1[0], uv1[1]);

					return lerp(col0, col1, s);
				}
			}

			template<typename T = std::uint8_t, typename = std::enable_if_t<std::is_integral<T>::value | std::is_unsigned<T>::value | std::is_floating_point<T>::value>>
			inline detail::sampler<T> create(std::uint32_t size = 16, std::uint8_t channel_ = 3)
			{
				return detail::sampler<T>(size, channel_);
			}

			template<typename T = std::uint8_t, typename = std::enable_if_t<std::is_integral<T>::value | std::is_unsigned<T>::value | std::is_floating_point<T>::value>>
			inline detail::sampler<T> create(std::istream& stream) noexcept(false)
			{
				return detail::sampler<T>(stream);
			}

			template<typename T>
			inline std::enable_if_t<std::is_floating_point<T>::value> lookup(const detail::sampler<T>& tex, const T in[3], T out[3])
			{
				auto v = detail::lookup(tex, in);
				out[0] = v.x;
				out[1] = v.y;
				out[2] = v.z;
			}

			template<typename T>
			inline std::enable_if_t<std::is_integral<T>::value | std::is_unsigned<T>::value> lookup(const detail::sampler<T>& tex, const T in[3], T out[3])
			{
				float c[3] = { (float)in[0] / std::numeric_limits<T>::max(), (float)in[1] / std::numeric_limits<T>::max(), (float)in[2] / std::numeric_limits<T>::max() };
				auto v = detail::lookup(tex, c);
				out[0] = static_cast<T>(v.x);
				out[1] = static_cast<T>(v.y);
				out[2] = static_cast<T>(v.z);
			}
		}
	}
}

#endif