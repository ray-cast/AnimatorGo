#include <octoon/geometry/geometry.h>

namespace octoon::geometry
{
	OctoonImplementSubClass(Geometry, video::RenderObject, "Geometry")

	Geometry::Geometry() noexcept
		: isCastShadow_(true)
		, isReceiveShadow_(true)
		, subset_(0)
	{
	}

	Geometry::~Geometry() noexcept
	{
		this->setActive(false);
	}

	void
	Geometry::setReceiveShadow(bool enable) noexcept
	{
		isReceiveShadow_ = enable;
	}

	bool
	Geometry::getReceiveShadow() const noexcept
	{
		return isReceiveShadow_;
	}

	void
	Geometry::setCastShadow(bool value) noexcept
	{
		isCastShadow_ = value;
	}

	bool
	Geometry::getCastShadow()  const noexcept
	{
		return isCastShadow_;
	}

	void
	Geometry::setMeshSubset(std::size_t index) noexcept
	{
		this->subset_ = index;
	}

	std::size_t
	Geometry::getMeshSubset() const noexcept
	{
		return this->subset_;
	}

	void
	Geometry::setMesh(std::shared_ptr<mesh::Mesh>&& mesh) noexcept
	{
		this->mesh_ = std::move(mesh);
	}

	void
	Geometry::setMesh(const std::shared_ptr<mesh::Mesh>& mesh) noexcept
	{
		this->mesh_ = mesh;
	}

	const std::shared_ptr<mesh::Mesh>&
	Geometry::getMesh() const noexcept
	{
		return this->mesh_;
	}

	void
	Geometry::setMaterial(std::shared_ptr<material::Material>&& material) noexcept
	{
		material_ = std::move(material);
	}

	void
	Geometry::setMaterial(const std::shared_ptr<material::Material>& material) noexcept
	{
		material_ = material;
	}

	const std::shared_ptr<material::Material>&
	Geometry::getMaterial() const noexcept
	{
		return material_;
	}
}