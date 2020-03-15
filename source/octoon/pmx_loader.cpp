#include <octoon/pmx_loader.h>
#include <octoon/model/modtypes.h>
#include <octoon/mesh/mesh.h>
#include <octoon/material/mesh_basic_material.h>
#include <octoon/model/model.h>
#include <octoon/texture_loader.h>
#include <octoon/io/fstream.h>
#include <octoon/math/mathfwd.h>
#include <octoon/math/mathutil.h>
#include <octoon/runtime/string.h>

#include <map>
#include <cstring>
#include <codecvt>

using namespace octoon::io;
using namespace octoon::math;

namespace octoon
{
	bool PmxLoader::doCanRead(io::istream& stream) const noexcept
	{
		PmxHeader header;
		if (stream.read((char*)&header, sizeof(header)))
		{
			if ((header.magic[0] == 'p' || header.magic[0] == 'P') &&
				(header.magic[1] == 'm' || header.magic[1] == 'M') &&
				(header.magic[2] == 'x' || header.magic[2] == 'X'))
			{
				if (header.version >= 2.0 || header.version <= 2.1)
					return true;
			}
		}
		return false;
	}

	bool PmxLoader::doCanRead(std::string_view type) const noexcept
	{
		return type.compare("pmx") == 0;
	}

	bool PmxLoader::doCanRead(const char* type) const noexcept
	{
		return std::strncmp(type, "pmx", 3) == 0;
	}

	bool PmxLoader::doLoad(std::string_view filepath, PMX& pmx) noexcept
	{
		io::ifstream stream;
		if (!stream.open(std::string(filepath))) return false;

		if (!stream.read((char*)&pmx.header, sizeof(pmx.header))) return false;
		if (!stream.read((char*)&pmx.description.japanModelLength, sizeof(pmx.description.japanModelLength))) return false;

		if (pmx.description.japanModelLength > 0)
		{
			pmx.description.japanModelName.resize(pmx.description.japanModelLength);

			if (!stream.read((char*)&pmx.description.japanModelName[0], pmx.description.japanModelLength)) return false;
		}

		if (!stream.read((char*)&pmx.description.englishModelLength, sizeof(pmx.description.englishModelLength))) return false;

		if (pmx.description.englishModelLength > 0)
		{
			pmx.description.englishModelName.resize(pmx.description.englishModelLength);

			if (!stream.read((char*)&pmx.description.englishModelName[0], pmx.description.englishModelLength)) return false;
		}

		if (!stream.read((char*)&pmx.description.japanCommentLength, sizeof(pmx.description.japanCommentLength))) return false;

		if (pmx.description.japanCommentLength > 0)
		{
			pmx.description.japanCommentName.resize(pmx.description.japanCommentLength);

			if (!stream.read((char*)&pmx.description.japanCommentName[0], pmx.description.japanCommentLength)) return false;
		}

		if (!stream.read((char*)&pmx.description.englishCommentLength, sizeof(pmx.description.englishCommentLength))) return false;

		if (pmx.description.englishCommentLength > 0)
		{
			pmx.description.englishCommentName.resize(pmx.description.englishCommentLength);

			if (!stream.read((char*)&pmx.description.englishCommentName[0], pmx.description.englishCommentLength)) return false;
		}

		if (!stream.read((char*)&pmx.numVertices, sizeof(pmx.numVertices))) return false;

		if (pmx.numVertices > 0)
		{
			pmx.vertices.resize(pmx.numVertices);

			for (auto& vertex : pmx.vertices)
			{
				if (pmx.header.addUVCount == 0)
				{
					std::streamsize size = sizeof(vertex.position) + sizeof(vertex.normal) + sizeof(vertex.coord);
					if (!stream.read((char*)&vertex.position, size)) return false;
				}
				else
				{
					std::streamsize size = sizeof(vertex.position) + sizeof(vertex.normal) + sizeof(vertex.coord) + sizeof(vertex.addCoord[0]) * pmx.header.addUVCount;
					if (!stream.read((char*)&vertex.position, size)) return false;
				}

				if (!stream.read((char*)&vertex.type, sizeof(vertex.type))) return false;
				switch (vertex.type)
				{
					case PMX_BDEF1:
					{
						if (!stream.read((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
						vertex.weight.weight1 = 1.0f;
					}
					break;
					case PMX_BDEF2:
					{
						if (!stream.read((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
						if (!stream.read((char*)&vertex.weight.bone2, pmx.header.sizeOfBone)) return false;
						if (!stream.read((char*)&vertex.weight.weight1, sizeof(vertex.weight.weight2))) return false;
						vertex.weight.weight2 = 1.0f - vertex.weight.weight1;
					}
					break;
					case PMX_BDEF4:
					{
						if (!stream.read((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
						if (!stream.read((char*)&vertex.weight.bone2, pmx.header.sizeOfBone)) return false;
						if (!stream.read((char*)&vertex.weight.bone3, pmx.header.sizeOfBone)) return false;
						if (!stream.read((char*)&vertex.weight.bone4, pmx.header.sizeOfBone)) return false;
						if (!stream.read((char*)&vertex.weight.weight1, sizeof(vertex.weight.weight1))) return false;
						if (!stream.read((char*)&vertex.weight.weight2, sizeof(vertex.weight.weight2))) return false;
						if (!stream.read((char*)&vertex.weight.weight3, sizeof(vertex.weight.weight3))) return false;
						if (!stream.read((char*)&vertex.weight.weight4, sizeof(vertex.weight.weight4))) return false;
					}
					break;
					case PMX_SDEF:
					{
						if (!stream.read((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
						if (!stream.read((char*)&vertex.weight.bone2, pmx.header.sizeOfBone)) return false;
						if (!stream.read((char*)&vertex.weight.weight1, sizeof(vertex.weight.weight1))) return false;
						if (!stream.read((char*)&vertex.weight.SDEF_C, sizeof(vertex.weight.SDEF_C))) return false;
						if (!stream.read((char*)&vertex.weight.SDEF_R0, sizeof(vertex.weight.SDEF_R0))) return false;
						if (!stream.read((char*)&vertex.weight.SDEF_R1, sizeof(vertex.weight.SDEF_R1))) return false;

						vertex.weight.weight2 = 1.0f - vertex.weight.weight1;
					}
					break;
					case PMX_QDEF:
					{
						if (!stream.read((char*)& vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
						if (!stream.read((char*)& vertex.weight.bone2, pmx.header.sizeOfBone)) return false;
						if (!stream.read((char*)& vertex.weight.bone3, pmx.header.sizeOfBone)) return false;
						if (!stream.read((char*)& vertex.weight.bone4, pmx.header.sizeOfBone)) return false;
						if (!stream.read((char*)& vertex.weight.weight1, sizeof(vertex.weight.weight1))) return false;
						if (!stream.read((char*)& vertex.weight.weight2, sizeof(vertex.weight.weight2))) return false;
						if (!stream.read((char*)& vertex.weight.weight3, sizeof(vertex.weight.weight3))) return false;
						if (!stream.read((char*)& vertex.weight.weight4, sizeof(vertex.weight.weight4))) return false;
					}
					default:
						return false;
				}

				if (!stream.read((char*)&vertex.edge, sizeof(vertex.edge))) return false;
			}
		}

		if (!stream.read((char*)&pmx.numIndices, sizeof(pmx.numIndices))) return false;

		if (pmx.numIndices > 0)
		{
			pmx.indices.resize(pmx.numIndices * pmx.header.sizeOfIndices);
			if (!stream.read((char*)pmx.indices.data(), pmx.indices.size())) return false;
		}

		if (!stream.read((char*)&pmx.numTextures, sizeof(pmx.numTextures))) return false;

		if (pmx.numTextures > 0)
		{
			pmx.textures.resize(pmx.numTextures);

			for (auto& texture : pmx.textures)
			{
				if (!stream.read((char*)&texture.length, sizeof(texture.length))) return false;
				if (!stream.read((char*)&texture.name, texture.length)) return false;
			}
		}

		if (!stream.read((char*)&pmx.numMaterials, sizeof(pmx.numMaterials))) return false;

		if (pmx.numMaterials > 0)
		{
			pmx.materials.resize(pmx.numMaterials);

			for (auto& material : pmx.materials)
			{
				if (!stream.read((char*)&material.name.length, sizeof(material.name.length))) return false;
				if (!stream.read((char*)&material.name.name, material.name.length)) return false;
				if (!stream.read((char*)&material.nameEng.length, sizeof(material.nameEng.length))) return false;
				if (!stream.read((char*)&material.nameEng.name, material.nameEng.length)) return false;
				if (!stream.read((char*)&material.Diffuse, sizeof(material.Diffuse))) return false;
				if (!stream.read((char*)&material.Opacity, sizeof(material.Opacity))) return false;
				if (!stream.read((char*)&material.Specular, sizeof(material.Specular))) return false;
				if (!stream.read((char*)&material.Shininess, sizeof(material.Shininess))) return false;
				if (!stream.read((char*)&material.Ambient, sizeof(material.Ambient))) return false;
				if (!stream.read((char*)&material.Flag, sizeof(material.Flag))) return false;
				if (!stream.read((char*)&material.EdgeColor, sizeof(material.EdgeColor))) return false;
				if (!stream.read((char*)&material.EdgeSize, sizeof(material.EdgeSize))) return false;
				if (!stream.read((char*)&material.TextureIndex, pmx.header.sizeOfTexture)) return false;
				if (!stream.read((char*)&material.SphereTextureIndex, pmx.header.sizeOfTexture)) return false;
				if (!stream.read((char*)&material.SphereMode, sizeof(material.SphereMode))) return false;
				if (!stream.read((char*)&material.ToonIndex, sizeof(material.ToonIndex))) return false;

				if (material.ToonIndex == 1)
				{
					if (!stream.read((char*)&material.ToonTexture, 1)) return false;
				}
				else
				{
					if (!stream.read((char*)&material.ToonTexture, pmx.header.sizeOfTexture)) return false;
				}

				if (!stream.read((char*)&material.memLength, sizeof(material.memLength))) return false;
				if (material.memLength > 0)
				{
					if (!stream.read((char*)&material.mem, material.memLength)) return false;
				}

				if (!stream.read((char*)&material.FaceCount, sizeof(material.FaceCount))) return false;
			}
		}

		if (!stream.read((char*)&pmx.numBones, sizeof(pmx.numBones))) return false;

		if (pmx.numBones > 0)
		{
			pmx.bones.resize(pmx.numBones);

			for (auto& bone : pmx.bones)
			{
				if (!stream.read((char*)&bone.name.length, sizeof(bone.name.length))) return false;
				if (!stream.read((char*)&bone.name.name, bone.name.length)) return false;
				if (!stream.read((char*)&bone.nameEng.length, sizeof(bone.nameEng.length))) return false;
				if (!stream.read((char*)&bone.nameEng.name, bone.nameEng.length)) return false;

				if (!stream.read((char*)&bone.position, sizeof(bone.position))) return false;
				if (!stream.read((char*)&bone.Parent, pmx.header.sizeOfBone)) return false;
				if (!stream.read((char*)&bone.Level, sizeof(bone.Level))) return false;
				if (!stream.read((char*)&bone.Flag, sizeof(bone.Flag))) return false;

				if (bone.Flag & PMX_BONE_DISPLAY)
					bone.Visable = true;
				else
					bone.Visable = false;

				if (bone.Flag & PMX_BONE_INDEX)
				{
					if (!stream.read((char*)&bone.ConnectedBoneIndex, pmx.header.sizeOfBone)) return false;
				}
				else
				{
					if (!stream.read((char*)&bone.Offset, sizeof(bone.Offset))) return false;
				}

				if ((bone.Flag & (PMX_BONE_ADD_ROTATION | PMX_BONE_ADD_MOVE)) != 0)
				{
					if (!stream.read((char*)&bone.ProvidedParentBoneIndex, pmx.header.sizeOfBone)) return false;
					if (!stream.read((char*)&bone.ProvidedRatio, sizeof(bone.ProvidedRatio))) return false;
				}
				else
				{
					bone.ProvidedParentBoneIndex = std::numeric_limits<PmxUInt16>::max();
					bone.ProvidedRatio = 0;
				}

				if (bone.Flag & PMX_BONE_FIXED_AXIS)
				{
					if (!stream.read((char*)&bone.AxisDirection, sizeof(bone.AxisDirection))) return false;
				}

				if (bone.Flag & PMX_BONE_LOCAL_AXIS)
				{
					if (!stream.read((char*)&bone.DimentionXDirection, sizeof(bone.DimentionXDirection))) return false;
					if (!stream.read((char*)&bone.DimentionZDirection, sizeof(bone.DimentionZDirection))) return false;
				}

				if (bone.Flag & PMX_BONE_EXTERNAL_PARENT_TRANSFORM)
				{
					if (!stream.read((char*)& bone.ExternalParent, sizeof(bone.ExternalParent))) return false;
				}

				if (bone.Flag & PMX_BONE_IK)
				{
					if (!stream.read((char*)&bone.IKTargetBoneIndex, pmx.header.sizeOfBone)) return false;
					if (!stream.read((char*)&bone.IKLoopCount, sizeof(bone.IKLoopCount))) return false;
					if (!stream.read((char*)&bone.IKLimitedRadian, sizeof(bone.IKLimitedRadian))) return false;
					if (!stream.read((char*)&bone.IKLinkCount, sizeof(bone.IKLinkCount))) return false;

					if (bone.IKLinkCount > 0)
					{
						bone.IKList.resize(bone.IKLinkCount);

						for (auto& chain : bone.IKList)
						{
							if (!stream.read((char*)&chain.BoneIndex, pmx.header.sizeOfBone)) return false;
							if (!stream.read((char*)&chain.rotateLimited, (std::streamsize)sizeof(chain.rotateLimited))) return false;
							if (chain.rotateLimited)
							{
								if (!stream.read((char*)&chain.minimumRadian, (std::streamsize)sizeof(chain.minimumRadian))) return false;
								if (!stream.read((char*)&chain.maximumRadian, (std::streamsize)sizeof(chain.maximumRadian))) return false;
							}
						}
					}
				}
			}
		}

		if (!stream.read((char*)&pmx.numMorphs, sizeof(pmx.numMorphs))) return false;

		if (pmx.numMorphs > 0)
		{
			pmx.morphs.resize(pmx.numMorphs);

			for (auto& morph : pmx.morphs)
			{
				if (!stream.read((char*)&morph.name.length, sizeof(morph.name.length))) return false;
				if (!stream.read((char*)&morph.name.name, morph.name.length)) return false;
				if (!stream.read((char*)&morph.nameEng.length, sizeof(morph.nameEng.length))) return false;
				if (!stream.read((char*)&morph.nameEng.name, morph.nameEng.length)) return false;
				if (!stream.read((char*)&morph.control, sizeof(morph.control))) return false;
				if (!stream.read((char*)&morph.morphType, sizeof(morph.morphType))) return false;
				if (!stream.read((char*)&morph.morphCount, sizeof(morph.morphCount))) return false;

				if (morph.morphType == PmxMorphType::PMX_MorphTypeGroup)
				{
					morph.groupList.resize(morph.morphCount);

					for (auto& group : morph.groupList)
					{
						if (!stream.read((char*)& group.morphIndex, pmx.header.sizeOfMorph)) return false;
						if (!stream.read((char*)& group.morphRate, sizeof(group.morphRate))) return false;
					}
				}
				else if (morph.morphType == PmxMorphType::PMX_MorphTypeVertex)
				{
					morph.vertices.resize(morph.morphCount);

					for (auto& vertex : morph.vertices)
					{
						if (!stream.read((char*)&vertex.index, pmx.header.sizeOfIndices)) return false;
						if (!stream.read((char*)&vertex.offset, sizeof(vertex.offset))) return false;
					}
				}
				else if (morph.morphType == PmxMorphType::PMX_MorphTypeBone)
				{
					morph.boneList.resize(morph.morphCount);

					for (auto& bone : morph.boneList)
					{
						if (!stream.read((char*)&bone.boneIndex, pmx.header.sizeOfBone)) return false;
						if (!stream.read((char*)&bone.position, sizeof(bone.position))) return false;
						if (!stream.read((char*)&bone.rotation, sizeof(bone.rotation))) return false;
					}
				}
				else if (morph.morphType == PmxMorphType::PMX_MorphTypeUV || morph.morphType == PmxMorphType::PMX_MorphTypeExtraUV1 ||
							morph.morphType == PmxMorphType::PMX_MorphTypeExtraUV2 || morph.morphType == PmxMorphType::PMX_MorphTypeExtraUV3 ||
							morph.morphType == PmxMorphType::PMX_MorphTypeExtraUV4)
				{
					morph.texcoordList.resize(morph.morphCount);

					for (auto& texcoord : morph.texcoordList)
					{
						if (!stream.read((char*)&texcoord.index, pmx.header.sizeOfIndices)) return false;
						if (!stream.read((char*)&texcoord.offset, sizeof(texcoord.offset))) return false;
					}
				}
				else if (morph.morphType == PmxMorphType::PMX_MorphTypeMaterial)
				{
					morph.materialList.resize(morph.morphCount);

					for (auto& material : morph.materialList)
					{
						if (!stream.read((char*)&material.index, pmx.header.sizeOfMaterial)) return false;
						if (!stream.read((char*)&material.offset, sizeof(material.offset))) return false;
						if (!stream.read((char*)&material.diffuse, sizeof(material.diffuse))) return false;
						if (!stream.read((char*)&material.specular, sizeof(material.specular))) return false;
						if (!stream.read((char*)&material.shininess, sizeof(material.shininess))) return false;
						if (!stream.read((char*)&material.ambient, sizeof(material.ambient))) return false;
						if (!stream.read((char*)&material.edgeColor, sizeof(material.edgeColor))) return false;
						if (!stream.read((char*)&material.edgeSize, sizeof(material.edgeSize))) return false;
						if (!stream.read((char*)&material.tex, sizeof(material.tex))) return false;
						if (!stream.read((char*)&material.sphere, sizeof(material.sphere))) return false;
						if (!stream.read((char*)&material.toon, sizeof(material.toon))) return false;
					}
				}
			}
		}

		if (!stream.read((char*)&pmx.numDisplayFrames, sizeof(pmx.numDisplayFrames))) return false;

		if (pmx.numDisplayFrames > 0)
		{
			pmx.displayFrames.resize(pmx.numDisplayFrames);

			for (auto& displayFrame : pmx.displayFrames)
			{
				if (!stream.read((char*)&displayFrame.name.length, sizeof(displayFrame.name.length))) return false;
				if (!stream.read((char*)&displayFrame.name.name, displayFrame.name.length)) return false;
				if (!stream.read((char*)&displayFrame.nameEng.length, sizeof(displayFrame.nameEng.length))) return false;
				if (!stream.read((char*)&displayFrame.nameEng.name, displayFrame.nameEng.length)) return false;
				if (!stream.read((char*)&displayFrame.type, sizeof(displayFrame.type))) return false;
				if (!stream.read((char*)&displayFrame.elementsWithinFrame, sizeof(displayFrame.elementsWithinFrame))) return false;

				displayFrame.elements.resize(displayFrame.elementsWithinFrame);
				for (auto& element : displayFrame.elements)
				{
					if (!stream.read((char*)&element.target, sizeof(element.target))) return false;

					if (element.target == 0)
					{
						if (!stream.read((char*)&element.index, pmx.header.sizeOfBone))
							return false;
					}
					else if (element.target == 1)
					{
						if (!stream.read((char*)&element.index, pmx.header.sizeOfMorph))
							return false;
					}
				}
			}
		}

		if (!stream.read((char*)&pmx.numRigidbodys, sizeof(pmx.numRigidbodys))) return false;

		if (pmx.numRigidbodys > 0)
		{
			pmx.rigidbodies.resize(pmx.numRigidbodys);

			for (auto& rigidbody : pmx.rigidbodies)
			{
				if (!stream.read((char*)&rigidbody.name.length, sizeof(rigidbody.name.length))) return false;
				if (!stream.read((char*)&rigidbody.name.name, rigidbody.name.length)) return false;
				if (!stream.read((char*)&rigidbody.nameEng.length, sizeof(rigidbody.nameEng.length))) return false;
				if (!stream.read((char*)&rigidbody.nameEng.name, rigidbody.nameEng.length)) return false;

				if (!stream.read((char*)&rigidbody.bone, pmx.header.sizeOfBone)) return false;
				if (!stream.read((char*)&rigidbody.group, sizeof(rigidbody.group))) return false;
				if (!stream.read((char*)&rigidbody.groupMask, sizeof(rigidbody.groupMask))) return false;

				if (!stream.read((char*)&rigidbody.shape, sizeof(rigidbody.shape))) return false;

				if (!stream.read((char*)&rigidbody.scale, sizeof(rigidbody.scale))) return false;
				if (!stream.read((char*)&rigidbody.position, sizeof(rigidbody.position))) return false;
				if (!stream.read((char*)&rigidbody.rotate, sizeof(rigidbody.rotate))) return false;

				if (!stream.read((char*)&rigidbody.mass, sizeof(rigidbody.mass))) return false;
				if (!stream.read((char*)&rigidbody.movementDecay, sizeof(rigidbody.movementDecay))) return false;
				if (!stream.read((char*)&rigidbody.rotationDecay, sizeof(rigidbody.rotationDecay))) return false;
				if (!stream.read((char*)&rigidbody.elasticity, sizeof(rigidbody.elasticity))) return false;
				if (!stream.read((char*)&rigidbody.friction, sizeof(rigidbody.friction))) return false;
				if (!stream.read((char*)&rigidbody.physicsOperation, sizeof(rigidbody.physicsOperation))) return false;
			}
		}

		if (!stream.read((char*)&pmx.numJoints, sizeof(pmx.numJoints))) return false;

		if (pmx.numJoints > 0)
		{
			pmx.joints.resize(pmx.numJoints);

			for (auto& joint : pmx.joints)
			{
				if (!stream.read((char*)&joint.name.length, sizeof(joint.name.length))) return false;
				if (!stream.read((char*)&joint.name.name, joint.name.length)) return false;
				if (!stream.read((char*)&joint.nameEng.length, sizeof(joint.nameEng.length))) return false;
				if (!stream.read((char*)&joint.nameEng.name, joint.nameEng.length)) return false;

				if (!stream.read((char*)&joint.type, sizeof(joint.type))) return false;

				if (!stream.read((char*)&joint.relatedRigidBodyIndexA, pmx.header.sizeOfBody)) return false;
				if (!stream.read((char*)&joint.relatedRigidBodyIndexB, pmx.header.sizeOfBody)) return false;

				if (!stream.read((char*)&joint.position, sizeof(joint.position))) return false;
				if (!stream.read((char*)&joint.rotation, sizeof(joint.rotation))) return false;

				if (!stream.read((char*)&joint.movementLowerLimit, sizeof(joint.movementLowerLimit))) return false;
				if (!stream.read((char*)&joint.movementUpperLimit, sizeof(joint.movementUpperLimit))) return false;

				if (!stream.read((char*)&joint.rotationLowerLimit, sizeof(joint.rotationLowerLimit))) return false;
				if (!stream.read((char*)&joint.rotationUpperLimit, sizeof(joint.rotationUpperLimit))) return false;

				if (!stream.read((char*)&joint.springMovementConstant, sizeof(joint.springMovementConstant))) return false;
				if (!stream.read((char*)&joint.springRotationConstant, sizeof(joint.springRotationConstant))) return false;
			}
		}

		if (pmx.header.version > 2.0)
		{
			if (!stream.read((char*)& pmx.numSoftbodies, sizeof(pmx.numSoftbodies))) return false;

			if (pmx.numSoftbodies > 0)
			{
				pmx.softbodies.resize(pmx.numSoftbodies);

				for (auto& body : pmx.softbodies)
				{
					if (!stream.read((char*)& body.name.length, sizeof(body.name.length))) return false;
					if (!stream.read((char*)& body.name.name, body.name.length)) return false;
					if (!stream.read((char*)& body.nameEng.length, sizeof(body.nameEng.length))) return false;
					if (!stream.read((char*)& body.nameEng.name, body.nameEng.length)) return false;

					if (!stream.read((char*)& body.type, sizeof(body.type))) return false;

					if (!stream.read((char*)& body.materialIndex, pmx.header.sizeOfMaterial)) return false;

					if (!stream.read((char*)& body.group, sizeof(body.group))) return false;
					if (!stream.read((char*)& body.groupMask, sizeof(body.groupMask))) return false;

					if (!stream.read((char*)& body.flag, sizeof(body.flag))) return false;

					if (!stream.read((char*)& body.blinkLength, sizeof(body.blinkLength))) return false;
					if (!stream.read((char*)& body.numClusters, sizeof(body.numClusters))) return false;

					if (!stream.read((char*)& body.totalMass, sizeof(body.totalMass))) return false;
					if (!stream.read((char*)& body.collisionMargin, sizeof(body.collisionMargin))) return false;

					if (!stream.read((char*)& body.aeroModel, sizeof(body.aeroModel))) return false;

					if (!stream.read((char*)& body.VCF, sizeof(body.VCF))) return false;
					if (!stream.read((char*)& body.DP, sizeof(body.DP))) return false;
					if (!stream.read((char*)& body.DG, sizeof(body.DG))) return false;
					if (!stream.read((char*)& body.LF, sizeof(body.LF))) return false;
					if (!stream.read((char*)& body.PR, sizeof(body.PR))) return false;
					if (!stream.read((char*)& body.VC, sizeof(body.VC))) return false;
					if (!stream.read((char*)& body.DF, sizeof(body.DF))) return false;
					if (!stream.read((char*)& body.MT, sizeof(body.MT))) return false;
					if (!stream.read((char*)& body.CHR, sizeof(body.CHR))) return false;
					if (!stream.read((char*)& body.KHR, sizeof(body.KHR))) return false;
					if (!stream.read((char*)& body.SHR, sizeof(body.SHR))) return false;
					if (!stream.read((char*)& body.AHR, sizeof(body.AHR))) return false;

					if (!stream.read((char*)& body.SRHR_CL, sizeof(body.SRHR_CL))) return false;
					if (!stream.read((char*)& body.SKHR_CL, sizeof(body.SKHR_CL))) return false;
					if (!stream.read((char*)& body.SSHR_CL, sizeof(body.SSHR_CL))) return false;
					if (!stream.read((char*)& body.SR_SPLT_CL, sizeof(body.SR_SPLT_CL))) return false;
					if (!stream.read((char*)& body.SK_SPLT_CL, sizeof(body.SK_SPLT_CL))) return false;
					if (!stream.read((char*)& body.SS_SPLT_CL, sizeof(body.SS_SPLT_CL))) return false;

					if (!stream.read((char*)& body.V_IT, sizeof(body.V_IT))) return false;
					if (!stream.read((char*)& body.P_IT, sizeof(body.P_IT))) return false;
					if (!stream.read((char*)& body.D_IT, sizeof(body.D_IT))) return false;
					if (!stream.read((char*)& body.C_IT, sizeof(body.C_IT))) return false;

					if (!stream.read((char*)& body.LST, sizeof(body.LST))) return false;
					if (!stream.read((char*)& body.AST, sizeof(body.AST))) return false;
					if (!stream.read((char*)& body.VST, sizeof(body.VST))) return false;

					if (!stream.read((char*)& body.numRigidbody, sizeof(body.numRigidbody))) return false;
					if (body.numRigidbody > 0)
					{
						body.anchorRigidbodies.resize(body.numRigidbody);

						for (auto& ar : body.anchorRigidbodies)
						{
							if (!stream.read((char*)& ar.rigidBodyIndex, pmx.header.sizeOfBody)) return false;
							if (!stream.read((char*)& ar.vertexIndex, sizeof(ar.vertexIndex))) return false;
							if (!stream.read((char*)& ar.nearMode, sizeof(ar.nearMode))) return false;
						}
					}

					if (!stream.read((char*)& body.numIndices, sizeof(body.numIndices))) return false;
					if (body.numIndices > 0)
					{
						body.pinVertexIndices.resize(body.numIndices * pmx.header.sizeOfIndices);
						if (!stream.read((char*) body.pinVertexIndices.data(), body.numIndices * pmx.header.sizeOfIndices)) return false;
					}
				}
			}
		}

		return true;
	}

	bool PmxLoader::doLoad(std::string_view filepath, model::Model& model) noexcept
	{
		PMX pmx;
		if (!this->doLoad(filepath, pmx))
			return false;

		auto rootPath = runtime::string::directory(std::string(filepath));

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> cv;

		for (auto& it : pmx.materials)
		{
			auto material = std::make_shared<material::MeshBasicMaterial>();
			material->setName(cv.to_bytes(it.name.name));
			material->setColor(math::srgb2linear(it.Diffuse));
			material->setOpacity(it.Opacity);

			std::uint32_t limits = 0;
			if (pmx.header.sizeOfTexture == 1)
				limits = std::numeric_limits<PmxUInt8>::max();
			else if (pmx.header.sizeOfTexture == 2)
				limits = std::numeric_limits<PmxUInt16>::max();
			else if (pmx.header.sizeOfTexture == 4)
				limits = std::numeric_limits<PmxUInt32>::max();

			if (it.TextureIndex < limits)
			{
				std::string u8_conv = cv.to_bytes(pmx.textures[it.TextureIndex].name);
				material->setColorTexture(TextureLoader::load(rootPath + "/" + u8_conv));
			}

			bool hasAlphaTexture = it.TextureIndex < limits ? std::wstring_view(pmx.textures[it.TextureIndex].name).find(L".png") != std::string::npos : false;
			if (it.Opacity < 1.0 || hasAlphaTexture) {
				hal::GraphicsColorBlend blend;
				blend.setBlendEnable(true);
				blend.setBlendSrc(hal::GraphicsBlendFactor::SrcAlpha);
				blend.setBlendDest(hal::GraphicsBlendFactor::OneMinusSrcAlpha);

				std::vector<hal::GraphicsColorBlend> blends;
				blends.push_back(blend);

				material->setColorBlends(std::move(blends));
			}

			model.materials.emplace_back(std::move(material));
		}

		math::float4x4s bindposes(pmx.bones.size());
		for (std::size_t i = 0; i < pmx.bones.size(); i++)
			bindposes[i].makeTranslate(-pmx.bones[i].position);

		float3s vertices_;
		float3s normals_;
		float2s texcoords_;
		skelecton::VertexWeights weights;

		vertices_.resize(pmx.numVertices);
		normals_.resize(pmx.numVertices);
		texcoords_.resize(pmx.numVertices);

		if (pmx.numBones)
			weights.resize(pmx.numVertices);

		for (std::size_t i = 0; i < pmx.numVertices; i++)
		{
			auto& v = pmx.vertices[i];

			vertices_[i] = v.position;
			normals_[i] = v.normal;
			texcoords_[i] = v.coord;

			if (pmx.numBones)
			{
				skelecton::VertexWeight weight;
				weight.weight1 = v.weight.weight1;
				weight.weight2 = v.weight.weight2;
				weight.weight3 = v.weight.weight3;
				weight.weight4 = v.weight.weight4;
				weight.bone1 = v.weight.bone1 < pmx.numBones ? v.weight.bone1 : 0;
				weight.bone2 = v.weight.bone2 < pmx.numBones ? v.weight.bone2 : 0;
				weight.bone3 = v.weight.bone3 < pmx.numBones ? v.weight.bone3 : 0;
				weight.bone4 = v.weight.bone4 < pmx.numBones ? v.weight.bone4 : 0;

				weights[i] = weight;
			}
		}

		auto mesh = std::make_shared<mesh::Mesh>();
		mesh->setBindposes(std::move(bindposes));
		mesh->setVertexArray(std::move(vertices_));
		mesh->setNormalArray(std::move(normals_));
		mesh->setTexcoordArray(std::move(texcoords_));
		mesh->setWeightArray(std::move(weights));

		PmxUInt32 startIndices = 0;

		for (std::size_t i = 0; i < pmx.materials.size(); i++)
		{
			uint1s indices_(pmx.materials[i].FaceCount);

			for (std::size_t j = startIndices; j < startIndices + pmx.materials[i].FaceCount; j++)
			{
				std::uint32_t index = 0;
				if (pmx.header.sizeOfIndices == 1)
					index = *((std::uint8_t*)pmx.indices.data() + j);
				else if (pmx.header.sizeOfIndices == 2)
					index = *((std::uint16_t*)pmx.indices.data() + j);
				else if (pmx.header.sizeOfIndices == 4)
					index = *((std::uint32_t*)pmx.indices.data() + j);

				indices_[j - startIndices] = index;
			}

			mesh->setIndicesArray(std::move(indices_), i);

			startIndices += pmx.materials[i].FaceCount;
		}

		model.meshes.emplace_back(std::move(mesh));

		for (std::size_t i = 0; i < pmx.bones.size(); i++)
		{
			auto& it = pmx.bones[i];

			skelecton::Bone bone;
			bone.setName(cv.to_bytes(it.name.name));
			bone.setPosition(it.position);
			bone.setParent(it.Parent);
			bone.setVisable(it.Visable);
			bone.setAdditiveParent(it.ProvidedParentBoneIndex);
			bone.setAdditiveUseLocal(!(it.Flag & PMX_BONE_ADD_LOCAL));

			if (it.Flag & PMX_BONE_ADD_MOVE)
				bone.setAdditiveMoveRatio(it.ProvidedRatio);
			if (it.Flag & PMX_BONE_ADD_ROTATION)
				bone.setAdditiveRotationRatio(it.ProvidedRatio);

			model.bones.emplace_back(std::make_shared<skelecton::Bone>(bone));

			if (it.Flag & PMX_BONE_IK)
			{
				model::IKAttr attr;
				attr.boneIndex = static_cast<uint16_t>(i);
				attr.targetBoneIndex = it.IKTargetBoneIndex;
				attr.chainLength = it.IKLinkCount;
				attr.iterations = it.IKLoopCount;

				for (auto& ik : it.IKList)
				{
					model::IKChild child;
					child.boneIndex = ik.BoneIndex;
					child.angleRadian = it.IKLimitedRadian;
					child.minimumRadian = ik.minimumRadian;
					child.maximumRadian = ik.maximumRadian;
					child.rotateLimited = ik.rotateLimited;

					attr.child.push_back(child);
				}

				model.iks.emplace_back(std::make_shared<model::IKAttr>(attr));
			}
		}

		for (auto& it : pmx.morphs)
		{
			switch (it.morphType)
			{
			case PmxMorphType::PMX_MorphTypeVertex:
			{
				auto morph = std::make_shared<model::Morph>();
				morph->name = cv.to_bytes(it.name.name);
				morph->morphType = it.morphType;
				morph->control = it.control;
				morph->morphCount = it.morphCount;
				morph->vertices.reserve(it.vertices.size());

				for (auto& v : it.vertices)
				{
					model::MorphVertex vertex;
					vertex.index = v.index;
					vertex.offset = v.offset;
					morph->vertices.push_back(vertex);
				}

				model.morphs.emplace_back(morph);
			}
			break;
			}
		}

		for (auto& it : pmx.rigidbodies)
		{
			auto body = std::make_shared<model::Rigidbody>();
			body->name = cv.to_bytes(it.name.name);
			body->bone = it.bone;
			body->group = it.group;
			body->groupMask = it.groupMask;
			body->shape = (model::ShapeType)it.shape;
			body->scale = it.scale;
			body->position = it.position;
			body->rotation = it.rotate;
			body->mass = it.mass;
			body->movementDecay = it.movementDecay;
			body->rotationDecay = it.rotationDecay;
			body->elasticity = it.elasticity;
			body->friction = it.friction;
			body->physicsOperation = it.physicsOperation;

			if (body->shape == model::ShapeType::ShapeTypeCapsule && body->scale.y == 0.0f)
				body->shape = model::ShapeType::ShapeTypeSphere;

			model.rigidbodies.emplace_back(std::move(body));
		}

		for (auto& it : pmx.joints)
		{
			auto joint = std::make_shared<model::Joint>();
			joint->name = cv.to_bytes(it.name.name);
			joint->type = it.type;
			joint->bodyIndexA = it.relatedRigidBodyIndexA;
			joint->bodyIndexB = it.relatedRigidBodyIndexB;
			joint->position = it.position;
			joint->rotation = it.rotation;
			joint->movementLowerLimit = it.movementLowerLimit;
			joint->movementUpperLimit = it.movementUpperLimit;
			joint->rotationLowerLimit = it.rotationLowerLimit;
			joint->rotationUpperLimit = it.rotationUpperLimit;
			joint->springMovementConstant = it.springMovementConstant;
			joint->springRotationConstant = it.springRotationConstant;

			model.joints.emplace_back(std::move(joint));
		}

		for (auto& it : pmx.softbodies)
		{
			auto softbody = std::make_shared<model::Softbody>();
			softbody->name = cv.to_bytes(it.name.name);
			softbody->materialIndex = it.materialIndex;
			softbody->group = it.group;
			softbody->groupMask = it.groupMask;
			softbody->blinkLength = it.blinkLength;
			softbody->numClusters = it.numClusters;
			softbody->totalMass = it.totalMass;
			softbody->collisionMargin = it.collisionMargin;
			softbody->aeroModel = it.aeroModel;
			softbody->LST = it.LST;
			softbody->anchorRigidbodies.reserve(it.numRigidbody);
				
			for (auto& rigidbody : it.anchorRigidbodies)
				softbody->anchorRigidbodies.push_back(rigidbody.rigidBodyIndex);

			for (std::size_t i = 0; i < it.numIndices; i++)
			{
				std::uint32_t index = 0;
				if (pmx.header.sizeOfIndices == 1)
					index = *((std::uint8_t*)it.pinVertexIndices.data() + i);
				else if (pmx.header.sizeOfIndices == 2)
					index = *((std::uint16_t*)it.pinVertexIndices.data() + i);
				else if (pmx.header.sizeOfIndices == 4)
					index = *((std::uint32_t*)it.pinVertexIndices.data() + i);

				softbody->pinVertexIndices.push_back(index);
			}

			model.softbodies.emplace_back(std::move(softbody));
		}

		return true;
	}

	bool PmxLoader::doSave(ostream& stream, const PMX& pmx) noexcept
	{
		if (!stream.write((char*)&pmx.header, sizeof(pmx.header))) return false;

		if (!stream.write((char*)&pmx.description.japanModelLength, sizeof(pmx.description.japanModelLength))) return false;
		if (pmx.description.japanModelLength)
			if (!stream.write((char*)&pmx.description.japanModelName[0], pmx.description.japanModelLength)) return false;

		if (!stream.write((char*)&pmx.description.englishModelLength, sizeof(pmx.description.englishModelLength))) return false;
		if (pmx.description.englishModelLength)
			if (!stream.write((char*)&pmx.description.englishModelName[0], pmx.description.englishModelLength)) return false;

		if (!stream.write((char*)&pmx.description.japanCommentLength, sizeof(pmx.description.japanCommentLength))) return false;
		if (pmx.description.japanCommentLength)
			if (!stream.write((char*)&pmx.description.japanCommentName[0], pmx.description.japanCommentLength)) return false;

		if (!stream.write((char*)&pmx.description.englishCommentLength, sizeof(pmx.description.englishCommentLength))) return false;
		if (pmx.description.englishCommentLength)
			if (!stream.write((char*)&pmx.description.englishCommentName[0], pmx.description.englishCommentLength)) return false;

		if (!stream.write((char*)&pmx.numVertices, sizeof(pmx.numVertices))) return false;
		if (pmx.numVertices)
		{
			for (auto& vertex : pmx.vertices)
			{
				if (pmx.header.addUVCount > 0)
				{
					std::streamsize size = sizeof(vertex.position) + sizeof(vertex.normal) + sizeof(vertex.coord) + sizeof(vertex.addCoord[0]) * pmx.header.addUVCount;
					if (!stream.write((char*)&vertex.position, size)) return false;
				}
				else
				{
					std::streamsize size = sizeof(vertex.position) + sizeof(vertex.normal) + sizeof(vertex.coord);
					if (!stream.write((char*)&vertex.position, size)) return false;
				}

				if (!stream.write((char*)&vertex.type, sizeof(vertex.type))) return false;

				switch (vertex.type)
				{
					case PMX_BDEF1:
					{
						if (!stream.write((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
					}
					break;
					case PMX_BDEF2:
					{
						if (!stream.write((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
						if (!stream.write((char*)&vertex.weight.bone2, pmx.header.sizeOfBone)) return false;
						if (!stream.write((char*)&vertex.weight.weight1, sizeof(vertex.weight.weight2))) return false;
					}
					break;
					case PMX_BDEF4:
					{
						if (!stream.write((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
						if (!stream.write((char*)&vertex.weight.bone2, pmx.header.sizeOfBone)) return false;
						if (!stream.write((char*)&vertex.weight.bone3, pmx.header.sizeOfBone)) return false;
						if (!stream.write((char*)&vertex.weight.bone4, pmx.header.sizeOfBone)) return false;
						if (!stream.write((char*)&vertex.weight.weight1, sizeof(vertex.weight.weight1))) return false;
						if (!stream.write((char*)&vertex.weight.weight2, sizeof(vertex.weight.weight2))) return false;
						if (!stream.write((char*)&vertex.weight.weight3, sizeof(vertex.weight.weight3))) return false;
						if (!stream.write((char*)&vertex.weight.weight4, sizeof(vertex.weight.weight4))) return false;
					}
					break;
					case PMX_SDEF:
					{
						if (!stream.write((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
						if (!stream.write((char*)&vertex.weight.bone2, pmx.header.sizeOfBone)) return false;
						if (!stream.write((char*)&vertex.weight.weight1, sizeof(vertex.weight.weight1))) return false;
						if (!stream.write((char*)&vertex.weight.SDEF_C, sizeof(vertex.weight.SDEF_C))) return false;
						if (!stream.write((char*)&vertex.weight.SDEF_R0, sizeof(vertex.weight.SDEF_R0))) return false;
						if (!stream.write((char*)&vertex.weight.SDEF_R1, sizeof(vertex.weight.SDEF_R1))) return false;
					}
					break;
					case PMX_QDEF:
					{
						if (!stream.write((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
						if (!stream.write((char*)&vertex.weight.bone2, pmx.header.sizeOfBone)) return false;
						if (!stream.write((char*)&vertex.weight.weight1, sizeof(vertex.weight.weight1))) return false;
					}
					default:
						return false;
				}

				if (!stream.write((char*)&vertex.edge, sizeof(vertex.edge))) return false;
			}
		}

		if (!stream.write((char*)&pmx.numIndices, sizeof(pmx.numIndices))) return false;
		if (pmx.numIndices)
			if (!stream.write((char*)pmx.indices.data(), pmx.indices.size())) return false;

		if (!stream.write((char*)&pmx.numTextures, sizeof(pmx.numTextures))) return false;
		if (pmx.numTextures)
		{
			for (auto& texture : pmx.textures)
			{
				if (!stream.write((char*)&texture.length, sizeof(texture.length))) return false;
				if (!stream.write((char*)&texture.name, texture.length)) return false;
			}
		}

		if (!stream.write((char*)&pmx.numMaterials, sizeof(pmx.numMaterials))) return false;
		if (pmx.numMaterials)
		{
			for (auto& material : pmx.materials)
			{
				if (!stream.write((char*)&material.name.length, sizeof(material.name.length))) return false;

				if (material.name.length)
					if (!stream.write((char*)&material.name.name, material.name.length)) return false;

				if (!stream.write((char*)&material.nameEng.length, sizeof(material.nameEng.length))) return false;

				if (material.nameEng.length)
					if (!stream.write((char*)&material.nameEng.name, material.nameEng.length)) return false;

				if (!stream.write((char*)&material.Diffuse, sizeof(material.Diffuse))) return false;
				if (!stream.write((char*)&material.Opacity, sizeof(material.Opacity))) return false;
				if (!stream.write((char*)&material.Specular, sizeof(material.Specular))) return false;
				if (!stream.write((char*)&material.Shininess, sizeof(material.Shininess))) return false;
				if (!stream.write((char*)&material.Ambient, sizeof(material.Ambient))) return false;
				if (!stream.write((char*)&material.Flag, sizeof(material.Flag))) return false;
				if (!stream.write((char*)&material.EdgeColor, sizeof(material.EdgeColor))) return false;
				if (!stream.write((char*)&material.EdgeSize, sizeof(material.EdgeSize))) return false;
				if (!stream.write((char*)&material.TextureIndex, pmx.header.sizeOfTexture)) return false;
				if (!stream.write((char*)&material.SphereTextureIndex, pmx.header.sizeOfTexture)) return false;
				if (!stream.write((char*)&material.SphereMode, sizeof(material.SphereMode))) return false;
				if (!stream.write((char*)&material.ToonIndex, sizeof(material.ToonIndex))) return false;

				if (material.ToonIndex == 1)
				{
					if (!stream.write((char*)&material.ToonTexture, 1)) return false;
				}
				else
				{
					if (!stream.write((char*)&material.ToonTexture, pmx.header.sizeOfTexture)) return false;
				}

				if (!stream.write((char*)&material.memLength, sizeof(material.memLength))) return false;
				if (material.memLength > 0)
				{
					if (!stream.write((char*)&material.mem, material.memLength)) return false;
				}

				if (!stream.write((char*)&material.FaceCount, sizeof(material.FaceCount))) return false;
			}
		}

		if (!stream.write((char*)&pmx.numBones, sizeof(pmx.numBones))) return false;
		if (pmx.numBones)
		{
			for (auto& bone : pmx.bones)
			{
				if (!stream.write((char*)&bone.name.length, sizeof(bone.name.length))) return false;
				if (bone.name.length)
					if (!stream.write((char*)&bone.name.name, bone.name.length)) return false;
				if (!stream.write((char*)&bone.nameEng.length, sizeof(bone.nameEng.length))) return false;
				if (bone.nameEng.length)
					if (!stream.write((char*)&bone.nameEng.name, bone.nameEng.length)) return false;

				if (!stream.write((char*)&bone.position, sizeof(bone.position))) return false;
				if (!stream.write((char*)&bone.Parent, pmx.header.sizeOfBone)) return false;
				if (!stream.write((char*)&bone.Level, sizeof(bone.Level))) return false;
				if (!stream.write((char*)&bone.Flag, sizeof(bone.Flag))) return false;

				if (bone.Flag & PMX_BONE_INDEX)
				{
					if (!stream.write((char*)&bone.ConnectedBoneIndex, pmx.header.sizeOfBone)) return false;
				}
				else
				{
					if (!stream.write((char*)&bone.Offset, sizeof(bone.Offset))) return false;
				}

				if (bone.Flag & (PMX_BONE_ADD_ROTATION | PMX_BONE_ADD_MOVE))
				{
					if (!stream.write((char*)&bone.ProvidedParentBoneIndex, pmx.header.sizeOfBone)) return false;
					if (!stream.write((char*)&bone.ProvidedRatio, sizeof(bone.ProvidedRatio))) return false;
				}

				if (bone.Flag & PMX_BONE_FIXED_AXIS)
				{
					if (!stream.write((char*)&bone.AxisDirection, sizeof(bone.AxisDirection))) return false;
				}

				if (bone.Flag & PMX_BONE_LOCAL_AXIS)
				{
					if (!stream.write((char*)&bone.DimentionXDirection, sizeof(bone.DimentionXDirection))) return false;
					if (!stream.write((char*)&bone.DimentionZDirection, sizeof(bone.DimentionZDirection))) return false;
				}

				if (bone.Flag & PMX_BONE_IK)
				{
					if (!stream.write((char*)&bone.IKTargetBoneIndex, pmx.header.sizeOfBone)) return false;
					if (!stream.write((char*)&bone.IKLoopCount, sizeof(bone.IKLoopCount))) return false;
					if (!stream.write((char*)&bone.IKLimitedRadian, sizeof(bone.IKLimitedRadian))) return false;
					if (!stream.write((char*)&bone.IKLinkCount, sizeof(bone.IKLinkCount))) return false;

					if (bone.IKLinkCount > 0)
					{
						for (auto& chain : bone.IKList)
						{
							if (!stream.write((char*)&chain.BoneIndex, pmx.header.sizeOfBone)) return false;
							if (!stream.write((char*)&chain.rotateLimited, (std::streamsize)sizeof(chain.rotateLimited))) return false;
							if (chain.rotateLimited)
							{
								if (!stream.write((char*)&chain.minimumRadian, (std::streamsize)sizeof(chain.minimumRadian))) return false;
								if (!stream.write((char*)&chain.maximumRadian, (std::streamsize)sizeof(chain.maximumRadian))) return false;
							}
						}
					}
				}
			}
		}

		if (!stream.write((char*)&pmx.numMorphs, sizeof(pmx.numMorphs))) return false;
		if (pmx.numMorphs)
		{
			for (auto& morph : pmx.morphs)
			{
				if (morph.name.length) 
					if (!stream.write((char*)&morph.name.length, sizeof(morph.name.length))) return false;
				if (!stream.write((char*)&morph.name.name, morph.name.length)) return false;
				if (!stream.write((char*)&morph.nameEng.length, sizeof(morph.nameEng.length))) return false;
				if (morph.nameEng.length)
					if (!stream.write((char*)&morph.nameEng.name, morph.nameEng.length)) return false;
				if (!stream.write((char*)&morph.control, sizeof(morph.control))) return false;
				if (!stream.write((char*)&morph.morphType, sizeof(morph.morphType))) return false;
				if (!stream.write((char*)&morph.morphCount, sizeof(morph.morphCount))) return false;

				if (morph.morphType == PmxMorphType::PMX_MorphTypeGroup)
				{
					for (auto& group : morph.groupList)
					{
						if (!stream.write((char*)& group.morphIndex, pmx.header.sizeOfMorph)) return false;
						if (!stream.write((char*)& group.morphRate, sizeof(group.morphRate))) return false;
					}
				}
				else if (morph.morphType == PmxMorphType::PMX_MorphTypeVertex)
				{
					for (auto& vertex : morph.vertices)
					{
						if (!stream.write((char*)&vertex.index, pmx.header.sizeOfIndices)) return false;
						if (!stream.write((char*)&vertex.offset, sizeof(vertex.offset))) return false;
					}
				}
				else if (morph.morphType == PmxMorphType::PMX_MorphTypeBone)
				{
					for (auto& bone : morph.boneList)
					{
						if (!stream.write((char*)&bone.boneIndex, pmx.header.sizeOfBone)) return false;
						if (!stream.write((char*)&bone.position, sizeof(bone.position))) return false;
						if (!stream.write((char*)&bone.rotation, sizeof(bone.rotation))) return false;
					}
				}
				else if (morph.morphType == PmxMorphType::PMX_MorphTypeUV || morph.morphType == PmxMorphType::PMX_MorphTypeExtraUV1 ||
							morph.morphType == PmxMorphType::PMX_MorphTypeExtraUV2 || morph.morphType == PmxMorphType::PMX_MorphTypeExtraUV3 ||
							morph.morphType == PmxMorphType::PMX_MorphTypeExtraUV4)
				{
					for (auto& texcoord : morph.texcoordList)
					{
						if (!stream.write((char*)&texcoord.index, pmx.header.sizeOfIndices)) return false;
						if (!stream.write((char*)&texcoord.offset, sizeof(texcoord.offset))) return false;
					}
				}
				else if (morph.morphType == PmxMorphType::PMX_MorphTypeMaterial)
				{
					for (auto& material : morph.materialList)
					{
						if (!stream.write((char*)&material.index, pmx.header.sizeOfMaterial)) return false;
						if (!stream.write((char*)&material.offset, sizeof(material.offset))) return false;
						if (!stream.write((char*)&material.diffuse, sizeof(material.diffuse))) return false;
						if (!stream.write((char*)&material.specular, sizeof(material.specular))) return false;
						if (!stream.write((char*)&material.shininess, sizeof(material.shininess))) return false;
						if (!stream.write((char*)&material.ambient, sizeof(material.ambient))) return false;
						if (!stream.write((char*)&material.edgeColor, sizeof(material.edgeColor))) return false;
						if (!stream.write((char*)&material.edgeSize, sizeof(material.edgeSize))) return false;
						if (!stream.write((char*)&material.tex, sizeof(material.tex))) return false;
						if (!stream.write((char*)&material.sphere, sizeof(material.sphere))) return false;
						if (!stream.write((char*)&material.toon, sizeof(material.toon))) return false;
					}
				}
			}
		}

		if (!stream.write((char*)&pmx.numDisplayFrames, sizeof(pmx.numDisplayFrames))) return false;
		if (pmx.numDisplayFrames)
		{
			for (auto& displayFrame : pmx.displayFrames)
			{
				if (!stream.write((char*)&displayFrame.name.length, sizeof(displayFrame.name.length))) return false;
				if (displayFrame.name.length)
					if (!stream.write((char*)&displayFrame.name.name, displayFrame.name.length)) return false;
				if (!stream.write((char*)&displayFrame.nameEng.length, sizeof(displayFrame.nameEng.length))) return false;
				if (displayFrame.nameEng.length)
					if (!stream.write((char*)&displayFrame.nameEng.name, displayFrame.nameEng.length)) return false;
				if (!stream.write((char*)&displayFrame.type, sizeof(displayFrame.type))) return false;
				if (!stream.write((char*)&displayFrame.elementsWithinFrame, sizeof(displayFrame.elementsWithinFrame))) return false;

				for (auto& element : displayFrame.elements)
				{
					if (!stream.write((char*)&element.target, sizeof(element.target))) return false;

					if (element.target == 0)
					{
						if (!stream.write((char*)&element.index, pmx.header.sizeOfBone))
							return false;
					}
					else if (element.target == 1)
					{
						if (!stream.write((char*)&element.index, pmx.header.sizeOfMorph))
							return false;
					}
				}
			}
		}

		if (!stream.write((char*)&pmx.numRigidbodys, sizeof(pmx.numRigidbodys))) return false;
		if (pmx.numRigidbodys)
		{
			for (auto& rigidbody : pmx.rigidbodies)
			{
				if (!stream.write((char*)&rigidbody.name.length, sizeof(rigidbody.name.length))) return false;
				if (rigidbody.name.length)
					if (!stream.write((char*)&rigidbody.name.name, rigidbody.name.length)) return false;
				if (!stream.write((char*)&rigidbody.nameEng.length, sizeof(rigidbody.nameEng.length))) return false;
				if (rigidbody.nameEng.length)
					if (!stream.write((char*)&rigidbody.nameEng.name, rigidbody.nameEng.length)) return false;

				if (!stream.write((char*)&rigidbody.bone, pmx.header.sizeOfBone)) return false;
				if (!stream.write((char*)&rigidbody.group, sizeof(rigidbody.group))) return false;
				if (!stream.write((char*)&rigidbody.groupMask, sizeof(rigidbody.groupMask))) return false;

				if (!stream.write((char*)&rigidbody.shape, sizeof(rigidbody.shape))) return false;

				if (!stream.write((char*)&rigidbody.scale, sizeof(rigidbody.scale))) return false;
				if (!stream.write((char*)&rigidbody.position, sizeof(rigidbody.position))) return false;
				if (!stream.write((char*)&rigidbody.rotate, sizeof(rigidbody.rotate))) return false;

				if (!stream.write((char*)&rigidbody.mass, sizeof(rigidbody.mass))) return false;
				if (!stream.write((char*)&rigidbody.movementDecay, sizeof(rigidbody.movementDecay))) return false;
				if (!stream.write((char*)&rigidbody.rotationDecay, sizeof(rigidbody.rotationDecay))) return false;
				if (!stream.write((char*)&rigidbody.elasticity, sizeof(rigidbody.elasticity))) return false;
				if (!stream.write((char*)&rigidbody.friction, sizeof(rigidbody.friction))) return false;
				if (!stream.write((char*)&rigidbody.physicsOperation, sizeof(rigidbody.physicsOperation))) return false;
			}
		}

		if (!stream.write((char*)&pmx.numJoints, sizeof(pmx.numJoints))) return false;
		if (pmx.numJoints)
		{
			for (auto& joint : pmx.joints)
			{
				if (!stream.write((char*)&joint.name.length, sizeof(joint.name.length))) return false;
				if (joint.name.length)
					if (!stream.write((char*)&joint.name.name, joint.name.length)) return false;
				if (!stream.write((char*)&joint.nameEng.length, sizeof(joint.nameEng.length))) return false;
				if (joint.nameEng.length)
					if (!stream.write((char*)&joint.nameEng.name, joint.nameEng.length)) return false;

				if (!stream.write((char*)&joint.type, sizeof(joint.type))) return false;

				if (joint.type != 0)
					return false;

				if (!stream.write((char*)&joint.relatedRigidBodyIndexA, pmx.header.sizeOfBody)) return false;
				if (!stream.write((char*)&joint.relatedRigidBodyIndexB, pmx.header.sizeOfBody)) return false;

				if (!stream.write((char*)&joint.position, sizeof(joint.position))) return false;
				if (!stream.write((char*)&joint.rotation, sizeof(joint.rotation))) return false;

				if (!stream.write((char*)&joint.movementLowerLimit, sizeof(joint.movementLowerLimit))) return false;
				if (!stream.write((char*)&joint.movementUpperLimit, sizeof(joint.movementUpperLimit))) return false;

				if (!stream.write((char*)&joint.rotationLowerLimit, sizeof(joint.rotationLowerLimit))) return false;
				if (!stream.write((char*)&joint.rotationUpperLimit, sizeof(joint.rotationUpperLimit))) return false;

				if (!stream.write((char*)&joint.springMovementConstant, sizeof(joint.springMovementConstant))) return false;
				if (!stream.write((char*)&joint.springRotationConstant, sizeof(joint.springRotationConstant))) return false;
			}
		}

		return true;
	}

	bool PmxLoader::doSave(ostream& stream, const model::Model& model) noexcept
	{
		return false;
	}
}