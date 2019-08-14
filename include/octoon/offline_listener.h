#ifndef OCTOON_OFFLINE_LISTENER_H_
#define OCTOON_OFFLINE_LISTENER_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT OfflineListener
	{
	public:
		OfflineListener() noexcept;
		~OfflineListener() noexcept;

		virtual void onPreRender() noexcept = 0;
		virtual void onPostRender() noexcept = 0;

	private:
		OfflineListener(const OfflineListener&) = delete;
		OfflineListener& operator=(const OfflineListener&) = delete;
	};
}

#endif