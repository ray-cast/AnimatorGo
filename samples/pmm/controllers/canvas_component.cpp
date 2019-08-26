#include "canvas_component.h"
#include "mysticlit_behaviour.h"
#include <octoon/offline_feature.h>
#include <octoon/image/image.h>

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

	void
	CanvasComponent::save(const std::string& filepath) noexcept
	{
		auto canvas = this->getContext()->profile->canvasModule;
		auto width = canvas->width;
		auto height = canvas->height;
		auto output = canvas->outputBuffer.data();

		octoon::image::Image image;

		if (image.create(octoon::image::Format::R8G8B8SRGB, width, height))
		{
			auto data = (char*)image.data();

#			pragma omp parallel for num_threads(4)
			for (std::uint32_t y = 0; y < height; y++)
			{
				for (std::uint32_t x = 0; x < width; x++)
				{
					auto src = y * width + x;
					auto dst = ((height - y - 1) * width + x) * 3;

					data[dst] = (std::uint8_t)(output[src].x * 255);
					data[dst + 1] = (std::uint8_t)(output[src].y * 255);
					data[dst + 2] = (std::uint8_t)(output[src].z * 255);
				}
			}

			image.save(filepath, "png");
		}		

	}
}