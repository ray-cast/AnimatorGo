#ifndef OCTOON_ALGORITHM_H_
#define OCTOON_ALGORITHM_H_

#include <vector>
#include <algorithm>

namespace octoon
{
	namespace runtime
	{
		template<class _Tx, class _Ty>
		inline typename std::vector<_Tx>::const_iterator find(const std::vector<_Tx>& array, const _Ty& _Val)
		{
			return std::find(array.begin(), array.end(), _Val);
		}

		template<class _Tx, class _Ty>
		inline typename std::vector<_Tx>::const_iterator find_if(const std::vector<_Tx>& array, const _Ty& _Pred)
		{
			return std::find_if(array.begin(), array.end(), _Pred);
		}
	}
}

#endif