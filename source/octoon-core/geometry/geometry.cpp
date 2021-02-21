#include <octoon/geometry/geometry.h>

namespace octoon
{
	OctoonImplementSubClass(Geometry, video::RenderObject, "Geometry")

	Geometry::Geometry() noexcept
		: isCastShadow_(true)
		, isReceiveShadow_(true)
		, isGlobalIllumination_(false)
	{
	}

	Geometry::~Geometry() noexcept
	{
	}

	void
	Geometry::setReceiveShadow(bool enable) noexcept
	{
		this->setDirty(true);
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
		this->setDirty(true);
		isCastShadow_ = value;
	}

	bool
	Geometry::getCastShadow()  const noexcept
	{
		return isCastShadow_;
	}

	void
	Geometry::setGlobalIllumination(bool enable) noexcept
	{
		this->setDirty(true);
		this->isGlobalIllumination_ = enable;
	}
	
	bool
	Geometry::getGlobalIllumination() const noexcept
	{
		return this->isGlobalIllumination_;
	}

	void
	Geometry::setMesh(std::shared_ptr<Mesh>&& mesh) noexcept
	{
		this->setDirty(true);
		this->mesh_ = std::move(mesh);
		this->setBoundingBox(mesh ? mesh->getBoundingBoxAll() : math::BoundingBox::Empty);
	}

	void
	Geometry::setMesh(const std::shared_ptr<Mesh>& mesh) noexcept
	{
		this->setDirty(true);
		this->mesh_ = mesh;
		this->setBoundingBox(mesh ? mesh->getBoundingBoxAll() : math::BoundingBox::Empty);
	}

	const std::shared_ptr<Mesh>&
	Geometry::getMesh() const noexcept
	{
		return this->mesh_;
	}

	void
	Geometry::setMaterial(std::shared_ptr<Material>&& material) noexcept
	{
		if (!this->materials_.empty())
		{
			if (this->materials_.front() != material)
				material->setDirty(true);
		}

		this->setDirty(true);
		this->materials_.clear();
		this->materials_.push_back(std::move(material));
	}
	
	void
	Geometry::setMaterial(const std::shared_ptr<Material>& material) noexcept
	{
		if (!this->materials_.empty())
		{
			if (this->materials_.front() != material)
				material->setDirty(true);
		}

		this->setDirty(true);
		this->materials_.clear();
		this->materials_.push_back(material);
	}
	
	const std::shared_ptr<Material>&
	Geometry::getMaterial(std::size_t n) const noexcept
	{
		assert(this->materials_.size() > n);
		return this->materials_[n];
	}

	void
	Geometry::setMaterials(std::vector<std::shared_ptr<Material>>&& materials) noexcept
	{
		auto size = std::min(materials.size(), materials_.size());
		for (std::size_t i = 0; i < size; i++)
		{
			if (this->materials_[i] != materials[i])
				materials[i]->setDirty(true);
		}
		for (std::size_t i = size; i < materials.size(); i++)
			materials[i]->setDirty(true);
		this->setDirty(true);
		materials_ = std::move(materials);
	}

	void
	Geometry::setMaterials(const std::vector<std::shared_ptr<Material>>& materials) noexcept
	{
		auto size = std::min(materials.size(), materials_.size());
		for (std::size_t i = 0; i < size; i++)
		{
			if (this->materials_[i] != materials[i])
				materials[i]->setDirty(true);
		}
		for (std::size_t i = size; i < materials.size(); i++)
			materials[i]->setDirty(true);
		this->setDirty(true);
		materials_ = materials;
	}

	const std::vector<std::shared_ptr<Material>>&
	Geometry::getMaterials() const noexcept
	{
		return materials_;
	}
}