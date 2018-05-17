#ifndef OCTOON_MATH_VECTOR4_H_
#define OCTOON_MATH_VECTOR4_H_

#include <octoon/math/trait.h>
#include <octoon/math/mathfwd.h>

namespace octoon
{
	namespace math
	{
		namespace detail
		{
			template<typename T>
			class Vector4 final
			{
			public:
				typedef typename trait::type_addition<T>::value_type value_type;
				typedef typename trait::type_addition<T>::pointer pointer;
				typedef typename trait::type_addition<T>::const_pointer const_pointer;
				typedef typename trait::type_addition<T>::reference reference;
				typedef typename trait::type_addition<T>::const_reference const_reference;

				static const Vector4<T> Zero;
				static const Vector4<T> One;
				static const Vector4<T> UnitX;
				static const Vector4<T> UnitY;
				static const Vector4<T> UnitZ;
				static const Vector4<T> UnitW;

				union
				{
					struct
					{
						T x, y, z, w;
					};

					struct
					{
						T r, g, b, a;
					};

					struct
					{
						T left, top, width, height;
					};
				};

				Vector4() = default;
				Vector4(T xyz, T ww)  noexcept : x(xyz), y(xyz), z(xyz), w(ww) {}
				Vector4(T xx, T yy, T zz, T ww = 1.0)  noexcept : x(xx), y(yy), z(zz), w(ww) {}
				Vector4(T xx, const Vector3<T>& yzw) noexcept : x(xx), y(yzw.x), z(yzw.y), w(yzw.z) {}
				Vector4(T xx, T yy, const Vector2<T>& zw) noexcept : x(xx), y(yy), z(zw.x), w(zw.y) {}
				Vector4(const Vector3<T>& v, T w)  noexcept : x(v.x), y(v.y), z(v.z), w(w) {}
				Vector4(const Vector2<T>& xy, const Vector2<T>& zw) noexcept : x(xy.x), y(xy.y), z(zw.x), w(zw.y) {}
				Vector4(const Vector2<T>& xy, T z, T w) noexcept : x(xy.x), y(xy.y), z(z), w(w) {}
				explicit Vector4(T xyzw) noexcept : x(xyzw), y(xyzw), z(xyzw), w(xyzw) {}
				explicit Vector4(const T xyzw[4]) noexcept : x(xyzw[0]), y(xyzw[1]), z(xyzw[2]), w(xyzw[3]) {}
				~Vector4() = default;

				Vector4<T>& operator+=(T scale) noexcept { x += scale; y += scale; z += scale; w += scale; return *this; }
				Vector4<T>& operator-=(T scale) noexcept { x -= scale; y -= scale; z -= scale; w -= scale; return *this; }
				Vector4<T>& operator*=(T scale) noexcept { x *= scale; y *= scale; z *= scale; w *= scale; return *this; }
				Vector4<T>& operator/=(T scale) noexcept { x /= scale; y /= scale; z /= scale; w /= scale; return *this; }

				Vector4<T>& operator+=(const Vector2<T>& rt) noexcept { x += rt.x; y += rt.y; return *this; }
				Vector4<T>& operator-=(const Vector2<T>& rt) noexcept { x -= rt.x; y -= rt.y; return *this; }
				Vector4<T>& operator/=(const Vector2<T>& rt) noexcept { x *= rt.x; y *= rt.y; return *this; }
				Vector4<T>& operator*=(const Vector2<T>& rt) noexcept { x /= rt.x; y /= rt.y; return *this; }

				Vector4<T>& operator+=(const Vector3<T>& rt) noexcept { x += rt.x; y += rt.y; z += rt.z; return *this; }
				Vector4<T>& operator-=(const Vector3<T>& rt) noexcept { x -= rt.x; y -= rt.y; z -= rt.z; return *this; }
				Vector4<T>& operator/=(const Vector3<T>& rt) noexcept { x *= rt.x; y *= rt.y; z *= rt.z; return *this; }
				Vector4<T>& operator*=(const Vector3<T>& rt) noexcept { x /= rt.x; y /= rt.y; z /= rt.z; return *this; }

				Vector4<T>& operator+=(const Vector4<T>& rt) noexcept { x += rt.x; y += rt.y; z += rt.z; w += rt.w; return *this; }
				Vector4<T>& operator-=(const Vector4<T>& rt) noexcept { x -= rt.x; y -= rt.y; z -= rt.z; w -= rt.w; return *this; }
				Vector4<T>& operator/=(const Vector4<T>& rt) noexcept { x *= rt.x; y *= rt.y; z *= rt.z; w *= rt.w; return *this; }
				Vector4<T>& operator*=(const Vector4<T>& rt) noexcept { x /= rt.x; y /= rt.y; z /= rt.z; w /= rt.w; return *this; }

				template<typename S, typename = std::enable_if_t<trait::is_integral_v<S> || trait::is_floating_point_v<S>>>
				explicit operator Vector4<S>() const noexcept
				{
					return Vector4<S>(static_cast<S>(x), static_cast<S>(y), static_cast<S>(z), static_cast<S>(w));
				}

				constexpr T operator[](std::uint8_t i) const noexcept
				{
					assert(i < 4);
					return *(&x + i);
				}

				constexpr T& operator[](std::uint8_t i) noexcept
				{
					assert(i < 4);
					return *(&x + i);
				}

				constexpr Vector2<T> xx()  const noexcept { return Vector2<T>(x, x); }
				constexpr Vector2<T> xy()  const noexcept { return Vector2<T>(x, y); }
				constexpr Vector2<T> xz()  const noexcept { return Vector2<T>(x, z); }
				constexpr Vector2<T> xw()  const noexcept { return Vector2<T>(x, w); }
				constexpr Vector3<T> xxx() const noexcept { return Vector3<T>(x, x, x); }
				constexpr Vector3<T> xxy() const noexcept { return Vector3<T>(x, x, y); }
				constexpr Vector3<T> xxz() const noexcept { return Vector3<T>(x, x, z); }
				constexpr Vector3<T> xxw() const noexcept { return Vector3<T>(x, x, w); }
				constexpr Vector3<T> xyx() const noexcept { return Vector3<T>(x, y, x); }
				constexpr Vector3<T> xyy() const noexcept { return Vector3<T>(x, y, y); }
				constexpr Vector3<T> xyz() const noexcept { return Vector3<T>(x, y, z); }
				constexpr Vector3<T> xyw() const noexcept { return Vector3<T>(x, y, w); }
				constexpr Vector3<T> xzx() const noexcept { return Vector3<T>(x, z, x); }
				constexpr Vector3<T> xzy() const noexcept { return Vector3<T>(x, z, y); }
				constexpr Vector3<T> xzz() const noexcept { return Vector3<T>(x, z, z); }
				constexpr Vector3<T> xzw() const noexcept { return Vector3<T>(x, z, w); }
				constexpr Vector3<T> xwx() const noexcept { return Vector3<T>(x, w, x); }
				constexpr Vector3<T> xwy() const noexcept { return Vector3<T>(x, w, y); }
				constexpr Vector3<T> xwz() const noexcept { return Vector3<T>(x, w, z); }
				constexpr Vector3<T> xww() const noexcept { return Vector3<T>(x, w, w); }
				constexpr Vector2<T> yx()  const noexcept { return Vector2<T>(y, x); }
				constexpr Vector2<T> yy()  const noexcept { return Vector2<T>(y, y); }
				constexpr Vector2<T> yz()  const noexcept { return Vector2<T>(y, z); }
				constexpr Vector2<T> yw()  const noexcept { return Vector2<T>(y, w); }
				constexpr Vector3<T> yxx() const noexcept { return Vector3<T>(y, x, x); }
				constexpr Vector3<T> yxy() const noexcept { return Vector3<T>(y, x, y); }
				constexpr Vector3<T> yxz() const noexcept { return Vector3<T>(y, x, z); }
				constexpr Vector3<T> yxw() const noexcept { return Vector3<T>(y, x, w); }
				constexpr Vector3<T> yyx() const noexcept { return Vector3<T>(y, y, x); }
				constexpr Vector3<T> yyy() const noexcept { return Vector3<T>(y, y, y); }
				constexpr Vector3<T> yyz() const noexcept { return Vector3<T>(y, y, z); }
				constexpr Vector3<T> yyw() const noexcept { return Vector3<T>(y, y, w); }
				constexpr Vector3<T> yzx() const noexcept { return Vector3<T>(y, z, x); }
				constexpr Vector3<T> yzy() const noexcept { return Vector3<T>(y, z, y); }
				constexpr Vector3<T> yzz() const noexcept { return Vector3<T>(y, z, z); }
				constexpr Vector3<T> yzw() const noexcept { return Vector3<T>(y, z, w); }
				constexpr Vector3<T> ywx() const noexcept { return Vector3<T>(y, w, x); }
				constexpr Vector3<T> ywy() const noexcept { return Vector3<T>(y, w, y); }
				constexpr Vector3<T> ywz() const noexcept { return Vector3<T>(y, w, z); }
				constexpr Vector3<T> yww() const noexcept { return Vector3<T>(y, w, w); }
				constexpr Vector2<T> zx()  const noexcept { return Vector2<T>(z, x); }
				constexpr Vector2<T> zy()  const noexcept { return Vector2<T>(z, y); }
				constexpr Vector2<T> zz()  const noexcept { return Vector2<T>(z, z); }
				constexpr Vector2<T> zw()  const noexcept { return Vector2<T>(z, w); }
				constexpr Vector3<T> zxx() const noexcept { return Vector3<T>(z, x, x); }
				constexpr Vector3<T> zxy() const noexcept { return Vector3<T>(z, x, y); }
				constexpr Vector3<T> zxz() const noexcept { return Vector3<T>(z, x, z); }
				constexpr Vector3<T> zxw() const noexcept { return Vector3<T>(z, x, w); }
				constexpr Vector3<T> zyx() const noexcept { return Vector3<T>(z, y, x); }
				constexpr Vector3<T> zyy() const noexcept { return Vector3<T>(z, y, y); }
				constexpr Vector3<T> zyz() const noexcept { return Vector3<T>(z, y, z); }
				constexpr Vector3<T> zyw() const noexcept { return Vector3<T>(z, y, w); }
				constexpr Vector3<T> zzx() const noexcept { return Vector3<T>(z, z, x); }
				constexpr Vector3<T> zzy() const noexcept { return Vector3<T>(z, z, y); }
				constexpr Vector3<T> zzz() const noexcept { return Vector3<T>(z, z, z); }
				constexpr Vector3<T> zzw() const noexcept { return Vector3<T>(z, z, w); }
				constexpr Vector3<T> zwx() const noexcept { return Vector3<T>(z, w, x); }
				constexpr Vector3<T> zwy() const noexcept { return Vector3<T>(z, w, y); }
				constexpr Vector3<T> zwz() const noexcept { return Vector3<T>(z, w, z); }
				constexpr Vector3<T> zww() const noexcept { return Vector3<T>(z, w, w); }
				constexpr Vector2<T> wx()  const noexcept { return Vector2<T>(w, x); }
				constexpr Vector2<T> wy()  const noexcept { return Vector2<T>(w, y); }
				constexpr Vector2<T> wz()  const noexcept { return Vector2<T>(w, z); }
				constexpr Vector2<T> ww()  const noexcept { return Vector2<T>(w, w); }
				constexpr Vector3<T> wxx() const noexcept { return Vector3<T>(w, x, x); }
				constexpr Vector3<T> wxy() const noexcept { return Vector3<T>(w, x, y); }
				constexpr Vector3<T> wxz() const noexcept { return Vector3<T>(w, x, z); }
				constexpr Vector3<T> wxw() const noexcept { return Vector3<T>(w, x, w); }
				constexpr Vector3<T> wyx() const noexcept { return Vector3<T>(w, y, x); }
				constexpr Vector3<T> wyy() const noexcept { return Vector3<T>(w, y, y); }
				constexpr Vector3<T> wyz() const noexcept { return Vector3<T>(w, y, z); }
				constexpr Vector3<T> wyw() const noexcept { return Vector3<T>(w, y, w); }
				constexpr Vector3<T> wzx() const noexcept { return Vector3<T>(w, z, x); }
				constexpr Vector3<T> wzy() const noexcept { return Vector3<T>(w, z, y); }
				constexpr Vector3<T> wzz() const noexcept { return Vector3<T>(w, z, z); }
				constexpr Vector3<T> wzw() const noexcept { return Vector3<T>(w, z, w); }
				constexpr Vector3<T> wwx() const noexcept { return Vector3<T>(w, w, x); }
				constexpr Vector3<T> wwy() const noexcept { return Vector3<T>(w, w, y); }
				constexpr Vector3<T> wwz() const noexcept { return Vector3<T>(w, w, z); }
				constexpr Vector3<T> www() const noexcept { return Vector3<T>(w, w, w); }
				constexpr Vector4<T> xxxx() const noexcept { return Vector4<T>(x, x, x, x); }
				constexpr Vector4<T> xxxy() const noexcept { return Vector4<T>(x, x, x, y); }
				constexpr Vector4<T> xxxz() const noexcept { return Vector4<T>(x, x, x, z); }
				constexpr Vector4<T> xxxw() const noexcept { return Vector4<T>(x, x, x, w); }
				constexpr Vector4<T> xxyx() const noexcept { return Vector4<T>(x, x, y, x); }
				constexpr Vector4<T> xxyy() const noexcept { return Vector4<T>(x, x, y, y); }
				constexpr Vector4<T> xxyz() const noexcept { return Vector4<T>(x, x, y, z); }
				constexpr Vector4<T> xxyw() const noexcept { return Vector4<T>(x, x, y, w); }
				constexpr Vector4<T> xxzx() const noexcept { return Vector4<T>(x, x, z, x); }
				constexpr Vector4<T> xxzy() const noexcept { return Vector4<T>(x, x, z, y); }
				constexpr Vector4<T> xxzz() const noexcept { return Vector4<T>(x, x, z, z); }
				constexpr Vector4<T> xxzw() const noexcept { return Vector4<T>(x, x, z, w); }
				constexpr Vector4<T> xxwx() const noexcept { return Vector4<T>(x, x, w, x); }
				constexpr Vector4<T> xxwy() const noexcept { return Vector4<T>(x, x, w, y); }
				constexpr Vector4<T> xxwz() const noexcept { return Vector4<T>(x, x, w, z); }
				constexpr Vector4<T> xxww() const noexcept { return Vector4<T>(x, x, w, w); }
				constexpr Vector4<T> xyxx() const noexcept { return Vector4<T>(x, y, x, x); }
				constexpr Vector4<T> xyxy() const noexcept { return Vector4<T>(x, y, x, y); }
				constexpr Vector4<T> xyxz() const noexcept { return Vector4<T>(x, y, x, z); }
				constexpr Vector4<T> xyxw() const noexcept { return Vector4<T>(x, y, x, w); }
				constexpr Vector4<T> xyyx() const noexcept { return Vector4<T>(x, y, y, x); }
				constexpr Vector4<T> xyyy() const noexcept { return Vector4<T>(x, y, y, y); }
				constexpr Vector4<T> xyyz() const noexcept { return Vector4<T>(x, y, y, z); }
				constexpr Vector4<T> xyyw() const noexcept { return Vector4<T>(x, y, y, w); }
				constexpr Vector4<T> xyzx() const noexcept { return Vector4<T>(x, y, z, x); }
				constexpr Vector4<T> xyzy() const noexcept { return Vector4<T>(x, y, z, y); }
				constexpr Vector4<T> xyzz() const noexcept { return Vector4<T>(x, y, z, z); }
				constexpr Vector4<T> xyzw() const noexcept { return Vector4<T>(x, y, z, w); }
				constexpr Vector4<T> xywx() const noexcept { return Vector4<T>(x, y, w, x); }
				constexpr Vector4<T> xywy() const noexcept { return Vector4<T>(x, y, w, y); }
				constexpr Vector4<T> xywz() const noexcept { return Vector4<T>(x, y, w, z); }
				constexpr Vector4<T> xyww() const noexcept { return Vector4<T>(x, y, w, w); }
				constexpr Vector4<T> xzxx() const noexcept { return Vector4<T>(x, z, x, x); }
				constexpr Vector4<T> xzxy() const noexcept { return Vector4<T>(x, z, x, y); }
				constexpr Vector4<T> xzxz() const noexcept { return Vector4<T>(x, z, x, z); }
				constexpr Vector4<T> xzxw() const noexcept { return Vector4<T>(x, z, x, w); }
				constexpr Vector4<T> xzyx() const noexcept { return Vector4<T>(x, z, y, x); }
				constexpr Vector4<T> xzyy() const noexcept { return Vector4<T>(x, z, y, y); }
				constexpr Vector4<T> xzyz() const noexcept { return Vector4<T>(x, z, y, z); }
				constexpr Vector4<T> xzyw() const noexcept { return Vector4<T>(x, z, y, w); }
				constexpr Vector4<T> xzzx() const noexcept { return Vector4<T>(x, z, z, x); }
				constexpr Vector4<T> xzzy() const noexcept { return Vector4<T>(x, z, z, y); }
				constexpr Vector4<T> xzzz() const noexcept { return Vector4<T>(x, z, z, z); }
				constexpr Vector4<T> xzzw() const noexcept { return Vector4<T>(x, z, z, w); }
				constexpr Vector4<T> xzwx() const noexcept { return Vector4<T>(x, z, w, x); }
				constexpr Vector4<T> xzwy() const noexcept { return Vector4<T>(x, z, w, y); }
				constexpr Vector4<T> xzwz() const noexcept { return Vector4<T>(x, z, w, z); }
				constexpr Vector4<T> xzww() const noexcept { return Vector4<T>(x, z, w, w); }
				constexpr Vector4<T> xwxx() const noexcept { return Vector4<T>(x, w, x, x); }
				constexpr Vector4<T> xwxy() const noexcept { return Vector4<T>(x, w, x, y); }
				constexpr Vector4<T> xwxz() const noexcept { return Vector4<T>(x, w, x, z); }
				constexpr Vector4<T> xwxw() const noexcept { return Vector4<T>(x, w, x, w); }
				constexpr Vector4<T> xwyx() const noexcept { return Vector4<T>(x, w, y, x); }
				constexpr Vector4<T> xwyy() const noexcept { return Vector4<T>(x, w, y, y); }
				constexpr Vector4<T> xwyz() const noexcept { return Vector4<T>(x, w, y, z); }
				constexpr Vector4<T> xwyw() const noexcept { return Vector4<T>(x, w, y, w); }
				constexpr Vector4<T> xwzx() const noexcept { return Vector4<T>(x, w, z, x); }
				constexpr Vector4<T> xwzy() const noexcept { return Vector4<T>(x, w, z, y); }
				constexpr Vector4<T> xwzz() const noexcept { return Vector4<T>(x, w, z, z); }
				constexpr Vector4<T> xwzw() const noexcept { return Vector4<T>(x, w, z, w); }
				constexpr Vector4<T> xwwx() const noexcept { return Vector4<T>(x, w, w, x); }
				constexpr Vector4<T> xwwy() const noexcept { return Vector4<T>(x, w, w, y); }
				constexpr Vector4<T> xwwz() const noexcept { return Vector4<T>(x, w, w, z); }
				constexpr Vector4<T> xwww() const noexcept { return Vector4<T>(x, w, w, w); }
				constexpr Vector4<T> yxxx() const noexcept { return Vector4<T>(y, x, x, x); }
				constexpr Vector4<T> yxxy() const noexcept { return Vector4<T>(y, x, x, y); }
				constexpr Vector4<T> yxxz() const noexcept { return Vector4<T>(y, x, x, z); }
				constexpr Vector4<T> yxxw() const noexcept { return Vector4<T>(y, x, x, w); }
				constexpr Vector4<T> yxyx() const noexcept { return Vector4<T>(y, x, y, x); }
				constexpr Vector4<T> yxyy() const noexcept { return Vector4<T>(y, x, y, y); }
				constexpr Vector4<T> yxyz() const noexcept { return Vector4<T>(y, x, y, z); }
				constexpr Vector4<T> yxyw() const noexcept { return Vector4<T>(y, x, y, w); }
				constexpr Vector4<T> yxzx() const noexcept { return Vector4<T>(y, x, z, x); }
				constexpr Vector4<T> yxzy() const noexcept { return Vector4<T>(y, x, z, y); }
				constexpr Vector4<T> yxzz() const noexcept { return Vector4<T>(y, x, z, z); }
				constexpr Vector4<T> yxzw() const noexcept { return Vector4<T>(y, x, z, w); }
				constexpr Vector4<T> yxwx() const noexcept { return Vector4<T>(y, x, w, x); }
				constexpr Vector4<T> yxwy() const noexcept { return Vector4<T>(y, x, w, y); }
				constexpr Vector4<T> yxwz() const noexcept { return Vector4<T>(y, x, w, z); }
				constexpr Vector4<T> yxww() const noexcept { return Vector4<T>(y, x, w, w); }
				constexpr Vector4<T> yyxx() const noexcept { return Vector4<T>(y, y, x, x); }
				constexpr Vector4<T> yyxy() const noexcept { return Vector4<T>(y, y, x, y); }
				constexpr Vector4<T> yyxz() const noexcept { return Vector4<T>(y, y, x, z); }
				constexpr Vector4<T> yyxw() const noexcept { return Vector4<T>(y, y, x, w); }
				constexpr Vector4<T> yyyx() const noexcept { return Vector4<T>(y, y, y, x); }
				constexpr Vector4<T> yyyy() const noexcept { return Vector4<T>(y, y, y, y); }
				constexpr Vector4<T> yyyz() const noexcept { return Vector4<T>(y, y, y, z); }
				constexpr Vector4<T> yyyw() const noexcept { return Vector4<T>(y, y, y, w); }
				constexpr Vector4<T> yyzx() const noexcept { return Vector4<T>(y, y, z, x); }
				constexpr Vector4<T> yyzy() const noexcept { return Vector4<T>(y, y, z, y); }
				constexpr Vector4<T> yyzz() const noexcept { return Vector4<T>(y, y, z, z); }
				constexpr Vector4<T> yyzw() const noexcept { return Vector4<T>(y, y, z, w); }
				constexpr Vector4<T> yywx() const noexcept { return Vector4<T>(y, y, w, x); }
				constexpr Vector4<T> yywy() const noexcept { return Vector4<T>(y, y, w, y); }
				constexpr Vector4<T> yywz() const noexcept { return Vector4<T>(y, y, w, z); }
				constexpr Vector4<T> yyww() const noexcept { return Vector4<T>(y, y, w, w); }
				constexpr Vector4<T> yzxx() const noexcept { return Vector4<T>(y, z, x, x); }
				constexpr Vector4<T> yzxy() const noexcept { return Vector4<T>(y, z, x, y); }
				constexpr Vector4<T> yzxz() const noexcept { return Vector4<T>(y, z, x, z); }
				constexpr Vector4<T> yzxw() const noexcept { return Vector4<T>(y, z, x, w); }
				constexpr Vector4<T> yzyx() const noexcept { return Vector4<T>(y, z, y, x); }
				constexpr Vector4<T> yzyy() const noexcept { return Vector4<T>(y, z, y, y); }
				constexpr Vector4<T> yzyz() const noexcept { return Vector4<T>(y, z, y, z); }
				constexpr Vector4<T> yzyw() const noexcept { return Vector4<T>(y, z, y, w); }
				constexpr Vector4<T> yzzx() const noexcept { return Vector4<T>(y, z, z, x); }
				constexpr Vector4<T> yzzy() const noexcept { return Vector4<T>(y, z, z, y); }
				constexpr Vector4<T> yzzz() const noexcept { return Vector4<T>(y, z, z, z); }
				constexpr Vector4<T> yzzw() const noexcept { return Vector4<T>(y, z, z, w); }
				constexpr Vector4<T> yzwx() const noexcept { return Vector4<T>(y, z, w, x); }
				constexpr Vector4<T> yzwy() const noexcept { return Vector4<T>(y, z, w, y); }
				constexpr Vector4<T> yzwz() const noexcept { return Vector4<T>(y, z, w, z); }
				constexpr Vector4<T> yzww() const noexcept { return Vector4<T>(y, z, w, w); }
				constexpr Vector4<T> ywxx() const noexcept { return Vector4<T>(y, w, x, x); }
				constexpr Vector4<T> ywxy() const noexcept { return Vector4<T>(y, w, x, y); }
				constexpr Vector4<T> ywxz() const noexcept { return Vector4<T>(y, w, x, z); }
				constexpr Vector4<T> ywxw() const noexcept { return Vector4<T>(y, w, x, w); }
				constexpr Vector4<T> ywyx() const noexcept { return Vector4<T>(y, w, y, x); }
				constexpr Vector4<T> ywyy() const noexcept { return Vector4<T>(y, w, y, y); }
				constexpr Vector4<T> ywyz() const noexcept { return Vector4<T>(y, w, y, z); }
				constexpr Vector4<T> ywyw() const noexcept { return Vector4<T>(y, w, y, w); }
				constexpr Vector4<T> ywzx() const noexcept { return Vector4<T>(y, w, z, x); }
				constexpr Vector4<T> ywzy() const noexcept { return Vector4<T>(y, w, z, y); }
				constexpr Vector4<T> ywzz() const noexcept { return Vector4<T>(y, w, z, z); }
				constexpr Vector4<T> ywzw() const noexcept { return Vector4<T>(y, w, z, w); }
				constexpr Vector4<T> ywwx() const noexcept { return Vector4<T>(y, w, w, x); }
				constexpr Vector4<T> ywwy() const noexcept { return Vector4<T>(y, w, w, y); }
				constexpr Vector4<T> ywwz() const noexcept { return Vector4<T>(y, w, w, z); }
				constexpr Vector4<T> ywww() const noexcept { return Vector4<T>(y, w, w, w); }
				constexpr Vector4<T> zxxx() const noexcept { return Vector4<T>(z, x, x, x); }
				constexpr Vector4<T> zxxy() const noexcept { return Vector4<T>(z, x, x, y); }
				constexpr Vector4<T> zxxz() const noexcept { return Vector4<T>(z, x, x, z); }
				constexpr Vector4<T> zxxw() const noexcept { return Vector4<T>(z, x, x, w); }
				constexpr Vector4<T> zxyx() const noexcept { return Vector4<T>(z, x, y, x); }
				constexpr Vector4<T> zxyy() const noexcept { return Vector4<T>(z, x, y, y); }
				constexpr Vector4<T> zxyz() const noexcept { return Vector4<T>(z, x, y, z); }
				constexpr Vector4<T> zxyw() const noexcept { return Vector4<T>(z, x, y, w); }
				constexpr Vector4<T> zxzx() const noexcept { return Vector4<T>(z, x, z, x); }
				constexpr Vector4<T> zxzy() const noexcept { return Vector4<T>(z, x, z, y); }
				constexpr Vector4<T> zxzz() const noexcept { return Vector4<T>(z, x, z, z); }
				constexpr Vector4<T> zxzw() const noexcept { return Vector4<T>(z, x, z, w); }
				constexpr Vector4<T> zxwx() const noexcept { return Vector4<T>(z, x, w, x); }
				constexpr Vector4<T> zxwy() const noexcept { return Vector4<T>(z, x, w, y); }
				constexpr Vector4<T> zxwz() const noexcept { return Vector4<T>(z, x, w, z); }
				constexpr Vector4<T> zxww() const noexcept { return Vector4<T>(z, x, w, w); }
				constexpr Vector4<T> zyxx() const noexcept { return Vector4<T>(z, y, x, x); }
				constexpr Vector4<T> zyxy() const noexcept { return Vector4<T>(z, y, x, y); }
				constexpr Vector4<T> zyxz() const noexcept { return Vector4<T>(z, y, x, z); }
				constexpr Vector4<T> zyxw() const noexcept { return Vector4<T>(z, y, x, w); }
				constexpr Vector4<T> zyyx() const noexcept { return Vector4<T>(z, y, y, x); }
				constexpr Vector4<T> zyyy() const noexcept { return Vector4<T>(z, y, y, y); }
				constexpr Vector4<T> zyyz() const noexcept { return Vector4<T>(z, y, y, z); }
				constexpr Vector4<T> zyyw() const noexcept { return Vector4<T>(z, y, y, w); }
				constexpr Vector4<T> zyzx() const noexcept { return Vector4<T>(z, y, z, x); }
				constexpr Vector4<T> zyzy() const noexcept { return Vector4<T>(z, y, z, y); }
				constexpr Vector4<T> zyzz() const noexcept { return Vector4<T>(z, y, z, z); }
				constexpr Vector4<T> zyzw() const noexcept { return Vector4<T>(z, y, z, w); }
				constexpr Vector4<T> zywx() const noexcept { return Vector4<T>(z, y, w, x); }
				constexpr Vector4<T> zywy() const noexcept { return Vector4<T>(z, y, w, y); }
				constexpr Vector4<T> zywz() const noexcept { return Vector4<T>(z, y, w, z); }
				constexpr Vector4<T> zyww() const noexcept { return Vector4<T>(z, y, w, w); }
				constexpr Vector4<T> zzxx() const noexcept { return Vector4<T>(z, z, x, x); }
				constexpr Vector4<T> zzxy() const noexcept { return Vector4<T>(z, z, x, y); }
				constexpr Vector4<T> zzxz() const noexcept { return Vector4<T>(z, z, x, z); }
				constexpr Vector4<T> zzxw() const noexcept { return Vector4<T>(z, z, x, w); }
				constexpr Vector4<T> zzyx() const noexcept { return Vector4<T>(z, z, y, x); }
				constexpr Vector4<T> zzyy() const noexcept { return Vector4<T>(z, z, y, y); }
				constexpr Vector4<T> zzyz() const noexcept { return Vector4<T>(z, z, y, z); }
				constexpr Vector4<T> zzyw() const noexcept { return Vector4<T>(z, z, y, w); }
				constexpr Vector4<T> zzzx() const noexcept { return Vector4<T>(z, z, z, x); }
				constexpr Vector4<T> zzzy() const noexcept { return Vector4<T>(z, z, z, y); }
				constexpr Vector4<T> zzzz() const noexcept { return Vector4<T>(z, z, z, z); }
				constexpr Vector4<T> zzzw() const noexcept { return Vector4<T>(z, z, z, w); }
				constexpr Vector4<T> zzwx() const noexcept { return Vector4<T>(z, z, w, x); }
				constexpr Vector4<T> zzwy() const noexcept { return Vector4<T>(z, z, w, y); }
				constexpr Vector4<T> zzwz() const noexcept { return Vector4<T>(z, z, w, z); }
				constexpr Vector4<T> zzww() const noexcept { return Vector4<T>(z, z, w, w); }
				constexpr Vector4<T> zwxx() const noexcept { return Vector4<T>(z, w, x, x); }
				constexpr Vector4<T> zwxy() const noexcept { return Vector4<T>(z, w, x, y); }
				constexpr Vector4<T> zwxz() const noexcept { return Vector4<T>(z, w, x, z); }
				constexpr Vector4<T> zwxw() const noexcept { return Vector4<T>(z, w, x, w); }
				constexpr Vector4<T> zwyx() const noexcept { return Vector4<T>(z, w, y, x); }
				constexpr Vector4<T> zwyy() const noexcept { return Vector4<T>(z, w, y, y); }
				constexpr Vector4<T> zwyz() const noexcept { return Vector4<T>(z, w, y, z); }
				constexpr Vector4<T> zwyw() const noexcept { return Vector4<T>(z, w, y, w); }
				constexpr Vector4<T> zwzx() const noexcept { return Vector4<T>(z, w, z, x); }
				constexpr Vector4<T> zwzy() const noexcept { return Vector4<T>(z, w, z, y); }
				constexpr Vector4<T> zwzz() const noexcept { return Vector4<T>(z, w, z, z); }
				constexpr Vector4<T> zwzw() const noexcept { return Vector4<T>(z, w, z, w); }
				constexpr Vector4<T> zwwx() const noexcept { return Vector4<T>(z, w, w, x); }
				constexpr Vector4<T> zwwy() const noexcept { return Vector4<T>(z, w, w, y); }
				constexpr Vector4<T> zwwz() const noexcept { return Vector4<T>(z, w, w, z); }
				constexpr Vector4<T> zwww() const noexcept { return Vector4<T>(z, w, w, w); }
				constexpr Vector4<T> wxxx() const noexcept { return Vector4<T>(w, x, x, x); }
				constexpr Vector4<T> wxxy() const noexcept { return Vector4<T>(w, x, x, y); }
				constexpr Vector4<T> wxxz() const noexcept { return Vector4<T>(w, x, x, z); }
				constexpr Vector4<T> wxxw() const noexcept { return Vector4<T>(w, x, x, w); }
				constexpr Vector4<T> wxyx() const noexcept { return Vector4<T>(w, x, y, x); }
				constexpr Vector4<T> wxyy() const noexcept { return Vector4<T>(w, x, y, y); }
				constexpr Vector4<T> wxyz() const noexcept { return Vector4<T>(w, x, y, z); }
				constexpr Vector4<T> wxyw() const noexcept { return Vector4<T>(w, x, y, w); }
				constexpr Vector4<T> wxzx() const noexcept { return Vector4<T>(w, x, z, x); }
				constexpr Vector4<T> wxzy() const noexcept { return Vector4<T>(w, x, z, y); }
				constexpr Vector4<T> wxzz() const noexcept { return Vector4<T>(w, x, z, z); }
				constexpr Vector4<T> wxzw() const noexcept { return Vector4<T>(w, x, z, w); }
				constexpr Vector4<T> wxwx() const noexcept { return Vector4<T>(w, x, w, x); }
				constexpr Vector4<T> wxwy() const noexcept { return Vector4<T>(w, x, w, y); }
				constexpr Vector4<T> wxwz() const noexcept { return Vector4<T>(w, x, w, z); }
				constexpr Vector4<T> wxww() const noexcept { return Vector4<T>(w, x, w, w); }
				constexpr Vector4<T> wyxx() const noexcept { return Vector4<T>(w, y, x, x); }
				constexpr Vector4<T> wyxy() const noexcept { return Vector4<T>(w, y, x, y); }
				constexpr Vector4<T> wyxz() const noexcept { return Vector4<T>(w, y, x, z); }
				constexpr Vector4<T> wyxw() const noexcept { return Vector4<T>(w, y, x, w); }
				constexpr Vector4<T> wyyx() const noexcept { return Vector4<T>(w, y, y, x); }
				constexpr Vector4<T> wyyy() const noexcept { return Vector4<T>(w, y, y, y); }
				constexpr Vector4<T> wyyz() const noexcept { return Vector4<T>(w, y, y, z); }
				constexpr Vector4<T> wyyw() const noexcept { return Vector4<T>(w, y, y, w); }
				constexpr Vector4<T> wyzx() const noexcept { return Vector4<T>(w, y, z, x); }
				constexpr Vector4<T> wyzy() const noexcept { return Vector4<T>(w, y, z, y); }
				constexpr Vector4<T> wyzz() const noexcept { return Vector4<T>(w, y, z, z); }
				constexpr Vector4<T> wyzw() const noexcept { return Vector4<T>(w, y, z, w); }
				constexpr Vector4<T> wywx() const noexcept { return Vector4<T>(w, y, w, x); }
				constexpr Vector4<T> wywy() const noexcept { return Vector4<T>(w, y, w, y); }
				constexpr Vector4<T> wywz() const noexcept { return Vector4<T>(w, y, w, z); }
				constexpr Vector4<T> wyww() const noexcept { return Vector4<T>(w, y, w, w); }
				constexpr Vector4<T> wzxx() const noexcept { return Vector4<T>(w, z, x, x); }
				constexpr Vector4<T> wzxy() const noexcept { return Vector4<T>(w, z, x, y); }
				constexpr Vector4<T> wzxz() const noexcept { return Vector4<T>(w, z, x, z); }
				constexpr Vector4<T> wzxw() const noexcept { return Vector4<T>(w, z, x, w); }
				constexpr Vector4<T> wzyx() const noexcept { return Vector4<T>(w, z, y, x); }
				constexpr Vector4<T> wzyy() const noexcept { return Vector4<T>(w, z, y, y); }
				constexpr Vector4<T> wzyz() const noexcept { return Vector4<T>(w, z, y, z); }
				constexpr Vector4<T> wzyw() const noexcept { return Vector4<T>(w, z, y, w); }
				constexpr Vector4<T> wzzx() const noexcept { return Vector4<T>(w, z, z, x); }
				constexpr Vector4<T> wzzy() const noexcept { return Vector4<T>(w, z, z, y); }
				constexpr Vector4<T> wzzz() const noexcept { return Vector4<T>(w, z, z, z); }
				constexpr Vector4<T> wzzw() const noexcept { return Vector4<T>(w, z, z, w); }
				constexpr Vector4<T> wzwx() const noexcept { return Vector4<T>(w, z, w, x); }
				constexpr Vector4<T> wzwy() const noexcept { return Vector4<T>(w, z, w, y); }
				constexpr Vector4<T> wzwz() const noexcept { return Vector4<T>(w, z, w, z); }
				constexpr Vector4<T> wzww() const noexcept { return Vector4<T>(w, z, w, w); }
				constexpr Vector4<T> wwxx() const noexcept { return Vector4<T>(w, w, x, x); }
				constexpr Vector4<T> wwxy() const noexcept { return Vector4<T>(w, w, x, y); }
				constexpr Vector4<T> wwxz() const noexcept { return Vector4<T>(w, w, x, z); }
				constexpr Vector4<T> wwxw() const noexcept { return Vector4<T>(w, w, x, w); }
				constexpr Vector4<T> wwyx() const noexcept { return Vector4<T>(w, w, y, x); }
				constexpr Vector4<T> wwyy() const noexcept { return Vector4<T>(w, w, y, y); }
				constexpr Vector4<T> wwyz() const noexcept { return Vector4<T>(w, w, y, z); }
				constexpr Vector4<T> wwyw() const noexcept { return Vector4<T>(w, w, y, w); }
				constexpr Vector4<T> wwzx() const noexcept { return Vector4<T>(w, w, z, x); }
				constexpr Vector4<T> wwzy() const noexcept { return Vector4<T>(w, w, z, y); }
				constexpr Vector4<T> wwzz() const noexcept { return Vector4<T>(w, w, z, z); }
				constexpr Vector4<T> wwzw() const noexcept { return Vector4<T>(w, w, z, w); }
				constexpr Vector4<T> wwwx() const noexcept { return Vector4<T>(w, w, w, x); }
				constexpr Vector4<T> wwwy() const noexcept { return Vector4<T>(w, w, w, y); }
				constexpr Vector4<T> wwwz() const noexcept { return Vector4<T>(w, w, w, z); }
				constexpr Vector4<T> wwww() const noexcept { return Vector4<T>(w, w, w, w); }

				Vector4<T>& set(T xx, T yy, T zz) noexcept { x = xx; y = yy; z = zz; return *this; }
				Vector4<T>& set(T xx, T yy, T zz, T ww) noexcept { x = xx; y = yy; z = zz; w = ww; return *this; }
				Vector4<T>& set(const Vector2<T>& v) noexcept { x = v.x; y = v.y; return *this; }
				Vector4<T>& set(const Vector3<T>& v) noexcept { x = v.x; y = v.y; z = v.z; return *this; }
				Vector4<T>& set(T xyzw[3]) noexcept { x = xyzw[0]; y = xyzw[1]; z = xyzw[2]; w = xyzw[3]; return *this; }

				constexpr reference get(std::uint8_t index) noexcept
				{
					switch (index)
					{
					case 0 : return x;
					case 1 : return y;
					case 2 : return z;
					case 3 : return w;
					case 'x': return x;
					case 'y': return y;
					case 'z': return z;
					case 'w': return w;
					default:
						assert(false);
						return x;
					}
				}

				pointer ptr() noexcept { return (pointer)this; }
				const_pointer ptr() const noexcept { return (const_pointer)this; }
				pointer data() noexcept { return (pointer)this; }
				const_pointer data() const noexcept { return (const_pointer)this; }

			public:
				friend bool operator==(const Vector4<T>& v1, const Vector4<T>& v2) noexcept
				{
					return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
				}

				friend bool operator!=(const Vector4<T>& v1, const Vector4<T>& v2) noexcept
				{
					return !(v1 == v2);
				}

				friend Vector4<T> operator-(const Vector4<T>& v) noexcept
				{
					return Vector4<T>(-v.x, -v.y, -v.z, -v.w);
				}

				friend Vector4<T> operator+(const Vector4<T>& v1, const Vector4<T>& v2) noexcept
				{
					return Vector4<T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
				}

				friend Vector4<T> operator-(const Vector4<T>& v1, const Vector4<T>& v2) noexcept
				{
					return Vector4<T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
				}

				friend Vector4<T> operator*(const Vector4<T>& v1, const Vector4<T>& v2) noexcept
				{
					return Vector4<T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
				}

				friend Vector4<T> operator/(const Vector4<T>& v1, const Vector4<T>& v2) noexcept
				{
					return Vector4<T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
				}

				friend Vector4<T> operator*(const Vector4<T>& v1, T scale) noexcept
				{
					return Vector4<T>(v1.x * scale, v1.y * scale, v1.z * scale, v1.w * scale);
				}

				friend Vector4<T> operator/(const Vector4<T>& v1, T scale) noexcept
				{
					return Vector4<T>(v1.x / scale, v1.y / scale, v1.z / scale, v1.w / scale);
				}

				template<typename ostream, std::enable_if_t<trait::has_left_shift<ostream, T>::value, int> = 0>
				friend ostream& operator << (ostream& os, const Vector4<T>& v)
				{
					os << v.x << ", " << v.y << ", " << v.z << ", " << v.w;
					return os;
				}

				template<typename istream, std::enable_if_t<trait::has_right_shift<istream>::value, int> = 0>
				friend istream& operator >> (istream& is, Vector4<T>& v)
				{
					is >> v.x;
					is.ignore(2);
					is >> v.y;
					is.ignore(2);
					is >> v.z;
					is.ignore(2);
					is >> v.w;
					return is;
				}
			};

			template<typename T> const Vector4<T> Vector4<T>::Zero = Vector4<T>((T)0, (T)0, (T)0, (T)0);
			template<typename T> const Vector4<T> Vector4<T>::One = Vector4<T>((T)1, (T)1, (T)1, (T)1);
			template<typename T> const Vector4<T> Vector4<T>::UnitX = Vector4<T>((T)1, (T)0, (T)0, (T)1);
			template<typename T> const Vector4<T> Vector4<T>::UnitY = Vector4<T>((T)0, (T)1, (T)0, (T)1);
			template<typename T> const Vector4<T> Vector4<T>::UnitZ = Vector4<T>((T)0, (T)0, (T)1, (T)1);
			template<typename T> const Vector4<T> Vector4<T>::UnitW = Vector4<T>((T)0, (T)0, (T)0, (T)1);
		}

		template<typename T>
		inline T dot(const detail::Vector4<T>& v1, const detail::Vector4<T>& v2) noexcept
		{
			return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
		}

		template<typename T>
		inline T length2(const detail::Vector4<T>& v) noexcept
		{
			return dot(v, v);
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline T length(const detail::Vector4<T>& v) noexcept
		{
			return std::sqrt(length2(v));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline T distance(const detail::Vector4<T>& v1, const detail::Vector4<T>& v2) noexcept
		{
			return length(v1 - v2);
		}

		template<typename T>
		inline T sqrDistance(const detail::Vector4<T>& v1, const detail::Vector4<T>& v2) noexcept
		{
			return length2(v1 - v2);
		}

		template <typename T>
		inline T min4(const detail::Vector4<T>& v) noexcept
		{
			return std::min(std::min(std::min(v.x, v.y), v.z), v.w);
		}

		template <typename T>
		inline T max4(const detail::Vector4<T>& v) noexcept
		{
			return std::max(std::max(std::max(v.x, v.y), v.z), v.w);
		}

		template<typename T>
		inline bool equal(const detail::Vector4<T>& v1, const detail::Vector4<T>& v2) noexcept
		{
			return v1 == v2;
		}

		template<typename T>
		inline bool isfinite(const detail::Vector4<T>& v) noexcept
		{
			return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z) && std::isfinite(v.w);
		}

		template<typename T>
		inline detail::Vector4<T> cross(const detail::Vector4<T>& v1, const detail::Vector4<T>& v2) noexcept
		{
			return detail::Vector4<T>(
				v1.w * v2.x + v1.x * v2.w + v1.z * v2.y - v1.y * v2.z,
				v1.w * v2.y + v1.y * v2.w + v1.x * v2.z - v1.z * v2.x,
				v1.w * v2.z + v1.z * v2.w + v1.y * v2.x - v1.x * v2.y,
				v1.w * v2.w - v1.x * v2.x - v1.y * v2.y - v1.z * v2.z);
		}

		template<typename T>
		inline detail::Vector4<T> negate(const detail::Vector4<T>& v) noexcept
		{
			return detail::Vector4<T>(-v.x, -v.y, -v.z, -v.w);
		}

		template<typename T>
		inline detail::Vector4<T> clamp(const detail::Vector4<T>& t, T min, T max) noexcept
		{
			return detail::Vector4<T>(
				std::max(min, std::min(max, t.x)),
				std::max(min, std::min(max, t.y)),
				std::max(min, std::min(max, t.z)),
				std::max(min, std::min(max, t.w))
				);
		}

		template<typename T>
		inline detail::Vector4<T> saturate(const detail::Vector4<T>& v) noexcept
		{
			return clamp(v, T(0), T(1));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> normalize(const detail::Vector4<T>& v) noexcept
		{
			T magSq = length2(v);
			if (magSq > 0.0f)
			{
				T invSqrt = 1.0f / std::sqrt(magSq);
				return v * invSqrt;
			}

			return v;
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> snorm2unorm(const detail::Vector4<T>& v) noexcept
		{
			return v * 0.5f + 0.5f;
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> unorm2snorm(const detail::Vector4<T>& v) noexcept
		{
			return v * 2.0f - 1.0f;
		}

		template <typename T>
		inline detail::Vector4<T> min(const detail::Vector4<T>& a, const detail::Vector4<T>& b) noexcept
		{
			return detail::Vector4<T>(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w));
		}

		template <typename T>
		inline detail::Vector4<T> max(const detail::Vector4<T>& a, const detail::Vector4<T>& b) noexcept
		{
			return detail::Vector4<T>(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w));
		}

		template<typename T>
		inline detail::Vector4<T> abs(const detail::Vector4<T>& v) noexcept
		{
			return detail::Vector4<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> cos(const detail::Vector4<T>& v) noexcept
		{
			return detail::Vector4<T>(std::cos(v.x), std::cos(v.y), std::cos(v.z), std::cos(v.w));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> sin(const detail::Vector4<T>& v) noexcept
		{
			return detail::Vector4<T>(std::sin(v.x), std::sin(v.y), std::sin(v.z), std::sin(v.w));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> tan(const detail::Vector4<T>& v) noexcept
		{
			return detail::Vector4<T>(std::tan(v.x), std::tan(v.y), std::tan(v.z), std::tan(v.w));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> acos(const detail::Vector4<T>& v) noexcept
		{
			return detail::Vector4<T>(std::acos(v.x), std::acos(v.y), std::acos(v.z), std::acos(v.w));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> asin(const detail::Vector4<T>& v) noexcept
		{
			return detail::Vector4<T>(std::asin(v.x), std::asin(v.y), std::asin(v.z), std::asin(v.w));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> atan(const detail::Vector4<T>& v) noexcept
		{
			return detail::Vector4<T>(std::atan(v.x), std::atan(v.y), std::atan(v.z), std::atan(v.w));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> exp(const detail::Vector4<T>& v) noexcept
		{
			return detail::Vector4<T>(std::exp(v.x), std::exp(v.y), std::exp(v.z), std::exp(v.w));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> exp2(const detail::Vector4<T>& v) noexcept
		{
			return detail::Vector4<T>(std::exp2(v.x), std::exp2(v.y), std::exp2(v.z), std::exp2(v.w));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> log(const detail::Vector4<T>& v) noexcept
		{
			return detail::Vector4<T>(std::log(v.x), std::log(v.y), std::log(v.z), std::log(v.w));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> log2(const detail::Vector4<T>& v) noexcept
		{
			return detail::Vector4<T>(std::log2(v.x), std::log2(v.y), std::log2(v.z), std::log2(v.w));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> log10(const detail::Vector4<T>& v) noexcept
		{
			return detail::Vector4<T>(std::log10(v.x), std::log10(v.y), std::log10(v.z), std::log10(v.w));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> sqrt(const detail::Vector4<T>& v) noexcept
		{
			return detail::Vector4<T>(std::sqrt(v.x), std::sqrt(v.y), std::sqrt(v.z), std::sqrt(v.w));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> pow(const detail::Vector4<T>& v1, T scale) noexcept
		{
			return detail::Vector4<T>(std::pow(v1.x, scale), std::pow(v1.y, scale), std::pow(v1.z, scale), std::pow(v1.w, scale));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> pow(const detail::Vector4<T>& v1, const detail::Vector4<T>& v2) noexcept
		{
			return detail::Vector4<T>(std::pow(v1.x, v2.x), std::pow(v1.y, v2.y), std::pow(v1.z, v2.z), std::pow(v1.w, v2.w));
		}

		template<typename T>
		inline detail::Vector4<T> random(const detail::Vector4<T>& min, const detail::Vector4<T>& max) noexcept
		{
			return detail::Vector4<T>(random(min.x, max.x), random(min.y, max.y), random(min.z, max.z), random(min.w, max.w));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> ceil(const detail::Vector4<T>& v) noexcept
		{
			return detail::Vector4<T>(std::ceil(v.x), std::ceil(v.y), std::ceil(v.z), std::ceil(v.w));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector4<T> floor(const detail::Vector4<T>& v) noexcept
		{
			return detail::Vector4<T>(std::floor(v.x), std::floor(v.y), std::floor(v.z), std::floor(v.w));
		}
	}
}

#endif