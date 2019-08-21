#include "canvas_component.h"
#include "mysticlit_behaviour.h"
#include <octoon/offline_feature.h>

namespace MysticLit
{
	CanvasComponent::CanvasComponent() noexcept
		: active_(false)
	{
	}

	CanvasComponent::~CanvasComponent() noexcept
	{
	}

	void
	CanvasComponent::setActive(bool active) noexcept
	{
		active_ = active;
	}

	bool
	CanvasComponent::getActive() const noexcept
	{
		return active_;
	}

	void
	CanvasComponent::onEnable() noexcept
	{
	}

	void
	CanvasComponent::onDisable() noexcept
	{
	}

	void
	CanvasComponent::onPostProcess() noexcept
	{
		auto& context = this->getContext();
		auto offlineFeature = context->behaviour->getFeature<octoon::OfflineFeature>();
		offlineFeature->readColorFramebuffer(this->getModel()->colorBuffer.data());
		offlineFeature->readAlbedoFramebuffer(this->getModel()->albedoBuffer.data());
		offlineFeature->readNormalFramebuffer(this->getModel()->normalBuffer.data());
	}
}