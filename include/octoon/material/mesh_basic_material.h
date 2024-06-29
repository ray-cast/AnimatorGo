#ifndef OCTOON_MESH_BASIC_MATERIAL_H_
#define OCTOON_MESH_BASIC_MATERIAL_H_

#include <octoon/material/material.h>

namespace octoon
{
	class OCTOON_EXPORT MeshBasicMaterial : public Material
	{
		OctoonDeclareSubClass(MeshBasicMaterial, Material);
	public:
		MeshBasicMaterial() noexcept;
		MeshBasicMaterial(const math::float3& color) noexcept;
		virtual ~MeshBasicMaterial() noexcept;

		void setColor(const math::float3& color) noexcept;
		const math::float3& getColor() const noexcept;

		void setOffset(const math::float2& offset) noexcept;
		const math::float2& getOffset() const noexcept;

		void setRepeat(const math::float2& repeat) noexcept;
		const math::float2& getRepeat() const noexcept;

		void setOpacity(float opacity) noexcept;
		float getOpacity() const noexcept;

		void setColorMap(const std::shared_ptr<Texture>& map) noexcept;
		const std::shared_ptr<Texture>& getColorMap() const noexcept;

		void setGamma(float gamma) noexcept;
		float getGamma() const noexcept;

		std::shared_ptr<Material> clone() const noexcept override;

	private:
		MeshBasicMaterial(const MeshBasicMaterial&) = delete;
		MeshBasicMaterial& operator=(const MeshBasicMaterial&) = delete;

	private:
		float gamma_;
		float opacity_;
		math::float3 color_;
		math::float2 offset_;
		math::float2 repeat_;
		std::shared_ptr<Texture> edgeTexture_;
	};
}

#endif