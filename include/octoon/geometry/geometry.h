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

		void setMeshSubset(std::size_t index) noexcept;
		std::size_t getMeshSubset() const noexcept;

		void setMesh(std::shared_ptr<mesh::Mesh>&& mesh) noexcept;
		void setMesh(const std::shared_ptr<mesh::Mesh>& mesh) noexcept;
		const std::shared_ptr<mesh::Mesh>& getMesh() const noexcept;

		void setMaterial(std::shared_ptr<material::Material>&& material) noexcept;
		void setMaterial(const std::shared_ptr<material::Material>& material) noexcept;
		const std::shared_ptr<material::Material>& getMaterial() const noexcept;

	private:
		bool isCastShadow_;
		bool isReceiveShadow_;

		std::size_t subset_;

		std::shared_ptr<mesh::Mesh> mesh_;
		std::shared_ptr<material::Material> material_;
	};
}

#endif