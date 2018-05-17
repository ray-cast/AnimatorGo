#ifndef OCTOON_MATH_MATRIX4X4_H_
#define OCTOON_MATH_MATRIX4X4_H_

#include <octoon/math/mat3.h>
#include <octoon/math/quat.h>
#include <octoon/math/vector4.h>

namespace octoon
{
	namespace math
	{
		namespace detail
		{
			template<typename T>
			class Matrix4x4 final
			{
			public:
				typedef typename trait::type_addition<T>::value_type value_type;
				typedef typename trait::type_addition<T>::pointer pointer;
				typedef typename trait::type_addition<T>::const_pointer const_pointer;
				typedef typename trait::type_addition<T>::reference reference;
				typedef typename trait::type_addition<T>::const_reference const_reference;

				static const Matrix4x4<T> Zero;
				static const Matrix4x4<T> One;

				union
				{
					struct
					{
						T a1, a2, a3, a4;
						T b1, b2, b3, b4;
						T c1, c2, c3, c4;
						T d1, d2, d3, d4;
					};

					struct
					{
						Vector4<T> right;
						Vector4<T> up;
						Vector4<T> forward;
						Vector4<T> position;
					};
				};

				Matrix4x4() = default;
				Matrix4x4(const Matrix4x4<T>& m1, const Matrix4x4<T>& m2) noexcept { this->makeMatrix(m1, m2); }
				Matrix4x4(const Vector3<T>& axis, T angle, const Vector3<T>& translate) noexcept { this->makeRotation(axis, angle, translate); }
				Matrix4x4(const Quaternion<T>& q, const Vector3<T>& translate) noexcept { this->makeRotation(q, translate); }
				Matrix4x4(T mt00, T mt01, T mt02, T mt03, T mt10, T mt11, T mt12, T mt13, T mt20, T mt21, T mt22, T mt23, T mt30, T mt31, T mt32, T mt33) noexcept
					: a1(mt00), a2(mt01), a3(mt02), a4(mt03)
					, b1(mt10), b2(mt11), b3(mt12), b4(mt13)
					, c1(mt20), c2(mt21), c3(mt22), c4(mt23)
					, d1(mt30), d2(mt31), d3(mt32), d4(mt33)
				{
				}

				explicit Matrix4x4(const Matrix3x3<T>& m) noexcept { this->makeMatrix(m); }

				~Matrix4x4() = default;

				template<typename S, typename = std::enable_if_t<trait::is_integral_v<S> || trait::is_floating_point_v<S>>>
				explicit operator Matrix4x4<S>() const noexcept
				{
					return Matrix4x4<S>(
						static_cast<S>(a1), static_cast<S>(a2), static_cast<S>(a3), static_cast<S>(a4),
						static_cast<S>(b1), static_cast<S>(b2), static_cast<S>(b3), static_cast<S>(b4),
						static_cast<S>(c1), static_cast<S>(c2), static_cast<S>(c3), static_cast<S>(c4),
						static_cast<S>(d1), static_cast<S>(d2), static_cast<S>(d3), static_cast<S>(d4));
				}

				reference operator[](std::uint8_t n) noexcept
				{
					assert(n < 16);
					return *((&a1) + n);
				}

				const_reference operator[](std::uint8_t n) const noexcept
				{
					assert(n < 16);
					return *((&a1) + n);
				}

				explicit operator pointer() noexcept
				{
					return this->ptr();
				}

				explicit operator const_pointer() const noexcept
				{
					return this->ptr();
				}

				Matrix4x4<T>& makeMatrix(T mt00, T mt01, T mt02, T mt03, T mt10, T mt11, T mt12, T mt13, T mt20, T mt21, T mt22, T mt23, T mt30, T mt31, T mt32, T mt33) noexcept
				{
					a1 = mt00; a2 = mt01; a3 = mt02; a4 = mt03;
					b1 = mt10; b2 = mt11; b3 = mt12; b4 = mt13;
					c1 = mt20; c2 = mt21; c3 = mt22; c4 = mt23;
					d1 = mt30; d2 = mt31; d3 = mt32; d4 = mt33;
					return *this;
				}

				Matrix4x4<T>& makeMatrix(const Matrix3x3<T>& m) noexcept
				{
					a1 = m.a1; a2 = m.a2; a3 = m.a3;
					b1 = m.b1; b2 = m.b2; b3 = m.b3;
					c1 = m.c1; c2 = m.c2; c3 = m.c3;
					return *this;
				}

				Matrix4x4<T>& makeMatrix(const Vector4<T>& v1, const Vector4<T>& v2, const Vector4<T>& v3, const Vector4<T>& v4) noexcept
				{
					a1 = v1.x; a2 = v1.y; a3 = v1.z; a4 = v1.w;
					b1 = v2.x; b2 = v2.y; b3 = v2.z; b4 = v2.w;
					c1 = v3.x; c2 = v3.y; c3 = v3.z; c4 = v3.w;
					d1 = v4.x; d2 = v4.y; d3 = v4.z; d4 = v4.w;
					return *this;
				}

				Matrix3x3<T>& makeIdentity() noexcept
				{
					a1 = 1.0f; a2 = 0.0f; a3 = 0.0f; a4 = 0.0f;
					b1 = 0.0f; b2 = 1.0f; b3 = 0.0f; b4 = 0.0f;
					c1 = 0.0f; c2 = 0.0f; c3 = 1.0f; c4 = 0.0f;
					d1 = 0.0f; d2 = 0.0f; d3 = 0.0f; d4 = 1.0f;
					return *this;
				}

				Matrix4x4<T>& makeMatrix(const Matrix4x4<T>& m1, const Matrix4x4<T>& m2) noexcept
				{
					assert(&m1 != this && &m2 != this);

					this->a1 = m1.a1 * m2.a1 + m1.b1 * m2.a2 + m1.c1 * m2.a3 + m1.d1 * m2.a4;
					this->a2 = m1.a2 * m2.a1 + m1.b2 * m2.a2 + m1.c2 * m2.a3 + m1.d2 * m2.a4;
					this->a3 = m1.a3 * m2.a1 + m1.b3 * m2.a2 + m1.c3 * m2.a3 + m1.d3 * m2.a4;
					this->a4 = m1.a4 * m2.a1 + m1.b4 * m2.a2 + m1.c4 * m2.a3 + m1.d4 * m2.a4;

					this->b1 = m1.a1 * m2.b1 + m1.b1 * m2.b2 + m1.c1 * m2.b3 + m1.d1 * m2.b4;
					this->b2 = m1.a2 * m2.b1 + m1.b2 * m2.b2 + m1.c2 * m2.b3 + m1.d2 * m2.b4;
					this->b3 = m1.a3 * m2.b1 + m1.b3 * m2.b2 + m1.c3 * m2.b3 + m1.d3 * m2.b4;
					this->b4 = m1.a4 * m2.b1 + m1.b4 * m2.b2 + m1.c4 * m2.b3 + m1.d4 * m2.b4;

					this->c1 = m1.a1 * m2.c1 + m1.b1 * m2.c2 + m1.c1 * m2.c3 + m1.d1 * m2.c4;
					this->c2 = m1.a2 * m2.c1 + m1.b2 * m2.c2 + m1.c2 * m2.c3 + m1.d2 * m2.c4;
					this->c3 = m1.a3 * m2.c1 + m1.b3 * m2.c2 + m1.c3 * m2.c3 + m1.d3 * m2.c4;
					this->c4 = m1.a4 * m2.c1 + m1.b4 * m2.c2 + m1.c4 * m2.c3 + m1.d4 * m2.c4;

					this->d1 = m1.a1 * m2.d1 + m1.b1 * m2.d2 + m1.c1 * m2.d3 + m1.d1 * m2.d4;
					this->d2 = m1.a2 * m2.d1 + m1.b2 * m2.d2 + m1.c2 * m2.d3 + m1.d2 * m2.d4;
					this->d3 = m1.a3 * m2.d1 + m1.b3 * m2.d2 + m1.c3 * m2.d3 + m1.d3 * m2.d4;
					this->d4 = m1.a4 * m2.d1 + m1.b4 * m2.d2 + m1.c4 * m2.d3 + m1.d4 * m2.d4;
					return *this;
				}

				T determinant() const noexcept
				{
					return a1 * b2*c3*d4 - a1 * b2*c4*d3 + a1 * b3*c4*d2 - a1 * b3*c2*d4
						+ a1 * b4*c2*d3 - a1 * b4*c3*d2 - a2 * b3*c4*d1 + a2 * b3*c1*d4
						- a2 * b4*c1*d3 + a2 * b4*c3*d1 - a2 * b1*c3*d4 + a2 * b1*c4*d3
						+ a3 * b4*c1*d2 - a3 * b4*c2*d1 + a3 * b1*c2*d4 - a3 * b1*c4*d2
						+ a3 * b2*c4*d1 - a3 * b2*c1*d4 - a4 * b1*c2*d3 + a4 * b1*c3*d2
						- a4 * b2*c3*d1 + a4 * b2*c1*d3 - a4 * b3*c1*d2 + a4 * b3*c2*d1;
				}

				Matrix4x4<T>& applyMatrix(const Matrix4x4<T>& m) noexcept
				{
					Matrix4x4 matrix(*this);
					makeMatrix(matrix, m);
					return *this;
				}

				Matrix4x4<T>& makeTranslate(const Vector2<T>& pt) noexcept { return makeTranslate(pt.x, pt.y, 0); }
				Matrix4x4<T>& makeTranslate(const Vector3<T>& pt) noexcept { return makeTranslate(pt.x, pt.y, pt.z); }
				Matrix4x4<T>& makeTranslate(T x, T y) noexcept { return makeTranslate(x, y, 0); }
				Matrix4x4<T>& makeTranslate(T x, T y, T z) noexcept
				{
					makeMatrix(1, 0, 0, 0,
								0, 1, 0, 0,
								0, 0, 1, 0,
								x, y, z, 1);
					return *this;
				}

				Matrix4x4<T>& setTranslate(const Vector2<T>& pt) noexcept { return setTranslate(pt.x, pt.y); }
				Matrix4x4<T>& setTranslate(const Vector3<T>& pt) noexcept { return setTranslate(pt.x, pt.y, pt.z); }
				Matrix4x4<T>& setTranslate(T x, T y) noexcept
				{
					d1 = x;
					d2 = y;
					return *this;
				}

				Matrix4x4<T>& setTranslate(T x, T y, T z) noexcept
				{
					d1 = x;
					d2 = y;
					d3 = z;
					return *this;
				}

				Matrix4x4<T>& translate(const Vector2<T>& pt) noexcept { return translate(pt.x, pt.y, 0); }
				Matrix4x4<T>& translate(const Vector3<T>& pt) noexcept { return translate(pt.x, pt.y, pt.z); }
				Matrix4x4<T>& translate(T x, T y) noexcept
				{
					d1 += x;
					d2 += y;
					return *this;
				}

				Matrix4x4<T>& translate(T x, T y, T z) noexcept
				{
					d1 += x;
					d2 += y;
					d3 += z;
					return *this;
				}

				const Vector3<T>& get_translate() const noexcept
				{
					return *(Vector3<T>*)&d1;
				}

				Matrix4x4<T>& makeScale(const Vector2<T>& sz) noexcept { return makeScale(sz.x, sz.y, 1.0f); }
				Matrix4x4<T>& makeScale(const Vector3<T>& sz) noexcept { return makeScale(sz.x, sz.y, sz.z); }
				Matrix4x4<T>& makeScale(T cx, T cy) noexcept { return makeScale(cx, cy, 1.0f); }
				Matrix4x4<T>& makeScale(T x, T y, T z) noexcept
				{
					set(
						x, 0, 0, 0,
						0, y, 0, 0,
						0, 0, z, 0,
						0, 0, 0, 1);
					return *this;
				}

				Matrix4x4<T>& scale(const Vector2<T>& sz) noexcept { return scale(sz.x, sz.y, 1.0f); }
				Matrix4x4<T>& scale(const Vector3<T>& sz) noexcept { return scale(sz.x, sz.y, sz.z); }
				Matrix4x4<T>& scale(T v) noexcept { return scale(v, v, v); }
				Matrix4x4<T>& scale(T cx, T cy) noexcept { return scale(cx, cy, 1.0f); }
				Matrix4x4<T>& scale(T x, T y, T z) noexcept
				{
					a1 *= x; a2 *= x; a3 *= x;
					b1 *= y; b2 *= y; b3 *= y;
					c1 *= z; c2 *= z; c3 *= z;

					return *this;
				}

				Vector3<T> getScale() const noexcept
				{
					Vector3<T> vRows[3] =
					{
						Vector3<T>(this->a1, this->a2, this->a3),
						Vector3<T>(this->b1, this->b2, this->b3),
						Vector3<T>(this->c1, this->c2, this->c3)
					};

					Vector3<T> scaling;
					scaling.x = vRows[0].length();
					scaling.y = vRows[1].length();
					scaling.z = vRows[2].length();

					if (determinant() < 0)
					{
						scaling.x = -scaling.x;
						scaling.y = -scaling.y;
						scaling.z = -scaling.z;
					}

					return scaling;
				}

				Matrix4x4<T>& makeRotationX(T theta) noexcept
				{
					T c, s;
					sinCos(&s, &c, theta);

					a1 = 1; a2 = 0; a3 = 0; a4 = 0;
					b1 = 0; b2 = c; b3 = s; b4 = 0;
					c1 = 0; c2 =-s; c3 = c; c4 = 0;
					d1 = 0; d2 = 0; d3 = 0; c4 = 1;

					return *this;
				}

				Matrix4x4<T>& makeRotationY(T theta) noexcept
				{
					T c, s;
					sinCos(&s, &c, theta);

					a1 = c; a2 = 0; a3 =-s; a4 = 0;
					b1 = 0; b2 = 1; b3 = 0; b4 = 0;
					c1 = s; c2 = 0; c3 = c; c4 = 0;
					d1 = 0; d2 = 0; d3 = 0; c4 = 1;

					return *this;
				}

				Matrix4x4<T>& makeRotationZ(T theta) noexcept
				{
					T c, s;
					sinCos(&s, &c, theta);

					a1 = c; a2 = s; a3 = 0; a4 = 0;
					b1 =-s; b2 = c; b3 = 0; b4 = 0;
					c1 = 0; c2 = 0; c3 = 1; c4 = 0;
					d1 = 0; d2 = 0; d3 = 0; c4 = 1;

					return *this;
				}

				Matrix4x4<T>& makeRotation(const Vector3<T>& axis, T theta, const Vector3<T>& translate = Vector3<T>::Zero) noexcept
				{
					T c, s;
					sinCos(&s, &c, theta);

					T x = axis.x;
					T y = axis.y;
					T z = axis.z;

					T t = 1 - c;
					T tx = t * x;
					T ty = t * y;
					T tz = t * z;

					a1 = (tx * x + c);
					a2 = (tx * y + s * z);
					a3 = (tx * z - s * y);
					a4 = 0;

					b1 = (tx * y - s * z);
					b2 = (ty * y + c);
					b3 = (ty * z + s * x);
					b4 = 0;

					c1 = (tx * z + s * y);
					c2 = (ty * z - s * x);
					c3 = (tz * z + c);
					c4 = 0;

					d1 = translate.x;
					d2 = translate.y;
					d3 = translate.z;
					d4 = T(1.0f);

					return *this;
				}

				Matrix4x4<T>& makeRotation(const Quaternion<T>& q, const Vector3<T>& translate = Vector3<T>::Zero) noexcept
				{
					T xs = q.x * T(2.0f), ys = q.y * T(2.0f), zs = q.z * T(2.0f);
					T wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
					T xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
					T yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;

					a1 = T(1.0f) - (yy + zz);
					a2 = xy + wz;
					a3 = xz - wy;
					a4 = T(0.0f);

					b1 = xy - wz;
					b2 = T(1.0f) - (xx + zz);
					b3 = yz + wx;
					b4 = T(0.0f);

					c1 = xz + wy;
					c2 = yz - wx;
					c3 = T(1.0f) - (xx + yy);
					c4 = T(0.0f);

					d1 = translate.x;
					d2 = translate.y;
					d3 = translate.z;
					d4 = T(1.0f);

					return *this;
				}

				Matrix4x4<T>& makeRotation(const Vector3<T>& forward_, const Vector3<T>& up_, const Vector3<T>& right_, const Vector3<T>& translate_ = Vector3<T>::Zero) noexcept
				{
					this->right = Vector4<T>(right_, 0.0);
					this->up = Vector4<T>(up_, 0.0);
					this->forward = Vector4<T>(forward_, 0.0);
					this->position = Vector4<T>(translate_, 1.0);

					return *this;
				}

				Matrix4x4<T>& makeRotation(const Vector4<T>& forward_, const Vector4<T>& up_, const Vector4<T>& right_, const Vector4<T>& translate_ = Vector4<T>::UnitW) noexcept
				{
					this->right = right_;
					this->up = up_;
					this->forward = forward_;
					this->position = translate_;

					return *this;
				}

				Quaternion<T> getRotate() const noexcept
				{
					const Vector3<T>& right = this->getRight();
					const Vector3<T>& up = this->getUp();
					const Vector3<T>& forward = this->getForward();

					Vector3<T> scaling;
					scaling.x = length(right);
					scaling.y = length(up);
					scaling.z = length(forward);

					T det = (a1 * b2 - a2 * b1) * (c3)-(a1 * b3 - a3 * b1) * (c2)+(a2 * b3 - a3 * b2) * (c1);
					if (det < 0)
					{
						scaling.x = -scaling.x;
						scaling.y = -scaling.y;
						scaling.z = -scaling.z;
					}

					if (scaling.x != T(0.0)) right /= scaling.x;
					if (scaling.y != T(0.0)) up /= scaling.y;
					if (scaling.z != T(0.0)) forward /= scaling.z;

					return Quaternion<T>(forward, up, right);
				}

				Matrix4x4<T>& makeTransform(const Vector3<T>& translate, const Quaternion<T>& rotate) noexcept
				{
					return this->makeRotation(rotate, translate);
				}

				Matrix4x4<T>& makeTransform(const Vector3<T>& translate, const Quaternion<T>& rotate, const Vector3<T>& scale) noexcept
				{
					this->makeTransform(translate, rotate);
					this->scale(scale);
					return *this;
				}

				const Matrix4x4<T>& getTransform(Vector3<T>& translate, Quaternion<T>& rotation, Vector3<T>& scaling) const noexcept
				{
					auto right_ = this->getRight();
					auto up_ = this->getUp();
					auto forward_ = this->getForward();

					translate.x = this->d1;
					translate.y = this->d2;
					translate.z = this->d3;

					scaling.x = length(right_);
					scaling.y = length(up_);
					scaling.z = length(forward_);

					T det = (this->a1 * this->b2 - this->a2 * this->b1) * (this->c3) -
						(this->a1 * this->b3 - this->a3 * this->b1) * (this->c2) +
						(this->a2 * this->b3 - this->a3 * this->b2) * (this->c1);

					if (det < 0)
					{
						scaling.x = -scaling.x;
						scaling.y = -scaling.y;
						scaling.z = -scaling.z;
					}

					if (scaling.x != T(0.0)) right_ /= scaling.x;
					if (scaling.y != T(0.0)) up_ /= scaling.y;
					if (scaling.z != T(0.0)) forward_ /= scaling.z;

					rotation.makeRotation(forward_, up_, right_);
					return *this;
				}

				const Matrix4x4<T>& getTransformWithoutScaler(Vector3<T>& translate, Quaternion<T>& rotation) const noexcept
				{
					translate.x = this->d1;
					translate.y = this->d2;
					translate.z = this->d3;

					rotation.makeRotation(this->getForward(), this->getUp(), this->getRight());
					return *this;
				}

				const Vector3<T>& getRight() const noexcept
				{
					return *(Vector3<T>*)&a1;
				}

				const Vector3<T>& getUp() const noexcept
				{
					return *(Vector3<T>*)&b1;
				}

				const Vector3<T>& getForward() const noexcept
				{
					return *(Vector3<T>*)&c1;
				}

				Vector4<T> getAxisX() const noexcept
				{
					return Vector4<T>(a1, b1, c1, d1);
				}

				Vector4<T> getAxisY() const noexcept
				{
					return Vector4<T>(a2, b2, c2, d2);
				}

				Vector4<T> getAxisZ() const noexcept
				{
					return Vector4<T>(a3, b3, c3, d3);
				}

				Vector4<T> getAxisW() const noexcept
				{
					return Vector4<T>(a4, b4, c4, d4);
				}

				pointer ptr() noexcept { return (pointer)&a1; }
				const_pointer ptr() const noexcept { return (const_pointer)&a1; }

				pointer data() noexcept { return (pointer)&a1; }
				const_pointer data() const noexcept { return (const_pointer)&a1; }

			public:

				friend bool operator==(const Matrix4x4<T>& m1, const Matrix4x4<T>& m2) noexcept
				{
					return
						m1.a1 == m2.a1 && m1.a2 == m2.a2 && m1.a3 == m2.a3 && m1.a4 == m2.a4 &&
						m1.b1 == m2.b1 && m1.b2 == m2.b2 && m1.b3 == m2.b3 && m1.b4 == m2.b4 &&
						m1.c1 == m2.c1 && m1.c2 == m2.c2 && m1.c3 == m2.c3 && m1.c4 == m2.c4 &&
						m1.d1 == m2.d1 && m1.d2 == m2.d2 && m1.d3 == m2.d3 && m1.d4 == m2.c4;
				}

				friend bool operator!=(const Matrix4x4<T>& m1, const Matrix4x4<T>& m2) noexcept
				{
					return
						m1.a1 != m2.a1 || m1.a2 != m2.a2 || m1.a3 != m2.a3 || m1.a4 != m2.a4 ||
						m1.b1 != m2.b1 || m1.b2 != m2.b2 || m1.b3 != m2.b3 || m1.b4 != m2.b4 ||
						m1.c1 != m2.c1 || m1.c2 != m2.c2 || m1.c3 != m2.c3 || m1.c4 != m2.c4 ||
						m1.d1 != m2.d1 || m1.d2 != m2.d2 || m1.d3 != m2.d3 || m1.d4 != m2.c4;
				}

				friend Matrix4x4<T> operator*(const Matrix4x4<T>& m1, const Matrix4x4<T>& m2) noexcept
				{
					return Matrix4x4<T>(m1, m2);
				}

				friend Vector3<T> operator*(const Vector3<T>& v, const Matrix4x4<T>& m) noexcept
				{
					T d = 1.0f / (m.d1 * v.x + m.d2 * v.y + m.d3 * v.z + m.d4);
					return Vector3<T>(
						(v.x * m.a1 + v.y * m.a2 + v.z * m.a3 + m.a4) * d,
						(v.x * m.b1 + v.y * m.b2 + v.z * m.b3 + m.b4) * d,
						(v.x * m.c1 + v.y * m.c2 + v.z * m.c3 + m.c4) * d);
				}

				friend Vector3<T> operator*(const Matrix4x4<T>& m, const Vector3<T>& v) noexcept
				{
					T d = 1.0f / (m.a4 * v.x + m.b4 * v.y + m.c4 * v.z + m.d4);
					return Vector3<T>(
						(v.x * m.a1 + v.y * m.b1 + v.z * m.c1 + m.d1) * d,
						(v.x * m.a2 + v.y * m.b2 + v.z * m.c2 + m.d2) * d,
						(v.x * m.a3 + v.y * m.b3 + v.z * m.c3 + m.d3) * d);
				}

				friend Vector3<T>& operator*=(const Matrix4x4<T>& m, Vector3<T>& v) noexcept
				{
					v = m * v;
					return v;
				}

				friend Vector3<T>& operator*=(Vector3<T>& v, const Matrix4x4<T>& m) noexcept
				{
					v = v * m;
					return v;
				}

				friend Vector4<T> operator*(const Vector4<T>& v, const Matrix4x4<T>& m) noexcept
				{
					return Vector4<T>(
						m.a1 * v.x + m.a2 * v.y + m.a3 * v.z + v.w * m.a4,
						m.b1 * v.x + m.b2 * v.y + m.b3 * v.z + v.w * m.b4,
						m.c1 * v.x + m.c2 * v.y + m.c3 * v.z + v.w * m.c4,
						m.d1 * v.x + m.d2 * v.y + m.d3 * v.z + v.w * m.d4);
				}

				friend Vector4<T> operator*(const Matrix4x4<T>& m, const Vector4<T>& v) noexcept
				{
					return Vector4<T>(
						m.a1 * v.x + m.b1 * v.y + m.c1 * v.z + m.d1 * v.w,
						m.a2 * v.x + m.b2 * v.y + m.c2 * v.z + m.d2 * v.w,
						m.a3 * v.x + m.b3 * v.y + m.c3 * v.z + m.d3 * v.w,
						m.a4 * v.x + m.b4 * v.y + m.c4 * v.z + m.d4 * v.w);
				}

				friend Vector4<T>& operator*=(Vector4<T>& v, const Matrix4x4<T>& m) noexcept
				{
					v = v * m;
					return v;
				}

				friend Matrix4x4<T>& operator*=(Matrix4x4<T>& m1, const Matrix4x4<T>& m2) noexcept
				{
					m1.makeMatrix(m1, m2);
					return m1;
				}
			};

			template<typename T> const Matrix4x4<T> Matrix4x4<T>::Zero(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
			template<typename T> const Matrix4x4<T> Matrix4x4<T>::One(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		}

		template<typename T>
		inline T determinant(const detail::Matrix4x4<T>& m) noexcept
		{
			return m.a1 * m.b2*m.c3*m.d4 - m.a1 * m.b2*m.c4*m.d3 + m.a1 * m.b3*m.c4*m.d2 - m.a1 * m.b3*m.c2*m.d4
				 + m.a1 * m.b4*m.c2*m.d3 - m.a1 * m.b4*m.c3*m.d2 - m.a2 * m.b3*m.c4*m.d1 + m.a2 * m.b3*m.c1*m.d4
				 - m.a2 * m.b4*m.c1*m.d3 + m.a2 * m.b4*m.c3*m.d1 - m.a2 * m.b1*m.c3*m.d4 + m.a2 * m.b1*m.c4*m.d3
				 + m.a3 * m.b4*m.c1*m.d2 - m.a3 * m.b4*m.c2*m.d1 + m.a3 * m.b1*m.c2*m.d4 - m.a3 * m.b1*m.c4*m.d2
				 + m.a3 * m.b2*m.c4*m.d1 - m.a3 * m.b2*m.c1*m.d4 - m.a4 * m.b1*m.c2*m.d3 + m.a4 * m.b1*m.c3*m.d2
				 - m.a4 * m.b2*m.c3*m.d1 + m.a4 * m.b2*m.c1*m.d3 - m.a4 * m.b3*m.c1*m.d2 + m.a4 * m.b3*m.c2*m.d1;
		}

		template<typename T>
		inline const detail::Vector3<T>& right(const detail::Matrix4x4<T>& m) noexcept
		{
			return *(detail::Vector3<T>*)&m.a1;
		}

		template<typename T>
		inline const detail::Vector3<T>& up(const detail::Matrix4x4<T>& m) noexcept
		{
			return *(detail::Vector3<T>*)&m.b1;
		}

		template<typename T>
		inline const detail::Vector3<T>& forward(const detail::Matrix4x4<T>& m) noexcept
		{
			return *(detail::Vector3<T>*)&m.c1;
		}

		template<typename T>
		inline bool isIdentity(const detail::Matrix4x4<T>& m) noexcept
		{
			constexpr T epsilon = static_cast<T>(EPSILON_E4);
			return (
				m.a2 <= epsilon && m.a2 >= -epsilon &&
				m.a3 <= epsilon && m.a3 >= -epsilon &&
				m.b1 <= epsilon && m.b1 >= -epsilon &&
				m.b3 <= epsilon && m.b3 >= -epsilon &&
				m.c1 <= epsilon && m.c1 >= -epsilon &&
				m.c2 <= epsilon && m.c2 >= -epsilon &&
				m.a4 <= epsilon && m.a4 >= -epsilon &&
				m.b4 <= epsilon && m.b4 >= -epsilon &&
				m.c4 <= epsilon && m.c4 >= -epsilon &&
				m.a1 <= 1.f + epsilon && m.a1 >= 1.f - epsilon &&
				m.b2 <= 1.f + epsilon && m.b2 >= 1.f - epsilon &&
				m.c3 <= 1.f + epsilon && m.c3 >= 1.f - epsilon);
		}

		template<typename T>
		inline bool isOnlyTranslate(const detail::Matrix4x4<T>& m) noexcept
		{
			constexpr T epsilon = static_cast<T>(EPSILON_E4);
			return (
				m.a2 <= epsilon && m.a2 >= -epsilon &&
				m.a3 <= epsilon && m.a3 >= -epsilon &&
				m.b1 <= epsilon && m.b1 >= -epsilon &&
				m.b3 <= epsilon && m.b3 >= -epsilon &&
				m.c1 <= epsilon && m.c1 >= -epsilon &&
				m.c2 <= epsilon && m.c2 >= -epsilon &&
				m.a1 <= 1.f + epsilon && m.a1 >= 1.f - epsilon &&
				m.b2 <= 1.f + epsilon && m.b2 >= 1.f - epsilon &&
				m.c3 <= 1.f + epsilon && m.c3 >= 1.f - epsilon);
		}

		template<typename T>
		inline bool isOnlyRotate(const detail::Matrix4x4<T>& m) noexcept
		{
			constexpr T epsilon = 10e-3f;
			return (
				m.a1 <= 1.f + epsilon && m.a1 >= 1.f - epsilon &&
				m.a2 <= 1.f + epsilon && m.a2 >= 1.f - epsilon &&
				m.a3 <= 1.f + epsilon && m.a3 >= 1.f - epsilon &&
				m.b1 <= 1.f + epsilon && m.b1 >= 1.f - epsilon &&
				m.b2 <= 1.f + epsilon && m.b2 >= 1.f - epsilon &&
				m.b3 <= 1.f + epsilon && m.b3 >= 1.f - epsilon &&
				m.c1 <= 1.f + epsilon && m.c1 >= 1.f - epsilon &&
				m.c2 <= 1.f + epsilon && m.c2 >= 1.f - epsilon &&
				m.c3 <= 1.f + epsilon && m.c3 >= 1.f - epsilon);
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> orthonormalize(const detail::Matrix4x4<T>& m) noexcept
		{
			detail::Vector3<T> x = m.getRight();
			detail::Vector3<T> y = m.getUp();
			detail::Vector3<T> z;

			x = normalize(x);
			z = cross(x, y);
			z = normalize(z);
			y = cross(z, x);
			y = normalize(y);

			return detail::Matrix4x4<T>(
				x.x, x.y, x.z, 0.0f,
				y.x, y.y, y.z, 0.0f,
				z.x, z.y, z.z, 0.0f,
				m.d1, m.d2, m.d3, 1.0f);
		}

		template<typename T>
		inline detail::Matrix4x4<T> transpose(const detail::Matrix4x4<T>& _m) noexcept
		{
			detail::Matrix4x4<T> m = _m;
			std::swap(m.b1, m.a2);
			std::swap(m.c1, m.a3);
			std::swap(m.c2, m.b3);
			std::swap(m.d1, m.a4);
			std::swap(m.d2, m.b4);
			std::swap(m.d3, m.c4);

			return m;
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> inverse(const detail::Matrix4x4<T>& _m) noexcept
		{
			detail::Matrix4x4<T> m;

			const T det = _m.determinant();
			if (det == static_cast<T>(0.0))
			{
				const T nan = std::numeric_limits<T>::quiet_NaN();
				m.makeMatrix
				(
					nan, nan, nan, nan,
					nan, nan, nan, nan,
					nan, nan, nan, nan,
					nan, nan, nan, nan
				);

				return m;
			}

			const T invdet = static_cast<T>(1.0) / det;
			m.a1 = invdet * (_m.b2 * (_m.c3 * _m.d4 - _m.c4 * _m.d3) + _m.b3 * (_m.c4 * _m.d2 - _m.c2 * _m.d4) + _m.b4 * (_m.c2 * _m.d3 - _m.c3 * _m.d2));
			m.a2 = -invdet * (_m.a2 * (_m.c3 * _m.d4 - _m.c4 * _m.d3) + _m.a3 * (_m.c4 * _m.d2 - _m.c2 * _m.d4) + _m.a4 * (_m.c2 * _m.d3 - _m.c3 * _m.d2));
			m.a3 = invdet * (_m.a2 * (_m.b3 * _m.d4 - _m.b4 * _m.d3) + _m.a3 * (_m.b4 * _m.d2 - _m.b2 * _m.d4) + _m.a4 * (_m.b2 * _m.d3 - _m.b3 * _m.d2));
			m.a4 = -invdet * (_m.a2 * (_m.b3 * _m.c4 - _m.b4 * _m.c3) + _m.a3 * (_m.b4 * _m.c2 - _m.b2 * _m.c4) + _m.a4 * (_m.b2 * _m.c3 - _m.b3 * _m.c2));
			m.b1 = -invdet * (_m.b1 * (_m.c3 * _m.d4 - _m.c4 * _m.d3) + _m.b3 * (_m.c4 * _m.d1 - _m.c1 * _m.d4) + _m.b4 * (_m.c1 * _m.d3 - _m.c3 * _m.d1));
			m.b2 = invdet * (_m.a1 * (_m.c3 * _m.d4 - _m.c4 * _m.d3) + _m.a3 * (_m.c4 * _m.d1 - _m.c1 * _m.d4) + _m.a4 * (_m.c1 * _m.d3 - _m.c3 * _m.d1));
			m.b3 = -invdet * (_m.a1 * (_m.b3 * _m.d4 - _m.b4 * _m.d3) + _m.a3 * (_m.b4 * _m.d1 - _m.b1 * _m.d4) + _m.a4 * (_m.b1 * _m.d3 - _m.b3 * _m.d1));
			m.b4 = invdet * (_m.a1 * (_m.b3 * _m.c4 - _m.b4 * _m.c3) + _m.a3 * (_m.b4 * _m.c1 - _m.b1 * _m.c4) + _m.a4 * (_m.b1 * _m.c3 - _m.b3 * _m.c1));
			m.c1 = invdet * (_m.b1 * (_m.c2 * _m.d4 - _m.c4 * _m.d2) + _m.b2 * (_m.c4 * _m.d1 - _m.c1 * _m.d4) + _m.b4 * (_m.c1 * _m.d2 - _m.c2 * _m.d1));
			m.c2 = -invdet * (_m.a1 * (_m.c2 * _m.d4 - _m.c4 * _m.d2) + _m.a2 * (_m.c4 * _m.d1 - _m.c1 * _m.d4) + _m.a4 * (_m.c1 * _m.d2 - _m.c2 * _m.d1));
			m.c3 = invdet * (_m.a1 * (_m.b2 * _m.d4 - _m.b4 * _m.d2) + _m.a2 * (_m.b4 * _m.d1 - _m.b1 * _m.d4) + _m.a4 * (_m.b1 * _m.d2 - _m.b2 * _m.d1));
			m.c4 = -invdet * (_m.a1 * (_m.b2 * _m.c4 - _m.b4 * _m.c2) + _m.a2 * (_m.b4 * _m.c1 - _m.b1 * _m.c4) + _m.a4 * (_m.b1 * _m.c2 - _m.b2 * _m.c1));
			m.d1 = -invdet * (_m.b1 * (_m.c2 * _m.d3 - _m.c3 * _m.d2) + _m.b2 * (_m.c3 * _m.d1 - _m.c1 * _m.d3) + _m.b3 * (_m.c1 * _m.d2 - _m.c2 * _m.d1));
			m.d2 = invdet * (_m.a1 * (_m.c2 * _m.d3 - _m.c3 * _m.d2) + _m.a2 * (_m.c3 * _m.d1 - _m.c1 * _m.d3) + _m.a3 * (_m.c1 * _m.d2 - _m.c2 * _m.d1));
			m.d3 = -invdet * (_m.a1 * (_m.b2 * _m.d3 - _m.b3 * _m.d2) + _m.a2 * (_m.b3 * _m.d1 - _m.b1 * _m.d3) + _m.a3 * (_m.b1 * _m.d2 - _m.b2 * _m.d1));
			m.d4 = invdet * (_m.a1 * (_m.b2 * _m.c3 - _m.b3 * _m.c2) + _m.a2 * (_m.b3 * _m.c1 - _m.b1 * _m.c3) + _m.a3 * (_m.b1 * _m.c2 - _m.b2 * _m.c1));

			return m;
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline void decomposeTransform(const detail::Matrix4x4<T>& m, detail::Vector3<T>& translate, detail::Quaternion<T>& rotation, detail::Vector3<T>& scaling) noexcept
		{
			auto right_ = m.getRight();
			auto up_ = m.getUp();
			auto forward_ = m.getForward();

			translate.x = m.d1;
			translate.y = m.d2;
			translate.z = m.d3;

			scaling.x = length(right_);
			scaling.y = length(up_);
			scaling.z = length(forward_);

			T det = (m.a1 * m.b2 - m.a2 * m.b1) * (m.c3) - (m.a1 * m.b3 - m.a3 * m.b1) * (m.c2) + (m.a2 * m.b3 - m.a3 * m.b2) * (m.c1);
			if (det < 0)
			{
				scaling.x = -scaling.x;
				scaling.y = -scaling.y;
				scaling.z = -scaling.z;
			}

			if (scaling.x != T(0.0)) right_ /= scaling.x;
			if (scaling.y != T(0.0)) up_ /= scaling.y;
			if (scaling.z != T(0.0)) forward_ /= scaling.z;

			rotation.makeRotation(forward_, up_, right_);
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline void decomposeTransformWithoutScaler(const detail::Matrix4x4<T>& m, detail::Vector3<T>& translate, detail::Quaternion<T>& rotation) noexcept
		{
			assert(isOnlyRotate(m));

			translate.x = m.d1;
			translate.y = m.d2;
			translate.z = m.d3;

			rotation.makeRotation(m.getForward(), m.getUp(), m.getRight());
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline bool decomposeFrustumtRH(const detail::Matrix4x4<T>& m, T& left, T& right, T& bottom, T& top, T& znear, T& zfar) noexcept
		{
			if (m.a4 != 0.0 || m.b4 != 0.0 || m.c4 != -1.0 || m.c4 != 0.0)
				return false;

			T temp_near = m.d3 / (m.c3 - 1.0f);
			T temp_far = m.d3 / (1.0f + m.c3);

			left = temp_near * (m.c1 - 1.0f) / m.a1;
			right = temp_near * (1.0f + m.c1) / m.a1;

			top = temp_near * (1.0f + m.c2) / m.b2;
			bottom = temp_near * (m.c2 - 1.0f) / m.b2;

			znear = temp_near;
			zfar = temp_far;

			return true;
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline bool decomposeOrthoRH(const detail::Matrix4x4<T>& m, T& left, T& right, T& bottom, T& top, T& znear, T& zfar) noexcept
		{
			if (m.a4 != 0.0 || m.b4 != 0.0 || m.c4 != 0.0 || m.c4 != 1.0) return false;

			znear = (m.d3 + 1.0f) / m.c3;
			zfar = (m.d3 - 1.0f) / m.c3;

			left = -(1.0f + m.d1) / m.a1;
			right = (1.0f - m.d1) / m.a1;

			bottom = -(1.0f + m.d2) / m.b2;
			top = (1.0f - m.d2) / m.b2;

			return true;
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline bool decomposePerspectiveRH(const detail::Matrix4x4<T>& m, T& fovy, T& aspectRatio, T& znear, T& zfar) noexcept
		{
			T right = 0;
			T left = 0;
			T top = 0;
			T bottom = 0;

			T temp_near = 0;
			T temp_far = 0;

			bool r = decomposeFrustumtRH(m, left, right, bottom, top, temp_near, temp_far);
			if (r)
			{
				fovy = degrees(atan(top / temp_near) - atan(bottom / temp_near));
				aspectRatio = (right - left) / (top - bottom);
			}

			znear = temp_near;
			zfar = temp_far;
			return r;
		}

		template<typename T>
		inline detail::Vector3<T> invTranslateVector3(const detail::Matrix4x4<T>& m, const detail::Vector3<T>& v)
		{
			detail::Vector3<T> temp, result;
			temp.x = v.x - m.d1;
			temp.y = v.y - m.d2;
			temp.z = v.z - m.d3;

			result.x = temp.x * m.a1 + temp.y * m.a2 + temp.z * m.a3;
			result.y = temp.x * m.b1 + temp.y * m.b2 + temp.z * m.b3;
			result.z = temp.x * m.c1 + temp.y * m.c2 + temp.z * m.c3;
			return result;
		}

		template<typename T>
		inline detail::Vector3<T> invRotateVector3(const detail::Matrix4x4<T>& m, const detail::Vector3<T>& v)
		{
			detail::Vector3<T> result;
			result.x = v.x * m.a1 + v.y * m.a2 + v.z * m.a3;
			result.y = v.x * m.b1 + v.y * m.b2 + v.z * m.b3;
			result.z = v.x * m.c1 + v.y * m.c2 + v.z * m.c3;

			return result;
		}

		template<typename T>
		inline detail::Matrix4x4<T> transformMultiply(const detail::Matrix4x4<T>& m1, const detail::Matrix4x4<T>& m2)
		{
			detail::Matrix4x4<T> out;
			out.a1 = m1.a1 * m2.a1 + m1.b1 * m2.a2 + m1.c1 * m2.a3;
			out.a2 = m1.a2 * m2.a1 + m1.b2 * m2.a2 + m1.c2 * m2.a3;
			out.a3 = m1.a3 * m2.a1 + m1.b3 * m2.a2 + m1.c3 * m2.a3;
			out.a4 = 0;

			out.b1 = m1.a1 * m2.b1 + m1.b1 * m2.b2 + m1.c1 * m2.b3;
			out.b2 = m1.a2 * m2.b1 + m1.b2 * m2.b2 + m1.c2 * m2.b3;
			out.b3 = m1.a3 * m2.b1 + m1.b3 * m2.b2 + m1.c3 * m2.b3;
			out.b4 = 0;

			out.c1 = m1.a1 * m2.c1 + m1.b1 * m2.c2 + m1.c1 * m2.c3;
			out.c2 = m1.a2 * m2.c1 + m1.b2 * m2.c2 + m1.c2 * m2.c3;
			out.c3 = m1.a3 * m2.c1 + m1.b3 * m2.c2 + m1.c3 * m2.c3;
			out.c4 = 0;

			out.d1 = m1.a1 * m2.d1 + m1.b1 * m2.d2 + m1.c1 * m2.d3 + m1.d1;
			out.d2 = m1.a2 * m2.d1 + m1.b2 * m2.d2 + m1.c2 * m2.d3 + m1.d2;
			out.d3 = m1.a3 * m2.d1 + m1.b3 * m2.d2 + m1.c3 * m2.d3 + m1.d3;
			out.d4 = 1;

			return out;
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> transformInverse(const detail::Matrix4x4<T>& m) noexcept
		{
			detail::Matrix4x4<T> out;

			T det = (m.a1 * m.b2 - m.a2 * m.b1) * (m.c3) - (m.a1 * m.b3 - m.a3 * m.b1) * (m.c2) + (m.a2 * m.b3 - m.a3 * m.b2) * (m.c1);
			if (det == static_cast<T>(0.0))
				return detail::Matrix4x4<T>::One;

			T invdet = 1.0f / det;

			out.a1 = invdet * (m.b2 * m.c3 + m.b3 * -m.c2);
			out.a2 = invdet * (m.c2 * m.a3 + m.c3 * -m.a2);
			out.a3 = invdet * (m.a2 * m.b3 - m.a3 * m.b2);
			out.a4 = 0.0f;
			out.b1 = invdet * (m.b3 * m.c1 + m.b1 * -m.c3);
			out.b2 = invdet * (m.c3 * m.a1 + m.c1 * -m.a3);
			out.b3 = invdet * (m.a3 * m.b1 - m.a1 * m.b3);
			out.b4 = 0.0f;
			out.c1 = invdet * (m.b1 * m.c2 + m.b2 * -m.c1);
			out.c2 = invdet * (m.c1 * m.a2 + m.c2 * -m.a1);
			out.c3 = invdet * (m.a1 * m.b2 - m.a2 * m.b1);
			out.c4 = 0.0f;
			out.d1 = invdet * (m.b1 * (m.c3 * m.d2 - m.c2 * m.d3) + m.b2 * (m.c1 * m.d3 - m.c3 * m.d1) + m.b3 * (m.c2 * m.d1 - m.c1 * m.d2));
			out.d2 = invdet * (m.c1 * (m.a3 * m.d2 - m.a2 * m.d3) + m.c2 * (m.a1 * m.d3 - m.a3 * m.d1) + m.c3 * (m.a2 * m.d1 - m.a1 * m.d2));
			out.d3 = invdet * (m.d1 * (m.a3 * m.b2 - m.a2 * m.b3) + m.d2 * (m.a1 * m.b3 - m.a3 * m.b1) + m.d3 * (m.a2 * m.b1 - m.a1 * m.b2));
			out.d4 = invdet * (m.a1 * (m.b2 * m.c3 - m.b3 * m.c2) + m.a2 * (m.b3 * m.c1 - m.b1 * m.c3) + m.a3 * (m.b1 * m.c2 - m.b2 * m.c1));

			return out;
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> transformInverseWithoutScaler(const detail::Matrix4x4<T>& m) noexcept
		{
			auto& right = m.getRight();
			auto& up = m.getUp();
			auto& forward = m.getForward();
			auto translate = -invRotateVector3(m, m.get_translate());

			return detail::Matrix4x4<T>(
				right.x, up.x, forward.x, 0.0f,
				right.y, up.y, forward.y, 0.0f,
				right.z, up.z, forward.z, 0.0f,
				translate.x, translate.y, translate.z, 1.0f);
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makeRotationX(T theta) noexcept
		{
			T c, s;
			sinCos(&s, &c, theta);

			detail::Matrix4x4<T> m;
			m.a1 = 1; m.a2 = 0;  m.a3 = 0; m.a4 = 0;
			m.b1 = 0; m.b2 = c;  m.b3 = s; m.b4 = 0;
			m.c1 = 0; m.c2 = -s; m.c3 = c; m.c4 = 0;
			m.d1 = 0; m.d2 = 0;  m.d3 = 0; m.c4 = 1;

			return m;
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makeRotationY(T theta) noexcept
		{
			T c, s;
			sinCos(&s, &c, theta);

			detail::Matrix4x4<T> m;
			m.a1 = c; m.a2 = 0; m.a3 = -s; m.a4 = 0;
			m.b1 = 0; m.b2 = 1; m.b3 = 0;  m.b4 = 0;
			m.c1 = s; m.c2 = 0; m.c3 = c;  m.c4 = 0;
			m.d1 = 0; m.d2 = 0; m.d3 = 0;  m.c4 = 1;

			return m;
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makeRotationZ(T theta) noexcept
		{
			T c, s;
			sinCos(&s, &c, theta);

			detail::Matrix4x4<T> m;
			m.a1 = c;  m.a2 = s; m.a3 = 0; m.a4 = 0;
			m.b1 = -s; m.b2 = c; m.b3 = 0; m.b4 = 0;
			m.c1 = 0;  m.c2 = 0; m.c3 = 1; m.c4 = 0;
			m.d1 = 0;  m.d2 = 0; m.d3 = 0; m.c4 = 1;

			return m;
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makeRotation(const detail::Vector3<T>& axis, T theta, const detail::Vector3<T>& translate = detail::Vector3<T>::Zero) noexcept
		{
			T c, s;
			sinCos(&s, &c, theta);

			T x = axis.x;
			T y = axis.y;
			T z = axis.z;

			T t = 1 - c;
			T tx = t * x;
			T ty = t * y;
			T tz = t * z;

			detail::Matrix4x4<T> m;
			m.a1 = (tx * x + c);
			m.a2 = (tx * y + s * z);
			m.a3 = (tx * z - s * y);
			m.a4 = 0;

			m.b1 = (tx * y - s * z);
			m.b2 = (ty * y + c);
			m.b3 = (ty * z + s * x);
			m.b4 = 0;

			m.c1 = (tx * z + s * y);
			m.c2 = (ty * z - s * x);
			m.c3 = (tz * z + c);
			m.c4 = 0;

			m.d1 = translate.x;
			m.d2 = translate.y;
			m.d3 = translate.z;
			m.d4 = T(1.0f);

			return m;
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makeRotation(const detail::Quaternion<T>& q, const detail::Vector3<T>& translate = detail::Vector3<T>::Zero) noexcept
		{
			T xs = q.x * T(2.0f), ys = q.y * T(2.0f), zs = q.z * T(2.0f);
			T wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
			T xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
			T yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;

			detail::Matrix4x4<T> m;
			m.a1 = T(1.0f) - (yy + zz);
			m.a2 = xy + wz;
			m.a3 = xz - wy;
			m.a4 = T(0.0f);

			m.b1 = xy - wz;
			m.b2 = T(1.0f) - (xx + zz);
			m.b3 = yz + wx;
			m.b4 = T(0.0f);

			m.c1 = xz + wy;
			m.c2 = yz - wx;
			m.c3 = T(1.0f) - (xx + yy);
			m.c4 = T(0.0f);

			m.d1 = translate.x;
			m.d2 = translate.y;
			m.d3 = translate.z;
			m.d4 = T(1.0f);

			return m;
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makeRotation(const detail::Vector3<T>& forward, const detail::Vector3<T>& up, const detail::Vector3<T>& right, const detail::Vector3<T>& translate = detail::Vector3<T>::Zero) noexcept
		{
			detail::Matrix4x4<T> m;
			m.right = detail::Vector4<T>(right, 0.0);
			m.up = detail::Vector4<T>(up, 0.0);
			m.forward = detail::Vector4<T>(forward, 0.0);
			m.position = detail::Vector4<T>(translate, 1.0);

			return m;
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makeRotation(const detail::Vector4<T>& forward, const detail::Vector4<T>& up, const detail::Vector4<T>& right, const detail::Vector4<T>& translate = detail::Vector4<T>::UnitW) noexcept
		{
			detail::Matrix4x4<T> m;
			m.right = right;
			m.up = up;
			m.forward = forward;
			m.position = translate;

			return m;
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makeOrthoLH(T width, T height, T znear, T zfar) noexcept
		{
			T cx = 2.0f / width;
			T cy = 2.0f / height;
			T cz = 1.0f / (zfar - znear);
			T tz = znear / (znear - zfar);

			return detail::Matrix4x4<T>(
				cx, 0.0, 0.0, 0.0,
				0.0, cy, 0.0, 0.0,
				0.0, 0.0, cz, 0.0,
				0.0, 0.0, tz, 1.0);
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makeOrthoLH(T left, T right, T bottom, T top, T znear, T zfar) noexcept
		{
			T tx = -(right + left) / (right - left);
			T ty = -(top + bottom) / (top - bottom);
			T tz = -znear / (zfar - znear);
			T cx = 2.0f / (right - left);
			T cy = 2.0f / (top - bottom);
			T cz = 1.0f / (zfar - znear);

			return detail::Matrix4x4<T>(
				cx, 0.0, 0.0, 0.0,
				0.0, cy, 0.0, 0.0,
				0.0, 0.0, cz, 0.0,
				tx, ty, tz, 1.0);
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makeOrthoRH(T left, T right, T bottom, T top, T znear, T zfar) noexcept
		{
			T tx = -(right + left) / (right - left);
			T ty = -(top + bottom) / (top - bottom);
			T tz = -(zfar + znear) / (zfar - znear);
			T cx = 2.0f / (right - left);
			T cy = 2.0f / (top - bottom);
			T cz = -2.0f / (zfar - znear);

			return detail::Matrix4x4<T>(
				cx, 0.0, 0.0, 0.0,
				0.0, cy, 0.0, 0.0,
				0.0, 0.0, cz, 0.0,
				tx, ty, tz, 1.0);
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makeFrustumtLH(T left, T right, T bottom, T top, T znear, T zfar) noexcept
		{
			T A = (left + right) / (left - right);
			T B = (bottom + top) / (bottom - top);
			T C = (zfar > DBL_MAX) ? 1.0f : (zfar + znear) / (zfar - znear);
			T D = (zfar > DBL_MAX) ? znear : 2.0f * znear * zfar / (znear - zfar);
			T cx = 2.0f * znear / (right - left);
			T cy = 2.0f * znear / (top - bottom);

			return detail::Matrix4x4<T>(
				cx, 0.0, 0.0, 0.0,
				0.0, cy, 0.0, 0.0,
				0.0, 0.0, C, 1.0,
				A, B, D, 0.0);
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makeFrustumtRH(T left, T right, T bottom, T top, T znear, T zfar) noexcept
		{
			T A = (right + left) / (right - left);
			T B = (top + bottom) / (top - bottom);
			T C = (zfar > DBL_MAX) ? -1.0f : -(zfar + znear) / (zfar - znear);
			T D = (zfar > DBL_MAX) ? -znear : -2.0f * zfar * znear / (zfar - znear);
			T cx = 2.0f * znear / (right - left);
			T cy = 2.0f * znear / (top - bottom);

			return detail::Matrix4x4<T>(
				cx, 0.0, 0.0, 0.0,
				0.0, cy, 0.0, 0.0,
				0.0, 0.0, C, -1.0,
				A, B, D, 0.0);
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makePerspectiveFovLH(const T& fov, const T& aspect, const T& nearPlane, const T& farPlane) noexcept
		{
			const T h = 1.0f / tan(radians(fov * 0.5f));
			const T w = h / aspect;
			const T q = farPlane / (farPlane - nearPlane);

			return detail::Matrix4x4<T>(
				w, 0, 0, 0,
				0, h, 0, 0,
				0, 0, q, 1,
				0, 0, -nearPlane * q, 0);
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makePerspectiveFovRH(const T& fov, const T& aspect, const T& nearPlane, const T& farPlane) noexcept
		{
			const T h = 1.0f / tan(radians(fov * 0.5f));
			const T w = h / aspect;
			const T q = farPlane / (farPlane - nearPlane);

			return detail::Matrix4x4<T>(
				w, 0, 0, 0,
				0, h, 0, 0,
				0, 0, q, -1,
				0, 0, nearPlane * q, 0);
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makePerspectiveOffCenterLH(const T& fovy, const T& aspectRatio, const T& znear, const T& zfar) noexcept
		{
			T tan_fovy = tan(radians(fovy * 0.5f));
			T right = tan_fovy * aspectRatio * znear;
			T left = -right;
			T top = tan_fovy * znear;
			T bottom = -top;

			return makeFrustumtLH(left, right, bottom, top, znear, zfar);
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makePerspectiveOffCenterRH(const T& fovy, const T& aspectRatio, const T& znear, const T& zfar) noexcept
		{
			T tan_fovy = tan(radians(fovy * 0.5f));
			T _right = tan_fovy * aspectRatio * znear;
			T left = -_right;
			T top = tan_fovy * znear;
			T bottom = -top;

			return makeFrustumtRH(left, _right, bottom, top, znear, zfar);
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makeLookatLH(const detail::Vector3<T>& eye, const detail::Vector3<T>& center, const detail::Vector3<T>& up) noexcept
		{
			detail::Vector3<T> x, y, z;

			z = center - eye;
			z = normalize(z);

			x = cross(up, z);
			x = normalize(x);

			y = cross(z, x);
			y = normalize(y);

			detail::Matrix4x4<T> m(
				x.x, y.x, z.x, 0.0,
				x.y, y.y, z.y, 0.0,
				x.z, y.z, z.z, 0.0,
				0.0, 0.0, 0.0, 1.0);

			detail::Vector3<T> tmp = -eye;
			if (tmp.x != 0)
			{
				m.d1 += tmp.x * m.a1;
				m.d2 += tmp.x * m.a2;
				m.d3 += tmp.x * m.a3;
				m.d4 += tmp.x * m.a4;
			}

			if (tmp.y != 0)
			{
				m.d1 += tmp.y * m.b1;
				m.d2 += tmp.y * m.b2;
				m.d3 += tmp.y * m.b3;
				m.d4 += tmp.y * m.b4;
			}

			if (tmp.z != 0)
			{
				m.d1 += tmp.z * m.c1;
				m.d2 += tmp.z * m.c2;
				m.d3 += tmp.z * m.c3;
				m.d4 += tmp.z * m.c4;
			}

			return m;
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makeLookatRH(const detail::Vector3<T>& eye, const detail::Vector3<T>& center, const detail::Vector3<T>& up) noexcept
		{
			detail::Vector3<T> x, y, z;

			z = eye - center;
			z = normalize(z);

			x = cross(up, z);
			x = normalize(x);

			y = cross(z, x);
			y = normalize(y);

			detail::Matrix4x4<T> m(
				x.x, y.x, z.x, 0.0,
				x.y, y.y, z.y, 0.0,
				x.z, y.z, z.z, 0.0,
				0.0, 0.0, 0.0, 1.0);

			detail::Vector3<T> tmp = -eye;
			if (tmp.x != 0)
			{
				m.d1 += tmp.x * m.a1;
				m.d2 += tmp.x * m.a2;
				m.d3 += tmp.x * m.a3;
				m.d4 += tmp.x * m.a4;
			}

			if (tmp.y != 0)
			{
				m.d1 += tmp.y * m.b1;
				m.d2 += tmp.y * m.b2;
				m.d3 += tmp.y * m.b3;
				m.d4 += tmp.y * m.b4;
			}

			if (tmp.z != 0)
			{
				m.d1 += tmp.z * m.c1;
				m.d2 += tmp.z * m.c2;
				m.d3 += tmp.z * m.c3;
				m.d4 += tmp.z * m.c4;
			}

			return m;
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Matrix4x4<T> makeViewport(std::uint32_t left, std::uint32_t top, std::uint32_t width, std::uint32_t height) noexcept
		{
			T cx = (T)(width >> 1);
			T cy = (T)(height >> 1);
			T A = cx + left;
			T B = cy + top;

			return detail::Matrix4x4<T>(
				cx, 0.0, 0.0, 0.0,
				0.0, cy, 0.0, 0.0,
				0.0, 0.0, 0.5, 0.0,
				A, B, 0.5, 1.0);
		}
	}
}

#endif