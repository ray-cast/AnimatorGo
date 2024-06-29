#ifndef OCTOON_PMX_H_
#define OCTOON_PMX_H_

#include <octoon/math/math.h>
#include <octoon/io/iostream.h>
#include <octoon/runtime/object.h>
#include <codecvt>
#include <filesystem>

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
#define PMX_BONE_FIXED_AXIS   1 << 10
#define PMX_BONE_LOCAL_AXIS   1 << 11
#define PMX_BONE_PHYSICS_TRANSFORM 1 << 12
#define PMX_BONE_EXTERNAL_PARENT_TRANSFORM 1 << 13

namespace octoon
{
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
	struct PmxVector2
	{
		float x;
		float y;

		void operator=(const math::float2& v)
		{
			x = v.x;
			y = v.y;
		}
	};

	struct PmxVector3
	{
		float x;
		float y;
		float z;

		void operator=(const math::float3& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
		}
	};

	struct PmxVector4
	{
		float x;
		float y;
		float z;
		float w;

		void operator=(const math::float4& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			w = v.w;
		}
	};

	struct PmxQuaternion
	{
		float x;
		float y;
		float z;
		float w;

		void operator=(const math::Quaternion& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			w = v.w;
		}
	};

	struct PmxHeader
	{
		PmxUInt8 magic[3];       // ʼ��ΪPMX
		PmxUInt8 offset;         // ʼ��Ϊ 0x20
		PmxFloat version;        // �汾
		PmxUInt8 dataSize;       // ʼ�� 0x08
		PmxUInt8 encode;         // 0x00 UTF-16(LE) 0x01 UTF-8
		PmxUInt8 addUVCount;     // ׷�ӵ�UV ( 1 ~ 4 )
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

	typedef PmxUInt8 PmxIndex; //��������

	struct PmxName
	{
		PmxUInt32 length;
		PmxChar name[MAX_PATH];

		PmxName()
		{
			this->length = 0;
			std::memset(name, 0, MAX_PATH);
		}

		PmxName(const std::string& name)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> cv;
			std::wstring utf16 = cv.from_bytes(name);

			this->length = (PmxUInt32)(utf16.size() * 2);
			std::memset(this->name, 0, MAX_PATH);
			std::memcpy(this->name, utf16.data(), this->length);
		}
	};

	struct PmxMaterial
	{
		PmxName    name;
		PmxName    nameEng;
		PmxVector3 Diffuse;
		PmxFloat   Opacity;
		PmxVector3 Specular;
		PmxFloat   Shininess;
		PmxVector3 Ambient;
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
		PmxUInt16  Visable;
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
		PmxUInt32  ExternalParent;
		std::vector<PmxIK> IKList;
	};

	enum PmxMorphType : std::uint8_t
	{
		PMX_MorphTypeGroup = 0,
		PMX_MorphTypeVertex = 1,
		PMX_MorphTypeBone = 2,
		PMX_MorphTypeUV = 3,
		PMX_MorphTypeExtraUV1 = 4,
		PMX_MorphTypeExtraUV2 = 5,
		PMX_MorphTypeExtraUV3 = 6,
		PMX_MorphTypeExtraUV4 = 7,
		PMX_MorphTypeMaterial = 8
	};

	enum PmxShapeType : std::uint8_t
	{
		ShapeTypeSphere,
		ShapeTypeSquare,
		ShapeTypeCapsule
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
		PmxQuaternion rotation;
	};

	struct PmxMorphMaterial
	{
		PmxUInt32 index;
		PmxUInt8  offset;
		PmxVector4 diffuse;
		PmxFloat  shininess;
		PmxVector3 specular;
		PmxVector3 ambient;
		PmxVector4 edgeColor;
		PmxFloat  edgeSize;
		PmxVector4 tex;
		PmxVector4 sphere;
		PmxVector4 toon;
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

		std::vector<PmxMorphGroup>    groupList;
		std::vector<PmxMorphVertex>   vertices;
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

	struct PmxSoftbody
	{
		PmxName name;
		PmxName nameEng;

		PmxUInt8 type; // 0 : TriMesh, 1 : Rope

		std::int16_t materialIndex;

		PmxUInt8  group;
		PmxUInt16 groupMask;

		enum class SoftbodyMask : uint8_t
		{
			BLink = 0x01,
			Cluster = 0x02,
			HybridLink = 0x04,
		};
		SoftbodyMask	flag;

		std::int32_t blinkLength;
		std::int32_t numClusters;

		float totalMass;
		float collisionMargin;

		enum class AeroModel : int32_t
		{
			kAeroModelV_TwoSided,
			kAeroModelV_OneSided,
			kAeroModelF_TwoSided,
			kAeroModelF_OneSided,
		};
		std::int32_t aeroModel;

		float VCF;
		float DP;
		float DG;
		float LF;
		float PR;
		float VC;
		float DF;
		float MT;
		float CHR;
		float KHR;
		float SHR;
		float AHR;

		float SRHR_CL;
		float SKHR_CL;
		float SSHR_CL;
		float SR_SPLT_CL;
		float SK_SPLT_CL;
		float SS_SPLT_CL;

		int32_t	V_IT;
		int32_t	P_IT;
		int32_t	D_IT;
		int32_t	C_IT;

		float LST;
		float AST;
		float VST;

		struct AnchorRigidbody
		{
			int16_t	rigidBodyIndex;
			int16_t	vertexIndex;
			uint8_t	nearMode; //0:FF 1:ON
		};

		std::uint32_t numRigidbody;
		std::uint32_t numIndices;

		std::vector<AnchorRigidbody> anchorRigidbodies;
		std::vector<std::uint8_t> pinVertexIndices;
	};

	class OCTOON_EXPORT PMX final : public Object
	{
		OctoonDeclareSubClass(PMX, Object)
	public:
		PmxHeader header;
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
		PmxUInt32 numSoftbodies;

		std::vector<PmxVertex> vertices;
		std::vector<PmxIndex> indices;
		std::vector<PmxName> textures;
		std::vector<PmxMaterial> materials;
		std::vector<PmxBone> bones;
		std::vector<PmxMorph> morphs;
		std::vector<PmxDisplayFrame> displayFrames;
		std::vector<PmxRigidbody> rigidbodies;
		std::vector<PmxJoint> joints;
		std::vector<PmxSoftbody> softbodies;

		static bool canRead(std::istream& stream) noexcept;
		static bool canRead(std::string_view type) noexcept;
		static bool canRead(const char* type) noexcept;

		static bool load(const std::filesystem::path& filepath, PMX& pmx) noexcept;
		static bool save(std::ostream& stream, const PMX& pmx) noexcept;
	};
}

#endif