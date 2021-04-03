#ifndef OCTOON_MESH_H_
#define OCTOON_MESH_H_

#include <octoon/model/bone.h>
#include <octoon/mesh/combine_mesh.h>
#include <octoon/model/vertex_weight.h>
#include <octoon/math/math.h>
#include <octoon/runtime/rtti_interface.h>

#define TEXTURE_ARRAY_COUNT 4

namespace octoon
{
	struct MeshHit
	{
		class Mesh* object;
		std::size_t mesh;
		float distance;
		math::float3 point;
	};

	class OCTOON_EXPORT Mesh : public runtime::RttiInterface
	{
		OctoonDeclareSubClass(Mesh, runtime::RttiInterface)
	public:
		Mesh() noexcept;
		virtual ~Mesh() noexcept;

		void setName(std::string_view name) noexcept;
		const std::string& getName() const noexcept;

		void setVertexArray(const math::float3s& array) noexcept;
		void setNormalArray(const math::float3s& array) noexcept;
		void setColorArray(const math::float4s& array) noexcept;
		void setTangentArray(const math::float4s& array) noexcept;
		void setTexcoordArray(const math::float2s& array, std::uint8_t n = 0) noexcept;
		void setWeightArray(const VertexWeights& array) noexcept;
		void setIndicesArray(const math::uint1s& array, std::size_t n = 0) noexcept;
		void setBindposes(const math::float4x4s& array) noexcept;

		void setVertexArray(math::float3s&& array) noexcept;
		void setNormalArray(math::float3s&& array) noexcept;
		void setColorArray(math::float4s&& array) noexcept;
		void setTangentArray(math::float4s&& array) noexcept;
		void setTexcoordArray(math::float2s&& array, std::uint8_t n = 0) noexcept;
		void setWeightArray(VertexWeights&& array) noexcept;
		void setIndicesArray(math::uint1s&& array, std::size_t n = 0) noexcept;
		void setBindposes(math::float4x4s&& array) noexcept;

		math::float3s& getVertexArray() noexcept;
		math::float3s& getNormalArray() noexcept;
		math::float4s& getTangentArray() noexcept;
		math::float4s& getColorArray() noexcept;
		math::float2s& getTexcoordArray(std::uint8_t n = 0) noexcept;
		VertexWeights& getWeightArray() noexcept;
		math::uint1s& getIndicesArray(std::size_t n = 0) noexcept;
		math::float4x4s& getBindposes() noexcept;

		const math::float3s& getVertexArray() const noexcept;
		const math::float3s& getNormalArray() const noexcept;
		const math::float4s& getTangentArray() const noexcept;
		const math::float4s& getColorArray() const noexcept;
		const math::float2s& getTexcoordArray(std::uint8_t n = 0) const noexcept;
		const VertexWeights& getWeightArray() const noexcept;
		const math::uint1s& getIndicesArray(std::size_t n = 0) const noexcept;

		const Bones& getBoneArray(const Bones& array) const noexcept;
		const math::float4x4s& getBindposes() const noexcept;

		std::size_t getNumVertices() const noexcept;
		std::size_t getNumSubsets() const noexcept;
		std::size_t getTexcoordNums() const noexcept;

		void mergeVertices() noexcept;

		bool mergeMeshes(const Mesh& mesh, bool force = false) noexcept;
		bool mergeMeshes(const CombineMesh instances[], std::size_t numInstance, bool merge) noexcept;
		bool mergeMeshes(const std::vector<CombineMesh>& instances, bool merge) noexcept;

		void computeFaceNormals(std::vector<math::float3s>& faceNormals) noexcept;
		void computeVertexNormals() noexcept;
		void computeVertexNormals(std::size_t i) noexcept;
		void computeVertexNormals(const math::float3s& faceNormals) noexcept;
		void computeVertexNormals(std::size_t width, std::size_t height) noexcept;
		void computeTangents(std::uint8_t texSlot = 0) noexcept;
		void computeTangentQuats(math::float4s& tangentQuat) const noexcept;
		void computeBoundingBox() noexcept;
		void computeLightMap(std::uint32_t width, std::uint32_t height) noexcept;

		const math::BoundingBox& getBoundingBoxAll() const noexcept;
		const math::BoundingBox& getBoundingBox(std::size_t n) const noexcept;

		void setDirty(bool dirty) noexcept;
		bool isDirty() const noexcept;

		bool raycast(const math::Raycast& ray, MeshHit& hit) noexcept;
		bool raycastAll(const math::Raycast& ray, std::vector<MeshHit>& hits) noexcept;

		void clear() noexcept;
		std::shared_ptr<Mesh> clone() const noexcept;

	private:
		std::string _name;
		bool _dirty;

		math::float3s _vertices;
		math::float3s _normals;
		math::float4s _colors;
		math::float2s _texcoords[TEXTURE_ARRAY_COUNT];
		math::float4s _tangents;
		math::float4x4s _bindposes;
		math::BoundingBox _boundingBox;

		std::vector<Bone> _bones;
		std::vector<VertexWeight> _weights;

		std::vector<math::uint1s> _indices;
		std::vector<math::BoundingBox> _boundingBoxs;
	};

	using MeshPtr = std::shared_ptr<Mesh>;
	using Meshes = std::vector<MeshPtr>;
}

#endif
