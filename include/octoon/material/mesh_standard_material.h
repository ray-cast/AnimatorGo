#ifndef OCTOON_MESH_STANDARD_MATERIAL_H_
#define OCTOON_MESH_STANDARD_MATERIAL_H_

#include <octoon/material/material.h>

namespace octoon::material
{
	class OCTOON_EXPORT MeshStandardMaterial final : public Material
	{
		OctoonDeclareSubClass(MeshStandardMaterial, Material);
	public:
		MeshStandardMaterial() noexcept;
		MeshStandardMaterial(const math::float3& color) noexcept;
		virtual ~MeshStandardMaterial() noexcept;

		void setColor(const math::float3& color) noexcept;
		const math::float3& getColor() const noexcept;

		void setColorTexture(const hal::GraphicsTexturePtr& map) noexcept;
		const hal::GraphicsTexturePtr& getColorTexture() const noexcept;

		void setEmissive(const math::float3& color) noexcept;
		const math::float3& getEmissive() const noexcept;

		void setOpacity(float opacity) noexcept;
		float getOpacity() const noexcept;

		void setSmoothness(float smoothness) noexcept;
		float getSmoothness() const noexcept;

		void setMetalness(float metalness) noexcept;
		float getMetalness() const noexcept;

		void setAnisotropy(float anisotropy) noexcept;
		float getAnisotropy() const noexcept;

		void setSheen(float sheen) noexcept;
		float getSheen() const noexcept;

		void setReflectivity(float reflectivity) noexcept;
		float getReflectivity() const noexcept;

		void setRefractionRatio(float refractionRatio) noexcept;
		float getRefractionRatio() const noexcept;		

		void setClearCoat(float clearCoat) noexcept;
		float getClearCoat() const noexcept;

		void setClearCoatRoughness(float clearCoatRoughness) noexcept;
		float getClearCoatRoughness() const noexcept;

		void setSubsurface(float subsurface) noexcept;
		float getSubsurface() const noexcept;

		void setLightMapIntensity(float intensity) noexcept;
		float getLightMapIntensity() const noexcept;

		void setLightTexture(const hal::GraphicsTexturePtr& map) noexcept;
		const hal::GraphicsTexturePtr& getLightTexture() const noexcept;

		void setOffset(const math::float2& offset) noexcept;
		const math::float2& getOffset() const noexcept;

		void setRepeat(const math::float2& repeat) noexcept;
		const math::float2& getRepeat() const noexcept;

		void setGamma(float gamma) noexcept;
		float getGamma() const noexcept;

		std::shared_ptr<Material> clone() const noexcept override;

	private:
		MeshStandardMaterial(const MeshStandardMaterial&) = delete;
		MeshStandardMaterial& operator=(const MeshStandardMaterial&) = delete;

	private:
		float gamma_;
		float opacity_;
		float metalness_;
		float smoothness_;
		float anisotropy_;
		float sheen_;
		float reflectivity_;
		float refractionRatio_;
		float clearCoat_;
		float clearCoatRoughness_;
		float subsurface_;
		float lightMapIntensity_;
		math::float2 offset_;
		math::float2 repeat_;
		math::float3 color_;
		math::float3 emissive_;
		hal::GraphicsTexturePtr colorMap_;
		hal::GraphicsTexturePtr lightMap_;
	};
}

#endif