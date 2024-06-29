#include <octoon/path_component.h>
#include <octoon/model/text_meshing.h>

namespace octoon
{
	OctoonImplementSubClass(PathComponent, GameComponent, "Path")

	PathComponent::PathComponent() noexcept
		: isSharedPath_(false)
	{
	}

	PathComponent::PathComponent(Paths&& mesh, bool sharedPath) noexcept
	{
		this->setPath(mesh, sharedPath);
	}

	PathComponent::PathComponent(const Paths& mesh, bool sharedPath) noexcept
	{
		this->setPath(mesh, sharedPath);
	}

	PathComponent::~PathComponent() noexcept
	{
	}

	void
	PathComponent::setPath(Paths&& path, bool sharedPath) noexcept
	{
		paths_ = std::move(path);
		isSharedPath_ = sharedPath;
		this->uploadPathData();
	}

	void
	PathComponent::setPath(const Paths& path, bool sharedPath) noexcept
	{
		paths_ = path;
		isSharedPath_ = sharedPath;
		this->uploadPathData();
	}

	const Paths&
	PathComponent::getPath() const noexcept
	{
		return paths_;
	}

	bool
	PathComponent::isSharedPath() const noexcept
	{
		return isSharedPath_;
	}

	void
	PathComponent::uploadPathData() noexcept
	{
		if (!paths_.empty())
			mesh_ = std::make_shared<Mesh>(makeMesh(font::makeTextContours(paths_), 1.0f, false));
		else
			mesh_ = nullptr;

		this->onPathReplace();
	}

	GameComponentPtr
	PathComponent::clone() const noexcept
	{
		auto instance = std::make_shared<PathComponent>();
		instance->setName(instance->getName());
		instance->setPath(isSharedPath_ ? paths_ : Paths(paths_), isSharedPath_);

		return instance;
	}

	void
	PathComponent::onActivate() except
	{
		this->addMessageListener("octoon:mesh:get", std::bind(&PathComponent::uploadPathData, this));
		this->uploadPathData();
	}

	void
	PathComponent::onDeactivate() noexcept
	{
	}

	void
	PathComponent::onPathReplace() noexcept
	{
		if (this->getGameObject())
			this->sendMessage("octoon:mesh:update", mesh_);
	}
}