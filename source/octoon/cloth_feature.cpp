#include <octoon/cloth_feature.h>
#include <octoon/timer_feature.h>
#include <octoon/runtime/except.h>

#include <PxPhysicsAPI.h>
#include <NvCloth/Factory.h>
#include <NvCloth/Solver.h>

namespace octoon
{
    OctoonImplementSubClass(ClothFeature, GameFeature, "ClothFeature")

	class ProfilerCallback final : public physx::PxProfilerCallback
	{
	public:
		ProfilerCallback()
		{
		}

		~ProfilerCallback()
		{
		}

		virtual void* zoneStart(const char* eventName, bool detached, uint64_t contextId)
		{
			return nullptr;
		}

		virtual void zoneEnd(void* profilerData, const char* eventName, bool detached, uint64_t contextId)
		{
		}
	};

	ClothFeature::ClothFeature() except
		: factory_(nullptr)
		, needUpdate_(false)
		, defaultAllocatorCallback(std::make_unique<physx::PxDefaultAllocator>())
		, defaultErrorCallback(std::make_unique<physx::PxDefaultErrorCallback>())
		, profileCallback_(std::make_unique<ProfilerCallback>())
	{
		
	}

	ClothFeature::~ClothFeature() noexcept
	{
	}

    void
	ClothFeature::onActivate() except
    {
		this->addMessageListener("feature:timer:fixed", std::bind(&ClothFeature::onFixedUpdate, this, std::placeholders::_1));

		nv::cloth::InitializeNvCloth(defaultAllocatorCallback.get(), defaultErrorCallback.get(), nv::cloth::GetNvClothAssertHandler(), profileCallback_.get());

		factory_ = NvClothCreateFactoryCPU();
		if (!factory_)
			throw runtime::runtime_error::create("NvClothCreateFactoryCPU failed!");

		solver_ = factory_->createSolver();
		if (!factory_)
			throw runtime::runtime_error::create("ClothFeature::CreateSolver failed!");
    }

    void
	ClothFeature::onDeactivate() noexcept
    {
		if (factory_)
			NvClothDestroyFactory(factory_);
    }

    void
	ClothFeature::onReset() noexcept
    {
    }

    void
	ClothFeature::onFrameBegin() noexcept
    {
    }

    void
	ClothFeature::onFrame() except
    {
		if (needUpdate_)
		{
			auto timeFeature = this->getFeature<TimerFeature>();
			if (timeFeature)
			{
				if (solver_->beginSimulation(timeFeature->getTimeInterval() / CLOCKS_PER_SEC))
				{
					for (int i = 0; i < solver_->getSimulationChunkCount(); i++)
						solver_->simulateChunk(i);
					solver_->endSimulation();
				}
			}

			needUpdate_ = false;
		}
    }

    void
	ClothFeature::onFrameEnd() noexcept
    {
    }

	void
	ClothFeature::onFixedUpdate(const runtime::any& data) noexcept
	{
		needUpdate_ = true;
	}

	nv::cloth::Factory*
	ClothFeature::getContext()
	{
		return factory_;
	}

	nv::cloth::Solver*
	ClothFeature::getScene()
	{
		return solver_;
	}
}