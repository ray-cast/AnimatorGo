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
		* @brief 扭曲效果
		* @param[in] x 水平方向的变化量，范围在 -1.0 至 1.0
		* @param[in] y 垂直方向的变化量，范围在 -1.0 至 1.0
		* @paran[in] rotate 变形方式， true：水平方向变形， false：垂直方向变形
		* @details AI中具有一个公共的扭曲功能，单独提取出来
		*
		*  水平方向变形： 起始点在 pt，终止点为 pt.x - pt.x * pt.y
		*  垂直方向变形： 起始点在 pt，终止点为 pt.y - pt.x * pt.y
		*
		*  最后使用lerp函数对起始点和终止点，进行线性插值
		*
		* @return 一个lambda的函数，用于 Contour::invoke, ContourGrpup::invoke, PathEdge::invoke，PathGroup::invoke，对轮廓中的每一个点执行顶点变形
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> twist(float x, float y, bool rotate) noexcept;

		/*
		* @brief 弧形
		* @param[in] x 水平方向的变化量， 范围在 -1.0 至 1.0
		* @param[in] ratio 字体的外接矩形长宽比, 示例 : w / h
		* @paran[in] rotate 变形方式， true：水平方向变形， false：垂直方向变形
		* @details 暂时没有实现
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> fan(float x, float ratio, bool rotate) noexcept;

		/*
		* @brief 下弧形
		* @param[in] x 水平方向的变化量，范围在 -1.0 至 1.0
		* @param[in] ratio 字体的外接矩形长宽比, 示例 : w / h
		* @paran[in] rotate 变形方式， true：水平方向变形， false：垂直方向变形
		* @details lambda中输入的pt为归一化后的参数，并在锚点处建立坐标系
		*
		*  水平方向变形：
		*	x 的位移，靠近锚点的权重为0.5，边缘为1.0，并使用 (1.0f - abs(pt.x)，限制边缘位移不能溢出边界，作为x轴的终止点
		*	y 的位移，符号改为无符号，对x轴上的点使用cos拟合AI中的弧形，位移数量为字体的长宽比（ratio）作用在于y轴，作为y轴方向的终止点
		*
		*  垂直方向变形：
		*	x 的位移，符号改为无符号，对y轴上的点使用cos拟合AI中的弧形，位移数量为字体的长宽比（ratio）作用在于y轴，作为x轴方向的终止点
		*	y 的位移，靠近锚点的权重为0.5，边缘为1.0，并使用 (1.0f - abs(pt.y)，限制边缘位移不能溢出边界，作为y轴的终止点
		*
		*  最后使用lerp函数对起始点和终止点，进行线性插值
		*
		* @return 一个lambda的函数，用于 Contour::invoke, ContourGrpup::invoke, PathEdge::invoke，PathGroup::invoke，对轮廓中的每一个点执行顶点变形
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> coveLow(float x, float ratio, bool rotate) noexcept;

		/*
		* @brief 上弧形
		* @param[in] x 水平方向的变化量， 范围在 -1.0 至 1.0
		* @param[in] ratio 字体的外接矩形长宽比, 示例 : w / h
		* @paran[in] rotate 变形方式， true：水平方向变形， false：垂直方向变形
		* @details lambda中输入的pt为归一化后的参数，并在锚点处建立坐标系
		*
		*  水平方向变形：
		*	x 的位移，靠近锚点的权重为0.5，边缘为1.0，并使用 (1.0f - abs(pt.x)，限制边缘位移不能溢出边界，作为x轴的终止点
		*	y 的位移，符号改为无符号，对x轴上的点使用cos拟合AI中的弧形，位移数量为字体的长宽比（ratio）作用在于y轴，作为y轴方向的终止点
		*
		*  垂直方向变形：
		*	x 的位移，符号改为无符号，对y轴上的点使用cos拟合AI中的弧形，位移数量为字体的长宽比（ratio）作用在于y轴，作为x轴方向的终止点
		*	y 的位移，靠近锚点的权重为0.5，边缘为1.0，并使用 (1.0f - abs(pt.y)，限制边缘位移不能溢出边界，作为y轴的终止点
		*
		*  最后使用lerp函数对起始点和终止点，进行线性插值
		*
		* @return 一个lambda的函数，用于 Contour::invoke, ContourGrpup::invoke, PathEdge::invoke，PathGroup::invoke，对轮廓中的每一个点执行顶点变形
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> coveHigh(float x, float ratio, bool rotate) noexcept;

		/*
		* @brief 拱形
		* @param[in] x 水平方向的变化量， 范围在 -1.0 至 1.0
		* @param[in] ratio 字体的外接矩形长宽比, 示例 : w / h
		* @paran[in] rotate 变形方式， true：水平方向变形， false：垂直方向变形
		* @details lambda中输入的pt为归一化后的参数，并在锚点处建立坐标系
		*
		*  水平方向变形：
		*	x 的位移，靠近锚点的权重为0.5，边缘为1.0，并使用 (1.0f - abs(pt.x)，限制边缘位移不能溢出边界，作为x轴的终止点
		*	y 的位移，采取x轴上的点使用cos拟合AI中的弧形，位移数量为字体的长宽比（ratio）作用在于y轴，作为y轴方向的终止点
		*
		*  垂直方向变形：
		*	x 的位移，采取y轴上的点使用cos拟合AI中的弧形，位移数量为字体的长宽比（ratio）作用在于x轴，作为x轴方向的终止点
		*	y 的位移，靠近锚点的权重为0.5，边缘为1.0，并使用 (1.0f - abs(pt.y)，限制边缘位移不能溢出边界，作为y轴的终止点
		*
		*  最后使用lerp函数对起始点和终止点，进行线性插值, 其中 (1.0f - abs(pt.x)) * (1.0f - abs(x))，控制 x 的变化时与AI变化的匹配
		*
		* @return 一个lambda的函数，用于 Contour::invoke, ContourGrpup::invoke, PathEdge::invoke，PathGroup::invoke，对轮廓中的每一个点执行顶点变形
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> cove(float x, float ratio, bool rotate) noexcept;

		/*
		* @brief 突出
		* @param[in] x 水平方向的变化量， 范围在 -1.0 至 1.0
		* @param[in] ratio 字体的外接矩形长宽比, 示例 : w / h
		* @paran[in] rotate 变形方式， true：水平方向变形， false：垂直方向变形
		* @details lambda中输入的pt为归一化后的参数，并在锚点处建立坐标系
		*
		*  水平方向变形：
		*	x 的位移，靠近锚点的权重为0.5，边缘为1.0，并使用 (1.0f - abs(pt.x)，限制边缘位移不能溢出边界，作为x轴的终止点
		*	y 的位移，采取x轴上的点使用cos拟合AI中的弧形，扩张倍率为字体的长宽比（ratio）作用在于y轴，作为y轴方向的终止点
		*
		*  垂直方向变形：
		*	x 的位移，采取y轴上的点使用cos拟合AI中的弧形，扩张倍率为字体的长宽比（ratio）作用在于x轴，作为x轴方向的终止点
		*	y 的位移，靠近锚点的权重为0.5，边缘为1.0，并使用 (1.0f - abs(pt.y)，限制边缘位移不能溢出边界，作为y轴的终止点
		*
		*  最后使用lerp函数对起始点和终止点，进行线性插值, 其中乘算 abs(x)，用于保证x的变化与AI变化的一致性
		*
		* @return 一个lambda的函数，用于 Contour::invoke, ContourGrpup::invoke, PathEdge::invoke，PathGroup::invoke，对轮廓中的每一个点执行顶点变形
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> bulege(float x, float ratio, bool rotate) noexcept;

		/*
		* @brief 凹壳
		* @param[in] x 水平方向的变化量， 范围在 -1.0 至 1.0
		* @param[in] ratio 字体的外接矩形长宽比, 示例 : w / h
		* @paran[in] rotate 变形方式， true：水平方向变形， false：垂直方向变形
		* @details lambda中输入的pt为归一化后的参数，并在锚点处建立坐标系
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> bulegeLow(float x, float ratio, bool rotate) noexcept;

		/*
		* @brief 凸壳
		* @param[in] x 水平方向的变化量， 范围在 -1.0 至 1.0
		* @param[in] ratio 字体的外接矩形长宽比, 示例 : w / h
		* @paran[in] rotate 变形方式， true：水平方向变形， false：垂直方向变形
		* @details lambda中输入的pt为归一化后的参数，并在锚点处建立坐标系
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> bulegeHigh(float x, float ratio, bool rotate) noexcept;

		/* 
		* @brief 旗形
		* @param[in] x 水平方向的变化量，范围在 -1.0 至 1.0
		* @param[in] y 垂直方向的变化量，范围在 -1.0 至 1.0
		* @details 参数 pt 为归一化后的每一个轮廓点, 或贝塞尔的控制点，起始点为pt，终止点在 pt - sin(pt * Pi), 参数 x 和 y，通过lerp函数线性插值
		* @return 一个lambda的函数，用于 Contour::invoke, ContourGrpup::invoke, PathEdge::invoke，PathGroup::invoke，对轮廓中的每一个点执行顶点变形
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> flag(float x, float y) noexcept;

		/*
		* @brief 波形
		* @param[in] x 水平方向的变化量，范围在 -1.0 至 1.0
		* @param[in] y 垂直方向的变化量，范围在 -1.0 至 1.0
		* @details lambda中输入的pt为归一化后的参数，并在锚点处建立坐标系
		*
		*  水平方向变形：
		*	起始点为 pt.x
		*	终止点为 pt.x - sin(pt.y * Pi) * (1.0f - abs(pt.x));
		*
		*  垂直方向变形：
		*	起始点为 pt.y
		*	终止点为 pt.y - sin(pt.x * Pi) * (1.0f - abs(pt.y));
		*
		*  使用lerp函数对起始点和终止点，进行线性插值
		*
		* @return 一个lambda的函数，用于 Contour::invoke, ContourGrpup::invoke, PathEdge::invoke，PathGroup::invoke，对轮廓中的每一个点执行顶点变形
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> wave(float x, float y) noexcept;

		/*
		* @brief 鱼形
		* @param[in] x 水平方向的变化量，范围在 -1.0 至 1.0
		* @param[in] y 垂直方向的变化量，范围在 -1.0 至 1.0
		* @param[in] weight 变形的倍率，使AI保持一致，默认 1.2
		* @details lambda中输入的pt为归一化后的参数，并在锚点处建立坐标系
		*
		*  水平方向变形：
		*	起始点为 pt.x
		*	终止点为 pt.x + pt.x * sin(pt.y * Pi);
		*
		*  垂直方向变形：
		*	起始点为 pt.y
		*	终止点为 pt.y + pt.y * sin(pt.x * Pi);
		*
		*  使用lerp函数对起始点和终止点，进行线性插值
		*
		* @return 一个lambda的函数，用于 Contour::invoke, ContourGrpup::invoke, PathEdge::invoke，PathGroup::invoke，对轮廓中的每一个点执行顶点变形
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> fish(float x, float y, float weight = 1.2f) noexcept;

		/*
		* @brief 上升
		* @param[in] x 水平方向的变化量，范围在 -1.0 至 1.0
		* @param[in] y 垂直方向的变化量，范围在 -1.0 至 1.0
		* @details lambda中输入的pt为归一化后的参数，并在锚点处建立坐标系
		*
		*  水平方向变形：
		*	起始点为 pt.x
		*	终止点为 pt.x + pt.x * sin(pt.y * Pi);
		*
		*  垂直方向变形：
		*	起始点为 pt.y
		*	终止点为 pt.y + pt.y * sin(pt.x * Pi);
		*
		*  使用lerp函数对起始点和终止点，进行线性插值
		*
		* @return 一个lambda的函数，用于 Contour::invoke, ContourGrpup::invoke, PathEdge::invoke，PathGroup::invoke，对轮廓中的每一个点执行顶点变形
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> slope(float x, float y) noexcept;

		/*
		* @brief 鱼眼
		* @param[in] x 水平方向的变化量， 范围在 -1.0 至 1.0
		* @details lambda中输入的pt为归一化后的参数，并在锚点处建立坐标系
		*
		* Solid angle：http://www.rorydriscoll.com/2012/01/15/cubemap-texel-solid-angle/
		*
		* 细节：
		*	起始点为 pt
		*	终止点为 pt + pt * solid angle * (1.0f - pt^4), 其中 (1.0f - pt^4) 限制边缘溢出
		*
		*  使用lerp函数对起始点和终止点，进行线性插值
		*
		* @return 一个lambda的函数，用于 Contour::invoke, ContourGrpup::invoke, PathEdge::invoke，PathGroup::invoke，对轮廓中的每一个点执行顶点变形
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> fishEye(float x) noexcept;

		/*
		* @brief 膨胀
		* @param[in] x 水平方向的变化量， 范围在 -1.0 至 1.0
		* @param[in] ratio 字体的外接矩形长宽比, 示例 : w / h
		* @paran[in] rotate 变形方式， true：水平方向变形， false：垂直方向变形
		* @details lambda中输入的pt为归一化后的参数，并在锚点处建立坐标系
		*
		*  水平方向变形：
		*	起始点为 pt
		*	终止点为 pt + pt * (最大的长度 - 锚点到pt的长度) * (锚点中心权重为0.5，边缘权重为1.0的插值)
		*
		*  使用lerp函数对起始点和终止点，进行线性插值
		*
		* @return 一个lambda的函数，用于 Contour::invoke, ContourGrpup::invoke, PathEdge::invoke，PathGroup::invoke，对轮廓中的每一个点执行顶点变形
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> expandOut(float x, float ratio, bool rotate) noexcept;

		/*
		* @brief 挤压
		* @param[in] x 水平方向的变化量， 范围在 -1.0 至 1.0
		* @param[in] ratio 字体的外接矩形长宽比, 示例 : w / h
		* @paran[in] rotate 变形方式， true：水平方向变形， false：垂直方向变形
		* @details lambda中输入的pt为归一化后的参数，并在锚点处建立坐标系
		*
		*  变形：
		*	起始点为 pt.x
		*	终止点为 pt.x - pt.x * (最大的长度 - 锚点到pt的长度) * (锚点中心权重为0.5，边缘权重为1.0) * (变形方式, 水平为 -x，垂直为 x)
		*	终止点为 pt.y + pt.y * (最大的长度 - 锚点到pt的长度) * (锚点中心权重为0.5，边缘权重为1.0) * (变形方式, 水平为 -x，垂直为 x)
		*
		*  使用lerp函数对起始点和终止点，进行线性插值
		*
		* @return 一个lambda的函数，用于 Contour::invoke, ContourGrpup::invoke, PathEdge::invoke，PathGroup::invoke，对轮廓中的每一个点执行顶点变形
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> expandIn(float x, float ratio, bool rotate) noexcept;

		/*
		* @brief 扭转
		* @param[in] x 水平方向的变化量， 范围在 -1.0 至 1.0
		* @param[in] ratio 字体的外接矩形长宽比, 示例 : w / h
		* @paran[in] rotate 变形方式， true：水平方向变形， false：垂直方向变形
		* @details lambda中输入的pt为归一化后的参数，并在锚点处建立坐标系
		*
		*  变形： 锚点到pt上的距离作为旋转角度，对pt上的每个点旋转
		*
		* @return 一个lambda的函数，用于 Contour::invoke, ContourGrpup::invoke, PathEdge::invoke，PathGroup::invoke，对轮廓中的每一个点执行顶点变形
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> spin(float x, float ratio, bool rotate) noexcept;

		/*
		* @brief 扭转
		* @param[in] x 水平方向的变化量，范围在 -1.0 至 1.0
		* @param[in] y 垂直方向的变化量，范围在 -1.0 至 1.0
		* @details lambda中输入的pt为归一化后的参数，并在锚点处建立坐标系
		*
		*  变形： http://tksharpless.net/vedutismo/Pannini/panini.pdf
		*
		* @return 一个lambda的函数，用于 Contour::invoke, ContourGrpup::invoke, PathEdge::invoke，PathGroup::invoke，对轮廓中的每一个点执行顶点变形
		*/
		OCTOON_EXPORT std::function<math::float3(const math::float3&)> panini(float x, float y) noexcept;
	}
}

#endif