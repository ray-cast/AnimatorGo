#ifndef OCTOON_PATH_COMPONENT_H_
#define OCTOON_PATH_COMPONENT_H_

#include <octoon/model/path.h>
#include <octoon/game_component.h>
#include <functional>

namespace octoon
{
	class OCTOON_EXPORT PathComponent : public GameComponent
	{
		OctoonDeclareSubClass(PathComponent, GameComponent)
	public:
		typedef std::function<void(const model::PathPtr&)> OnPathReplaceEvent;
		typedef std::vector<OnPathReplaceEvent*> OnPathReplaceEvents;

	public:
		PathComponent() noexcept;
		PathComponent(model::Path&& mesh, bool sharedPath = false) noexcept;
		PathComponent(model::PathPtr&& mesh, bool sharedPath = false) noexcept;
		PathComponent(const model::Path& mesh, bool sharedPath = false) noexcept;
		PathComponent(const model::PathPtr& mesh, bool sharedPath = false) noexcept;
		virtual ~PathComponent() noexcept;

		void setPath(model::Path&& mesh, bool sharedPath = false) noexcept;
		void setPath(model::PathPtr&& mesh, bool sharedPath = false) noexcept;
		void setPath(const model::PathPtr& mesh, bool sharedPath = false) noexcept;
		const model::PathPtr& getPath() const noexcept;

		bool isSharedPath() const noexcept;

		void uploadPathData() noexcept;

		void addPathListener(OnPathReplaceEvent* func) noexcept;
		void removePathListener(const OnPathReplaceEvent* func) noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		virtual void onPathReplace(const model::PathPtr& mesh) noexcept;

	private:
		PathComponent(const PathComponent&) = delete;
		PathComponent& operator=(const PathComponent&) = delete;

	private:
		bool isSharedPath_;
		model::PathPtr path_;
		OnPathReplaceEvents delegates_;
	};
}

#endif