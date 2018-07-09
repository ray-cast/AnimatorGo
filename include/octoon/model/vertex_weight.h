#ifndef OCTOON_MODEL_VERTEX_WEIGHT_H_
#define OCTOON_MODEL_VERTEX_WEIGHT_H_

#include <octoon/model/modtypes.h>

namespace octoon
{
	namespace model
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
	}
}

#endif