#include <octoon/contour_component.h>

namespace octoon
{
	OctoonImplementSubClass(ContourComponent, GameComponent, "Contour")

	ContourComponent::ContourComponent() noexcept
		: isSharedContour_(false)
	{
	}

	ContourComponent::ContourComponent(model::Contour&& mesh, bool sharedContour) noexcept
	{
		this->setContour(std::make_unique<model::Contour>(std::move(mesh)), sharedContour);
	}

	ContourComponent::ContourComponent(const model::Contour& mesh, bool sharedContour) noexcept
	{
		this->setContour(std::make_shared<model::Contour>(mesh), sharedContour);
	}

	ContourComponent::ContourComponent(model::ContourPtr&& mesh, bool sharedContour) noexcept
	{
		this->setContour(std::move(mesh), sharedContour);
	}

	ContourComponent::ContourComponent(const model::ContourPtr& mesh, bool sharedContour) noexcept
	{
		this->setContour(mesh, sharedContour);
	}

	ContourComponent::~ContourComponent() noexcept
	{
	}

	void
	ContourComponent::setContour(model::Contour&& mesh, bool sharedContour) noexcept
	{
		this->setContour(std::make_shared<model::Contour>(std::move(mesh)));
	}

	void
	ContourComponent::setContour(model::ContourPtr&& mesh, bool sharedContour) noexcept
	{
		if (contour_ != mesh)
		{
			contour_ = std::move(mesh);
			isSharedContour_ = sharedContour;
			this->onContourReplace(contour_);
		}
	}

	void
	ContourComponent::setContour(const model::ContourPtr& mesh, bool sharedContour) noexcept
	{
		if (contour_ != mesh)
		{
			contour_ = mesh;
			isSharedContour_ = sharedContour;
			this->onContourReplace(contour_);
		}
	}

	const model::ContourPtr&
	ContourComponent::getContour() const noexcept
	{
		return contour_;
	}

	bool
	ContourComponent::isSharedContour() const noexcept
	{
		return isSharedContour_;
	}

	void
	ContourComponent::uploadContourData() noexcept
	{
		for (auto& it : delegates_)
			(*it)(contour_);
	}

	void
	ContourComponent::addContourListener(OnContourReplaceEvent* func) noexcept
	{
		delegates_.push_back(func);
	}

	void
	ContourComponent::removeContourListener(const OnContourReplaceEvent* func) noexcept
	{
		auto it = std::find(delegates_.begin(), delegates_.end(), func);
		if (it != delegates_.end())
			delegates_.erase(it);
	}

	GameComponentPtr
	ContourComponent::clone() const noexcept
	{
		auto instance = std::make_shared<ContourComponent>();
		instance->setName(instance->getName());
		instance->setContour(contour_ ? (isSharedContour_ ? contour_ : contour_->clone()) : nullptr, isSharedContour_);

		return instance;
	}

	void
	ContourComponent::onContourReplace(const model::ContourPtr& mesh) noexcept
	{
		for (auto& it : delegates_)
			(*it)(mesh);
	}
}