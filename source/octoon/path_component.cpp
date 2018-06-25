#include <octoon/path_component.h>

namespace octoon
{
	OctoonImplementSubClass(PathComponent, GameComponent, "Path")

	PathComponent::PathComponent() noexcept
		: isSharedPath_(false)
	{
	}

	PathComponent::PathComponent(model::Path&& mesh, bool sharedPath) noexcept
	{
		this->setPath(std::make_unique<model::Path>(std::move(mesh)), sharedPath);
	}

	PathComponent::PathComponent(const model::Path& mesh, bool sharedPath) noexcept
	{
		this->setPath(std::make_shared<model::Path>(mesh), sharedPath);
	}

	PathComponent::PathComponent(model::PathPtr&& mesh, bool sharedPath) noexcept
	{
		this->setPath(std::move(mesh), sharedPath);
	}

	PathComponent::PathComponent(const model::PathPtr& mesh, bool sharedPath) noexcept
	{
		this->setPath(mesh, sharedPath);
	}

	PathComponent::~PathComponent() noexcept
	{
	}

	void
	PathComponent::setPath(model::Path&& mesh, bool sharedPath) noexcept
	{
		this->setPath(std::make_shared<model::Path>(std::move(mesh)));
	}

	void
	PathComponent::setPath(model::PathPtr&& mesh, bool sharedPath) noexcept
	{
		if (path_ != mesh)
		{
			path_ = std::move(mesh);
			isSharedPath_ = sharedPath;
			this->onPathReplace(path_);
		}
	}

	void
	PathComponent::setPath(const model::PathPtr& mesh, bool sharedPath) noexcept
	{
		if (path_ != mesh)
		{
			path_ = mesh;
			isSharedPath_ = sharedPath;
			this->onPathReplace(path_);
		}
	}

	const model::PathPtr&
	PathComponent::getPath() const noexcept
	{
		return path_;
	}

	bool
	PathComponent::isSharedPath() const noexcept
	{
		return isSharedPath_;
	}

	void
	PathComponent::uploadPathData() noexcept
	{
		for (auto& it : delegates_)
			(*it)(path_);
	}

	void
	PathComponent::addPathListener(OnPathReplaceEvent* func) noexcept
	{
		delegates_.push_back(func);
	}

	void
	PathComponent::removePathListener(const OnPathReplaceEvent* func) noexcept
	{
		auto it = std::find(delegates_.begin(), delegates_.end(), func);
		if (it != delegates_.end())
			delegates_.erase(it);
	}

	GameComponentPtr
	PathComponent::clone() const noexcept
	{
		auto instance = std::make_shared<PathComponent>();
		instance->setName(instance->getName());
		instance->setPath(path_ ? (isSharedPath_ ? path_ : path_->clone()) : nullptr, isSharedPath_);

		return instance;
	}

	void
	PathComponent::onPathReplace(const model::PathPtr& mesh) noexcept
	{
		for (auto& it : delegates_)
			(*it)(mesh);
	}
}