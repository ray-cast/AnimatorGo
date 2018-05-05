#ifndef OCTOON_RIGIDBODY_COMPONENT_H_
#define OCTOON_RIGIDBODY_COMPONENT_H_

#include <memory>
#include <vector>
#include <octoon/game_component.h>
#include <octoon/physics_feature.h>
#include <octoon/collider_component.h>
#include <octoon/math/math.h>
#include <octoon/runtime/singleton.h>

namespace physx
{
	class PxRigidDynamic;
	class PxRigidBody;
	class PxRigidActor;
}

namespace octoon
{
    enum class RigidbodySleepMode:int
    {
        NeverSleep, //Rigidbody never automatically sleeps.
        StartAwake, //Rigidbody is initially awake.
        StartAsleep, //Rigidbody is initially asleep.
    };

    class OCTOON_EXPORT Rigidbody final : public GameComponent
	{
		OctoonDeclareSubClass(Rigidbody, runtime::RttiInterface)
        public:
            Rigidbody() noexcept;
			Rigidbody(bool type) noexcept;
			Rigidbody(bool type, float mass) noexcept;
			Rigidbody(bool type, float mass, const math::Vector3& offset) noexcept;
            ~Rigidbody();
            virtual GameComponentPtr clone() const noexcept;

            void setAngularVelocity(const math::float3& v) noexcept;
			math::float3 getAngularVelocity() const noexcept;

            void setMass(float m) noexcept;
            float getMass() const noexcept;

			void setMassOffset(math::Vector3 offset) noexcept;
			math::Vector3 getMassOffset() const noexcept;

            void setSleepMode(RigidbodySleepMode mode) noexcept;
            RigidbodySleepMode getSleepMode() const noexcept;

			void setIsKinematic(bool type) noexcept;
			bool getIsKinematic() const noexcept;

			physx::PxRigidActor* getRigidbody() noexcept { return body; }

        private:
        	virtual void onActivate() except override;
            virtual void onDeactivate() noexcept override;

            virtual void onAttachComponent(const GameComponentPtr& component) noexcept;
            virtual void onDetachComponent(const GameComponentPtr& component) noexcept;

			virtual void onFrameBegin() except override;
			virtual void onFrame() except override;
			virtual void onFrameEnd() except override;

			virtual void onMoveAfter() noexcept;

			void buildRigidBody() except;
			void releaseRigidBody() noexcept;
        private:
			physx::PxRigidActor* body;

            float angularVelocity;
            float gravityScale; // The degree to which this object is affected by gravity.
			bool isKinematic;
            float mass; // Mass of the rigidbody.
			math::Vector3 massOffset;
            RigidbodySleepMode sleepMode;
            math::Vector3 velocity;

            friend class Collider;
            friend class BoxCollider;
			friend class SphereCollider;
    };
}

#endif // OCTOON_RIGIDBODY_COMPONENT_H_