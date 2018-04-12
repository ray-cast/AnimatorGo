#ifndef OCTOON_PROPERTY_H_
#define OCTOON_PROPERTY_H_

#include <octoon/model/modtypes.h>
#include <octoon/model/bone.h>
#include <octoon/model/combine_mesh.h>

#include <octoon/math/mathfwd.h>
#include <octoon/math/vector2.h>
#include <octoon/math/vector3.h>
#include <octoon/math/boundingbox.h>

#include <string>

namespace octoon
{
	namespace model
	{
		enum ShapeType
		{
			ShapeTypeCircle,
			ShapeTypeSquare,
			ShapeTypeCapsule
		};

		class CameraProperty final
		{
		};

		class LightProperty final
		{
		};

		class RigidbodyProperty
		{
		public:
			std::string name;

			std::uint32_t bone;

			std::uint8_t group;
			std::uint16_t groupMask;
			ShapeType shape;

			octoon::math::float3 scale;
			octoon::math::float3 position;
			octoon::math::float3 rotation;

			float mass;
			float movementDecay;
			float rotationDecay;
			float elasticity;
			float friction;

			std::uint8_t physicsOperation; //0:Follow Bone (static), 1:Physics Calc. (dynamic), 2: Physics Calc. + Bone position matching
		};

		class JointProperty
		{
		public:
			std::string name;

			octoon::math::float3 position;
			octoon::math::float3 rotation;

			std::uint32_t bodyIndexA;
			std::uint32_t bodyIndexB;

			octoon::math::float3 movementLowerLimit;
			octoon::math::float3 movementUpperLimit;
			octoon::math::float3 rotationLowerLimit;
			octoon::math::float3 rotationUpperLimit;

			octoon::math::float3 springMovementConstant;
			octoon::math::float3 springRotationConstant;
		};

		class TextureProperty final
		{
		public:
			TextureProperty() noexcept;
			TextureProperty(const std::string& filename) noexcept;

			TextureProperty(const char* filename, std::size_t length) noexcept;

			std::string   name;
			std::uint32_t width;
			std::uint32_t height;
		};


		class MaterialProperty final
		{
		public:
			struct MaterialParam
			{
				std::string key;

				std::size_t type;
				std::size_t index;
				std::size_t length;
				std::size_t dataType;

				char* data;
			};

		public:
			MaterialProperty() noexcept;
			~MaterialProperty() noexcept;

			bool set(const char* key, std::size_t type, std::size_t index, int value) noexcept;
			bool set(const char* key, std::size_t type, std::size_t index, float value) noexcept;
			bool set(const char* key, std::size_t type, std::size_t index, const octoon::math::Vector3& value) noexcept;
			bool set(const char* key, std::size_t type, std::size_t index, const octoon::math::Vector4& value) noexcept;
			bool set(const char* key, std::size_t type, std::size_t index, const char* value) noexcept;
			bool set(const char* key, std::size_t type, std::size_t index, const unsigned char* value) noexcept;
			bool set(const char* key, std::size_t type, std::size_t index, const std::string& value) noexcept;

			bool get(const char* key, std::size_t type, std::size_t index, int& value) const noexcept;
			bool get(const char* key, std::size_t type, std::size_t index, float& value) const noexcept;
			bool get(const char* key, std::size_t type, std::size_t index, octoon::math::Vector3& value) const noexcept;
			bool get(const char* key, std::size_t type, std::size_t index, octoon::math::Vector4& value) const noexcept;
			bool get(const char* key, std::size_t type, std::size_t index, std::string& value) const noexcept;
			bool get(const char* key, std::size_t type, std::size_t index, MaterialParam** out) const noexcept;

		private:
			std::vector<MaterialParam*> _properties;
		};

		struct MeshSubset final
		{
			MeshSubset() noexcept;
			MeshSubset(std::uint32_t startVertices, std::uint32_t startIndices, std::uint32_t _indicesCount, std::uint32_t _verticesOffset, std::uint32_t _indicesOffset) noexcept;
			
			std::uint32_t startVertices;
			std::uint32_t startIndices;

			std::uint32_t indicesCount;

			std::uint32_t offsetVertices;
			std::uint32_t offsetIndices;

			octoon::math::BoundingBox boundingBox;
		};

		typedef std::vector<MeshSubset> MeshSubsets;

		class MeshProperty final : public std::enable_shared_from_this<MeshProperty>
		{
		public:
			MeshProperty() noexcept;
			~MeshProperty() noexcept;

			void setName(const std::string& name) noexcept;
			const std::string& getName() const noexcept;

			void setVertexArray(const octoon::math::Float3Array& array) noexcept;
			void setNormalArray(const octoon::math::Float3Array& array) noexcept;
			void setColorArray(const octoon::math::Float4Array& array) noexcept;
			void setTangentArray(const octoon::math::Float4Array& array) noexcept;
			void setTexcoordArray(const octoon::math::Float2Array& array, std::uint8_t n = 0) noexcept;
			void setWeightArray(const VertexWeights& array) noexcept;
			void setIndicesArray(const octoon::math::Uint1Array& array) noexcept;
			void setBindposes(const octoon::math::Float4x4Array& array) noexcept;
			void setMeshSubsets(const MeshSubsets& subsets) noexcept;

			void setVertexArray(octoon::math::Float3Array&& array) noexcept;
			void setNormalArray(octoon::math::Float3Array&& array) noexcept;
			void setColorArray(octoon::math::Float4Array&& array) noexcept;
			void setTangentArray(octoon::math::Float4Array&& array) noexcept;
			void setTexcoordArray(octoon::math::Float2Array&& array, std::uint8_t n = 0) noexcept;
			void setWeightArray(VertexWeights&& array) noexcept;
			void setIndicesArray(octoon::math::Uint1Array&& array) noexcept;
			void setBindposes(octoon::math::Float4x4Array&& array) noexcept;
			void setMeshSubsets(MeshSubsets&& subsets) noexcept;

			octoon::math::Float3Array& getVertexArray() noexcept;
			octoon::math::Float3Array& getNormalArray() noexcept;
			octoon::math::Float4Array& getTangentArray() noexcept;
			octoon::math::Float4Array& getColorArray() noexcept;
			octoon::math::Float2Array& getTexcoordArray(std::uint8_t n = 0) noexcept;
			VertexWeights& getWeightArray() noexcept;
			octoon::math::Uint1Array& getIndicesArray() noexcept;
			octoon::math::Float4x4Array& getBindposes() noexcept;
			MeshSubsets& getMeshSubsets() noexcept;

			const octoon::math::Float3Array& getVertexArray() const noexcept;
			const octoon::math::Float3Array& getNormalArray() const noexcept;
			const octoon::math::Float4Array& getTangentArray() const noexcept;
			const octoon::math::Float4Array& getColorArray() const noexcept;
			const octoon::math::Float2Array& getTexcoordArray(std::uint8_t n = 0) const noexcept;
			const VertexWeights& getWeightArray() const noexcept;
			const octoon::math::Uint1Array& getIndicesArray() const noexcept;
			const MeshSubsets& getMeshSubsets() const noexcept;

			const Bones& getBoneArray(const Bones& array) const noexcept;
			const octoon::math::Float4x4Array& getBindposes() const noexcept;

			std::size_t getNumVertices() const noexcept;
			std::size_t getNumIndices() const noexcept;
			std::size_t getTexcoordNums() const noexcept;

			void makeCircle(float radius, std::uint32_t segments, float thetaStart = 0, float thetaLength = octoon::math::PI) noexcept;
			void makePlane(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1) noexcept;
			void makePlane(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir, bool clear = true) noexcept;
			void makePlaneWireframe(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir, bool clear = true) noexcept;
			void makeFloor(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1) noexcept;
			void makeNoise(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1) noexcept;
			void makeCube(float width, float height, float depth, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1, std::uint32_t depthSegments = 1) noexcept;
			void makeCubeWireframe(float width, float height, float depth, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1, std::uint32_t depthSegments = 1) noexcept;
			void makeRing(float innerRadius, float outerRadius, std::uint32_t thetaSegments, std::uint32_t phiSegments, float thetaStart = 0, float thetaLength = octoon::math::PI) noexcept;
			void makeSphere(float radius, std::uint32_t widthSegments = 8, std::uint32_t heightSegments = 6, float phiStart = 0.0, float phiLength = octoon::math::PI_2, float thetaStart = 0, float thetaLength = octoon::math::PI) noexcept;
			void makeVolumes(float fovy, float znear, float zfar) noexcept;
			void makeCone(float radius, float height, std::uint32_t segments = 32, float thetaStart = 0, float thetaLength = octoon::math::PI_2) noexcept;

			bool combineMeshes(const CombineMesh instances[], std::size_t numInstance, bool merge) noexcept;
			bool combineMeshes(const CombineMeshes& instances, bool merge) noexcept;

			void mergeVertices() noexcept;

			void computeFaceNormals(octoon::math::Float3Array& faceNormals) noexcept;
			void computeVertexNormals() noexcept;
			void computeVertexNormals(const octoon::math::Float3Array& faceNormals) noexcept;
			void computeVertexNormals(std::size_t width, std::size_t height) noexcept;
			void computeTangents(std::uint8_t texSlot = 0) noexcept;
			void computeTangentQuats(octoon::math::Float4Array& tangentQuat) const noexcept;
			void computeBoundingBox() noexcept;

			const octoon::math::BoundingBox& getBoundingBox() const noexcept;

			void clear() noexcept;
			MeshPropertyPtr clone() noexcept;

		private:
			std::string _name;

			octoon::math::Float3Array _vertices;
			octoon::math::Float3Array _normals;
			octoon::math::Float4Array _colors;
			octoon::math::Float2Array _texcoords[TEXTURE_ARRAY_COUNT];
			octoon::math::Float4Array _tangents;
			VertexWeights _weights;
			octoon::math::Float4x4Array _bindposes;

			octoon::math::Uint1Array _indices;

			Bones _bones;
			octoon::math::BoundingBox _boundingBox;

			MeshSubsets _meshSubsets;
		};
	}
}
#endif // !OCTOON_PROPERTY_H_
