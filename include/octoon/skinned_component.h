#ifndef OCTOON_SKINNED_COMPONENT_H_
#define OCTOON_SKINNED_COMPONENT_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT SkinnedComponent : public GameComponent
	{
		OctoonDeclareSubInterface(SkinnedComponent, GameComponent)
	public:
		SkinnedComponent() noexcept;
		virtual ~SkinnedComponent() noexcept;

		virtual void setName(const std::string& name) noexcept override;
		virtual void setName(std::string&& name) noexcept override;
		virtual const std::string& getName() const noexcept override;

		virtual void setControl(float control) noexcept;
		virtual float getControl() const noexcept;

	private:
		virtual void onTargetReplace(const std::string& name) noexcept;

	private:
		SkinnedComponent(const SkinnedComponent&) = delete;
		SkinnedComponent& operator=(const SkinnedComponent&) = delete;

	private:
		float control_;
	};
}

#endif