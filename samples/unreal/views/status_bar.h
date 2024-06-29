#ifndef UNREAL_STATUS_BAR_H_
#define UNREAL_STATUS_BAR_H_

#include <qstatusbar.h>

#include "unreal_profile.h"
#include "unreal_behaviour.h"

namespace unreal
{
	class StatusBar final : public QStatusBar
	{
		Q_OBJECT
	public:
		StatusBar(const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile);
		~StatusBar();

		void showEvent(QShowEvent* event) noexcept;

		void updateEvent() noexcept;

	private:
		octoon::GameObjectPtr behaviour_;
		std::shared_ptr<UnrealProfile> profile_;
	};
}

#endif