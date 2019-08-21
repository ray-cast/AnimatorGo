#ifndef MYSTICLIT_WINDOW_MODULE_H_
#define MYSTICLIT_WINDOW_MODULE_H_

#include <mysticLit_model.h>
#include <vector>
#include <octoon/math/vector3.h>

namespace MysticLit
{
	class CanvasModule : public MysticLitModule
	{
	public:
		CanvasModule() noexcept;
		virtual ~CanvasModule() noexcept;

		virtual void reset() noexcept override;

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