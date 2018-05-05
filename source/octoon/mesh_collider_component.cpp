#include <octoon/mesh_collider_component.h>
#include <octoon/rigidbody_component.h>
#include <octoon/runtime/except.h>
#include <PxPhysicsAPI.h>

namespace octoon
{
    OctoonImplementSubClass(MeshCollider, Collider, "MeshCollider")

    MeshCollider::MeshCollider() noexcept
		: isConvex(true)
    {
    }

	MeshCollider::MeshCollider(model::MeshPtr m) noexcept
		: isConvex(true), sharedMesh(m)
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

		if (isConvex)
		{
			physx::PxConvexMeshDesc convexDesc;
			convexDesc.points.count = sharedMesh->getNumVertices();
			convexDesc.points.stride = sizeof(physx::PxVec3);
			convexDesc.points.data = sharedMesh->getVertexArray().data();
			convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

			physx::PxDefaultMemoryOutputStream buf;
			physx::PxConvexMeshCookingResult::Enum result;
			if (!physics_feature->getCooking()->cookConvexMesh(convexDesc, buf, &result))
				throw runtime::runtime_error::create("cook convex mesh failed!");

			physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
			physx::PxConvexMesh* convexMesh = physics_feature->getSDK()->createConvexMesh(input);

			shape = physics_feature->getSDK()->createShape(physx::PxConvexMeshGeometry(convexMesh), *this->shared_material->getMaterial()); // reference count is 1
			if (!shape)
				throw runtime::runtime_error::create("create shape failed!");
		}
		else
		{
			physx::PxTriangleMeshDesc meshDesc;
			meshDesc.points.count = sharedMesh->getNumVertices();
			meshDesc.points.stride = sizeof(physx::PxVec3);
			meshDesc.points.data = sharedMesh->getVertexArray().data();

			meshDesc.triangles.count = sharedMesh->getNumIndices() / 3;
			meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
			meshDesc.triangles.data = sharedMesh->getIndicesArray().data();

			physx::PxDefaultMemoryOutputStream writeBuffer;
			physx::PxTriangleMeshCookingResult::Enum result;
			bool status = physics_feature->getCooking()->cookTriangleMesh(meshDesc, writeBuffer, &result);
			if (!status)
				throw runtime::runtime_error::create("cook triangle mesh failed!");

			physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

			physx::PxTriangleMeshGeometry triGeom;
			triGeom.triangleMesh = physics_feature->getSDK()->createTriangleMesh(readBuffer);

			shape = physics_feature->getSDK()->createShape(triGeom, *this->shared_material->getMaterial()); // reference count is 1
			if (!shape)
				throw runtime::runtime_error::create("create shape failed!");
		}
	}

	void MeshCollider::releaseCollider() except
	{
		shape->release(); // releases user reference, leaving reference count at 1
		shape = nullptr;
	}
}