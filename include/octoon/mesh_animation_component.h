#ifndef OCTOON_MESH_ANIMATION_COMPONENT_H_
#define OCTOON_MESH_ANIMATION_COMPONENT_H_

#include <octoon/mesh/mesh.h>
#include <octoon/animation_component.h>

namespace Alembic {
	namespace Abc {
		namespace v12 {
			class IObject;
		}
	}
}

namespace octoon
{
	class OCTOON_EXPORT MeshAnimationComponent final : public AnimationComponent
	{
		OctoonDeclareSubClass(MeshAnimationComponent, AnimationComponent)
	public:
		MeshAnimationComponent() noexcept;
		explicit MeshAnimationComponent(const std::string& path) noexcept(false);
		virtual ~MeshAnimationComponent() noexcept;

		bool play(const std::string& status = "default") noexcept override;
		void pause() noexcept override;
		void reset() noexcept override;

		void setTime(float time) noexcept override;
		float getTime() const noexcept override;

		void sample(float delta = 0.0f) noexcept;
		void evaluate(float delta = 0.0f) noexcept;

		void uploadMeshData() noexcept;

		const model::MeshPtr& getMesh() const noexcept;

		const animation::AnimatorStateInfo<float>& getCurrentAnimatorStateInfo() const noexcept override;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onFixedUpdate() noexcept override;
		void onLateUpdate() noexcept override;

		void onMeshReplace() noexcept;
		void onAnimationUpdate() noexcept;

	private:
		void setupAnimationData(const std::string& path) noexcept(false);
		void setupAnimationData(const Alembic::Abc::v12::IObject& object) noexcept(false);

	private:
		MeshAnimationComponent(const MeshAnimationComponent&) = delete;
		MeshAnimationComponent& operator=(const MeshAnimationComponent&) = delete;

	private:
		std::string path_;

		float minTime_;
		float maxTime_;

		bool needUpdate_;
		bool enableAnimation_;

		model::MeshPtr mesh_;

		animation::AnimatorStateInfo<float> animationState_;

		std::shared_ptr<Alembic::Abc::v12::IObject> object_;
	};
}

#endif