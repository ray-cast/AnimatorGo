#include <octoon/cloth_feature.h>
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
		, timeInterval_(0.02f)
		, defaultAllocatorCallback(std::make_unique<physx::PxDefaultAllocator>())
		, defaultErrorCallback(std::make_unique<physx::PxDefaultErrorCallback>())
		, profileCallback_(std::make_unique<ProfilerCallback>())
	{
		
	}

	ClothFeature::~ClothFeature() noexcept
	{
	}

	void
	ClothFeature::simulate(float delta) noexcept
	{
		assert(solver_);

		if (solver_->beginSimulation(timeInterval_))
		{
			for (int j = 0; j < solver_->getSimulationChunkCount(); j++)
				solver_->simulateChunk(j);
			solver_->endSimulation();
		}
	}

    void
	ClothFeature::onActivate() except
    {
		this->addMessageListener("feature:timer:fixed", std::bind(&ClothFeature::onFixedUpdate, this, std::placeholders::_1));

		nv::cloth::InitializeNvCloth(defaultAllocatorCallback.get(), defaultErrorCallback.get(), nv::cloth::GetNvClothAssertHandler(), profileCallback_.get());

		factory_ = NvClothCreateFactoryCPU();
		if (!factory_)
			throw runtime_error::create("NvClothCreateFactoryCPU failed!");

		solver_ = factory_->createSolver();
		if (!factory_)
			throw runtime_error::create("ClothFeature::CreateSolver failed!");
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
    }

    void
	ClothFeature::onFrameEnd() noexcept
    {
    }

	void
	ClothFeature::onFixedUpdate(const std::any& data) noexcept
	{
		if (data.type() == typeid(float))
		{
			timeInterval_ = std::any_cast<float>(data);
			if (timeInterval_ > 0.0f)
			{
				if (solver_->beginSimulation(timeInterval_))
				{
					for (int j = 0; j < solver_->getSimulationChunkCount(); j++)
						solver_->simulateChunk(j);
					solver_->endSimulation();
				}
			}
		}
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