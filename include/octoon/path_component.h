#ifndef OCTOON_PATH_COMPONENT_H_
#define OCTOON_PATH_COMPONENT_H_

#include <octoon/model/path.h>
#include <octoon/game_component.h>
#include <octoon/mesh/mesh.h>
#include <functional>

namespace octoon
{
	class OCTOON_EXPORT PathComponent final : public GameComponent
	{
		OctoonDeclareSubClass(PathComponent, GameComponent)
	public:
		PathComponent() noexcept;
		PathComponent(Paths&& mesh, bool sharedPath = false) noexcept;
		PathComponent(const Paths& mesh, bool sharedPath = false) noexcept;
		virtual ~PathComponent() noexcept;

		void setPath(Paths&& mesh, bool sharedPath = false) noexcept;
		void setPath(const Paths& mesh, bool sharedPath = false) noexcept;
		const Paths& getPath() const noexcept;

		bool isSharedPath() const noexcept;

		void uploadPathData() noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onPathReplace() noexcept;

	private:
		PathComponent(const PathComponent&) = delete;
		PathComponent& operator=(const PathComponent&) = delete;

	private:
		bool isSharedPath_;
		Paths paths_;
		MeshPtr mesh_;
	};
}

#endif