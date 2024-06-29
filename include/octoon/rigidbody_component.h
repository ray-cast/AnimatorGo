#ifndef OCTOON_RIGIDBODY_COMPONENT_H_
#define OCTOON_RIGIDBODY_COMPONENT_H_

#include <octoon/game_component.h>
#include <octoon/physics/physics_rigidbody.h>

namespace octoon
{
    enum class RigidbodySleepMode:int
    {
        NeverSleep, //RigidbodyComponent never automatically sleeps.
        StartAwake, //RigidbodyComponent is initially awake.
        StartAsleep, //RigidbodyComponent is initially asleep.
    };

    class OCTOON_EXPORT RigidbodyComponent final : public GameComponent, public PhysicsListener
	{
		OctoonDeclareSubClass(RigidbodyComponent, GameComponent)
    public:
        RigidbodyComponent() noexcept;
		RigidbodyComponent(bool type) noexcept;
		RigidbodyComponent(bool type, float mass) noexcept;
        virtual ~RigidbodyComponent();
        virtual GameComponentPtr clone() const noexcept;

        void setGravityScale(float scale) noexcept;
        void setMass(float m) noexcept;
        void setSleepMode(RigidbodySleepMode mode) noexcept;
        void setIsKinematic(bool type) noexcept;
        void setDynamicFriction(float f);
        void setStaticFriction(float f);
        void setRestitution(float f);
		void setLinearDamping(float value) noexcept;
		void setAngularDamping(float value) noexcept;
		void setGroup(std::uint8_t group) noexcept;
		void setGroupMask(std::uint16_t groupMask) noexcept;
		void setSleepThreshold(float threshold) noexcept;
		void setEnableCCD(bool enable) noexcept;
		void setSolverIterationCounts(std::uint32_t minPositionIters, std::uint32_t minVelocityIters) noexcept;
		void setLinearVelocity(const math::float3& linearVelocity) noexcept;
		void setAngularVelocity(const math::float3& angularVelocity) noexcept;
		void setInterpolationLinearVelocity(const math::float3& value) noexcept;
		void setInterpolationAngularVelocity(const math::float3& value) noexcept;

        float getDynamicFriction() const;
        float getStaticFriction() const;
        float getRestitution() const;
        float getGravityScale() const noexcept;
        float getMass() const noexcept;
		float getSleepThreshold() const noexcept;
		float getLinearDamping() const noexcept;
		float getAngularDamping() const noexcept;
		bool getEnableCCD() const noexcept;
		std::uint32_t getGroup() const noexcept;
		std::uint16_t getGroupMask() const noexcept;
		RigidbodySleepMode getSleepMode() const noexcept;
		math::float3 getLinearVelocity() const noexcept;
		math::float3 getAngularVelocity() const noexcept;

		void rotation(const math::Quaternion& quat) noexcept;
		void movePosition(const math::float3& position) noexcept;
		void setPositionAndRotation(const math::float3& position, const math::Quaternion& quat) noexcept;

		void clearForce() noexcept;
		void clearTorque() noexcept;

		void wakeUp() noexcept;

		bool getIsKinematic() const noexcept;

		std::shared_ptr<PhysicsRigidbody> getRigidbody();

    private:
        void onActivate() except override;
        void onDeactivate() noexcept override;

        void onAttachComponent(const GameComponentPtr& component) noexcept;
        void onDetachComponent(const GameComponentPtr& component) noexcept;

		void onFetchResult() noexcept override;

		void onMoveAfter() noexcept;

	private:
		void initializeRigidbody(class ColliderComponent& collder) noexcept;

	private:
        bool isKinematic_;
		bool enableCCD_;

        float gravityScale_;
        float mass_;
        float dynamicFriction_;
        float staticFriction_;
        float restitution_;
		float linearDamping_;
		float angularDamping_;
		float sleepThreshold_;
		std::uint8_t group_;
		std::uint16_t groupMask_;
		std::uint32_t minPositionIters_;
		std::uint32_t minVelocityIters_;

		math::float3 position_;
		math::Quaternion rotation_;

		math::float3 center_;
		math::Quaternion quaternion_;

		RigidbodySleepMode sleepMode_;

		std::shared_ptr<PhysicsRigidbody> rigidbody_;
    };
}

#endif