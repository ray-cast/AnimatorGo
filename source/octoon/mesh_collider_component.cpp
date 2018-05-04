#include <octoon/mesh_collider_component.h>
#include <octoon/rigidbody_component.h>
#include <octoon/runtime/except.h>
#include <PxPhysicsAPI.h>


namespace octoon
{
    OctoonImplementSubClass(MeshCollider, Collider, "MeshCollider")

    MeshCollider::MeshCollider() noexcept
    {

    }

	MeshCollider::MeshCollider(model::MeshPtr m) noexcept
		: sharedMesh(m)
	{
	}

    MeshCollider::~MeshCollider()
    {

    }

    GameComponentPtr MeshCollider::clone() const noexcept
    {
        return std::make_shared<MeshCollider>();
    }

	void MeshCollider::setMesh(model::MeshPtr m) noexcept
	{
		sharedMesh = m;
	}

	model::MeshPtr MeshCollider::getMesh() const noexcept
	{
		return sharedMesh;
	}

    void MeshCollider::onCollisionChange() noexcept
    {
    }

    void MeshCollider::onCollisionEnter() noexcept
    {
    }

    void MeshCollider::onCollisionExit() noexcept
    {
    }

    void MeshCollider::onCollisionStay() noexcept
    {

    }

    void MeshCollider::onAttach() except
    {
		buildCollider();
    }

    void MeshCollider::onDetach() noexcept
    {
		releaseCollider();
    }

    void MeshCollider::onAttachComponent(const GameComponentPtr& component) except
    {
    }

    void MeshCollider::onDetachComponent(const GameComponentPtr& component) noexcept
    {
    }

	void MeshCollider::buildCollider() except
	{
		auto physics_feature = GameApp::instance()->getFeature<PhysicsFeature>();

		physx::PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = sharedMesh->getNumVertices();
		meshDesc.points.stride = sizeof(physx::PxVec3);
		meshDesc.points.data = sharedMesh->getVertexArray().data();

		meshDesc.triangles.count = sharedMesh->getNumIndices();
		meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
		meshDesc.triangles.data = sharedMesh->getIndicesArray().data();

		physx::PxDefaultMemoryOutputStream writeBuffer;
		physx::PxTriangleMeshCookingResult::Enum result;
		bool status = physics_feature->getCooking()->cookTriangleMesh(meshDesc, writeBuffer, &result);
		if (!status)
		{
			runtime::runtime_error::create("cook triangle mesh failed!");
			return;
		}

		physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

		physx::PxTriangleMeshGeometry triGeom;
		triGeom.triangleMesh = physics_feature->getSDK()->createTriangleMesh(readBuffer);

		shape = physics_feature->getSDK()->createShape(triGeom, *this->shared_material->getMaterial()); // reference count is 1
		if (!shape)
			runtime::runtime_error::create("create shape failed!");
	}

	void MeshCollider::releaseCollider() except
	{
		shape->release(); // releases user reference, leaving reference count at 1
		shape = nullptr;
	}
}