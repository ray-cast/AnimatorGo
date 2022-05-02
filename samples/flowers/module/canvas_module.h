#ifndef FLOWER_CANVAS_MODULE_H_
#define FLOWER_CANVAS_MODULE_H_

#include <vector>
#include <flower_model.h>
#include <octoon/math/vector3.h>

namespace flower
{
	class CanvasModule final : public FlowerModule
	{
	public:
		CanvasModule() noexcept;
		virtual ~CanvasModule() noexcept;

		virtual void reset() noexcept override;
		virtual void resize(std::uint32_t width, std::uint32_t height) noexcept;

		virtual void load(octoon::runtime::json& reader) noexcept override;
		virtual void save(octoon::runtime::json& reader) noexcept override;

	private:
		CanvasModule(const CanvasModule&) = delete;
		CanvasModule& operator=(const CanvasModule&) = delete;

	public:
		std::uint32_t width;
		std::uint32_t height;

		std::vector<octoon::math::float3> colorBuffer;
		std::vector<octoon::math::float3> normalBuffer;
		std::vector<octoon::math::float3> albedoBuffer;
		std::vector<octoon::math::float3> outputBuffer;
	};
}

#endif