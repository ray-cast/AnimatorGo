#ifndef OCTOON_ALGORITHM_H_
#define OCTOON_ALGORITHM_H_

#include <vector>
#include <algorithm>

namespace octoon
{
	namespace runtime
	{
		template<class _Tx, class _Ty>
		inline typename std::vector<_Tx>::const_iterator find(const std::vector<_Tx>& _Tx, _Ty _Val)
		{
			return std::find(_Tx.begin(), _Tx.end(), _Val);
		}

		template<class _Tx, class _Ty>
		inline typename std::vector<_Tx>::const_iterator find_if(const std::vector<_Tx>& _Tx, _Ty _Pred)
		{
			return std::find_if(_Tx.begin(), _Tx.end(), _Pred);
		}
	}
}

#endif