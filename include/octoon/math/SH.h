#ifndef OCTOON_MATHSH_H_
#define OCTOON_MATH_SH_H_

#include <octoon/math/vector4.h>

namespace octoon
{
	namespace math
	{
		enum class SHCubeFace : std::uint8_t
		{
			FACE_POS_X = 0,
			FACE_NEG_X = 1,
			FACE_POS_Y = 2,
			FACE_NEG_Y = 3,
			FACE_POS_Z = 4,
			FACE_NEG_Z = 5,
		};

		namespace detail
		{
			template<typename T, std::uint8_t N>
			class SH final
			{
			public:
				using value_t = T;
				using size_t = std::size_t;

				static constexpr float CosineA0 = math::PI;
				static constexpr float CosineA1 = math::PI_2 / 3.0f;
				static constexpr float CosineA2 = math::PI / 4.0f;

				value_t coeff[N];

				SH() noexcept {}
				SH(value_t coeffs) noexcept { for (size_t i = 0; i < N; ++i) this->coeff[i] = coeffs; }

				template<typename S, typename = std::enable_if<std::is_pointer<S>::value>>
				explicit SH(S coeffs[N]) noexcept
				{
					for (size_t i = 0; i < N; i++)
					{
						assert(coeffs[i] <= std::numeric_limits<typename trait::type_addition<S>::value_type>::max());
						coeff[i] = coeffs[i];
					}
				}

				SH& operator+=(const SH& sh) noexcept { for (size_t i = 0; i < N; ++i) coeff[i] += sh.coeff[i]; return *this; }
				SH& operator-=(const SH& sh) noexcept { for (size_t i = 0; i < N; ++i) coeff[i] -= sh.coeff[i]; return *this; }
				SH& operator*=(const SH& sh) noexcept { for (size_t i = 0; i < N; ++i) coeff[i] *= sh.coeff[i]; return *this; }
				SH& operator/=(const SH& sh) noexcept { for (size_t i = 0; i < N; ++i) coeff[i] /= sh.coeff[i]; return *this; }

				template<typename T, typename = std::enable_if_t<std::is_same<T, value_t>::value || !std::is_same<T, value_t>::value && std::is_floating_point<T>::value>>
				SH& operator+=(const T& scale) noexcept
				{
					for (std::uint8_t i = 0; i < N; ++i)
						coeff[i] += scale;
					return *this;
				}

				template<typename T, typename = std::enable_if_t<std::is_same<T, value_t>::value || !std::is_same<T, value_t>::value && std::is_floating_point<T>::value>>
				SH& operator-=(const T& scale) noexcept
				{
					for (std::uint8_t i = 0; i < N; ++i)
						coeff[i] -= scale;
					return *this;
				}

				template<typename T, typename = std::enable_if_t<std::is_same<T, value_t>::value || !std::is_same<T, value_t>::value && std::is_floating_point<T>::value>>
				SH& operator*=(const T& scale) noexcept
				{
					for (std::uint8_t i = 0; i < N; ++i)
						coeff[i] *= scale;
					return *this;
				}

				template<typename T, typename = std::enable_if_t<std::is_same<T, value_t>::value || !std::is_same<T, value_t>::value && std::is_floating_point<T>::value>>
				SH& operator/=(const T& scale) noexcept
				{
					for (std::uint8_t i = 0; i < N; ++i)
						coeff[i] /= scale;
					return *this;
				}

				value_t& operator[](size_t n) noexcept { assert(n < N); return coeff[n]; }

				const value_t& operator[](size_t n) const noexcept { assert(n < N); return coeff[n]; }

				void ConvolveWithCosineKernel() noexcept
				{
					coeff[0] *= CosineA0;

					for (size_t i = 1; i < N; ++i)
						if (i < 4)
							coeff[i] *= CosineA1;
						else if (i < 9)
							coeff[i] *= CosineA2;
				}

				friend SH<T, N> operator+(const SH<T, N>& sh1, const SH<T, N>& sh2) noexcept
				{
					SH<T, N> result;
					for (SH<T, N>::size_t i = 0; i < N; ++i)
						result.coeff[i] = sh1.coeff[i] + sh2.coeff[i];
					return result;
				}

				friend SH<T, N> operator-(const SH<T, N>& sh1, const SH<T, N>& sh2) noexcept
				{
					SH<T, N> result;
					for (SH<T, N>::size_t i = 0; i < N; ++i)
						result.coeff[i] = sh1.coeff[i] - sh2.coeff[i];
					return result;
				}

				friend SH<T, N> operator*(const SH<T, N>& sh1, const SH<T, N>& sh2) noexcept
				{
					SH<T, N> result;
					for (SH<T, N>::size_t i = 0; i < N; ++i)
						result.coeff[i] = sh1.coeff[i] * sh2.coeff[i];
					return result;
				}

				friend SH<T, N> operator/(const SH<T, N>& sh1, const SH<T, N>& sh2) noexcept
				{
					SH<T, N> result;
					for (SH<T, N>::size_t i = 0; i < N; ++i)
						result.coeff[i] = sh1.coeff[i] / sh2.coeff[i];
					return result;
				}

				template<typename T, typename S, std::uint8_t N, typename = std::enable_if_t<std::is_same<T, S>::value || !std::is_same<T, S>::value && std::is_floating_point<T>::value>>
				friend SH<T, N> operator+(const SH<T, N>& sh1, const S& sh2) noexcept
				{
					SH<T, N> result;
					for (SH<T, N>::size_t i = 0; i < N; ++i)
						result.coeff[i] = sh1.coeff[i] + sh2;
					return result;
				}

				template<typename T, typename S, std::uint8_t N, typename = std::enable_if_t<std::is_same<T, S>::value || !std::is_same<T, S>::value && std::is_floating_point<T>::value>>
				friend SH<T, N> operator-(const SH<T, N>& sh1, const S& sh2) noexcept
				{
					SH<T, N> result;
					for (SH<T, N>::size_t i = 0; i < N; ++i)
						result.coeff[i] = sh1.coeff[i] - sh2;
					return result;
				}

				template<typename T, typename S, std::uint8_t N, typename = std::enable_if_t<std::is_same<T, S>::value || !std::is_same<T, S>::value && std::is_floating_point<T>::value>>
				friend SH<T, N> operator*(const SH<T, N>& sh1, const S& sh2) noexcept
				{
					SH<T, N> result;
					for (SH<T, N>::size_t i = 0; i < N; ++i)
						result.coeff[i] = sh1.coeff[i] * sh2;
					return result;
				}

				template<typename T, typename S, std::uint8_t N, typename = std::enable_if_t<std::is_same<T, S>::value || !std::is_same<T, S>::value && std::is_floating_point<T>::value>>
				friend SH<T, N> operator/(const SH<T, N>& sh1, const S& sh2) noexcept
				{
					SH<T, N> result;
					for (SH<T, N>::size_t i = 0; i < N; ++i)
						result.coeff[i] = sh1.coeff[i] / sh2;
					return result;
				}

				template<typename T, typename S, std::uint8_t N, typename = std::enable_if_t<std::is_same<T, S>::value || !std::is_same<T, S>::value && std::is_floating_point<T>::value>>
				friend SH<detail::Vector3<T>, N> operator+(const SH<detail::Vector3<T>, N>& sh1, const S& scale) noexcept
				{
					SH<detail::Vector3<T>, N> result;
					for (std::uint8_t i = 0; i < N; ++i)
						result.coeff[i] = sh1.coeff[i] + scale;
					return result;
				}

				template<typename T, typename S, std::uint8_t N, typename = std::enable_if_t<std::is_same<T, S>::value || !std::is_same<T, S>::value && std::is_floating_point<T>::value>>
				friend SH<detail::Vector3<T>, N> operator-(const SH<detail::Vector3<T>, N>& sh1, const S& scale) noexcept
				{
					SH<detail::Vector3<T>, N> result;
					for (std::uint8_t i = 0; i < N; ++i)
						result.coeff[i] = sh1.coeff[i] - scale;
					return result;
				}

				template<typename T, typename S, std::uint8_t N, typename = std::enable_if_t<std::is_same<T, S>::value || !std::is_same<T, S>::value && std::is_floating_point<T>::value>>
				friend SH<detail::Vector3<T>, N> operator*(const SH<detail::Vector3<T>, N>& sh1, const S& scale) noexcept
				{
					SH<detail::Vector3<T>, N> result;
					for (std::uint8_t i = 0; i < N; ++i)
						result.coeff[i] = sh1.coeff[i] * scale;
					return result;
				}

				template<typename T, typename S, std::uint8_t N, typename = std::enable_if_t<std::is_same<T, S>::value || !std::is_same<T, S>::value && std::is_floating_point<T>::value>>
				friend SH<detail::Vector3<T>, N> operator/(const SH<detail::Vector3<T>, N>& sh1, const S& scale) noexcept
				{
					SH<detail::Vector3<T>, N> result;
					for (std::uint8_t i = 0; i < N; ++i)
						result.coeff[i] = sh1.coeff[i] / scale;
					return result;
				}

				friend SH<detail::Vector3<T>, N> operator*(const SH<detail::Vector3<T>, N>& sh1, const detail::Vector3<T>& color) noexcept
				{
					SH<detail::Vector3<T>, N> result;
					for (SH<T, N>::size_t i = 0; i < N; ++i)
						result.coeff[i] = sh1.coeff[i] * color;
					return result;
				}

				friend SH<detail::Vector3<T>, N> operator/(const SH<detail::Vector3<T>, N>& sh1, const detail::Vector3<T>& color) noexcept
				{
					SH<detail::Vector3<T>, N> result;
					for (SH<T, N>::size_t i = 0; i < N; ++i)
						result.coeff[i] = sh1.coeff[i] / color;
					return result;
				}

				friend SH<detail::Vector3<T>, N> operator*(const SH<T, N>& sh1, const detail::Vector3<T>& color) noexcept
				{
					SH<T, N> result;
					for (SH<T, N>::size_t i = 0; i < N; ++i)
						result.coeff[i] = sh1.coeff[i] * color;
					return result;
				}

				friend SH<detail::Vector3<T>, N> operator/(const SH<T, N>& sh1, const detail::Vector3<T>& color) noexcept
				{
					SH<T, N> result;
					for (SH<T, N>::size_t i = 0; i < N; ++i)
						result.coeff[i] = sh1.coeff[i] / color;
					return result;
				}

				friend SH<detail::Vector3<T>, N> operator*(const detail::Vector3<T>& color, const SH<T, N>& sh1) noexcept
				{
					SH<detail::Vector3<T>, N> result;
					for (SH<T, N>::size_t i = 0; i < N; ++i)
						result.coeff[i] = color * sh1.coeff[i];
					return result;
				}

				friend SH<detail::Vector3<T>, N> operator/(const detail::Vector3<T>& color, const SH<T, N>& sh1) noexcept
				{
					SH<detail::Vector3<T>, N> result;
					for (SH<T, N>::size_t i = 0; i < N; ++i)
						result.coeff[i] = color / sh1.coeff[i];
					return result;
				}

				friend SH<detail::Vector3<T>, N> operator*(const detail::Vector3<T>& color, const SH<detail::Vector3<T>, N>& sh1) noexcept
				{
					SH<detail::Vector3<T>, N> result;
					for (SH<T, N>::size_t i = 0; i < N; ++i)
						result.coeff[i] = color * sh1.coeff[i];
					return result;
				}

				friend SH<detail::Vector3<T>, N> operator/(const detail::Vector3<T>& color, const SH<detail::Vector3<T>, N>& sh1) noexcept
				{
					SH<detail::Vector3<T>, N> result;
					for (SH<T, N>::size_t i = 0; i < N; ++i)
						result.coeff[i] = color / sh1.coeff[i];
					return result;
				}
			};
		}

		template<typename T, std::uint8_t N>
		inline T dot(const detail::SH<T, N>& a, const detail::SH<T, N>& b) noexcept
		{
			T result = T(0);
			for (std::uint8_t i = 0; i < N; ++i)
				result += a.coeff[i] * b.coeff[i];
			return result;
		}

		template<typename T, std::uint8_t N>
		inline T length2(const detail::SH<T, N>& sh) noexcept
		{
			return dot(sh, sh);
		}

		template<typename T, std::uint8_t N>
		inline T length(const detail::SH<T, N>& sh) noexcept
		{
			return math::sqrt(dot(sh, sh));
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> min(const detail::SH<T, N>& a, const detail::SH<T, N>& b) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::min(a.coeff[i], b.coeff[i]);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> max(const detail::SH<T, N>& a, const detail::SH<T, N>& b) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::max(a.coeff[i], b.coeff[i]);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> normalize(const detail::SH<T, N>& sh) noexcept
		{
			T magSq = length2(sh);
			if (magSq > 0)
			{
				T invSqrt = 1.0 / sqrt(magSq);
				return sh * invSqrt;
			}

			return sh;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> negate(const detail::SH<T, N>& sh) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = -sh.coeff[i];
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> isfinite(const detail::SH<T, N>& sh) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::isfinite(sh.coeff[i]);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> abs(const detail::SH<T, N>& sh) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::abs(sh.coeff[i]);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> cos(const detail::SH<T, N>& sh) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::cos(sh.coeff[i]);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> sin(const detail::SH<T, N>& sh) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::sin(sh.coeff[i]);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> tan(const detail::SH<T, N>& sh) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::tan(sh.coeff[i]);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> acos(const detail::SH<T, N>& sh) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::acos(sh.coeff[i]);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> asin(const detail::SH<T, N>& sh) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::asin(sh.coeff[i]);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> atan(const detail::SH<T, N>& sh) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::atan(sh.coeff[i]);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> exp(const detail::SH<T, N>& sh) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::exp(sh.coeff[i]);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> exp2(const detail::SH<T, N>& sh) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::exp2(sh.coeff[i]);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> sqrt(const detail::SH<T, N>& sh) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::sqrt(sh.coeff[i]);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> log(const detail::SH<T, N>& sh) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::log(sh.coeff[i]);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> log2(const detail::SH<T, N>& sh) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::log2(sh.coeff[i]);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> log10(const detail::SH<T, N>& sh) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::log10(sh.coeff[i]);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> pow(const detail::SH<T, N>& sh, T y) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::pow(sh.coeff[i], y);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> pow(const detail::SH<T, N>& a, const detail::SH<T, N>& b) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::pow(a.coeff[i], b.coeff[i]);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> ceil(const detail::SH<T, N>& sh) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::ceil(sh.coeff[i]);
			return result;
		}

		template<typename T, std::uint8_t N>
		inline detail::SH<T, N> floor(const detail::SH<T, N>& sh) noexcept
		{
			detail::SH<T, N> result;
			for (std::uint8_t i = 0; i < N; ++i)
				result.coeff[i] = std::ceil(sh.coeff[i]);
			return result;
		}

		template<std::uint8_t N, typename T, typename = std::enable_if_t<N == 4>>
		inline detail::SH<T, 4> ProjectOntoSH(const detail::Vector3<T>& dir) noexcept
		{
			detail::SH<T, 4> sh;
			sh.coeff[0] = 0.282095f;
			sh.coeff[1] = -0.488603f * dir.y;
			sh.coeff[2] = 0.488603f * dir.z;
			sh.coeff[3] = -0.488603f * dir.x;

			return sh;
		}

		template<std::uint8_t N, typename T, typename = std::enable_if_t<N == 9>>
		inline detail::SH<T, 9> ProjectOntoSH(const detail::Vector3<T>& dir) noexcept
		{
			const T x = dir[0];
			const T y = dir[1];
			const T z = dir[2];

			const T x2 = x * x;
			const T y2 = y * y;
			const T z2 = z * z;

			const T xy = x * y;
			const T xz = x * z;
			const T yz = y * z;

			detail::SH<T, 9> sh;
			sh.coeff[0] = 0.282095f;
			sh.coeff[1] = -0.488603f * y;
			sh.coeff[2] = 0.488603f * z;
			sh.coeff[3] = -0.488603f * x;
			sh.coeff[4] = 1.092548f * xy;
			sh.coeff[5] = -1.092548f * yz;
			sh.coeff[6] = 0.315392f * (3.0f * z2 - 1.0f);
			sh.coeff[7] = -1.092548f * xz;
			sh.coeff[8] = 0.546274f * (x2 - y2);

			return sh;
		}

		template<std::uint8_t N, typename T, typename = std::enable_if_t<N == 16>>
		inline detail::SH<T, 16> ProjectOntoSH(const detail::Vector3<T>& dir) noexcept
		{
			const T x = dir[0];
			const T y = dir[1];
			const T z = dir[2];

			const T x2 = x * x;
			const T y2 = y * y;
			const T z2 = z * z;

			const T xy = x * y;
			const T xz = x * z;
			const T yz = y * z;

			detail::SH<T, 16> sh;
			sh.coeff[0] = 0.282095f;
			sh.coeff[1] = -0.488603f * y;
			sh.coeff[2] = 0.488603f * z;
			sh.coeff[3] = -0.488603f * x;

			sh.coeff[4] = 1.092548f * xy;
			sh.coeff[5] = -1.092548f * yz;
			sh.coeff[6] = 0.315392f * (3.0f * z2 - 1.0f);
			sh.coeff[7] = -1.092548f * xz;
			sh.coeff[8] = 0.546274f * (x2 - y2);

			sh.coeff[9] = -0.590044f * y * (3.0f * x2 - y2);
			sh.coeff[10] = 2.890610f * x * yz;
			sh.coeff[11] = -0.457046f * y * (-1.0f + 5.0f * z2);
			sh.coeff[12] = 0.373176f * z * (5.0f * z2 - 3.0f);
			sh.coeff[13] = 0.457046f * x * (1.0f - 5.0f * z2);
			sh.coeff[14] = 1.445310f * z * (x2 - y2);
			sh.coeff[15] = -0.590044f * x * (x2 - 3.0f * y2);

			return sh;
		}

		template<std::uint8_t N, typename T, typename = std::enable_if_t<N == 25>>
		inline detail::SH<T, 25> ProjectOntoSH(const detail::Vector3<T>& dir) noexcept
		{
			const T x = dir[0];
			const T y = dir[1];
			const T z = dir[2];

			const T x2 = x * x;
			const T y2 = y * y;
			const T z2 = z * z;

			const T xy = x * y;
			const T xz = x * z;
			const T yz = y * z;

			const T x4 = x2 * x2;
			const T y4 = y2 * y2;
			const T z4 = z2 * z2;

			detail::SH<T, 25> sh;
			sh.coeff[0] = 0.5f / std::sqrt(PI);
			sh.coeff[1] = -std::sqrt(3.0f / (PI * 4)) * y;
			sh.coeff[2] = std::sqrt(3.0f / (PI * 4)) * z;
			sh.coeff[3] = -std::sqrt(3.0f / (PI * 4)) * x;

			sh.coeff[4] = std::sqrt(15.0f / (PI * 4)) * xy;
			sh.coeff[5] = -std::sqrt(15.0f / (PI * 4)) * yz;
			sh.coeff[6] = std::sqrt(5.0f / (PI * 16)) * (3.0f * z2 - 1.0f);
			sh.coeff[7] = -std::sqrt(15.0f / (PI * 4)) * xz;
			sh.coeff[8] = std::sqrt(15.0f / (PI * 16)) * (x2 - y2);

			sh.coeff[9] = -std::sqrt(70.0f / (PI * 64)) * y * (3 * x2 - y2);
			sh.coeff[10] = std::sqrt(105.0f / (PI * 4)) * y * xz;
			sh.coeff[11] = -std::sqrt(21.0f / (PI * 16)) * y * (-1.0f + 5.0f * z2);
			sh.coeff[12] = std::sqrt(7.0f / (PI * 16)) * z * (5.0f * z2 - 3.0f);
			sh.coeff[13] = -std::sqrt(42.0f / (PI * 64)) * x * (-1.0f + 5.0f * z2);
			sh.coeff[14] = std::sqrt(105.0f / (PI * 16)) * z * (x2 - y2);
			sh.coeff[15] = -std::sqrt(70.0f / (PI * 64)) * x * (x2 - 3.0f * y2);

			sh.coeff[16] = std::sqrt(35.0f / (PI * 16)) * 3.0f * xy * (x2 - y2);
			sh.coeff[17] = -std::sqrt(70.0f / (PI * 64)) * 3.0f * yz * (3.0f * x2 - y2);
			sh.coeff[18] = std::sqrt(5.0f / (PI * 16)) * 3.0f * xy * (-1.0f + 7.0f * z2);
			sh.coeff[19] = -std::sqrt(10.0f / (PI * 64)) * 3.0f * yz * (-3.0f + 7.0f * z2);
			sh.coeff[20] = (105.0f * z4 - 90.0f * z2 + 9.0f) / (16.0f * std::sqrt(PI));
			sh.coeff[21] = -std::sqrt(10.0f / (PI * 64)) * 3.0f * xz * (-3.0f + 7.0f * z2);
			sh.coeff[22] = std::sqrt(5.0f / (PI * 64)) * 3.0f * (x2 - y2) * (-1.0f + 7.0f * z2);
			sh.coeff[23] = -std::sqrt(70.0f / (PI * 64)) * 3.0f * xz * (x2 - 3.0f * y2);
			sh.coeff[24] = std::sqrt(35.0f / (PI * 256)) * 3.0f * (x4 - 6.0f * y2 * x2 + y4);

			return sh;
		}

		template<std::uint8_t N, typename T, typename = std::enable_if_t<N == 36>>
		inline detail::SH<T, 36> ProjectOntoSH(const detail::Vector3<T>& dir) noexcept
		{
			detail::SH<T, 36> sh;
			sh.coeff[0] = 0.5f / std::sqrt(PI);
			sh.coeff[1] = -0.5f / std::sqrt(PI / 3.0f) * dir->y;
			sh.coeff[2] = 0.5f / std::sqrt(PI / 3.0f) * dir->z;
			sh.coeff[3] = -0.5f / std::sqrt(PI / 3.0f) * dir->x;
			sh.coeff[4] = 0.5f / std::sqrt(PI / 15.0f) * dir->x * dir->y;
			sh.coeff[5] = -0.5f / std::sqrt(PI / 15.0f) * dir->y * dir->z;
			sh.coeff[6] = 0.25f / std::sqrt(PI / 5.0f) * (3.0f * dir->z * dir->z - 1.0f);
			sh.coeff[7] = -0.5f / std::sqrt(PI / 15.0f) * dir->x * dir->z;
			sh.coeff[8] = 0.25f / std::sqrt(PI / 15.0f) * (dir->x * dir->x - dir->y * dir->y);
			sh.coeff[9] = -std::sqrt(70.0f / PI) / 8.0f * dir->y * (3.0f * dir->x * dir->x - dir->y * dir->y);
			sh.coeff[10] = std::sqrt(105.0f / PI) / 2.0f * dir->x * dir->y * dir->z;
			sh.coeff[11] = -std::sqrt(42.0 / PI) / 8.0f * dir->y * (-1.0f + 5.0f * dir->z * dir->z);
			sh.coeff[12] = std::sqrt(7.0f / PI) / 4.0f * dir->z * dir->z * dir->z * 5.0f - 3.0f * dir->z;
			sh.coeff[13] = std::sqrt(42.0 / PI) / 8.0f * dir->x * (1.0f - 5.0f * dir->z * dir->z);
			sh.coeff[14] = std::sqrt(105.0f / PI) / 4.0f * dir->z * (dir->x * dir->x - dir->y * dir->y);
			sh.coeff[15] = -std::sqrt(70.0f / PI) / 8.0f * dir->x * (dir->x * dir->x - 3.0f * dir->y * dir->y);
			sh.coeff[16] = std::sqrt(35.0f / PI) * dir->x * dir->y * (dir->x * dir->x - dir->y * dir->y) * 0.75;
			sh.coeff[17] = 3.0f * dir->z * sh.coeff[9];
			sh.coeff[18] = std::sqrt(5.0f / PI) * dir->x * dir->y * (7.0f * dir->z * dir->z - 1.0f) * 0.75;
			sh.coeff[19] = std::sqrt(10.0f / PI) * dir->y * dir->z * (3.0f - 7.0f * dir->z * dir->z) * 0.375f;
			sh.coeff[20] = 3.0f / (16.0f * std::sqrt(PI)) * (35.0f * dir->z * dir->z * dir->z * dir->z - 30.f * dir->z * dir->z + 3.0f);
			sh.coeff[21] = std::sqrt(10.0f / PI) * dir->x * dir->z * (3.0f - 7.0f * dir->z * dir->z) * 0.375f;
			sh.coeff[22] = std::sqrt(5.0f / PI) * (dir->x * dir->x - dir->y * dir->y) * (7.0f * dir->z * dir->z - 1.0f) * 0.375f;
			sh.coeff[23] = 3.0 * dir->z * sh.coeff[15];
			sh.coeff[24] = std::sqrt(35.0f / PI) * (dir->x * dir->x * dir->x * dir->x - 6.0f * dir->x * dir->x * dir->y * dir->y + dir->y * dir->y * dir->y * dir->y) * 3.0f / 16.0f;
			sh.coeff[25] = -std::sqrt(154.0f / PI) * dir->y * (5.0f * dir->x * dir->x * dir->x * dir->x - 10.0f * dir->x * dir->x * dir->y * dir->y + dir->y * dir->y * dir->y * dir->y) * 3.0f / 32.0f;
			sh.coeff[26] = std::sqrt(385.0f / PI) * dir->x * dir->y * dir->z * (dir->x * dir->x - dir->y * dir->y) * 0.75f;
			sh.coeff[27] = std::sqrt(770.0f / PI) / 32.0f * dir->y * (3.0f * dir->x * dir->x - dir->y * dir->y) * (1.0f - 9.0f * dir->z * dir->z);
			sh.coeff[28] = std::sqrt(1155.0f / PI) / 4.0f * dir->x * dir->y * dir->z * (3.0f * dir->z * dir->z - 1.0f);
			sh.coeff[29] = std::sqrt(165.0f / PI) / 16.0f * dir->y * (14.0f * dir->z * dir->z - 21.0f * dir->z * dir->z * dir->z * dir->z - 1.0f);
			sh.coeff[30] = std::sqrt(11.0f / PI) / 16.0f * dir->z * (63.0f * dir->z * dir->z * dir->z * dir->z - 70.0f * dir->z * dir->z + 15.0f);
			sh.coeff[31] = std::sqrt(165.0f / PI) / 16.0f * dir->x * (14.0f * dir->z * dir->z - 21.0f * dir->z * dir->z * dir->z * dir->z - 1.0f);
			sh.coeff[32] = std::sqrt(1155.0f / PI) / 8.0f * dir->z * (dir->x * dir->x - dir->y * dir->y) * (3.0f * dir->z * dir->z - 1.0f);
			sh.coeff[33] = std::sqrt(770.0f / PI) / 32.0f * dir->x * (dir->x * dir->x - 3.0f * dir->y * dir->y) * (1.0f - 9.0f * dir->z * dir->z);
			sh.coeff[34] = 3.0f / 16.0f * std::sqrt(385.0f / PI) * dir->z * (dir->x * dir->x * dir->x * dir->x - 6.0 * dir->x * dir->x * dir->y * dir->y + dir->y * dir->y * dir->y * dir->y);
			sh.coeff[35] = -3.0f / 32.0f * std::sqrt(154.0f / PI) * dir->x * (dir->x * dir->x * dir->x * dir->x - 10.0f * dir->x * dir->x * dir->y * dir->y + 5.0f * dir->y * dir->y * dir->y * dir->y);
			return sh;
		}

		template<typename T>
		inline detail::SH<T, 4> ProjectOntoH4(const detail::Vector3<T>& dir) noexcept
		{
			detail::SH<T, 4> result;
			result[0] = (1.0f / std::sqrt(2.0f * PI));
			result[1] = std::sqrt(1.5f / PI) * dir.y;
			result[2] = std::sqrt(1.5f / PI) * (2 * dir.z - 1.0f);
			result[3] = std::sqrt(1.5f / PI) * dir.x;

			return result;
		}

		template<typename T>
		inline detail::Vector3<T> EvalSH4Cosine(const detail::Vector3<T>& dir, const detail::SH<detail::Vector3<T>, 4>& sh) noexcept
		{
			detail::SH<T, 4> dirSH = ProjectOntoSH4(dir);
			dirSH.coeff[0] *= detail::SH<T, 4>::CosineA0;
			dirSH.coeff[1] *= detail::SH<T, 4>::CosineA1;
			dirSH.coeff[2] *= detail::SH<T, 4>::CosineA1;
			dirSH.coeff[3] *= detail::SH<T, 4>::CosineA1;

			detail::SH<T, 4> result;
			for (std::uint8_t i = 0; i < 4; ++i)
				result += dirSH.coeff[i] * sh.coeff[i];

			return result;
		}

		template<typename T>
		inline detail::Vector3<T> EvalSH9Cosine(const detail::Vector3<T>& dir, const detail::SH<detail::Vector3<T>, 9>& sh) noexcept
		{
			detail::SH<T, 9> dirSH = ProjectOntoSH9(dir);
			dirSH.coeff[0] *= detail::SH<T, 9>::CosineA0;
			dirSH.coeff[1] *= detail::SH<T, 9>::CosineA1;
			dirSH.coeff[2] *= detail::SH<T, 9>::CosineA1;
			dirSH.coeff[3] *= detail::SH<T, 9>::CosineA1;
			dirSH.coeff[4] *= detail::SH<T, 9>::CosineA2;
			dirSH.coeff[5] *= detail::SH<T, 9>::CosineA2;
			dirSH.coeff[6] *= detail::SH<T, 9>::CosineA2;
			dirSH.coeff[7] *= detail::SH<T, 9>::CosineA2;
			dirSH.coeff[8] *= detail::SH<T, 9>::CosineA2;

			detail::Vector3<T> result;
			for (std::uint8_t i = 0; i < 9; ++i)
				result += dirSH.coeff[i] * sh.coeff[i];

			return result;
		}

		template<typename T>
		inline T EvalH4(const detail::SH<T, 4>& h, const detail::Vector3<T>& dir) noexcept
		{
			H4 b = ProjectOntoH4(dir);
			return math::dot(h, b);
		}

		template<typename T>
		inline detail::SH<T, 4> ConvertToH4(const detail::SH<T, 9>& sh) noexcept
		{
			const T rt2 = sqrt(2.0f);
			const T rt32 = sqrt(3.0f / 2.0f);
			const T rt52 = sqrt(5.0f / 2.0f);
			const T rt152 = sqrt(15.0f / 2.0f);
			const T convMatrix[4][9] =
			{
				{ 1.0f / rt2, 0, 0.5f * rt32, 0, 0, 0, 0, 0, 0 },
				{ 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0, 0, 0 },
				{ 0, 0, 1.0f / (2.0f * rt2), 0, 0, 0, 0.25f * rt152, 0, 0 },
				{ 0, 0, 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0 }
			};

			detail::SH<T, 4> hBasis;

			for (std::uint8_t row = 0; row < 4; ++row)
			{
				hBasis.coeff[row] = 0.0f;
				for (std::uint8_t col = 0; col < 9; ++col)
					hBasis.coeff[row] += convMatrix[row][col] * sh.coeff[col];
			}

			return hBasis;
		}

		template<typename T>
		inline detail::SH<T, 6> ConvertToH6(const detail::SH<T, 9>& sh) noexcept
		{
			const T rt2 = sqrt(2.0f);
			const T rt32 = sqrt(3.0f / 2.0f);
			const T rt52 = sqrt(5.0f / 2.0f);
			const T rt152 = sqrt(15.0f / 2.0f);
			const T convMatrix[6][9] =
			{
				{ 1.0f / rt2, 0, 0.5f * rt32, 0, 0, 0, 0, 0, 0 },
				{ 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0, 0, 0 },
				{ 0, 0, 1.0f / (2.0f * rt2), 0, 0, 0, 0.25f * rt152, 0, 0 },
				{ 0, 0, 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0 },
				{ 0, 0, 0, 0, 1.0f / rt2, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 1.0f / rt2 }
			};

			detail::SH<T, 6> hBasis;

			for (std::uint8_t row = 0; row < 6; ++row)
			{
				hBasis.coeff[row] = 0.0f;
				for (std::uint8_t col = 0; col < 9; ++col)
					hBasis.coeff[row] += convMatrix[row][col] * sh.coeff[col];
			}

			return hBasis;
		}

		template<typename T>
		inline detail::SH<detail::Vector3<T>, 4> ConvertToH4(const detail::SH<detail::Vector3<T>, 9>& sh) noexcept
		{
			const T rt2 = sqrt(2.0f);
			const T rt32 = sqrt(3.0f / 2.0f);
			const T rt52 = sqrt(5.0f / 2.0f);
			const T rt152 = sqrt(15.0f / 2.0f);
			const T convMatrix[4][9] =
			{
				{ 1.0f / rt2, 0, 0.5f * rt32, 0, 0, 0, 0, 0, 0 },
				{ 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0, 0, 0 },
				{ 0, 0, 1.0f / (2.0f * rt2), 0, 0, 0, 0.25f * rt152, 0, 0 },
				{ 0, 0, 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0 }
			};

			detail::SH<detail::Vector3<T>, 4> hBasis;

			for (std::uint8_t row = 0; row < 4; ++row)
			{
				hBasis.coeff[row].set(0.0f);
				for (std::uint8_t col = 0; col < 9; ++col)
					hBasis.coeff[row] += convMatrix[row][col] * sh.coeff[col];
			}

			return hBasis;
		}

		template<typename T>
		inline detail::SH<detail::Vector3<T>, 6> ConvertToH6(const detail::SH<detail::Vector3<T>, 9>& sh) noexcept
		{
			const T rt2 = sqrt(2.0f);
			const T rt32 = sqrt(3.0f / 2.0f);
			const T rt52 = sqrt(5.0f / 2.0f);
			const T rt152 = sqrt(15.0f / 2.0f);
			const T convMatrix[6][9] =
			{
				{ 1.0f / rt2, 0, 0.5f * rt32, 0, 0, 0, 0, 0, 0 },
				{ 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0, 0, 0 },
				{ 0, 0, 1.0f / (2.0f * rt2), 0, 0, 0, 0.25f * rt152, 0, 0 },
				{ 0, 0, 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0 },
				{ 0, 0, 0, 0, 1.0f / rt2, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 1.0f / rt2 }
			};

			detail::SH<detail::Vector3<T>, 6> hBasis;

			for (std::uint8_t row = 0; row < 6; ++row)
			{
				hBasis.coeff[row].set(0.0f);
				for (std::uint8_t col = 0; col < 9; ++col)
					hBasis.coeff[row] += convMatrix[row][col] * sh.coeff[col];
			}

			return hBasis;
		}

		template<typename T>
		inline detail::Vector3<T> CalcCubeNormal(T u, T v, SHCubeFace face) noexcept
		{
			switch (face)
			{
			case SHCubeFace::FACE_POS_X:
				return normalize(detail::Vector3<T>(1.0f, -v, -u));
			case SHCubeFace::FACE_NEG_X:
				return normalize(detail::Vector3<T>(-1.0f, -v, u));
			case SHCubeFace::FACE_POS_Y:
				return normalize(detail::Vector3<T>(u, 1.0f, v));
			case SHCubeFace::FACE_NEG_Y:
				return normalize(detail::Vector3<T>(u, -1.0f, -v));
			case SHCubeFace::FACE_POS_Z:
				return normalize(detail::Vector3<T>(u, -v, 1.0f));
			case SHCubeFace::FACE_NEG_Z:
				return normalize(detail::Vector3<T>(-u, -v, -1.0f));
			default:
				assert(false);
				return detail::Vector3<T>::Zero;
			}
		}

		template<typename T>
		inline detail::Vector3<T> CalcCubeNormal(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, SHCubeFace face) noexcept
		{
			const T u = ((static_cast<T>(x) + 0.5f) / static_cast<T>(w)) * 2.0f - 1.0f;
			const T v = ((static_cast<T>(y) + 0.5f) / static_cast<T>(h)) * 2.0f - 1.0f;
			return CalcCubeNormal(u, v, face);
		}

		template<SHCubeFace face, typename T, std::enable_if_t<face == SHCubeFace::FACE_POS_X, int> = 0>
		inline detail::Vector3<T> CalcCubeNormal(T u, T v) noexcept
		{
			return normalize(detail::Vector3<T>(1.0f, -v, -u));
		}

		template<SHCubeFace face, typename T, std::enable_if_t<face == SHCubeFace::FACE_NEG_X, int> = 0>
		inline detail::Vector3<T> CalcCubeNormal(T u, T v) noexcept
		{
			return normalize(detail::Vector3<T>(-1.0f, -v, u));
		}

		template<SHCubeFace face, typename T, std::enable_if_t<face == SHCubeFace::FACE_POS_Y, int> = 0>
		inline detail::Vector3<T> CalcCubeNormal(T u, T v) noexcept
		{
			return normalize(detail::Vector3<T>(u, 1.0f, v));
		}

		template<SHCubeFace face, typename T, std::enable_if_t<face == SHCubeFace::FACE_NEG_Y, int> = 0>
		inline detail::Vector3<T> CalcCubeNormal(T u, T v) noexcept
		{
			return normalize(detail::Vector3<T>(u, -1.0f, -v));
		}

		template<SHCubeFace face, typename T, std::enable_if_t<face == SHCubeFace::FACE_POS_Z, int> = 0>
		inline detail::Vector3<T> CalcCubeNormal(T u, T v) noexcept
		{
			return normalize(detail::Vector3<T>(u, -v, 1.0f));
		}

		template<SHCubeFace face, typename T, std::enable_if_t<face == SHCubeFace::FACE_NEG_Z, int> = 0>
		inline detail::Vector3<T> CalcCubeNormal(T u, T v) noexcept
		{
			return normalize(detail::Vector3<T>(-u, -v, -1.0f));
		}

		template<SHCubeFace face, typename T, std::enable_if_t<face == SHCubeFace::FACE_POS_X, int> = 0>
		inline detail::Vector3<T> CalcCubeNormal(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h) noexcept
		{
			const T u = ((static_cast<T>(x) + 0.5f) / static_cast<T>(w)) * 2.0f - 1.0f;
			const T v = ((static_cast<T>(y) + 0.5f) / static_cast<T>(h)) * 2.0f - 1.0f;
			return normalize(detail::Vector3<T>(1.0f, -v, -u));
		}

		template<SHCubeFace face, typename T, std::enable_if_t<face == SHCubeFace::FACE_NEG_X, int> = 0>
		inline detail::Vector3<T> CalcCubeNormal(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h) noexcept
		{
			const T u = ((static_cast<T>(x) + 0.5f) / static_cast<T>(w)) * 2.0f - 1.0f;
			const T v = ((static_cast<T>(y) + 0.5f) / static_cast<T>(h)) * 2.0f - 1.0f;
			return normalize(detail::Vector3<T>(-1.0f, -v, u));
		}

		template<SHCubeFace face, typename T, std::enable_if_t<face == SHCubeFace::FACE_POS_Y, int> = 0>
		inline detail::Vector3<T> CalcCubeNormal(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h) noexcept
		{
			const T u = ((static_cast<T>(x) + 0.5f) / static_cast<T>(w)) * 2.0f - 1.0f;
			const T v = ((static_cast<T>(y) + 0.5f) / static_cast<T>(h)) * 2.0f - 1.0f;
			return normalize(detail::Vector3<T>(u, 1.0f, v));
		}

		template<SHCubeFace face, typename T, std::enable_if_t<face == SHCubeFace::FACE_NEG_Y, int> = 0>
		inline detail::Vector3<T> CalcCubeNormal(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h) noexcept
		{
			const T u = ((static_cast<T>(x) + 0.5f) / static_cast<T>(w)) * 2.0f - 1.0f;
			const T v = ((static_cast<T>(y) + 0.5f) / static_cast<T>(h)) * 2.0f - 1.0f;
			return normalize(detail::Vector3<T>(u, -1.0f, -v));
		}

		template<SHCubeFace face, typename T, std::enable_if_t<face == SHCubeFace::FACE_POS_Z, int> = 0>
		inline detail::Vector3<T> CalcCubeNormal(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h) noexcept
		{
			const T u = ((static_cast<T>(x) + 0.5f) / static_cast<T>(w)) * 2.0f - 1.0f;
			const T v = ((static_cast<T>(y) + 0.5f) / static_cast<T>(h)) * 2.0f - 1.0f;
			return normalize(detail::Vector3<T>(u, -v, 1.0f));
		}

		template<SHCubeFace face, typename T, std::enable_if_t<face == SHCubeFace::FACE_NEG_Z, int> = 0>
		inline detail::Vector3<T> CalcCubeNormal(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h) noexcept
		{
			const T u = ((static_cast<T>(x) + 0.5f) / static_cast<T>(w)) * 2.0f - 1.0f;
			const T v = ((static_cast<T>(y) + 0.5f) / static_cast<T>(h)) * 2.0f - 1.0f;
			return normalize(detail::Vector3<T>(-u, -v, -1.0f));
		}

		template<std::uint8_t N, typename value_t = float, typename data_t = float, SHCubeFace face, std::enable_if_t<(N == 6 || N == 9 || N == 25), int> = 0>
		detail::SH<detail::Vector3<value_t>, N> CalcCubefaceToSH(std::uint32_t w, std::uint32_t h, const data_t* data, std::uint8_t channel, double& weights) noexcept
		{
			assert(data);
			assert(w == h);

			detail::SH<detail::Vector3<value_t>, N> result(detail::Vector3<value_t>::Zero);

			for (std::uint32_t y = 0; y < h; ++y)
			{
				for (std::uint32_t x = 0; x < w; ++x)
				{
					const auto& sample = (detail::Vector3<data_t>&)data[(static_cast<std::uint8_t>(face)* w* h + y * w + x)* channel];

					const value_t u = ((static_cast<value_t>(x) + 0.5f) / static_cast<value_t>(w)) * 2.0f - 1.0f;
					const value_t v = ((static_cast<value_t>(y) + 0.5f) / static_cast<value_t>(h)) * 2.0f - 1.0f;

					const double temp = 1.0 + u * u + v * v;
					const double weight = 4.0 / (std::sqrt(temp) * temp);

					const detail::Vector3<value_t> dir = CalcCubeNormal<face, value_t>(u, v);

					const SH<value_t, N> sh = ProjectOntoSH<N, value_t>(dir);

					for (std::uint8_t i = 0; i < N; ++i)
						result.coeff[i] += sample * sh.coeff[i] * weight;

					weights += weight;
				}
			}

			return result;
		}

		template<std::uint8_t N = 9, typename value_t = float, typename data_t = float, std::enable_if_t<N == 6 || N == 9 || N == 25, int> = 0>
		detail::SH<detail::Vector3<value_t>, N> CalcCubemapToSH(std::uint32_t w, std::uint32_t h, const data_t* data, std::uint8_t channel = 3) noexcept
		{
			assert(data);
			assert(w == h);

			double weightSum = 0.0;

			detail::SH<detail::Vector3<value_t>, N> result(detail::Vector3<value_t>::Zero);
			result += CalcCubefaceToSH<N, value_t, data_t, SHCubeFace::FACE_POS_X>(w, h, data, channel, weightSum);
			result += CalcCubefaceToSH<N, value_t, data_t, SHCubeFace::FACE_NEG_X>(w, h, data, channel, weightSum);
			result += CalcCubefaceToSH<N, value_t, data_t, SHCubeFace::FACE_POS_Y>(w, h, data, channel, weightSum);
			result += CalcCubefaceToSH<N, value_t, data_t, SHCubeFace::FACE_NEG_Y>(w, h, data, channel, weightSum);
			result += CalcCubefaceToSH<N, value_t, data_t, SHCubeFace::FACE_POS_Z>(w, h, data, channel, weightSum);
			result += CalcCubefaceToSH<N, value_t, data_t, SHCubeFace::FACE_NEG_Z>(w, h, data, channel, weightSum);

			const double norm = (4.0 * PI) / weightSum;
			return result * norm;
		}

		template<std::uint8_t N, typename value_t = float, typename data_t = float, SHCubeFace face, std::enable_if_t<(N == 9 || N == 16), int> = 0>
		void CalcCubefaceToIrradiance(const detail::SH<detail::Vector3<value_t>, N>& shColor, std::uint32_t w, std::uint32_t h, detail::Vector3<data_t>* dst) noexcept
		{
			assert(dst);
			assert(w == h);

			detail::Vector3<data_t>* data = dst + w * h * static_cast<std::uint8_t>(face);

			for (std::uint32_t y = 0; y < h; ++y)
			{
				for (std::uint32_t x = 0; x < w; ++x)
				{
					const auto basis = ProjectOntoSH<N, value_t>(CalcCubeNormal<face, value_t>(x, y, w, h));
					detail::Vector3<value_t> color = shColor.coeff[0] * basis[0];

					std::uint8_t n = 1;
					for (; n < 4; ++n)
						color += shColor.coeff[n] * basis[n] * (2.0f / 3.0f);

					for (; n < 9; ++n)
						color += shColor.coeff[n] * basis[n] * (1.0f / 4.0f);

					*(data++) = static_cast<detail::Vector3<data_t>>(color);
				}
			}
		}

		template<std::uint8_t N, typename value_t = float, typename data_t = float, SHCubeFace face, std::enable_if_t<(N == 25), int> = 0>
		void CalcCubefaceToIrradiance(const detail::SH<detail::Vector3<value_t>, N>& shColor, std::uint32_t w, std::uint32_t h, detail::Vector3<data_t>* dst) noexcept
		{
			assert(dst);
			assert(w == h);

			detail::Vector3<data_t>* data = dst + w * h * static_cast<std::uint8_t>(face);

			for (std::uint32_t y = 0; y < h; ++y)
			{
				for (std::uint32_t x = 0; x < w; ++x)
				{
					const auto basis = ProjectOntoSH<N, value_t>(CalcCubeNormal<face, value_t>(x, y, w, h));

					detail::Vector3<value_t> color = shColor.coeff[0] * basis[0];

					std::uint8_t n = 1;
					for (; n < 4; ++n)
						color += shColor.coeff[n] * basis[n] * (2.0f / 3.0f);

					for (; n < 9; ++n)
						color += shColor.coeff[n] * basis[n] * (1.0f / 4.0f);

					for (n = 16; n < 25; ++n)
						color += shColor.coeff[n] * basis[n] * (-1.0f / 24.0f);

					*(data++) = static_cast<detail::Vector3<data_t>>(color);
				}
			}
		}

		template<std::uint8_t N = 9, typename value_t = float, typename data_t = float, std::enable_if_t<N == 6 || N == 9 || N == 25, int> = 0>
		void CalcCubemapToIrradiance(const detail::SH<detail::Vector3<value_t>, N>& shColor, std::uint32_t w, std::uint32_t h, detail::Vector3<data_t>* dst) noexcept
		{
			assert(dst);
			assert(w == h);

			CalcCubefaceToIrradiance<N, value_t, data_t, SHCubeFace::FACE_POS_X>(shColor, w, h, dst);
			CalcCubefaceToIrradiance<N, value_t, data_t, SHCubeFace::FACE_NEG_X>(shColor, w, h, dst);
			CalcCubefaceToIrradiance<N, value_t, data_t, SHCubeFace::FACE_POS_Y>(shColor, w, h, dst);
			CalcCubefaceToIrradiance<N, value_t, data_t, SHCubeFace::FACE_NEG_Y>(shColor, w, h, dst);
			CalcCubefaceToIrradiance<N, value_t, data_t, SHCubeFace::FACE_POS_Z>(shColor, w, h, dst);
			CalcCubefaceToIrradiance<N, value_t, data_t, SHCubeFace::FACE_NEG_Z>(shColor, w, h, dst);
		}
	}
}

#endif