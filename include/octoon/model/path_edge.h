#ifndef OCTOON_PATH_EDGE_H_
#define OCTOON_PATH_EDGE_H_

#include <octoon/model/path_types.h>
#include <functional>

namespace octoon
{
	struct PathPoint
	{
		math::float3 pt;
	};

	struct PathLine
	{
		math::float3 pt1;
		math::float3 pt2;
	};

	struct PathQuadratic
	{
		math::float3 pt1;
		math::float3 pt2;
		math::float3 control;
	};

	struct PathCubic
	{
		math::float3 pt1;
		math::float3 pt2;
		math::float3 control1;
		math::float3 control2;
	};

	class OCTOON_EXPORT PathEdge final
	{
	public:
		enum Type
		{
			Point,
			Line,
			Quadratic,
			Cubic
		};

		Type type;

		union
		{
			PathPoint point;
			PathLine line;
			PathQuadratic quad;
			PathCubic cubic;
		};

		PathEdge() noexcept;
		PathEdge(const math::float3& pt1) noexcept;
		PathEdge(const math::float3& pt1, const math::float3& pt2) noexcept;
		PathEdge(const math::float3& pt1, const math::float3& control, const math::float3& pt2) noexcept; // Quadratic Curve
		PathEdge(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2) noexcept; // Cubic Curve

		void makeEdge(const math::float3& point) noexcept;
		void makeEdge(const math::float3& pt1, const math::float3& pt2) noexcept;
		void makeEdge(const math::float3& pt1, const math::float3& control, const math::float3& pt2) noexcept; // Quadratic Curve
		void makeEdge(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2) noexcept; // Cubic Curve

		PathEdge& invoke(const std::function<math::float2(const math::float2&)>& func) noexcept;
		PathEdge& invoke(const std::function<math::float3(const math::float3&)>& func) noexcept;

	public:
		friend bool operator==(const PathEdge& a, const PathEdge& b) noexcept
		{
			if (a.type != b.type)
				return false;

			switch (a.type)
			{
			case Type::Point:
				if (a.point.pt != b.point.pt) return false;
				break;
			case Type::Line:
				if (a.line.pt1 != b.line.pt1) return false;
				if (a.line.pt2 != b.line.pt2) return false;
				break;
			case Type::Quadratic:
				if (a.quad.pt1 != b.quad.pt1) return false;
				if (a.quad.pt2 != b.quad.pt2) return false;
				if (a.quad.control != b.quad.control) return false;
				break;
			case Type::Cubic:
				if (a.cubic.pt1 != b.cubic.pt1) return false;
				if (a.cubic.pt2 != b.cubic.pt2) return false;
				if (a.cubic.control1 != b.cubic.control1) return false;
				if (a.cubic.control2 != b.cubic.control2) return false;
				break;
			default:
				return false;
			}

			return true;
		}

		friend bool operator!=(const PathEdge& a, const PathEdge& b) noexcept
		{
			return !(a == b);
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend PathEdge operator+(const PathEdge& edge, const T& scale) noexcept
		{
			PathEdge result;
			switch (edge.type)
			{
			case Type::Point:
				result.point.pt = edge.point.pt + scale;
				break;
			case Type::Line:
				result.line.pt1 = edge.line.pt1 + scale;
				result.line.pt2 = edge.line.pt2 + scale;
				break;
			case Type::Quadratic:
				result.quad.pt1 = edge.quad.pt1 + scale;
				result.quad.pt2 = edge.quad.pt2 + scale;
				result.quad.control = edge.quad.control + scale;
				break;
			case Type::Cubic:
				result.cubic.pt1 = edge.cubic.pt1 + scale;
				result.cubic.pt2 = edge.cubic.pt2 + scale;
				result.cubic.control1 = edge.cubic.control1 + scale;
				result.cubic.control2 = edge.cubic.control2 + scale;
				break;
			}

			return result;
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend PathEdge operator-(const PathEdge& edge, const T& scale) noexcept
		{
			PathEdge result;
			switch (edge.type)
			{
			case Type::Point:
				result.point.pt = edge.point.pt - scale;
				break;
			case Type::Line:
				result.line.pt1 = edge.line.pt1 - scale;
				result.line.pt2 = edge.line.pt2 - scale;
				break;
			case Type::Quadratic:
				result.quad.pt1 = edge.quad.pt1 - scale;
				result.quad.pt2 = edge.quad.pt2 - scale;
				result.quad.control = edge.quad.control - scale;
				break;
			case Type::Cubic:
				result.cubic.pt1 = edge.cubic.pt1 - scale;
				result.cubic.pt2 = edge.cubic.pt2 - scale;
				result.cubic.control1 = edge.cubic.control1 - scale;
				result.cubic.control2 = edge.cubic.control2 - scale;
				break;
			}

			return result;
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend PathEdge operator*(const PathEdge& edge, const T& scale) noexcept
		{
			PathEdge result;
			switch (edge.type)
			{
			case Type::Point:
				result.point.pt = edge.point.pt * scale;
				break;
			case Type::Line:
				result.line.pt1 = edge.line.pt1 * scale;
				result.line.pt2 = edge.line.pt2 * scale;
				break;
			case Type::Quadratic:
				result.quad.pt1 = edge.quad.pt1 * scale;
				result.quad.pt2 = edge.quad.pt2 * scale;
				result.quad.control = edge.quad.control * scale;
				break;
			case Type::Cubic:
				result.cubic.pt1 = edge.cubic.pt1 * scale;
				result.cubic.pt2 = edge.cubic.pt2 * scale;
				result.cubic.control1 = edge.cubic.control1 * scale;
				result.cubic.control2 = edge.cubic.control2 * scale;
				break;
			}

			return result;
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend PathEdge operator/(const PathEdge& edge, const T& scale) noexcept
		{
			PathEdge result;
			switch (edge.type)
			{
			case Type::Point:
				result.point.pt = edge.point.pt / scale;
				break;
			case Type::Line:
				result.line.pt1 = edge.line.pt1 / scale;
				result.line.pt2 = edge.line.pt2 / scale;
				break;
			case Type::Quadratic:
				result.quad.pt1 = edge.quad.pt1 / scale;
				result.quad.pt2 = edge.quad.pt2 / scale;
				result.quad.control = edge.quad.control / scale;
				break;
			case Type::Cubic:
				result.cubic.pt1 = edge.cubic.pt1 / scale;
				result.cubic.pt2 = edge.cubic.pt2 / scale;
				result.cubic.control1 = edge.cubic.control1 / scale;
				result.cubic.control2 = edge.cubic.control2 / scale;
				break;
			}

			return result;
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend PathEdge& operator+=(PathEdge& edge, const T& scale) noexcept
		{
			switch (edge.type)
			{
			case Type::Point:
				edge.point.pt += scale;
				break;
			case Type::Line:
				edge.line.pt1 += scale;
				edge.line.pt2 += scale;
				break;
			case Type::Quadratic:
				edge.quad.pt1 += scale;
				edge.quad.pt2 += scale;
				edge.quad.control += scale;
				break;
			case Type::Cubic:
				edge.cubic.pt1 += scale;
				edge.cubic.pt2 += scale;
				edge.cubic.control1 += scale;
				edge.cubic.control2 += scale;
				break;
			}

			return edge;
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend PathEdge& operator-=(PathEdge& edge, const T& scale) noexcept
		{
			switch (edge.type)
			{
			case Type::Point:
				edge.point.pt -= scale;
				break;
			case Type::Line:
				edge.line.pt1 -= scale;
				edge.line.pt2 -= scale;
				break;
			case Type::Quadratic:
				edge.quad.pt1 -= scale;
				edge.quad.pt2 -= scale;
				edge.quad.control -= scale;
				break;
			case Type::Cubic:
				edge.cubic.pt1 -= scale;
				edge.cubic.pt2 -= scale;
				edge.cubic.control1 -= scale;
				edge.cubic.control2 -= scale;
				break;
			}

			return edge;
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend PathEdge& operator*=(PathEdge& edge, const T& scale) noexcept
		{
			switch (edge.type)
			{
			case Type::Point:
				edge.point.pt *= scale;
				break;
			case Type::Line:
				edge.line.pt1 *= scale;
				edge.line.pt2 *= scale;
				break;
			case Type::Quadratic:
				edge.quad.pt1 *= scale;
				edge.quad.pt2 *= scale;
				edge.quad.control *= scale;
				break;
			case Type::Cubic:
				edge.cubic.pt1 *= scale;
				edge.cubic.pt2 *= scale;
				edge.cubic.control1 *= scale;
				edge.cubic.control2 *= scale;
				break;
			}

			return edge;
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend PathEdge& operator/=(PathEdge& edge, const T& scale) noexcept
		{
			switch (edge.type)
			{
			case Type::Point:
				edge.point.pt /= scale;
				break;
			case Type::Line:
				edge.line.pt1 /= scale;
				edge.line.pt2 /= scale;
				break;
			case Type::Quadratic:
				edge.quad.pt1 /= scale;
				edge.quad.pt2 /= scale;
				edge.quad.control /= scale;
				break;
			case Type::Cubic:
				edge.cubic.pt1 /= scale;
				edge.cubic.pt2 /= scale;
				edge.cubic.control1 /= scale;
				edge.cubic.control2 /= scale;
				break;
			}

			return edge;
		}

		friend PathEdge& operator<<(PathEdge& edge, const std::function<math::float2(const math::float2&)>& func) noexcept
		{
			return edge.invoke(func);
		}

		friend PathEdge& operator<<(PathEdge& edge, const std::function<math::float3(const math::float3&)>& func) noexcept
		{
			return edge.invoke(func);
		}

		friend std::ostream& operator << (std::ostream& os, const PathEdges& edges) noexcept
		{
			for (auto& it : edges)
				os << it;
			return os;
		}

		friend std::ostream& operator << (std::ostream& os, const PathEdge& e)
		{
			switch (e.type)
			{
			case Type::Point:
				os << "Point:";
				os << " pt " << e.point.pt << std::endl;
				break;
			case Type::Line:
				os << "Line:";
				os << " pt1 " << e.line.pt1;
				os << " pt2 " << e.line.pt2 << std::endl;
				break;
			case Type::Quadratic:
				os << "Quad:";
				os << " pt1 " << e.quad.pt1;
				os << " pt2 " << e.quad.pt2;
				os << " control " << e.quad.control << std::endl;
				break;
			case Type::Cubic:
				os << "Cubic:";
				os << " pt1 " << e.cubic.pt1;
				os << " pt2 " << e.cubic.pt2;
				os << " control1 " << e.cubic.control1;
				os << " control2 " << e.cubic.control2 << std::endl;
				break;
			}

			return os;
		}
	};

	namespace deform
	{
		/*
		* @brief Ť��Ч��
		* @param[in] x ˮƽ����ı仯������Χ�� -1.0 �� 1.0
		* @param[in] y ��ֱ����ı仯������Χ�� -1.0 �� 1.0
		* @paran[in] rotate ���η�ʽ�� true��ˮƽ������Σ� false����ֱ�������
		* @details AI�о���һ��������Ť�����ܣ�������ȡ����
		*
		*  ˮƽ������Σ� ��ʼ���� pt����ֹ��Ϊ pt.x - pt.x * pt.y
		*  ��ֱ������Σ� ��ʼ���� pt����ֹ��Ϊ pt.y - pt.x * pt.y
		*
		*  ���ʹ��lerp��������ʼ�����ֹ�㣬�������Բ�ֵ
		*
		* @return һ��lambda�ĺ��������� Contour::invoke, ContourGrpup::invoke, PathEdge::invoke��PathGroup::invoke���������е�ÿһ����ִ�ж������
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> twist(float x, float y, bool rotate) noexcept;

		/*
		* @brief ����
		* @param[in] x ˮƽ����ı仯���� ��Χ�� -1.0 �� 1.0
		* @param[in] ratio �������Ӿ��γ����, ʾ�� : w / h
		* @paran[in] rotate ���η�ʽ�� true��ˮƽ������Σ� false����ֱ�������
		* @details ��ʱû��ʵ��
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> fan(float x, float ratio, bool rotate) noexcept;

		/*
		* @brief �»���
		* @param[in] x ˮƽ����ı仯������Χ�� -1.0 �� 1.0
		* @param[in] ratio �������Ӿ��γ����, ʾ�� : w / h
		* @paran[in] rotate ���η�ʽ�� true��ˮƽ������Σ� false����ֱ�������
		* @details lambda�������ptΪ��һ����Ĳ���������ê�㴦��������ϵ
		*
		*  ˮƽ������Σ�
		*	x ��λ�ƣ�����ê���Ȩ��Ϊ0.5����ԵΪ1.0����ʹ�� (1.0f - abs(pt.x)�����Ʊ�Եλ�Ʋ�������߽磬��Ϊx�����ֹ��
		*	y ��λ�ƣ����Ÿ�Ϊ�޷��ţ���x���ϵĵ�ʹ��cos���AI�еĻ��Σ�λ������Ϊ����ĳ���ȣ�ratio����������y�ᣬ��Ϊy�᷽�����ֹ��
		*
		*  ��ֱ������Σ�
		*	x ��λ�ƣ����Ÿ�Ϊ�޷��ţ���y���ϵĵ�ʹ��cos���AI�еĻ��Σ�λ������Ϊ����ĳ���ȣ�ratio����������y�ᣬ��Ϊx�᷽�����ֹ��
		*	y ��λ�ƣ�����ê���Ȩ��Ϊ0.5����ԵΪ1.0����ʹ�� (1.0f - abs(pt.y)�����Ʊ�Եλ�Ʋ�������߽磬��Ϊy�����ֹ��
		*
		*  ���ʹ��lerp��������ʼ�����ֹ�㣬�������Բ�ֵ
		*
		* @return һ��lambda�ĺ��������� Contour::invoke, ContourGrpup::invoke, PathEdge::invoke��PathGroup::invoke���������е�ÿһ����ִ�ж������
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> coveLow(float x, float ratio, bool rotate) noexcept;

		/*
		* @brief �ϻ���
		* @param[in] x ˮƽ����ı仯���� ��Χ�� -1.0 �� 1.0
		* @param[in] ratio �������Ӿ��γ����, ʾ�� : w / h
		* @paran[in] rotate ���η�ʽ�� true��ˮƽ������Σ� false����ֱ�������
		* @details lambda�������ptΪ��һ����Ĳ���������ê�㴦��������ϵ
		*
		*  ˮƽ������Σ�
		*	x ��λ�ƣ�����ê���Ȩ��Ϊ0.5����ԵΪ1.0����ʹ�� (1.0f - abs(pt.x)�����Ʊ�Եλ�Ʋ�������߽磬��Ϊx�����ֹ��
		*	y ��λ�ƣ����Ÿ�Ϊ�޷��ţ���x���ϵĵ�ʹ��cos���AI�еĻ��Σ�λ������Ϊ����ĳ���ȣ�ratio����������y�ᣬ��Ϊy�᷽�����ֹ��
		*
		*  ��ֱ������Σ�
		*	x ��λ�ƣ����Ÿ�Ϊ�޷��ţ���y���ϵĵ�ʹ��cos���AI�еĻ��Σ�λ������Ϊ����ĳ���ȣ�ratio����������y�ᣬ��Ϊx�᷽�����ֹ��
		*	y ��λ�ƣ�����ê���Ȩ��Ϊ0.5����ԵΪ1.0����ʹ�� (1.0f - abs(pt.y)�����Ʊ�Եλ�Ʋ�������߽磬��Ϊy�����ֹ��
		*
		*  ���ʹ��lerp��������ʼ�����ֹ�㣬�������Բ�ֵ
		*
		* @return һ��lambda�ĺ��������� Contour::invoke, ContourGrpup::invoke, PathEdge::invoke��PathGroup::invoke���������е�ÿһ����ִ�ж������
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> coveHigh(float x, float ratio, bool rotate) noexcept;

		/*
		* @brief ����
		* @param[in] x ˮƽ����ı仯���� ��Χ�� -1.0 �� 1.0
		* @param[in] ratio �������Ӿ��γ����, ʾ�� : w / h
		* @paran[in] rotate ���η�ʽ�� true��ˮƽ������Σ� false����ֱ�������
		* @details lambda�������ptΪ��һ����Ĳ���������ê�㴦��������ϵ
		*
		*  ˮƽ������Σ�
		*	x ��λ�ƣ�����ê���Ȩ��Ϊ0.5����ԵΪ1.0����ʹ�� (1.0f - abs(pt.x)�����Ʊ�Եλ�Ʋ�������߽磬��Ϊx�����ֹ��
		*	y ��λ�ƣ���ȡx���ϵĵ�ʹ��cos���AI�еĻ��Σ�λ������Ϊ����ĳ���ȣ�ratio����������y�ᣬ��Ϊy�᷽�����ֹ��
		*
		*  ��ֱ������Σ�
		*	x ��λ�ƣ���ȡy���ϵĵ�ʹ��cos���AI�еĻ��Σ�λ������Ϊ����ĳ���ȣ�ratio����������x�ᣬ��Ϊx�᷽�����ֹ��
		*	y ��λ�ƣ�����ê���Ȩ��Ϊ0.5����ԵΪ1.0����ʹ�� (1.0f - abs(pt.y)�����Ʊ�Եλ�Ʋ�������߽磬��Ϊy�����ֹ��
		*
		*  ���ʹ��lerp��������ʼ�����ֹ�㣬�������Բ�ֵ, ���� (1.0f - abs(pt.x)) * (1.0f - abs(x))������ x �ı仯ʱ��AI�仯��ƥ��
		*
		* @return һ��lambda�ĺ��������� Contour::invoke, ContourGrpup::invoke, PathEdge::invoke��PathGroup::invoke���������е�ÿһ����ִ�ж������
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> cove(float x, float ratio, bool rotate) noexcept;

		/*
		* @brief ͻ��
		* @param[in] x ˮƽ����ı仯���� ��Χ�� -1.0 �� 1.0
		* @param[in] ratio �������Ӿ��γ����, ʾ�� : w / h
		* @paran[in] rotate ���η�ʽ�� true��ˮƽ������Σ� false����ֱ�������
		* @details lambda�������ptΪ��һ����Ĳ���������ê�㴦��������ϵ
		*
		*  ˮƽ������Σ�
		*	x ��λ�ƣ�����ê���Ȩ��Ϊ0.5����ԵΪ1.0����ʹ�� (1.0f - abs(pt.x)�����Ʊ�Եλ�Ʋ�������߽磬��Ϊx�����ֹ��
		*	y ��λ�ƣ���ȡx���ϵĵ�ʹ��cos���AI�еĻ��Σ����ű���Ϊ����ĳ���ȣ�ratio����������y�ᣬ��Ϊy�᷽�����ֹ��
		*
		*  ��ֱ������Σ�
		*	x ��λ�ƣ���ȡy���ϵĵ�ʹ��cos���AI�еĻ��Σ����ű���Ϊ����ĳ���ȣ�ratio����������x�ᣬ��Ϊx�᷽�����ֹ��
		*	y ��λ�ƣ�����ê���Ȩ��Ϊ0.5����ԵΪ1.0����ʹ�� (1.0f - abs(pt.y)�����Ʊ�Եλ�Ʋ�������߽磬��Ϊy�����ֹ��
		*
		*  ���ʹ��lerp��������ʼ�����ֹ�㣬�������Բ�ֵ, ���г��� abs(x)�����ڱ�֤x�ı仯��AI�仯��һ����
		*
		* @return һ��lambda�ĺ��������� Contour::invoke, ContourGrpup::invoke, PathEdge::invoke��PathGroup::invoke���������е�ÿһ����ִ�ж������
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> bulege(float x, float ratio, bool rotate) noexcept;

		/*
		* @brief ����
		* @param[in] x ˮƽ����ı仯���� ��Χ�� -1.0 �� 1.0
		* @param[in] ratio �������Ӿ��γ����, ʾ�� : w / h
		* @paran[in] rotate ���η�ʽ�� true��ˮƽ������Σ� false����ֱ�������
		* @details lambda�������ptΪ��һ����Ĳ���������ê�㴦��������ϵ
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> bulegeLow(float x, float ratio, bool rotate) noexcept;

		/*
		* @brief ͹��
		* @param[in] x ˮƽ����ı仯���� ��Χ�� -1.0 �� 1.0
		* @param[in] ratio �������Ӿ��γ����, ʾ�� : w / h
		* @paran[in] rotate ���η�ʽ�� true��ˮƽ������Σ� false����ֱ�������
		* @details lambda�������ptΪ��һ����Ĳ���������ê�㴦��������ϵ
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> bulegeHigh(float x, float ratio, bool rotate) noexcept;

		/* 
		* @brief ����
		* @param[in] x ˮƽ����ı仯������Χ�� -1.0 �� 1.0
		* @param[in] y ��ֱ����ı仯������Χ�� -1.0 �� 1.0
		* @details ���� pt Ϊ��һ�����ÿһ��������, �������Ŀ��Ƶ㣬��ʼ��Ϊpt����ֹ���� pt - sin(pt * Pi), ���� x �� y��ͨ��lerp�������Բ�ֵ
		* @return һ��lambda�ĺ��������� Contour::invoke, ContourGrpup::invoke, PathEdge::invoke��PathGroup::invoke���������е�ÿһ����ִ�ж������
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> flag(float x, float y) noexcept;

		/*
		* @brief ����
		* @param[in] x ˮƽ����ı仯������Χ�� -1.0 �� 1.0
		* @param[in] y ��ֱ����ı仯������Χ�� -1.0 �� 1.0
		* @details lambda�������ptΪ��һ����Ĳ���������ê�㴦��������ϵ
		*
		*  ˮƽ������Σ�
		*	��ʼ��Ϊ pt.x
		*	��ֹ��Ϊ pt.x - sin(pt.y * Pi) * (1.0f - abs(pt.x));
		*
		*  ��ֱ������Σ�
		*	��ʼ��Ϊ pt.y
		*	��ֹ��Ϊ pt.y - sin(pt.x * Pi) * (1.0f - abs(pt.y));
		*
		*  ʹ��lerp��������ʼ�����ֹ�㣬�������Բ�ֵ
		*
		* @return һ��lambda�ĺ��������� Contour::invoke, ContourGrpup::invoke, PathEdge::invoke��PathGroup::invoke���������е�ÿһ����ִ�ж������
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> wave(float x, float y) noexcept;

		/*
		* @brief ����
		* @param[in] x ˮƽ����ı仯������Χ�� -1.0 �� 1.0
		* @param[in] y ��ֱ����ı仯������Χ�� -1.0 �� 1.0
		* @param[in] weight ���εı��ʣ�ʹAI����һ�£�Ĭ�� 1.2
		* @details lambda�������ptΪ��һ����Ĳ���������ê�㴦��������ϵ
		*
		*  ˮƽ������Σ�
		*	��ʼ��Ϊ pt.x
		*	��ֹ��Ϊ pt.x + pt.x * sin(pt.y * Pi);
		*
		*  ��ֱ������Σ�
		*	��ʼ��Ϊ pt.y
		*	��ֹ��Ϊ pt.y + pt.y * sin(pt.x * Pi);
		*
		*  ʹ��lerp��������ʼ�����ֹ�㣬�������Բ�ֵ
		*
		* @return һ��lambda�ĺ��������� Contour::invoke, ContourGrpup::invoke, PathEdge::invoke��PathGroup::invoke���������е�ÿһ����ִ�ж������
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> fish(float x, float y, float weight = 1.2f) noexcept;

		/*
		* @brief ����
		* @param[in] x ˮƽ����ı仯������Χ�� -1.0 �� 1.0
		* @param[in] y ��ֱ����ı仯������Χ�� -1.0 �� 1.0
		* @details lambda�������ptΪ��һ����Ĳ���������ê�㴦��������ϵ
		*
		*  ˮƽ������Σ�
		*	��ʼ��Ϊ pt.x
		*	��ֹ��Ϊ pt.x + pt.x * sin(pt.y * Pi);
		*
		*  ��ֱ������Σ�
		*	��ʼ��Ϊ pt.y
		*	��ֹ��Ϊ pt.y + pt.y * sin(pt.x * Pi);
		*
		*  ʹ��lerp��������ʼ�����ֹ�㣬�������Բ�ֵ
		*
		* @return һ��lambda�ĺ��������� Contour::invoke, ContourGrpup::invoke, PathEdge::invoke��PathGroup::invoke���������е�ÿһ����ִ�ж������
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> slope(float x, float y) noexcept;

		/*
		* @brief ����
		* @param[in] x ˮƽ����ı仯���� ��Χ�� -1.0 �� 1.0
		* @details lambda�������ptΪ��һ����Ĳ���������ê�㴦��������ϵ
		*
		* Solid angle��http://www.rorydriscoll.com/2012/01/15/cubemap-texel-solid-angle/
		*
		* ϸ�ڣ�
		*	��ʼ��Ϊ pt
		*	��ֹ��Ϊ pt + pt * solid angle * (1.0f - pt^4), ���� (1.0f - pt^4) ���Ʊ�Ե���
		*
		*  ʹ��lerp��������ʼ�����ֹ�㣬�������Բ�ֵ
		*
		* @return һ��lambda�ĺ��������� Contour::invoke, ContourGrpup::invoke, PathEdge::invoke��PathGroup::invoke���������е�ÿһ����ִ�ж������
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> fishEye(float x) noexcept;

		/*
		* @brief ����
		* @param[in] x ˮƽ����ı仯���� ��Χ�� -1.0 �� 1.0
		* @param[in] ratio �������Ӿ��γ����, ʾ�� : w / h
		* @paran[in] rotate ���η�ʽ�� true��ˮƽ������Σ� false����ֱ�������
		* @details lambda�������ptΪ��һ����Ĳ���������ê�㴦��������ϵ
		*
		*  ˮƽ������Σ�
		*	��ʼ��Ϊ pt
		*	��ֹ��Ϊ pt + pt * (���ĳ��� - ê�㵽pt�ĳ���) * (ê������Ȩ��Ϊ0.5����ԵȨ��Ϊ1.0�Ĳ�ֵ)
		*
		*  ʹ��lerp��������ʼ�����ֹ�㣬�������Բ�ֵ
		*
		* @return һ��lambda�ĺ��������� Contour::invoke, ContourGrpup::invoke, PathEdge::invoke��PathGroup::invoke���������е�ÿһ����ִ�ж������
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> expandOut(float x, float ratio, bool rotate) noexcept;

		/*
		* @brief ��ѹ
		* @param[in] x ˮƽ����ı仯���� ��Χ�� -1.0 �� 1.0
		* @param[in] ratio �������Ӿ��γ����, ʾ�� : w / h
		* @paran[in] rotate ���η�ʽ�� true��ˮƽ������Σ� false����ֱ�������
		* @details lambda�������ptΪ��һ����Ĳ���������ê�㴦��������ϵ
		*
		*  ���Σ�
		*	��ʼ��Ϊ pt.x
		*	��ֹ��Ϊ pt.x - pt.x * (���ĳ��� - ê�㵽pt�ĳ���) * (ê������Ȩ��Ϊ0.5����ԵȨ��Ϊ1.0) * (���η�ʽ, ˮƽΪ -x����ֱΪ x)
		*	��ֹ��Ϊ pt.y + pt.y * (���ĳ��� - ê�㵽pt�ĳ���) * (ê������Ȩ��Ϊ0.5����ԵȨ��Ϊ1.0) * (���η�ʽ, ˮƽΪ -x����ֱΪ x)
		*
		*  ʹ��lerp��������ʼ�����ֹ�㣬�������Բ�ֵ
		*
		* @return һ��lambda�ĺ��������� Contour::invoke, ContourGrpup::invoke, PathEdge::invoke��PathGroup::invoke���������е�ÿһ����ִ�ж������
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> expandIn(float x, float ratio, bool rotate) noexcept;

		/*
		* @brief Ťת
		* @param[in] x ˮƽ����ı仯���� ��Χ�� -1.0 �� 1.0
		* @param[in] ratio �������Ӿ��γ����, ʾ�� : w / h
		* @paran[in] rotate ���η�ʽ�� true��ˮƽ������Σ� false����ֱ�������
		* @details lambda�������ptΪ��һ����Ĳ���������ê�㴦��������ϵ
		*
		*  ���Σ� ê�㵽pt�ϵľ�����Ϊ��ת�Ƕȣ���pt�ϵ�ÿ������ת
		*
		* @return һ��lambda�ĺ��������� Contour::invoke, ContourGrpup::invoke, PathEdge::invoke��PathGroup::invoke���������е�ÿһ����ִ�ж������
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> spin(float x, float ratio, bool rotate) noexcept;

		/*
		* @brief Ťת
		* @param[in] x ˮƽ����ı仯������Χ�� -1.0 �� 1.0
		* @param[in] y ��ֱ����ı仯������Χ�� -1.0 �� 1.0
		* @details lambda�������ptΪ��һ����Ĳ���������ê�㴦��������ϵ
		*
		*  ���Σ� http://tksharpless.net/vedutismo/Pannini/panini.pdf
		*
		* @return һ��lambda�ĺ��������� Contour::invoke, ContourGrpup::invoke, PathEdge::invoke��PathGroup::invoke���������е�ÿһ����ִ�ж������
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> panini(float x, float y) noexcept;
	}
}

#endif