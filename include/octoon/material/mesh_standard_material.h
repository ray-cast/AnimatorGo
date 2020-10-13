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
		MeshStandardMaterial(std::string_view name) noexcept;
		MeshStandardMaterial(const math::float3& color) noexcept;
		virtual ~MeshStandardMaterial() noexcept;

		void setColor(const math::float3& color) noexcept;
		void setEmissive(const math::float3& color) noexcept;
		void setEmissiveIntensity(float intensity) noexcept;
		void setOpacity(float opacity) noexcept;
		void setSmoothness(float smoothness) noexcept;
		void setRoughness(float roughness) noexcept;
		void setMetalness(float metalness) noexcept;
		void setAnisotropy(float anisotropy) noexcept;
		void setSheen(float sheen) noexcept;
		void setSpecular(float reflectivity) noexcept;
		void setRefractionRatio(float refractionRatio) noexcept;
		void setClearCoat(float clearCoat) noexcept;
		void setClearCoatRoughness(float clearCoatRoughness) noexcept;
		void setSubsurface(float subsurface) noexcept;
		void setSubsurfaceColor(const math::float3& subsurfaceColor) noexcept;
		void setIor(float ior) noexcept;
		void setTransmission(float transmission) noexcept;
		void setLightMapIntensity(float intensity) noexcept;
		void setColorMap(const hal::GraphicsTexturePtr& map) noexcept;
		void setOpacityMap(const hal::GraphicsTexturePtr& map) noexcept;
		void setNormalMap(const hal::GraphicsTexturePtr& map) noexcept;
		void setSpecularMap(const hal::GraphicsTexturePtr& map) noexcept;
		void setRoughnessMap(const hal::GraphicsTexturePtr& map) noexcept;
		void setMetalnessMap(const hal::GraphicsTexturePtr& map) noexcept;
		void setAnisotropyMap(const hal::GraphicsTexturePtr& map) noexcept;
		void setSheenMap(const hal::GraphicsTexturePtr& map) noexcept;
		void setClearCoatMap(const hal::GraphicsTexturePtr& map) noexcept;
		void setClearCoatRoughnessMap(const hal::GraphicsTexturePtr& map) noexcept;
		void setEmissiveMap(const hal::GraphicsTexturePtr& map) noexcept;
		void setSubsurfaceMap(const hal::GraphicsTexturePtr& map) noexcept;
		void setSubsurfaceColorMap(const hal::GraphicsTexturePtr& map) noexcept;
		void setLightTexture(const hal::GraphicsTexturePtr& map) noexcept;
		void setOffset(const math::float2& offset) noexcept;
		void setRepeat(const math::float2& repeat) noexcept;
		void setNormalScale(const math::float2& repeat) noexcept;
		void setGamma(float gamma) noexcept;

		const math::float3& getColor() const noexcept;
		const math::float3& getSubsurfaceColor() const noexcept;
		const math::float3& getEmissive() const noexcept;

		float getEmissiveIntensity() const noexcept;
		float getOpacity() const noexcept;
		float getSmoothness() const noexcept;
		float getRoughness() const noexcept;
		float getMetalness() const noexcept;
		float getAnisotropy() const noexcept;
		float getSheen() const noexcept;
		float getSpecular() const noexcept;
		float getRefractionRatio() const noexcept;
		float getClearCoat() const noexcept;
		float getClearCoatRoughness() const noexcept;
		float getSubsurface() const noexcept;
		float getIor() const noexcept;
		float getTransmission() const noexcept;
		float getLightMapIntensity() const noexcept;
		float getGamma() const noexcept;

		const hal::GraphicsTexturePtr& getColorMap() const noexcept;
		const hal::GraphicsTexturePtr& getOpacityMap() const noexcept;
		const hal::GraphicsTexturePtr& getNormalMap() const noexcept;
		const hal::GraphicsTexturePtr& getSpecularMap() const noexcept;
		const hal::GraphicsTexturePtr& getRoughnessMap() const noexcept;
		const hal::GraphicsTexturePtr& getMetalnessMap() const noexcept;
		const hal::GraphicsTexturePtr& getAnisotropyMap() const noexcept;
		const hal::GraphicsTexturePtr& getSheenMap() const noexcept;
		const hal::GraphicsTexturePtr& getClearCoatMap() const noexcept;
		const hal::GraphicsTexturePtr& getClearCoatRoughnessMap() const noexcept;		
		const hal::GraphicsTexturePtr& getSubsurfaceMap() const noexcept;
		const hal::GraphicsTexturePtr& getSubsurfaceColorMap() const noexcept;
		const hal::GraphicsTexturePtr& getEmissiveMap() const noexcept;
		const hal::GraphicsTexturePtr& getLightTexture() const noexcept;

		const math::float2& getOffset() const noexcept;
		const math::float2& getRepeat() const noexcept;
		const math::float2& getNormalScale() const noexcept;

		void setReceiveShadow(bool enable) noexcept;
		bool getReceiveShadow() const noexcept;

		std::shared_ptr<Material> clone() const noexcept override;

	private:
		MeshStandardMaterial(const MeshStandardMaterial&) = delete;
		MeshStandardMaterial& operator=(const MeshStandardMaterial&) = delete;

	private:
		bool receiveShadow_;
		float gamma_;
		float opacity_;
		float metalness_;
		float roughness_;
		float anisotropy_;
		float sheen_;
		float specular_;
		float refractionRatio_;
		float clearCoat_;
		float clearCoatRoughness_;
		float subsurface_;
		float ior_;
		float transmission_;
		float lightMapIntensity_;
		float emissiveIntensity_;
		math::float2 offset_;
		math::float2 repeat_;
		math::float2 normalScale_;
		math::float3 color_;
		math::float3 emissive_;
		math::float3 subsurfaceColor_;
		hal::GraphicsTexturePtr colorMap_;
		hal::GraphicsTexturePtr alphaMap_;
		hal::GraphicsTexturePtr normalMap_;
		hal::GraphicsTexturePtr specularMap_;
		hal::GraphicsTexturePtr roughnessMap_;
		hal::GraphicsTexturePtr metalnessMap_;
		hal::GraphicsTexturePtr anisotropyMap_;
		hal::GraphicsTexturePtr sheenMap_;
		hal::GraphicsTexturePtr clearcoatMap_;
		hal::GraphicsTexturePtr clearcoatRoughnessMap_;
		hal::GraphicsTexturePtr lightMap_;
		hal::GraphicsTexturePtr subsurfaceMap_;
		hal::GraphicsTexturePtr subsurfaceColorMap_;
		hal::GraphicsTexturePtr emissiveMap_;
	};
}

#endif