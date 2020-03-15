#include "mark_component.h"
#include "../libs/nativefiledialog/nfd.h"
#include "../rabbit_profile.h"

#include <octoon/offline_camera_component.h>
#include <octoon/perspective_camera_component.h>
#include <octoon/offline_skinned_mesh_renderer_component.h>

namespace rabbit
{
	MarkComponent::MarkComponent() noexcept
	{
	}

	MarkComponent::~MarkComponent() noexcept
	{
	}

	void
	MarkComponent::setActive(bool active) noexcept
	{
		if (this->getModel()->markEnable != active)
			this->getModel()->markEnable = active;
	}

	bool
	MarkComponent::getActive() const noexcept
	{
		return this->getModel()->markEnable;
	}

	void
	MarkComponent::onPostProcess() noexcept
	{
		auto& markModule = this->getContext()->profile->markModule;
		if (this->getContext()->profile->markModule->markEnable)
		{
			auto& window = this->getContext()->profile->canvasModule;

			auto width = std::min(markModule->width, window->width);
			auto height = std::min(markModule->height, window->height);
			auto data = window->outputBuffer.data();

			for (std::uint32_t y = 0; y < height; y++)
			{
				for (std::uint32_t x = 0; x < width; x++)
				{
					auto src = ((markModule->height - y - 1) * markModule->width + x) * markModule->channel;
					auto& dst = data[(y + markModule->y) * window->width + x + markModule->x];

					auto b = markModule->pixels[src] / 255.0f;
					auto g = markModule->pixels[src + 1] / 255.0f;
					auto r = markModule->pixels[src + 2] / 255.0f;
					
					float alpha = 1.0f;
					if (markModule->channel == 4)
						alpha = markModule->pixels[src + 3] / 255.0f;
					
					dst = octoon::math::lerp(dst, octoon::math::float3(r, g, b), alpha);
				}
			}
		}
	}
}