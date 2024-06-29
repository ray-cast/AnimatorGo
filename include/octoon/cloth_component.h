#ifndef OCTOON_CLOTH_COMPONENT_H_
#define OCTOON_CLOTH_COMPONENT_H_

#include <octoon/game_component.h>
#include <octoon/mesh/mesh.h>

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
		void setFriction(float friction) noexcept;
		void setSolverFrequency(float solverFrequency) noexcept;

		float getTotalMass() const noexcept;
		float getFriction() const noexcept;
		float getSolverFrequency() const noexcept;

		void setTarget(const GameObjectPtr& object) noexcept;
		GameObjectPtr getTarget() const noexcept;

		void setMaterialId(std::uint32_t n) noexcept;

		void setEnableCCD(bool enableContinuousCollision) noexcept;
		bool getEnableCCD() const noexcept;

		void addCollider(GameObjectPtr&& collider) noexcept;
		void addCollider(const GameObjectPtr& collider) noexcept;

		void setColliders(GameObjects&& collider) noexcept;
		void setColliders(const GameObjects& collider) noexcept;
		const GameObjects& getColliders() const noexcept;

		void setGravity(const math::float3& gravity) noexcept;
		const math::float3& getGravity() const noexcept;

		void setPartices(const math::float4s& partices) noexcept;
		const math::float4s& getPartices() const noexcept;

		void setIndices(const math::uint1s& indices) noexcept;
		const math::uint1s& getIndices() const noexcept;

		void setPinVertexIndices(const math::uint1s& indices) noexcept;
		const math::uint1s&  getPinVertexIndices() const noexcept;

		GameComponentPtr clone() const noexcept;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onFixedUpdate() noexcept override;
		void onLateUpdate() noexcept override;

	private:
		void updateCollideData() noexcept;
		void updateParticesData() noexcept;
		void uploadMeshData(const Mesh& mesh) noexcept;

	private:
		ClothComponent(const ClothComponent&) = delete;
		ClothComponent& operator=(const ClothComponent&) = delete;

	private:
		bool needUpdate_;
		bool enableContinuousCollision_;

		std::uint32_t materialId_;

		float totalMass_;
		float friction_;
		float solverFrequency_;

		GameObjects collides_;
		GameObjectPtr target_;

		math::float3 gravity_;
		math::uint1s pinVertexIndices_;

		math::float4s partices_;
		math::uint1s  particesIndices_;

		nv::cloth::Cloth* cloth_;
	};
}

#endif