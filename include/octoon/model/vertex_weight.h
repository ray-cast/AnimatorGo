#ifndef OCTOON_SKELECTON_VERTEX_WEIGHT_H_
#define OCTOON_SKELECTON_VERTEX_WEIGHT_H_

#include <vector>

namespace octoon::skelecton
{
	namespace detail
	{
		template<typename _Bone, typename _Float>
		class VertexWeight final
		{
		public:
			union
			{
				_Float weights[4];

				struct
				{
					_Float weight1;
					_Float weight2;
					_Float weight3;
					_Float weight4;
				};
			};

			union
			{
				_Bone bones[4];

				struct
				{
					_Bone bone1;
					_Bone bone2;
					_Bone bone3;
					_Bone bone4;
				};
			};
		};
	}

	using VertexWeight = detail::VertexWeight<std::uint16_t, float>;
	using VertexWeights = std::vector<VertexWeight>;
}

#endif