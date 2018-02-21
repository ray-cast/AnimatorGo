#ifndef OCTOON_MATHFWD_H_
#define OCTOON_MATHFWD_H_

#include <vector>

#include <octoon/math/mathutil.h>

namespace octoon
{
	namespace math
	{
		namespace detail
		{
			template<typename T = float>
			class Vector2;

			template<typename T = float>
			class Vector3;

			template<typename T = float>
			class Vector4;

			template<typename T = float>
			class Matrix2x2;

			template<typename T = float>
			class Matrix3x3;

			template<typename T = float>
			class Matrix4x4;

			template<typename T = float>
			class Quaternion;
		}

		// default
		using Vector2 = detail::Vector2<float>;
		using Vector3 = detail::Vector3<float>;
		using Vector4 = detail::Vector4<float>;
		using Matrix2x2 = detail::Matrix2x2<float>;
		using Matrix3x3 = detail::Matrix3x3<float>;
		using Matrix4x4 = detail::Matrix4x4<float>;
		using Quaternion = detail::Quaternion<float>;

		// float
		using float2x2 = detail::Matrix2x2<float>;
		using float3x3 = detail::Matrix3x3<float>;
		using float4x4 = detail::Matrix4x4<float>;
		using Quaternionf = detail::Quaternion<float>;

		// double
		using double2x2 = detail::Matrix2x2<double>;
		using double3x3 = detail::Matrix3x3<double>;
		using double4x4 = detail::Matrix4x4<double>;
		using Quaterniond = detail::Quaternion<double>;

		// char
		using char1 = std::int8_t;
		using char2 = detail::Vector2<std::int8_t>;
		using char3 = detail::Vector3<std::int8_t>;
		using char4 = detail::Vector4<std::int8_t>;

		// uchar
		using uchar1 = std::uint8_t;
		using uchar2 = detail::Vector2<std::uint8_t>;
		using uchar3 = detail::Vector3<std::uint8_t>;
		using uchar4 = detail::Vector4<std::uint8_t>;

		// short
		using short1 = std::int16_t;
		using short2 = detail::Vector2<std::int16_t>;
		using short3 = detail::Vector3<std::int16_t>;
		using short4 = detail::Vector4<std::int16_t>;

		// ushort
		using ushort1 = std::uint16_t;
		using ushort2 = detail::Vector2<std::uint16_t>;
		using ushort3 = detail::Vector3<std::uint16_t>;
		using ushort4 = detail::Vector4<std::uint16_t>;

		// int
		using int1 = std::int32_t;
		using int2 = detail::Vector2<std::int32_t>;
		using int3 = detail::Vector3<std::int32_t>;
		using int4 = detail::Vector4<std::int32_t>;

		// uint
		using uint1 = std::uint32_t;
		using uint2 = detail::Vector2<std::uint32_t>;
		using uint3 = detail::Vector3<std::uint32_t>;
		using uint4 = detail::Vector4<std::uint32_t>;

		// float
		using float1 = float;
		using float2 = detail::Vector2<float>;
		using float3 = detail::Vector3<float>;
		using float4 = detail::Vector4<float>;

		// double
		using double1 = double;
		using double2 = detail::Vector2<double>;
		using double3 = detail::Vector3<double>;
		using double4 = detail::Vector4<double>;

		// array
		using IntArray = std::vector<int>;
		using Int2Array = std::vector<int2>;
		using Int3Array = std::vector<int3>;
		using Int4Array = std::vector<int4>;
		using Uint1Array = std::vector<uint1>;
		using Uint2Array = std::vector<uint2>;
		using Uint3Array = std::vector<uint3>;
		using Uint4Array = std::vector<uint4>;
		using Vector2Array = std::vector<Vector2>;
		using Vector3Array = std::vector<Vector3>;
		using Vector4Array = std::vector<Vector4>;
		using FloatArray = std::vector<float> ;
		using Float2Array = std::vector<float2>;
		using Float3Array = std::vector<float3>;
		using Float4Array = std::vector<float4>;
		using DoubleArray = std::vector<double>;
		using Double2Array = std::vector<double2>;
		using Double3Array = std::vector<double3>;
		using Double4Array = std::vector<double4>;
		using Float2x2Array = std::vector<float2x2>;
		using Float3x3Array = std::vector<float3x3>;
		using Float4x4Array = std::vector<float4x4>;
		using double2x2Array = std::vector<double2x2>;
		using double3x3Array = std::vector<double3x3>;
		using double4x4Array = std::vector<double4x4>;
	}
}

#endif