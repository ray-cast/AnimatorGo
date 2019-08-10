#ifndef OCTOON_CLOTH_COMPONENT_H_
#define OCTOON_CLOTH_COMPONENT_H_

#include <octoon/game_component.h>
#include <octoon/model/mesh.h>

namespace nv
{
	namespace cloth
	{
		class Cloth;
	}
}
namespace octoon
{
	class OCTOON_EXPORT ClothComponent final : public GameComponent
	{
		OctoonDeclareSubClass(ClothComponent, GameComponent)
	public:
		ClothComponent() noexcept;
		ClothComponent(const GameObjects& collider) noexcept;
		~ClothComponent() noexcept;

		void setTotalMass(float totalMass) noexcept;
		float getTotalMass() const noexcept;

		void addCollider(GameObjectPtr&& collider) noexcept;
		void addCollider(const GameObjectPtr& collider) noexcept;

		void setColliders(GameObjects&& collider) noexcept;
		void setColliders(const GameObjects& collider) noexcept;
		const GameObjects& getColliders() const noexcept;

		GameComponentPtr clone() const noexcept;
	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveAfter() noexcept override;

		void onFixedUpdate() noexcept override;
		void onLateUpdate() noexcept override;

	private:
		void uploadMeshData(const model::Mesh& mesh) noexcept;

	private:
		ClothComponent(const ClothComponent&) = delete;
		ClothComponent& operator=(const ClothComponent&) = delete;

	private:
		bool needUpdate_;
		float totalMass_;

		nv::cloth::Cloth* cloth_;
		GameObjects collides_;
	};
}

#endif