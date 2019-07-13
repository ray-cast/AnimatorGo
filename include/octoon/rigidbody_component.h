#ifndef OCTOON_RIGIDBODY_COMPONENT_H_
#define OCTOON_RIGIDBODY_COMPONENT_H_

#include <memory>
#include <vector>
#include <octoon/game_component.h>
#include <octoon/physics_feature.h>
#include <octoon/collider_component.h>
#include <octoon/math/math.h>
#include <octoon/runtime/singleton.h>

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
            ~Rigidbody();
            virtual GameComponentPtr clone() const noexcept;

            void setAngularVelocity(float v) noexcept;
            float getAngularVelocity() const noexcept;

            void setGravityScale(float scale) noexcept;
            float getGravityScale() const noexcept;

            void setMass(float m) noexcept;
            float getMass() const noexcept;

            void setSleepMode(RigidbodySleepMode mode) noexcept;
            RigidbodySleepMode getSleepMode() const noexcept;

			void setIsKinematic(bool type) noexcept;
			bool getIsKinematic() const noexcept;

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
    };
}

#endif // OCTOON_RIGIDBODY_COMPONENT_H_