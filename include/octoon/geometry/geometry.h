#ifndef OCTOON_GEOMETRY_H_
#define OCTOON_GEOMETRY_H_

#include <octoon/mesh/mesh.h>
#include <octoon/material/material.h>
#include <octoon/video/render_object.h>

namespace octoon
{
	class OCTOON_EXPORT Geometry final : public video::RenderObject
	{
		OctoonDeclareSubClass(Geometry, RenderObject)
	public:
		Geometry() noexcept;
		~Geometry() noexcept;

		void setCastShadow(bool enable) noexcept;
		bool getCastShadow() const noexcept;

		void setReceiveShadow(bool enable) noexcept;
		bool getReceiveShadow() const noexcept;

		void setGlobalIllumination(bool enable) noexcept;
		bool getGlobalIllumination() const noexcept;

		void setMesh(std::shared_ptr<Mesh>&& mesh) noexcept;
		void setMesh(const std::shared_ptr<Mesh>& mesh) noexcept;
		const std::shared_ptr<Mesh>& getMesh() const noexcept;

		void setMaterial(std::shared_ptr<Material>&& material) noexcept;
		void setMaterial(const std::shared_ptr<Material>& material) noexcept;
		const std::shared_ptr<Material>& getMaterial(std::size_t n = 0) const noexcept;

		void setMaterials(std::vector<std::shared_ptr<Material>>&& materials) noexcept;
		void setMaterials(const std::vector<std::shared_ptr<Material>>& materials) noexcept;
		const std::vector<std::shared_ptr<Material>>& getMaterials() const noexcept;

	private:
		bool isCastShadow_;
		bool isReceiveShadow_;
		bool isGlobalIllumination_;

		std::shared_ptr<Mesh> mesh_;
		std::vector<std::shared_ptr<Material>> materials_;
	};
}

#endif