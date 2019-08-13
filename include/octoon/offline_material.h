#ifndef OCTOON_OFFLINE_MATERIAL_H_
#define OCTOON_OFFLINE_MATERIAL_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT OfflineMaterial final
	{
	public:
		OfflineMaterial() noexcept;
		~OfflineMaterial() noexcept;

	private:
		OfflineMaterial(const OfflineMaterial&) = delete;
		OfflineMaterial& operator=(const OfflineMaterial&) = delete;
	};
}

#endif