#ifndef OCTOON_GEOMETRY_H_
#define OCTOON_GEOMETRY_H_

#include <octoon/mesh/mesh.h>
#include <octoon/material/material.h>
#include <octoon/video/render_object.h>

namespace octoon::geometry
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

		void setMesh(std::shared_ptr<mesh::Mesh>&& mesh) noexcept;
		void setMesh(const std::shared_ptr<mesh::Mesh>& mesh) noexcept;
		const std::shared_ptr<mesh::Mesh>& getMesh() const noexcept;

		void setMaterial(std::shared_ptr<material::Material>&& material) noexcept;
		void setMaterial(const std::shared_ptr<material::Material>& material) noexcept;
		const std::shared_ptr<material::Material>& getMaterial(std::size_t n = 0) const noexcept;

		void setMaterials(std::vector<std::shared_ptr<material::Material>>&& materials) noexcept;
		void setMaterials(const std::vector<std::shared_ptr<material::Material>>& materials) noexcept;
		const std::vector<std::shared_ptr<material::Material>>& getMaterials() const noexcept;

	private:
		bool isCastShadow_;
		bool isReceiveShadow_;
		bool isGlobalIllumination_;

		std::shared_ptr<mesh::Mesh> mesh_;
		std::vector<std::shared_ptr<material::Material>> materials_;
	};
}

#endif