#ifndef OCTOON_PMX_LOADER_H_
#define OCTOON_PMX_LOADER_H_

#include <octoon/model/model_loader.h>

#ifndef MAX_PATH
#	define MAX_PATH 256
#endif

#define PMX_VERSION_2_0   2.0
#define PMX_BONE_INDEX    1 << 0
#define PMX_BONE_ROTATION 1 << 1
#define PMX_BONE_MOVE     1 << 2
#define PMX_BONE_DISPLAY  1 << 3
#define PMX_BONE_OPERATOR 1 << 4
#define PMX_BONE_IK       1 << 5
#define PMX_BONE_ADD_LOCAL    1 << 7
#define PMX_BONE_ADD_ROTATION 1 << 8
#define PMX_BONE_ADD_MOVE     1 << 9
#define PMX_BONE_PARENT       1 << 8
#define PMX_BONE_FIXED_AXIS   1 << 10
#define PMX_BONE_LOCAL_AXIS   1 << 11
#define PMX_BONE_PHYSICS_TRANSFORM 1 << 12
#define PMX_BONE_EXTERNAL_PARENT_TRANSFORM 1 << 13

namespace octoon
{
	namespace model
	{
		typedef math::Vector2  PmxVector2;
		typedef math::Vector3  PmxVector3;
		typedef math::Vector4  PmxVector4;

		typedef math::Vector3  PmxColor3;
		typedef math::Vector4  PmxColor4;
		typedef math::float3x3 PmxFloat3x3;
		typedef math::float4x4 PmxFloat4x4;

		typedef wchar_t                PmxChar;
		typedef std::int8_t            PmxInt8;
		typedef std::uint8_t           PmxUInt8;
		typedef std::uint16_t          PmxUInt16;
		typedef std::uint32_t          PmxUInt32;

		typedef float                  PmxFloat;

		enum PmxVertexSkinningType : std::uint8_t
		{
			PMX_BDEF1 = 0,
			PMX_BDEF2 = 1,
			PMX_BDEF4 = 2,
			PMX_SDEF = 3,
			PMX_QDEF = 4
		};

#pragma pack(push)
#pragma pack(push,1)

		struct PmxHeader
		{
			PmxUInt8 magic[3];       // 始终为PMX
			PmxUInt8 offset;         // 始终为 0x20
			PmxFloat version;        // 版本
			PmxUInt8 dataSize;       // 始终 0x08
			PmxUInt8 encode;         // 0x00 UTF-16(LE) 0x01 UTF-8
			PmxUInt8 addUVCount;     // 追加的UV ( 1 ~ 4 )
			PmxUInt8 sizeOfIndices;  // ( 1 or 2 or 4 )
			PmxUInt8 sizeOfTexture;  // ( 1 or 2 or 4 )
			PmxUInt8 sizeOfMaterial; // ( 1 or 2 or 4 )
			PmxUInt8 sizeOfBone;     // ( 1 or 2 or 4 )
			PmxUInt8 sizeOfMorph;    // ( 1 or 2 or 4 )
			PmxUInt8 sizeOfBody;     // ( 1 or 2 or 4 )
		};

#pragma pack(pop)

		struct PmxDescription
		{
			PmxUInt32 japanModelLength;
			PmxUInt32 japanCommentLength;

			PmxUInt32 englishModelLength;
			PmxUInt32 englishCommentLength;

			std::vector<PmxChar> japanModelName;
			std::vector<PmxChar> japanCommentName;

			std::vector<PmxChar> englishModelName;
			std::vector<PmxChar> englishCommentName;
		};

		struct PmxBoneWeight
		{
			PmxUInt16 bone1;
			PmxUInt16 bone2;
			PmxUInt16 bone3;
			PmxUInt16 bone4;

			PmxFloat weight1;
			PmxFloat weight2;
			PmxFloat weight3;
			PmxFloat weight4;

			PmxVector3 SDEF_C;
			PmxVector3 SDEF_R0;
			PmxVector3 SDEF_R1;
		};

		struct PmxVertex
		{
			PmxVector3 position;
			PmxVector3 normal;
			PmxVector2 coord;
			PmxVector4 addCoord[4];
			PmxVertexSkinningType type;
			PmxBoneWeight weight;
			PmxFloat   edge;
		};

		typedef PmxUInt8 PmxIndex; //顶点索引

		struct PmxName
		{
			PmxUInt32 length;
			PmxChar name[MAX_PATH];
		};

		struct PmxMaterial
		{
			PmxName    name;
			PmxName    nameEng;
			PmxColor3  Diffuse;
			PmxFloat   Opacity;
			PmxColor3  Specular;
			PmxFloat   Shininess;
			PmxColor3  Ambient;
			PmxUInt8   Flag;
			PmxVector4 EdgeColor;
			PmxFloat   EdgeSize;
			PmxUInt16  TextureIndex;
			PmxUInt16  SphereTextureIndex;
			PmxUInt8   SphereMode;
			PmxUInt8   ToonIndex;
			PmxUInt16  ToonTexture;
			PmxUInt32  memLength;
			PmxChar	   mem[MAX_PATH];
			PmxUInt32  FaceCount;
		};

		struct PmxIK
		{
			PmxUInt16  BoneIndex;
			PmxUInt8   rotateLimited;
			PmxVector3 minimumRadian;
			PmxVector3 maximumRadian;
		};

		struct PmxBone
		{
			PmxName    name;
			PmxName    nameEng;
			PmxVector3 position;
			PmxUInt16  Parent;
			PmxUInt32  Level;
			PmxUInt16  Flag;
			PmxUInt16  ConnectedBoneIndex;
			PmxUInt16  ProvidedParentBoneIndex;
			PmxFloat   ProvidedRatio;
			PmxVector3 Offset;
			PmxVector3 AxisDirection;
			PmxVector3 DimentionXDirection;
			PmxVector3 DimentionZDirection;
			PmxUInt16  IKTargetBoneIndex;
			PmxUInt32  IKLoopCount;
			PmxFloat   IKLimitedRadian;
			PmxUInt32  IKLinkCount;
			std::vector<PmxIK> IKList;
		};

		enum PmxMorphType : std::uint8_t
		{
			MorphTypeGroup = 0,
			MorphTypeVertex = 1,
			MorphTypeBone = 2,
			MorphTypeUV = 3,
			MorphTypeExtraUV1 = 4,
			MorphTypeExtraUV2 = 5,
			MorphTypeExtraUV3 = 6,
			MorphTypeExtraUV4 = 7,
			MorphTypeMaterial = 8
		};

		struct PmxMorphVertex
		{
			PmxUInt32  index;
			PmxVector3 offset;
		};

		struct PmxMorphTexcoord
		{
			PmxUInt32  index;
			PmxVector4 offset;
		};

		struct PmxMorphBone
		{
			PmxUInt32  boneIndex;
			PmxVector3 position;
			PmxVector3 rotate;
		};

		struct PmxMorphMaterial
		{
			PmxUInt32 index;
			PmxUInt8  offset;
			PmxColor4 diffuse;
			PmxFloat  shininess;
			PmxColor3 specular;
			PmxColor3 ambient;
			PmxColor4 edgeColor;
			PmxFloat  edgeSize;
			PmxColor4 tex;
			PmxColor4 sphere;
			PmxColor4 toon;
		};

		struct PmxMorphGroup
		{
			PmxUInt16 morphIndex;
			PmxFloat  morphRate;
		};

		struct PmxMorph
		{
			PmxName name;
			PmxName nameEng;

			PmxUInt8  control;
			PmxUInt8  morphType;
			PmxUInt32 morphCount;

			std::vector<PmxMorphGroup>   groupList;
			std::vector<PmxMorphVertex>   vertexList;
			std::vector<PmxMorphBone>     boneList;
			std::vector<PmxMorphTexcoord> texcoordList;
			std::vector<PmxMorphMaterial> materialList;
		};

		struct PmxDisplayFrameElement
		{
			PmxUInt8  target; // 1 : morph 0 : bone
			PmxUInt32 index;
		};

		struct PmxDisplayFrame
		{
			PmxName   name;
			PmxName   nameEng;
			PmxUInt8  type;
			PmxUInt32 elementsWithinFrame;
			std::vector<PmxDisplayFrameElement> elements;
		};

		struct PmxRigidbody
		{
			PmxName name;
			PmxName nameEng;

			PmxUInt32 bone;

			PmxUInt8  group;
			PmxUInt16 groupMask;
			PmxUInt8  shape; // 0:Circle 1:Square 2:Capsule

			PmxVector3 scale;
			PmxVector3 position;
			PmxVector3 rotate;

			PmxFloat mass;
			PmxFloat movementDecay; // movement reduction
			PmxFloat rotationDecay; // rotation reduction
			PmxFloat elasticity;    // recoil
			PmxFloat friction;      // strength of friction

			PmxUInt8 physicsOperation; //0:Follow Bone (static), 1:Physics Calc. (dynamic), 2: Physics Calc. + Bone position matching
		};

		struct PmxJoint
		{
			PmxName name;
			PmxName nameEng;

			PmxUInt8 type; //0 : Spring 6DOF; in PMX 2.0 always set to 0 (included to give room for expansion)

			PmxUInt32 relatedRigidBodyIndexA; //-1 if irrelevant
			PmxUInt32 relatedRigidBodyIndexB;

			PmxVector3 position;
			PmxVector3 rotation;

			PmxVector3 movementLowerLimit;
			PmxVector3 movementUpperLimit;
			PmxVector3 rotationLowerLimit; //-> radian angle
			PmxVector3 rotationUpperLimit; //-> radian angle

			PmxVector3 springMovementConstant;
			PmxVector3 springRotationConstant;
		};

		struct PMX
		{
			PmxHeader      header;
			PmxDescription description;

			PmxUInt32 numVertices;
			PmxUInt32 numIndices;
			PmxUInt32 numTextures;
			PmxUInt32 numMaterials;
			PmxUInt32 numBones;
			PmxUInt32 numMorphs;
			PmxUInt32 numDisplayFrames;
			PmxUInt32 numRigidbodys;
			PmxUInt32 numJoints;

			std::vector<PmxVertex> vertices;
			std::vector<PmxIndex> indices;
			std::vector<PmxName> textures;
			std::vector<PmxMaterial> materials;
			std::vector<PmxBone> bones;
			std::vector<PmxMorph> morphs;
			std::vector<PmxDisplayFrame> displayFrames;
			std::vector<PmxRigidbody> rigidbodys;
			std::vector<PmxJoint> joints;
		};

		class PmxLoader : public ModelLoader
		{
		public:
			PmxLoader() = default;
			~PmxLoader() = default;

			bool doCanRead(istream& stream) const noexcept;
			bool doCanRead(const std::string& type) const noexcept;
			bool doCanRead(const char* type) const noexcept;

			bool doLoad(istream& stream, PMX& pmx) noexcept;
			bool doLoad(istream& stream, Model& model) noexcept;

			bool doSave(ostream& stream, const PMX& pmx) noexcept;
			bool doSave(ostream& stream, const Model& model) noexcept;
		};
	}
}

#endif