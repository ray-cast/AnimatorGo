struct btDefaultMotionState;
class btBoxShape;
class btSphereShape;
class btCapsuleShape;
class btConvexShape;
class btCollisionShape;
class btCollisionObject;

class btTypedConstraint;
class btFixedConstraint;
class btGeneric6DofSpringConstraint;

class btRigidBody;

class btBroadphaseInterface;
class btCollisionDispatcher;
class btDefaultCollisionConfiguration;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btActionInterface;
class btPairCachingGhostObject;
class btOverlapFilterCallback;

namespace octoon
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