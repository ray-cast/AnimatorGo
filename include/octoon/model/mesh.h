#ifndef OCTOON_MODEL_MESH_H_
#define OCTOON_MODEL_MESH_H_

#include <octoon/model/bone.h>
#include <octoon/model/combine_mesh.h>
#include <octoon/model/vertex_weight.h>
#include <octoon/math/math.h>

#include <string>

namespace octoon
{
	namespace model
	{
		class OCTOON_EXPORT Mesh final
		{
		public:
			Mesh() noexcept;
			~Mesh() noexcept;

			void setName(std::string&& name) noexcept;
			void setName(const std::string& name) noexcept;
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

			void makeCircle(float radius, std::uint32_t segments, float thetaStart = 0, float thetaLength = math::PI_2) noexcept;
			void makePlane(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1) noexcept;
			void makePlane(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir, bool clear = true) noexcept;
			void makePlaneWireframe(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir, bool clear = true) noexcept;
			void makeFloor(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1) noexcept;
			void makeNoise(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1) noexcept;
			void makeCube(float width, float height, float depth, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1, std::uint32_t depthSegments = 1) noexcept;
			void makeCubeWireframe(float width, float height, float depth, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1, std::uint32_t depthSegments = 1) noexcept;
			void makeRing(float innerRadius, float outerRadius, std::uint32_t thetaSegments, std::uint32_t phiSegments, float thetaStart = 0, float thetaLength = math::PI) noexcept;
			void makeSphere(float radius, std::uint32_t widthSegments = 32, std::uint32_t heightSegments = 24, float phiStart = 0.0, float phiLength = math::PI_2, float thetaStart = 0, float thetaLength = math::PI) noexcept;
			void makeVolumes(float fovy, float znear, float zfar) noexcept;
			void makeCone(float radius, float height, std::uint32_t segments = 32, float thetaStart = 0, float thetaLength = math::PI_2) noexcept;

			bool combineMeshes(const Mesh& mesh, bool force = false) noexcept;
			bool combineMeshes(const CombineMesh instances[], std::size_t numInstance, bool merge) noexcept;
			bool combineMeshes(const CombineMeshes& instances, bool merge) noexcept;

			void mergeVertices() noexcept;

			void computeFaceNormals(std::vector<math::float3s>& faceNormals) noexcept;
			void computeVertexNormals() noexcept;
			void computeVertexNormals(std::size_t i) noexcept;
			void computeVertexNormals(const math::float3s& faceNormals) noexcept;
			void computeVertexNormals(std::size_t width, std::size_t height) noexcept;
			void computeTangents(std::uint8_t texSlot = 0) noexcept;
			void computeTangentQuats(math::float4s& tangentQuat) const noexcept;
			void computeBoundingBox() noexcept;

			const math::BoundingBox& getBoundingBoxAll() const noexcept;
			const math::BoundingBox& getBoundingBox(std::size_t n) const noexcept;

			void clear() noexcept;
			MeshPtr clone() const noexcept;

		private:
			std::string _name;

			Bones _bones;
			VertexWeights _weights;

			math::float3s _vertices;
			math::float3s _normals;
			math::float4s _colors;
			math::float2s _texcoords[TEXTURE_ARRAY_COUNT];
			math::float4s _tangents;
			math::float4x4s _bindposes;
			math::BoundingBox _boundingBox;

			std::vector<math::uint1s> _indices;
			std::vector<math::BoundingBox> _boundingBoxs;
		};

		inline Mesh makeCircle(float radius, std::uint32_t segments, float thetaStart = 0, float thetaLength = math::PI_2) noexcept
		{
			Mesh mesh;
			mesh.makeCircle(radius, segments, thetaStart, thetaLength);
			return mesh;
		}

		inline Mesh makePlane(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1) noexcept
		{
			Mesh mesh;
			mesh.makePlane(width, height, widthSegments, heightSegments);
			return mesh;
		}

		inline Mesh makePlane(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir) noexcept
		{
			Mesh mesh;
			mesh.makePlane(width, height, depth, widthSegments, heightSegments, depthSegments, u, v, udir, vdir);
			return mesh;
		}

		inline Mesh makePlaneWireframe(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir, bool clear = true) noexcept
		{
			Mesh mesh;
			mesh.makePlaneWireframe(width, height, depth, widthSegments, heightSegments, depthSegments, u, v, udir, vdir, clear);
			return mesh;
		}

		inline Mesh makeFloor(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1) noexcept
		{
			Mesh mesh;
			mesh.makeFloor(width, height, widthSegments, heightSegments);
			return mesh;
		}

		inline Mesh makeNoise(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1) noexcept
		{
			Mesh mesh;
			mesh.makeNoise(width, height, widthSegments, heightSegments);
			return mesh;
		}

		inline Mesh makeCube(float width, float height, float depth, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1, std::uint32_t depthSegments = 1) noexcept
		{
			Mesh mesh;
			mesh.makeCube(width, height, depth, widthSegments, heightSegments, depthSegments);
			return mesh;
		}

		inline Mesh makeCubeWireframe(float width, float height, float depth, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1, std::uint32_t depthSegments = 1) noexcept
		{
			Mesh mesh;
			mesh.makeCubeWireframe(width, height, depth, widthSegments, heightSegments, depthSegments);
			return mesh;
		}

		inline Mesh makeRing(float innerRadius, float outerRadius, std::uint32_t thetaSegments, std::uint32_t phiSegments, float thetaStart = 0, float thetaLength = math::PI) noexcept
		{
			Mesh mesh;
			mesh.makeRing(innerRadius, outerRadius, thetaSegments, phiSegments, thetaStart, thetaLength);
			return mesh;
		}

		inline Mesh makeSphere(float radius, std::uint32_t widthSegments = 32, std::uint32_t heightSegments = 24, float phiStart = 0.0, float phiLength = math::PI_2, float thetaStart = 0, float thetaLength = math::PI) noexcept
		{
			Mesh mesh;
			mesh.makeSphere(radius, widthSegments, heightSegments, phiStart, phiLength, thetaStart, thetaLength);
			return mesh;
		}

		inline Mesh makeVolumes(float fovy, float znear, float zfar) noexcept
		{
			Mesh mesh;
			mesh.makeVolumes(fovy, znear, zfar);
			return mesh;
		}

		inline Mesh makeCone(float radius, float height, std::uint32_t segments = 32, float thetaStart = 0, float thetaLength = math::PI_2) noexcept
		{
			Mesh mesh;
			mesh.makeCone(radius, height, segments, thetaStart, thetaLength);
			return mesh;
		}
	}
}
#endif
