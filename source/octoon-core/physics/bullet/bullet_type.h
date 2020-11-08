

namespace octoon
{
    namespace physics
    {
		class BulletContext;
		class BulletScene;
		class BulletRigidbody;
		class BulletShape;
		class BulletSphereShape;
		class BulletJoint;
		class BulletFixedJoint;
		class BulletConfigurableJoint;
    }
}

struct btDefaultMotionState;
class btBoxShape;
class btSphereShape;
class btCapsuleShape;
class btConvexShape;
class btCollisionShape;
class btCollisionObject;

class btFixedConstraint;

class btRigidBody;

class btBroadphaseInterface;
class btCollisionDispatcher;
class btDefaultCollisionConfiguration;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btTypedConstraint;
class btActionInterface;
class btPairCachingGhostObject;
class btGeneric6DofSpringConstraint;

namespace physx
{
	class PxProfilerCallback;
	class PxErrorCallback;
	class PxDefaultAllocator;
	class PxFoundation;
	class PxPvd;
	class PxPhysics;
	class PxCooking;
	class PxDefaultCpuDispatcher;
	class PxPvdTransport;

	class PxScene;
	class PxRigidActor;
	class PxRigidBody;
	class PxRigidStatic;
	class PxRigidDynamic;
	class PxMaterial;

	class PxTransform;
	class PxVec3;
	class PxQuat;
	class PxShape;
	

	class PxFixedJoint;
	class PxD6Joint;
} // namespace physx

namespace nv
{
	namespace cloth
	{
		class DxContextManagerCallback;
		class Factory;
	}
}