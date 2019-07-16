#ifndef OCTOON_RIGIDBODY_COMPONENT_H_
#define OCTOON_RIGIDBODY_COMPONENT_H_

#include <memory>
#include <vector>
#include <octoon/game_component.h>
#include <octoon/physics_feature.h>
#include <octoon/collider_component.h>
#include <octoon/math/math.h>
#include <octoon/runtime/singleton.h>

#include <octoon/physics/physics_rigidbody.h>

namespace octoon
{
    enum class RigidbodySleepMode:int
    {
        NeverSleep, //RigidbodyComponent never automatically sleeps.
        StartAwake, //RigidbodyComponent is initially awake.
        StartAsleep, //RigidbodyComponent is initially asleep.
    };

    class OCTOON_EXPORT RigidbodyComponent final : public GameComponent
	{
	OctoonDeclareSubClass(RigidbodyComponent, GameComponent)
    public:
        RigidbodyComponent() noexcept;
		RigidbodyComponent(bool type) noexcept;
		RigidbodyComponent(bool type, float mass) noexcept;
        ~RigidbodyComponent();
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

		void setDynamicFriction(float f);
		float getDynamicFriction() const;
		void setStaticFriction(float f);
		float getStaticFriction() const;
		void setRestitution(float f);
		float getRestitution() const;

		std::shared_ptr<physics::PhysicsRigidbody> getRigidbody();

    private:
        virtual void onActivate() except override;
        virtual void onDeactivate() noexcept override;

        virtual void onAttachComponent(const GameComponentPtr& component) noexcept;
        virtual void onDetachComponent(const GameComponentPtr& component) noexcept;

		virtual void onFrameEnd() except override;

		virtual void onMoveAfter() noexcept;

	private:
		std::shared_ptr<physics::PhysicsRigidbody> rigidbody;
    };
}

#endif // OCTOON_RIGIDBODY_COMPONENT_H_