#ifndef OCTOON_TEXTURE_LUT_H_
#define OCTOON_TEXTURE_LUT_H_

#include <limits>
#include <sstream>
#include <fstream>
#include <vector>
#include <cassert>

namespace octoon
{
	namespace detail
	{
		class exception : public std::exception
		{
		public:
			virtual const char* what() const noexcept override
			{
				return m.what();
			}

			const int id;

		protected:
			exception(int id_, const char* what_arg)
				: id(id_), m(what_arg)
			{}

			static std::string name(const std::string& ename, int id)
			{
				return "[lut.exception." + ename + "." + std::to_string(id) + "] ";
			}

		private:
			std::runtime_error m;
		};

		class parse_error : public exception
		{
		public:
			static parse_error create(int id, size_t byte_, const std::string& what_arg)
			{
				std::string w = exception::name("parse_error", id) + "parse error" + (byte_ != 0 ? (" at " + std::to_string(byte_)) : "") + ": " + what_arg;
				return parse_error(id, byte_, w.c_str());
			}

			const size_t byte;

		private:
			parse_error(int id_, size_t byte_, const char* what_arg)
				: exception(id_, what_arg), byte(byte_)
			{
			}
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

			const T operator[](std::uint8_t i) const noexcept
			{
				assert(i < 3);
				return *(&x + i);
			}

			const T& operator[](std::uint8_t i) noexcept
			{
				assert(i < 3);
				return *(&x + i);
			}
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

			const T operator[](std::uint8_t i) const noexcept
			{
				assert(i < 4);
				return *(&x + i);
			}

			const T& operator[](std::uint8_t i) noexcept
			{
				assert(i < 4);
				return *(&x + i);
			}
		};

		template<typename T, typename = std::enable_if_t<std::is_integral<T>::value | std::is_unsigned<T>::value | std::is_floating_point<T>::value>>
		class basic_lut
		{
		public:
			std::string name;

			std::uint32_t width;
			std::uint32_t height;
			std::uint8_t channel;
			std::unique_ptr<T[]> data;

			basic_lut() noexcept {};

			/*
			* @brief 创建3D lut 查找表
			* @param[in] data 3D lut 查找表的像素信息
			* @param[in] w 图片宽度
			* @param[in] h 图片高度
			* @param[in] c 通道数量, RGB = 3, RGBA = 4
			*/
			basic_lut(std::unique_ptr<T>&& _data, std::uint32_t w, std::uint32_t h, std::uint8_t c) noexcept : data(std::move(_data)), width(w), height(h), channel(c) {}

			/*
			* @brief 创建3D lut 查找表
			* @param[in] s .cube文件的起始内容
			* @param[in] n .cube文件的长度
			* @details 加载cube文件并且生成一张3D lut 查找表
			*/
			basic_lut(const char* s, std::size_t n) noexcept(false) { this->create(s, n); }

			/*
			* @brief 创建默认的 3D lut 查找表, 用于自定义滤镜效果
			* @param[in] size 查找表的大小，宽度为 size * size， 高度为 size
			* @param[in] channel 通道数量， RGB 为 3， RGBA 为 4
			* @details 创建默认的 3D lut 查找表, 用于自定义滤镜效果
			*/
			explicit basic_lut(std::uint32_t size, std::uint8_t channels_ = 3) noexcept { this->create(size, channels_); }

			/*
			* @brief 创建3D lut 查找表
			* @param[in] strean .cube文件的流
			* @details 加载cube流并且生成一张3D lut 查找表
			*/
			explicit basic_lut(std::istream& stream) noexcept(false) { this->create(stream); }

			/*
			* @brief 创建默认的 3D lut 查找表, 用于自定义滤镜效果
			* @param[in] size 查找表的大小，宽度为 size * size， 高度为 size
			* @param[in] channel 通道数量， RGB 为 3， RGBA 为 4
			* @details 创建默认的 3D lut 查找表, 用于自定义滤镜效果
			*/
			void create(std::uint32_t size, std::uint8_t channels_) noexcept
			{
				width = size * size;
				height = size;
				channel = channels_;
				data = std::make_unique<T[]>(width * height * channel);

				for (std::uint32_t x = 0; x < width; x++)
				{
					for (std::uint32_t y = 0; y < height; y++)
					{
						float u = float(x) / (width - 1) * ((width - 1.0f) / width);
						float v = float(y) / (height - 1) * ((height - 1.0f) / height);

						float uvw[3];
						uvw[0] = frac(u * size);
						uvw[1] = v;
						uvw[2] = u - uvw[0] / size;

						uvw[0] *= size / float(size - 1);
						uvw[1] *= size / float(size - 1);
						uvw[2] *= size / float(size - 1);

						std::size_t n = (width * y + x) * channel;
						data[n + 0] = cast<T>(uvw[0]);
						data[n + 1] = cast<T>(uvw[1]);
						data[n + 2] = cast<T>(uvw[2]);
					}
				}
			}
				
			/*
			* @brief 加载 .cube文件并且生成一张3D lut 查找表
			* @param[in] s .cube文件的起始内容
			* @param[in] n .cube文件的长度
			*/
			void create(const char* s, std::size_t n) noexcept(false)
			{
				this->create(std::istringstream(std::string(s, n)));
			}

			/*
			* @brief 加载 .cube流并且生成一张3D lut 查找表
			* @param[in] strean .cube文件的流
			*/
			void create(std::istream& stream) noexcept(false)
			{
				assert(stream.good());

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
					{
						std::string token;
						std::istringstream sin(line);

						sin >> token >> name;
						continue;
					}

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
				else
				{
					throw parse_error::create(0, 0, "The lut element does not match the size of lut");
				}					
			}

			/*
			* @brief 最近邻采样
			* @param[in] u x轴的方向的坐标，范围 0.0 ~ 1.0
			* @param[in] v y轴的方向的坐标，范围 0.0 ~ 1.0
			* @return 返回在归一化纹理坐标(u, v) 处通过最近邻采样获取的像素数据
			* @detail https://en.wikipedia.org/wiki/Nearest-neighbor_interpolation
			*/
			template<typename _Elem>
			std::enable_if_t<std::is_floating_point<_Elem>::value, Vector3<_Elem>> fetch(_Elem u, _Elem v) noexcept
			{
				assert(this->data);
				assert(this->channel == 3 || this->channel == 4);
				assert(this->width == this->height * this->height);

				auto x = std::min(std::max(0.0f, u), 1.0f) * this->width;
				auto y = std::min(std::max(0.0f, v), 1.0f) * this->height;
				auto n = (this->width * (int)std::round(y) + (int)std::round(x)) * this->channel;

				return Vector3<T>(this->data[n], this->data[n + 1], this->data[n + 2]);
			}

			/*
			* @brief 最近邻采样
			* @param[in] u x轴的方向的绝对像素坐标，范围 0 ~ width
			* @param[in] v y轴的方向的绝对像素坐标，范围 0 ~ height
			* @return 返回在绝对像素纹理坐标(u, v) 处通过最近邻采样像素数据
			* @detail https://en.wikipedia.org/wiki/Nearest-neighbor_interpolation
			*/
			template<typename _Elem>
			std::enable_if_t<std::is_unsigned<_Elem>::value | std::is_integral<_Elem>::value, Vector3<_Elem>> fetch(_Elem u, _Elem v) noexcept
			{
				assert(this->data);
				assert(this->channel == 3 || this->channel == 4);
				assert(this->width == this->height * this->height);

				auto x = std::min(std::max(0, u), this->width);
				auto y = std::min(std::max(0, v), this->height);
				auto n = (this->width * y + x) * this->channel;

				return Vector3<T>(this->data[n], this->data[n + 1], this->data[n + 2]);
			}

			/*
			* @brief 双线性过滤采样
			* @param[in] u x轴的方向的归一化坐标，范围 0.0 ~ 1.0
			* @param[in] v y轴的方向的归一化坐标，范围 0.0 ~ 1.0
			* @return 返回在归一化纹理坐标(u, v) 处通过双线性采样获取的像素数据
			* @detail https://en.wikipedia.org/wiki/Bilinear_interpolation#Alternative_algorithm
			*/
			template<typename _Elem>
			std::enable_if_t<std::is_floating_point<_Elem>::value, Vector3<_Elem>> lookup(_Elem u, _Elem v) noexcept
			{
				assert(u >= 0.0f && u <= 1.0f);
				assert(v >= 0.0f && v <= 1.0f);
				assert(this->data);
				assert(this->channel == 3 || this->channel == 4);
				assert(this->width == this->height * this->height);

				_Elem x = std::min(std::max(0.0f, u), 1.0f) * this->width;
				_Elem y = std::min(std::max(0.0f, v), 1.0f) * this->height;
				_Elem x2 = x >= this->width - 1 ? this->width - 1 : x + 1;
				_Elem y2 = y >= this->height - 1 ? this->height - 1 : y + 1;

				_Elem xw = frac(x);
				_Elem yw = frac(y);

				// bilinear interpolation
				_Elem xw1 = 1.0f - xw;
				_Elem xw2 = xw;
				_Elem yw1 = 1.0f - yw;
				_Elem yw2 = yw;

				auto n1 = (this->width * (int)y + (int)x) * this->channel;
				auto n2 = (this->width * (int)y + (int)x2) * this->channel;
				auto n3 = (this->width * (int)y2 + (int)x) * this->channel;
				auto n4 = (this->width * (int)y2 + (int)x2) * this->channel;

				auto v1 = Vector3<_Elem>(this->data[n1], this->data[n1 + 1], this->data[n1 + 2]);
				auto v2 = Vector3<_Elem>(this->data[n2], this->data[n2 + 1], this->data[n2 + 2]);
				auto v3 = Vector3<_Elem>(this->data[n3], this->data[n3 + 1], this->data[n3 + 2]);
				auto v4 = Vector3<_Elem>(this->data[n4], this->data[n4 + 1], this->data[n4 + 2]);

				v1 = v1 * xw1 * yw1;
				v2 = v2 * xw2 * yw1;
				v3 = v3 * xw1 * yw2;
				v4 = v4 * xw2 * yw2;

				return v1 + v2 + v3 + v4;
			}

			/*
			* @brief 立方体纹理坐标的双线性过滤采样
			* @param[in] u x轴的方向的归一化立方体坐标，范围 0.0 ~ 1.0
			* @param[in] v y轴的方向的归一化立方体坐标，范围 0.0 ~ 1.0
			* @param[in] w z轴的方向的归一化立方体坐标，范围 0.0 ~ 1.0
			* @return 返回在归一化立方体纹理坐标(u, v, w) 处通过双线性采样获取的像素数据
			* @detail https://en.wikipedia.org/wiki/Bilinear_interpolation#Alternative_algorithm
			*/
			template<typename _Elem>
			std::enable_if_t<std::is_floating_point<_Elem>::value, Vector3<_Elem>> lookup(const _Elem u, const _Elem v, const _Elem w)
			{
				assert(u >= 0.0f && u <= 1.0f);
				assert(v >= 0.0f && v <= 1.0f);
				assert(w >= 0.0f && w <= 1.0f);
				assert(this->data);
				assert(this->channel == 3 || this->channel == 4);
				assert(this->width == this->height * this->height);

				_Elem c[3];
				c[0] = u * ((this->height - 1.0f) / this->height);
				c[1] = v * ((this->height - 1.0f) / this->height);
				c[2] = w * ((this->height - 1.0f) / this->height);

				_Elem slice = c[2] * this->height - 0.5f / this->height;
				_Elem s = frac(slice);
				slice -= s;

				_Elem uu = std::max<_Elem>(0.0f, c[0] + slice) / this->height;
				_Elem vv = std::max<_Elem>(0.0f, c[1]);

				_Elem uv0[] = { uu, vv };
				_Elem uv1[] = { uu + 1.0f / this->height, vv };

				auto col0 = lookup(uv0[0], uv0[1]);
				auto col1 = lookup(uv1[0], uv1[1]);

				return lerp(col0, col1, s);
			}

			/*
			* @brief 立方体纹理的双线性过滤采样
			* @param[in] uvw[0] x轴的方向的归一化立方体坐标，范围 0.0 ~ 1.0
			* @param[in] uvw[1] y轴的方向的归一化立方体坐标，范围 0.0 ~ 1.0
			* @param[in] uvw[2] z轴的方向的归一化立方体坐标，范围 0.0 ~ 1.0
			* @return 返回在归一化立方体纹理坐标(u, v, w) 处通过双线性采样获取的像素数据
			* @detail https://en.wikipedia.org/wiki/Bilinear_interpolation#Alternative_algorithm
			*/
			template<typename _Elem>
			std::enable_if_t<std::is_floating_point<_Elem>::value, Vector3<_Elem>> lookup(const _Elem uvw[3])
			{
				return lookup(uvw[0], uvw[1], uvw[2]);
			}

			/*
			* @brief 立方体纹理的双线性过滤采样
			* @param[in] uvw  归一化立方体纹理坐标 (u, v, w)，范围 0.0 ~ 1.0
			* @param[out] out 归一化立方体纹理坐标 (u, v, w) 处通过双线性采样获取的像素数据
			* @detail https://en.wikipedia.org/wiki/Bilinear_interpolation#Alternative_algorithm
			*/
			template<typename _Elem>
			std::enable_if_t<std::is_floating_point<_Elem>::value> lookup(const _Elem uvw[3], _Elem out[3])
			{
				auto v = lookup(uvw);
				out[0] = v[0];
				out[1] = v[1];
				out[2] = v[2];
			}

			/*
			* @brief 立方体纹理的双线性过滤采样
			* @param[in] uvw  归一化立方体纹理坐标 (u, v, w)，范围 0.0f ~ 1.0f
			* @param[out] out 归一化立方体纹理坐标 (u, v, w) 处通过双线性采样获取的像素数据
			* @param[in] len 立方体纹理坐标的长度
			* @param[in] channel 立方体纹理坐标的通道数量, RGB = 3, RGBA = 4
			*/
			template<typename _Elem>
			std::enable_if_t<std::is_floating_point<_Elem>::value> lookup(const _Elem* uvw, _Elem* out, std::size_t len, std::uint8_t channel = 3)
			{
				assert(channel == 3 || channel == 4);

				for (std::size_t i = 0; i < len; i += channel)
					lookup(uvw + i, out + i);
			}

			/*
			* @brief 双线性过滤采样
			* @param[in] u x轴的方向的整数范围内未归一化坐标，范围 0 ~ 整数数据类型最大值
			* @param[in] v y轴的方向的整数范围内未归一化坐标，范围 0 ~ 整数数据类型最大值
			* @return 返回在整数范围内未归一化纹理坐标(u, v) 处通过双线性采样获取的像素数据
			* @detail https://en.wikipedia.org/wiki/Bilinear_interpolation#Alternative_algorithm
			*/
			template<typename _Elem, typename _Float = float>
			std::enable_if_t<std::is_integral<_Elem>::value | std::is_unsigned<_Elem>::value, Vector3<_Elem>> lookup(const _Elem u, const _Elem v)
			{
				auto uu = (_Float)u / std::numeric_limits<_Elem>::max();
				auto vv = (_Float)v / std::numeric_limits<_Elem>::max();
				auto pixel = lookup(uu, vv);
				return Vector3<_Elem>(cast<_Elem, T>(pixel[0]), cast<_Elem, T>(pixel[1]), cast<_Elem, T>(pixel[2]));
			}

			/*
			* @brief 立方体纹理的双线性过滤采样
			* @param[in] u x轴的方向的整数范围内未归一化立方体坐标，范围 0 ~ 整数数据类型最大值
			* @param[in] v y轴的方向的整数范围内未归一化立方体坐标，范围 0 ~ 整数数据类型最大值
			* @param[in] w z轴的方向的整数范围内未归一化立方体坐标，范围 0 ~ 整数数据类型最大值
			* @return 返回在整数范围内未归一化立方体纹理坐标(u, v, w) 处通过双线性采样获取的像素数据
			* @detail https://en.wikipedia.org/wiki/Bilinear_interpolation#Alternative_algorithm
			*/
			template<typename _Elem, typename _Float = float>
			std::enable_if_t<std::is_integral<_Elem>::value | std::is_unsigned<_Elem>::value, Vector3<_Elem>> lookup(const _Elem u, const _Elem v, const _Elem w)
			{
				auto uu = (_Float)u / std::numeric_limits<_Elem>::max();
				auto vv = (_Float)v / std::numeric_limits<_Elem>::max();
				auto ww = (_Float)w / std::numeric_limits<_Elem>::max();
				auto pixel = lookup(uu, vv, ww);
				return Vector3<_Elem>(cast<_Elem, T>((T)pixel[0]), cast<_Elem, T>((T)pixel[1]), cast<_Elem, T>((T)pixel[2]));
			}

			/*
			* @brief 立方体纹理的双线性过滤采样
			* @param[in] uvw[0] x轴的方向的整数范围内未归一化立方体坐标，范围 0 ~ 整数数据类型最大值
			* @param[in] uvw[1] y轴的方向的整数范围内未归一化立方体坐标，范围 0 ~ 整数数据类型最大值
			* @param[in] uvw[2] z轴的方向的整数范围内未归一化立方体坐标，范围 0 ~ 整数数据类型最大值
			* @return 返回在整数范围内未归一化立方体纹理坐标(u, v, w) 处通过双线性采样获取的像素数据
			* @detail https://en.wikipedia.org/wiki/Bilinear_interpolation#Alternative_algorithm
			*/
			template<typename _Elem>
			std::enable_if_t<std::is_integral<_Elem>::value | std::is_unsigned<_Elem>::value, Vector3<_Elem>> lookup(const _Elem uvw[3])
			{
				return lookup(uvw[0], uvw[1], uvw[2]);
			}

			/*
			* @brief 立方体纹理的双线性过滤采样
			* @param[in] uvw  整数范围内未归一化立方体纹理坐标 (u, v, w)，范围 0.0 ~ 整数数据类型最大值
			* @param[out] out 整数范围内未归一化立方体纹理坐标 (u, v, w) 处通过双线性采样获取的像素数据
			* @detail https://en.wikipedia.org/wiki/Bilinear_interpolation#Alternative_algorithm
			*/
			template<typename _Elem, typename _Float = float>
			std::enable_if_t<std::is_integral<_Elem>::value | std::is_unsigned<_Elem>::value> lookup(const _Elem uvw[3], _Elem out[3])
			{
				auto v = lookup<_Elem, _Float>(uvw[0], uvw[1], uvw[2]);
				out[0] = v[0];
				out[1] = v[1];
				out[2] = v[2];
			}

			/*
			* @brief 立方体纹理的双线性过滤采样
			* @param[in] uvw  整数范围内未归一化立方体纹理坐标 (u, v, w)，范围 0 ~ 整数数据类型最大值
			* @param[out] out 整数范围内未归一化立方体纹理坐标 (u, v, w) 处通过双线性采样获取的像素数据
			* @param[in] len 立方体纹理坐标的长度
			* @param[in] channel 立方体纹理坐标的通道数量, RGB = 3, RGBA = 4
			* @detail https://en.wikipedia.org/wiki/Bilinear_interpolation#Alternative_algorithm
			*/
			template<typename _Elem, typename _Float = float>
			std::enable_if_t<std::is_integral<_Elem>::value | std::is_unsigned<_Elem>::value> lookup(const _Elem* uvw, _Elem* out, std::size_t len, std::uint8_t channel = 3)
			{
				assert(channel == 3 || channel == 4);

				for (std::size_t i = 0; i < len; i += channel)
					lookup<_Elem, _Float>(uvw + i, out + i);
			}

			/*
			* @brief serialization function for LUT values
			* @return string containing the serialization of the LUT value as the .cube string
			*/
			std::string dump() const noexcept
			{
				assert(this->data);
				assert(this->channel == 3 || this->channel == 4);
				assert(this->width == this->height * this->height);

				std::ostringstream stream;

				stream << "TITLE " << name << std::endl;
				stream << std::endl;

				stream << "LUT_3D_SIZE " << this->height << std::endl;
				stream << std::endl;

				stream << "DOMAIN_MIN 0.0 0.0 0.0" << std::endl;
				stream << "DOMAIN_MAX 1.0 1.0 1.0" << std::endl;
				stream << std::endl;

				for (std::size_t y = 0; y < this->height; y++)
				{
					for (std::size_t z = 0; z < this->height; z++)
					{
						for (std::size_t x = 0; x < this->height; x++)
						{
							auto src = (z * width + (y * this->height + x)) * this->channel;
							auto r = cast<float>(data[src + 0]);
							auto g = cast<float>(data[src + 1]);
							auto b = cast<float>(data[src + 2]);

							stream << r << " " << g << " " << b << std::endl;
						}
					}
				}

				return stream.str();
			}

			void dumpTGA(std::ostream& stream) const noexcept
			{
				assert(stream);
				assert(this->width < std::numeric_limits<std::uint16_t>::max() || this->height < std::numeric_limits<std::uint16_t>::max());

				std::uint8_t  id_length = 0;
				std::uint8_t  colormap_type = 0;
				std::uint8_t  image_type = 2;
				std::uint16_t colormap_index = 0;
				std::uint16_t colormap_length = 0;
				std::uint8_t  colormap_size = 0;
				std::uint16_t x_origin = 0;
				std::uint16_t y_origin = 0;
				std::uint16_t w = (std::uint16_t)this->width;
				std::uint16_t h = (std::uint16_t)this->height;
				std::uint8_t  pixel_size = this->channel * 8;
				std::uint8_t  attributes = this->channel == 4 ? 8 : 0;			

				stream.write((char*)&id_length, sizeof(id_length));
				stream.write((char*)&colormap_type, sizeof(colormap_type));
				stream.write((char*)&image_type, sizeof(image_type));
				stream.write((char*)&colormap_index, sizeof(colormap_index));
				stream.write((char*)&colormap_length, sizeof(colormap_length));
				stream.write((char*)&colormap_size, sizeof(colormap_size));
				stream.write((char*)&x_origin, sizeof(x_origin));
				stream.write((char*)&y_origin, sizeof(y_origin));
				stream.write((char*)&w, sizeof(w));
				stream.write((char*)&h, sizeof(h));
				stream.write((char*)&pixel_size, sizeof(pixel_size));
				stream.write((char*)&attributes, sizeof(attributes));

				std::vector<std::uint8_t> buffer(this->width * this->height * this->channel);
				for (std::size_t i = 0; i < buffer.size(); i++)
					buffer[i] = cast<std::uint8_t>(this->data[i]);

				stream.write((char*)buffer.data(), this->width * this->height * this->channel);
			}

			void dumpTGA(const char* filepath) const noexcept(false)
			{
				auto stream = std::ofstream(filepath, std::ios_base::out | std::ios_base::binary);
				if (stream)
					this->dumpTGA(stream);
				else
					throw std::runtime_error("failed to open the file: " + std::string(filepath));
			}

			void dumpTGA(const std::string& filepath) const noexcept(false)
			{
				auto stream = std::ofstream(filepath, std::ios_base::out | std::ios_base::binary);
				if (stream)
					this->dumpTGA(stream);
				else
					throw std::runtime_error("failed to open the file: " + filepath);
			}

			/*
			* @brief deserialization from a .cube stream
			* @param[in] stream stream to read a serialized .cube value from
			* @return result of the deserialization
			*/
			static basic_lut parse(std::istream& stream) noexcept(false)
			{
				if (stream)
					return basic_lut(stream);
				else
					throw std::runtime_error("bad stream");
			}

			/*
			* @brief deserialization from a .cube file
			* @param[in] filepath path to read a .cube file from disk
			* @return result of the deserialization
			*/
			static basic_lut parse(const std::string& filepath) noexcept(false)
			{
				auto stream = std::ifstream(filepath);
				if (stream)
					return basic_lut(stream);
				else
					throw std::runtime_error("failed to open the file: " + filepath);
			}

			/*
			* @brief deserialization from a .cube file
			* @param[in] filepath path to read a .cube file from disk
			* @return result of the deserialization
			*/
			static basic_lut parse(const char* filepath) noexcept(false)
			{
				auto stream = std::ifstream(filepath);
				if (stream)
					return basic_lut(stream);
				else
					throw std::runtime_error(std::string("failed to open the file: ") + filepath);
			}

			/*
			* @brief serialization function for LUT values
			* @return stream containing the serialization of the LUT value as the .cube stream
			*/
			friend std::ostream& operator << (std::ostream& os, const basic_lut& lut) noexcept
			{
				os << lut.dump();
				return os;
			}

		private:
			/*
			* @brief there is nothing to do, when two typename (_Tx, _Ty) are the same types
			*/
			template<typename _Tx, typename _Ty>
			static std::enable_if_t<std::is_same<_Tx, _Ty>::value, _Tx> cast(_Ty x) noexcept
			{
				return x;
			}

			/*
			* @brief 'x' multiple with a maximum value of int, where input parameter (x) is a non-normalized value
			* @return return a normalized value
			*/
			template<typename _Tx, typename _Ty, typename = std::enable_if_t<std::is_integral<_Ty>::value | std::is_unsigned<_Ty>::value, _Tx>>
			static std::enable_if_t<std::is_floating_point<_Tx>::value, _Tx> cast(_Ty x) noexcept
			{
				return static_cast<_Tx>(_Tx(x) / std::numeric_limits<_Ty>::max());
			}

			/*
			* @brief 'x' multiple with a maximum value of int, where input parameter (x) is a normalized value
			* @return return a non-normalized value
			*/
			template<typename _Tx, typename _Ty, typename = std::enable_if_t<std::is_floating_point<_Ty>::value>>
			static std::enable_if_t<std::is_integral<_Tx>::value | std::is_unsigned<_Tx>::value, _Tx> cast(_Ty x) noexcept
			{
				return static_cast<_Tx>(std::min(std::max(x, 0.0f), 1.0f) * std::numeric_limits<_Tx>::max());
			}

			/*
			* @brief helper-function "frac", fraction calling, compute the fractional part of x, where input parameter (x) is a floating-point
			* @params[in] x the input parameter (x) must be a floating-point
			* @return The result is a fractional part of x
			*/
			template<typename _Tx>
			static std::enable_if_t<std::is_floating_point<_Tx>::value, _Tx> frac(const _Tx x) noexcept
			{
				return x - std::floor(x);
			}

			/*
			* @brief helper-function "frac", fraction calling, compute the fractional part of x, where input parameter (x) is a interger
			* @params[in] x the input parameter (x) must be a interger
			* @return The result is always zero
			*/
			template<typename _Tx>
			static std::enable_if_t<std::is_integral<_Tx>::value | std::is_unsigned<_Tx>::value, _Tx> frac(const _Tx x) noexcept
			{
				return 0;
			}

			/*
			* @brief helper-function "lerp", return an linear interpolation between two parameters (t1, t2) for a weight 't'
			* @param[in] t1 begin point
			* @param[in] t2 end point
			* @param[in] t unit interval, the unit interval is a value of weight that closed interval [0, 1]
			* @detail https://en.wikipedia.org/wiki/Linear_interpolation
			*/
			template<typename _Tx, typename _Ty>
			static _Tx lerp(const _Tx t1, const _Tx t2, const _Ty t) noexcept
			{
				if (t == 0) return t1; // due to float-precision arithmetic error
				if (t == 1) return t2; // due to float-precision arithmetic error
				return t1 * (1.0f - t) + t2 * t;
			}
		};
	}

	using lut = detail::basic_lut<std::uint8_t>;

	// Usage 1
	// auto lut = octoon::image::lut::parse("xxx.cube"); // load the lut from .cube file
	// for (std::size_t i = 0; i < image.size(); i++)
	// {
	//		auto& rgb = image[i];
	// 		auto data = lut.lookup(rgb.r, rgb.g, rgb.b); The (r,g,b) can be extended to support these types of std::uint8_t, std::uint16_t, std::uint32_t, float, double
	// 		rgb.r = data[0];
	// 		rgb.g = data[1];
	// 		rgb.b = data[2];
	// }

	// Usage 2
	// auto lut = octoon::image::lut::parse("xxx.cube"); // load the lut from .cube file
	// lut.lookup(image.data(), image.data(), image.size(), 3); The (r,g,b) can be extended to support these types of std::uint8_t, std::uint16_t, std::uint32_t, float, double

	// Serializable to .cube stream
	// Usage 1
	// std::cout << lut.dump();
	//
	// Usage 2 
	// std::cout << lut;
	//
	// Serializable to image
	// Usage 1
	// lut.dumpTGA("C:\\Users\\User Name\\Desktop\\1.tga");
	//
	// Usage 2
	// lut.dumpTGA(stream);
}

#endif