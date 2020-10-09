#ifndef OCTOON_CLOTH_FEATURE_H_
#define OCTOON_CLOTH_FEATURE_H_

#include <octoon/game_feature.h>

namespace nv
{
	namespace cloth
	{
		class Factory;
		class Solver;
	}
}

namespace physx
{
	class PxProfilerCallback;
	class PxErrorCallback;
	class PxDefaultAllocator;
}

namespace octoon
{
	class OCTOON_EXPORT ClothFeature final : public GameFeature
	{
		OctoonDeclareSubClass(ClothFeature, GameFeature)
	public:
		ClothFeature() except;
		virtual ~ClothFeature() noexcept;

	public:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onReset() noexcept override;

		void onFrameBegin() noexcept override;
		void onFrame() except;
		void onFrameEnd() noexcept override;

		void onFixedUpdate(const std::any& data) noexcept;

	public:
		nv::cloth::Factory* getContext();
		nv::cloth::Solver* getScene();

	private:
		ClothFeature(const ClothFeature&) = delete;
		ClothFeature& operator=(const ClothFeature&) = delete;

	private:
		float timeInterval_;

		nv::cloth::Solver* solver_;
		nv::cloth::Factory* factory_;

		std::unique_ptr<class ProfilerCallback> profileCallback_;
		std::unique_ptr<physx::PxErrorCallback> defaultErrorCallback;
		std::unique_ptr<physx::PxDefaultAllocator> defaultAllocatorCallback;
	};
}

#endif