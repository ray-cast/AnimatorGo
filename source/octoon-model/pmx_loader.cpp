#include <octoon/model/pmx_loader.h>
#include <octoon/model/pmx.h>
#include <octoon/model/modtypes.h>
#include <octoon/model/property.h>
#include <octoon/model/model.h>

#include <octoon/math/mathfwd.h>
#include <octoon/math/mathutil.h>

using namespace octoon::io;
using namespace octoon::math;

namespace octoon
{
	namespace model
	{
		bool PmxLoader::doCanLoad(istream& stream) noexcept
		{
			PmxHeader header;
			if (stream.read((char*)&header, sizeof(header)))
			{
				if (header.magic[0] == 'p' || header.magic[0] == 'P' &&
					header.magic[1] == 'm' || header.magic[1] == 'M' &&
					header.magic[2] == 'x' || header.magic[2] == 'X')
				{
					if (header.version == 2.0)
					{
						return true;
					}
				}
			}
			return false;
		}

		bool PmxLoader::doCanLoad(const std::string& type) noexcept
		{
			return type.compare("pmx") == 0;
		}

		bool PmxLoader::doCanLoad(const char* type) noexcept
		{
			return std::strncmp(type, "pmx", 3) == 0;
		}

		bool PmxLoader::doLoad(istream& stream, Pmx& pmx) noexcept
		{
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
							if (!stream.read((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
							if (!stream.read((char*)&vertex.weight.bone2, pmx.header.sizeOfBone)) return false;
							if (!stream.read((char*)&vertex.weight.weight1, sizeof(vertex.weight.weight1))) return false;

							vertex.weight.weight2 = 1.0f - vertex.weight.weight1;
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

					if (bone.Flag & PMX_BONE_INDEX)
					{
						if (!stream.read((char*)&bone.ConnectedBoneIndex, pmx.header.sizeOfBone)) return false;
					}
					else
					{
						if (!stream.read((char*)&bone.Offset, sizeof(bone.Offset))) return false;
					}

					if (bone.Flag & PMX_BONE_PARENT)
					{
						if (!stream.read((char*)&bone.ProvidedParentBoneIndex, pmx.header.sizeOfBone)) return false;
						if (!stream.read((char*)&bone.ProvidedRatio, sizeof(bone.ProvidedRatio))) return false;
					}

					if (bone.Flag & PMX_BONE_AXIS)
					{
						if (!stream.read((char*)&bone.AxisDirection, sizeof(bone.AxisDirection))) return false;
					}

					if (bone.Flag & PMX_BONE_ROTATE)
					{
						if (!stream.read((char*)&bone.DimentionXDirection, sizeof(bone.DimentionXDirection))) return false;
						if (!stream.read((char*)&bone.DimentionZDirection, sizeof(bone.DimentionZDirection))) return false;
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

							for (std::size_t j = 0; j < bone.IKLinkCount; j++)
							{
								if (!stream.read((char*)&bone.IKList[j].BoneIndex, pmx.header.sizeOfBone)) return false;
								if (!stream.read((char*)&bone.IKList[j].rotateLimited, (std::streamsize)sizeof(bone.IKList[j].rotateLimited))) return false;
								if (bone.IKList[j].rotateLimited)
								{
									if (!stream.read((char*)&bone.IKList[j].maximumRadian, (std::streamsize)sizeof(bone.IKList[j].maximumRadian))) return false;
									if (!stream.read((char*)&bone.IKList[j].minimumRadian, (std::streamsize)sizeof(bone.IKList[j].minimumRadian))) return false;
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

					if (morph.morphType == PmxMorphType::MorphTypeGroup)
					{
						if (!stream.read((char*)&morph.morphIndex, pmx.header.sizeOfMorph)) return false;
						if (!stream.read((char*)&morph.morphRate, sizeof(morph.morphRate))) return false;
					}
					else if (morph.morphType == PmxMorphType::MorphTypeVertex)
					{
						morph.vertexList.resize(morph.morphCount);

						for (auto& vertex : morph.vertexList)
						{
							if (!stream.read((char*)&vertex.index, pmx.header.sizeOfIndices)) return false;
							if (!stream.read((char*)&vertex.offset, sizeof(vertex.offset))) return false;
						}
					}
					else if (morph.morphType == PmxMorphType::MorphTypeBone)
					{
						morph.boneList.resize(morph.morphCount);

						for (auto& bone : morph.boneList)
						{
							if (!stream.read((char*)&bone.boneIndex, pmx.header.sizeOfBone)) return false;
							if (!stream.read((char*)&bone.position, sizeof(bone.position))) return false;
							if (!stream.read((char*)&bone.rotate, sizeof(bone.rotate))) return false;
						}
					}
					else if (morph.morphType == PmxMorphType::MorphTypeUV || morph.morphType == PmxMorphType::MorphTypeExtraUV1 ||
							 morph.morphType == PmxMorphType::MorphTypeExtraUV2 || morph.morphType == PmxMorphType::MorphTypeExtraUV3 ||
							 morph.morphType == PmxMorphType::MorphTypeExtraUV4)
					{
						morph.texcoordList.resize(morph.morphCount);

						for (auto& texcoord : morph.texcoordList)
						{
							if (!stream.read((char*)&texcoord.index, pmx.header.sizeOfIndices)) return false;
							if (!stream.read((char*)&texcoord.offset, sizeof(texcoord.offset))) return false;
						}
					}
					else if (morph.morphType == PmxMorphType::MorphTypeMaterial)
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
				pmx.rigidbodys.resize(pmx.numRigidbodys);

				for (auto& rigidbody : pmx.rigidbodys)
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

					if (joint.type != 0)
						return false;

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

			return true;
		}

		bool PmxLoader::doLoad(istream& stream, Model& model) noexcept
		{
			Pmx pmx;
			if (!this->doLoad(stream, pmx))
				return false;

			for (auto& it : pmx.materials)
			{
				auto material = std::make_shared<MaterialProperty>();

				if ((it.name.length >> 1) < MAX_PATH)
				{
					char name[MAX_PATH];
					auto length = wcstombs(nullptr, it.name.name, 0);
					if (length < MAX_PATH)
					{
						wcstombs(name, it.name.name, MAX_PATH);

						material->set(MATKEY_NAME, name);
					}
				}

				material->set(MATKEY_COLOR_DIFFUSE, octoon::math::srgb2linear(it.Diffuse));
				material->set(MATKEY_COLOR_AMBIENT, octoon::math::srgb2linear(it.Ambient));
				material->set(MATKEY_COLOR_SPECULAR, octoon::math::srgb2linear(it.Specular));
				material->set(MATKEY_OPACITY, it.Opacity);
				material->set(MATKEY_SHININESS, it.Shininess / 255.0f);

				std::uint32_t limits = 0;
				if (pmx.header.sizeOfTexture == 1)
					limits = std::numeric_limits<PmxUInt8>::max();
				else if (pmx.header.sizeOfTexture == 2)
					limits = std::numeric_limits<PmxUInt16>::max();
				else if (pmx.header.sizeOfTexture == 4)
					limits = std::numeric_limits<PmxUInt32>::max();

				if (it.TextureIndex < limits)
				{
					PmxName& texture = pmx.textures[it.TextureIndex];
					if ((texture.length >> 1) < MAX_PATH)
					{
						char name[MAX_PATH];
						::wcstombs(name, texture.name, MAX_PATH);

						material->set(MATKEY_TEXTURE_DIFFUSE(0), name);
						material->set(MATKEY_TEXTURE_AMBIENT(0), name);
					}
				}

				if (it.SphereTextureIndex < limits)
				{
					PmxName& texture = pmx.textures[it.SphereTextureIndex];
					if ((texture.length >> 1) < MAX_PATH)
					{
						char name[MAX_PATH];

						wcstombs(name, texture.name, MAX_PATH);

						material->set(MATKEY_COLOR_SPHEREMAP, name);
					}
				}

				model.addMaterial(std::move(material));
			}

			if (pmx.numVertices > 0 && pmx.numIndices > 0 && pmx.numMaterials > 0)
			{
				Float3Array vertices;
				Float3Array normals;
				Float2Array texcoords;
				VertexWeights weights;

				for (std::uint32_t i = 0; i < pmx.numVertices; i++)
				{
					PmxVertex& v = pmx.vertices[i];

					vertices.push_back(v.position);
					normals.push_back(v.normal);
					texcoords.push_back(v.coord);

					if (pmx.numBones > 1)
					{
						VertexWeight weight;
						weight.weight1 = v.weight.weight1;
						weight.weight2 = v.weight.weight2;
						weight.weight3 = v.weight.weight3;
						weight.weight4 = v.weight.weight4;
						weight.bone1 = static_cast<uint8_t>(v.weight.bone1);
						weight.bone2 = static_cast<uint8_t>(v.weight.bone2);
						weight.bone3 = static_cast<uint8_t>(v.weight.bone3);
						weight.bone4 = static_cast<uint8_t>(v.weight.bone4);

						weights.push_back(weight);
					}
				}

				Uint1Array indices(pmx.numIndices);
				PmxIndex* indicesData = pmx.indices.data();

				for (std::uint32_t i = 0; i < pmx.numIndices; i++, indicesData += pmx.header.sizeOfIndices)
				{
					if (pmx.header.sizeOfIndices == 1)
						indices[i] = *(std::uint8_t*)indicesData;
					else if (pmx.header.sizeOfIndices == 2)
						indices[i] = *(std::uint16_t*)indicesData;
					else if (pmx.header.sizeOfIndices == 4)
						indices[i] = *(std::uint16_t*)indicesData;
					else
						return false;
				}

				MeshSubsets subsets;
				std::size_t startIndices = 0;

				for (auto& it : pmx.materials)
				{
					subsets.push_back(MeshSubset(0, startIndices, it.FaceCount, 0, 0));
					startIndices += it.FaceCount;
				}

				MeshPropertyPtr mesh = std::make_shared<MeshProperty>();
				mesh->setVertexArray(std::move(vertices));
				mesh->setNormalArray(std::move(normals));
				mesh->setTexcoordArray(std::move(texcoords));
				mesh->setWeightArray(std::move(weights));
				mesh->setIndicesArray(std::move(indices));
				mesh->setMeshSubsets(std::move(subsets));

				model.addMesh(std::move(mesh));
			}

			if (pmx.numBones > 1)
			{
				std::size_t index = 0;
				for (auto& it : pmx.bones)
				{
					char name[MAX_PATH] = { 0 };
					if ((it.name.length) > MAX_PATH)
						return false;

					if (!wcstombs(name, it.name.name, MAX_PATH))
						return false;

					Bone bone;

					bone.setName(name);
					bone.setPosition(it.position);
					bone.setParent(it.Parent);

					model.addBone(std::make_shared<Bone>(bone));

					if (it.Flag & PMX_BONE_IK)
					{
						IKAttr attr;
						attr.boneIndex = static_cast<uint16_t>(index);
						attr.targetBoneIndex = it.IKTargetBoneIndex;
						attr.chainLength = it.IKLinkCount;
						attr.iterations = it.IKLoopCount;

						for (auto& ik : it.IKList)
						{
							IKChild child;
							child.boneIndex = ik.BoneIndex;
							child.angleWeight = degress(it.IKLimitedRadian) / 229.1831f;
							child.minimumDegrees = degress(ik.minimumRadian);
							child.maximumDegrees = degress(ik.maximumRadian);
							child.rotateLimited = ik.rotateLimited;

							attr.child.push_back(child);
						}

						model.addIK(std::make_shared<IKAttr>(attr));
					}

					index++;
				}
			}

			for (auto& it : pmx.rigidbodys)
			{
				char name[MAX_PATH] = { 0 };
				if ((it.name.length) > MAX_PATH)
					return false;

				if (!wcstombs(name, it.name.name, MAX_PATH))
					return false;

				auto body = std::make_shared<RigidbodyProperty>();
				body->name = name;
				body->bone = it.bone;
				body->group = it.group;
				body->groupMask = it.groupMask;
				body->shape = (ShapeType)it.shape;
				body->scale = it.scale;
				body->position = it.position;
				body->rotation = it.rotate;
				body->mass = it.mass;
				body->movementDecay = it.movementDecay;
				body->rotationDecay = it.rotationDecay;
				body->elasticity = it.elasticity;
				body->friction = it.friction;
				body->physicsOperation = it.physicsOperation;

				model.addRigidbody(std::move(body));
			}

			for (auto& it : pmx.joints)
			{
				char name[MAX_PATH] = { 0 };
				if ((it.name.length) > MAX_PATH)
					return false;

				if (!wcstombs(name, it.name.name, MAX_PATH))
					return false;

				auto joint = std::make_shared<JointProperty>();
				joint->name = name;
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

				model.addJoint(std::move(joint));
			}

			return true;
		}

		bool PmxLoader::doSave(octoon::io::ostream& stream, const Pmx& pmx) noexcept
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

					if (bone.Flag & PMX_BONE_PARENT)
					{
						if (!stream.write((char*)&bone.ProvidedParentBoneIndex, pmx.header.sizeOfBone)) return false;
						if (!stream.write((char*)&bone.ProvidedRatio, sizeof(bone.ProvidedRatio))) return false;
					}

					if (bone.Flag & PMX_BONE_AXIS)
					{
						if (!stream.write((char*)&bone.AxisDirection, sizeof(bone.AxisDirection))) return false;
					}

					if (bone.Flag & PMX_BONE_ROTATE)
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
							for (std::size_t j = 0; j < bone.IKLinkCount; j++)
							{
								if (!stream.write((char*)&bone.IKList[j].BoneIndex, pmx.header.sizeOfBone)) return false;
								if (!stream.write((char*)&bone.IKList[j].rotateLimited, (std::streamsize)sizeof(bone.IKList[j].rotateLimited))) return false;
								if (bone.IKList[j].rotateLimited)
								{
									if (!stream.write((char*)&bone.IKList[j].maximumRadian, (std::streamsize)sizeof(bone.IKList[j].maximumRadian))) return false;
									if (!stream.write((char*)&bone.IKList[j].minimumRadian, (std::streamsize)sizeof(bone.IKList[j].minimumRadian))) return false;
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

					if (morph.morphType == PmxMorphType::MorphTypeGroup)
					{
						if (!stream.write((char*)&morph.morphIndex, pmx.header.sizeOfMorph)) return false;
						if (!stream.write((char*)&morph.morphRate, sizeof(morph.morphRate))) return false;
					}
					else if (morph.morphType == PmxMorphType::MorphTypeVertex)
					{
						for (auto& vertex : morph.vertexList)
						{
							if (!stream.write((char*)&vertex.index, pmx.header.sizeOfIndices)) return false;
							if (!stream.write((char*)&vertex.offset, sizeof(vertex.offset))) return false;
						}
					}
					else if (morph.morphType == PmxMorphType::MorphTypeBone)
					{
						for (auto& bone : morph.boneList)
						{
							if (!stream.write((char*)&bone.boneIndex, pmx.header.sizeOfBone)) return false;
							if (!stream.write((char*)&bone.position, sizeof(bone.position))) return false;
							if (!stream.write((char*)&bone.rotate, sizeof(bone.rotate))) return false;
						}
					}
					else if (morph.morphType == PmxMorphType::MorphTypeUV || morph.morphType == PmxMorphType::MorphTypeExtraUV1 ||
							 morph.morphType == PmxMorphType::MorphTypeExtraUV2 || morph.morphType == PmxMorphType::MorphTypeExtraUV3 ||
							 morph.morphType == PmxMorphType::MorphTypeExtraUV4)
					{
						for (auto& texcoord : morph.texcoordList)
						{
							if (!stream.write((char*)&texcoord.index, pmx.header.sizeOfIndices)) return false;
							if (!stream.write((char*)&texcoord.offset, sizeof(texcoord.offset))) return false;
						}
					}
					else if (morph.morphType == PmxMorphType::MorphTypeMaterial)
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
				for (auto& rigidbody : pmx.rigidbodys)
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

		bool PmxLoader::doSave(octoon::io::ostream& stream, const Model& model) noexcept
		{
			return false;
		}
	}
}