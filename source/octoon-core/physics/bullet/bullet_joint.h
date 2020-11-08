#ifndef OCTOON_BULLET_JOINT_H_
#define OCTOON_BULLET_JOINT_H_

#include <memory>

#include <octoon/math/math.h>
#include <octoon/runtime/platform.h>
#include <octoon/physics/physics_joint.h>
#include <octoon/physics/physics_type.h>

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT BulletJoint : public PhysicsJoint
		{
		public:
			BulletJoint() noexcept;
			virtual ~BulletJoint() noexcept;

			virtual void connect(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs) = 0;

		private:
			BulletJoint(const BulletJoint&) = delete;
			BulletJoint& operator=(const BulletJoint&) = delete;
		};
	}
}

#endif