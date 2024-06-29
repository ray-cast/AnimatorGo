#include <octoon/fbx_importer.h>
#include <octoon/asset_importer.h>
#include <octoon/asset_database.h>
#include <octoon/material/mesh_standard_material.h>
#include <octoon/transform_component.h>
#include <octoon/point_light_component.h>
#include <octoon/film_camera_component.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>
#include <fstream>
#include <tuple>
#include <set>

namespace octoon
{
	static auto KFCURVENODE_T_X = "X";
	static auto KFCURVENODE_T_Y = "Y";
	static auto KFCURVENODE_T_Z = "Z";

	static auto KFCURVENODE_R_X = "X";
	static auto KFCURVENODE_R_Y = "Y";
	static auto KFCURVENODE_R_Z = "Z";
	static auto KFCURVENODE_R_W = "W";

	static auto KFCURVENODE_S_X = "X";
	static auto KFCURVENODE_S_Y = "Y";
	static auto KFCURVENODE_S_Z = "Z";

	OctoonImplementSubClass(FBXImporter, AssetImporter, "FBXImporter")

	FBXImporter::FBXImporter() noexcept
	{
	}

	FBXImporter::~FBXImporter() noexcept
	{
	}

	void ParseChannel(FbxNode* node, FbxAnimLayer* layer)
	{
		auto curve = node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X);
		if (curve == 0)
			return;

		FbxTime time;
		char timeString[256];

		auto keyCount = curve->KeyGetCount();

		for (int i = 0; i < keyCount; i++)
		{
			time = curve->KeyGetTime(i);

			auto matLocal = node->EvaluateLocalTransform(time);
			auto matGlobal = node->EvaluateGlobalTransform(time);

			time.GetTimeString(timeString, FbxUShort(256));

			std::cout << timeString << std::endl;
		}
	}

	void ParseLayer(FbxNode* node, FbxAnimLayer* layer)
	{
		ParseChannel(node, layer);

		auto childCount = node->GetChildCount();

		for (int i = 0; i < childCount; i++)
			ParseLayer(node->GetChild(i), layer);
	}

	int GetMaximumKeySpanFromSkeleton(FbxNode* node, FbxAnimLayer* animLayer)
	{
		int keySpan = 0;

		auto skeleton = node->GetSkeleton();
		if (skeleton)
		{
			FbxAnimCurve* translationCurveX = node->LclTranslation.GetCurve(animLayer, KFCURVENODE_T_X);
			FbxAnimCurve* translationCurveY = node->LclTranslation.GetCurve(animLayer, KFCURVENODE_T_Y);
			FbxAnimCurve* translationCurveZ = node->LclTranslation.GetCurve(animLayer, KFCURVENODE_T_Z);

			if (translationCurveX && translationCurveY && translationCurveZ)
			{
				int numKeys = std::max(std::max(translationCurveX->KeyGetCount(), translationCurveY->KeyGetCount()), translationCurveZ->KeyGetCount());
				keySpan = std::max(keySpan, numKeys);
			}

			FbxAnimCurve* rotationCurveX = node->LclRotation.GetCurve(animLayer, KFCURVENODE_R_X);
			FbxAnimCurve* rotationCurveY = node->LclRotation.GetCurve(animLayer, KFCURVENODE_R_Y);
			FbxAnimCurve* rotationCurveZ = node->LclRotation.GetCurve(animLayer, KFCURVENODE_R_Z);

			if (rotationCurveX && rotationCurveY && rotationCurveZ)
			{
				int numKeys = std::max(std::max(rotationCurveX->KeyGetCount(), rotationCurveY->KeyGetCount()), rotationCurveZ->KeyGetCount());
				keySpan = std::max(keySpan, numKeys);
			}

			FbxAnimCurve* scaleCurveX = node->LclScaling.GetCurve(animLayer, KFCURVENODE_S_X);
			FbxAnimCurve* scaleCurveY = node->LclScaling.GetCurve(animLayer, KFCURVENODE_S_Y);
			FbxAnimCurve* scaleCurveZ = node->LclScaling.GetCurve(animLayer, KFCURVENODE_S_Z);

			if (scaleCurveX && scaleCurveY && scaleCurveZ)
			{
				int numKeys = std::max(std::max(scaleCurveX->KeyGetCount(), scaleCurveY->KeyGetCount()), scaleCurveZ->KeyGetCount());
				keySpan = std::max(keySpan, numKeys);
			}

			int childCount = node->GetChildCount();
			for (int childIndex = 0; childIndex < childCount; childIndex++)
			{
				auto child = node->GetChild(childIndex);
				keySpan = std::max(keySpan, GetMaximumKeySpanFromSkeleton(child, animLayer));
			}
		}

		return keySpan;
	}

	void ReadClip(FbxNode* fbxNode, FbxAnimLayer* fbxAnimLayer, AnimationClip& clip, int& preInfType, int& postInfType, int keySpan)
	{
		Keyframes<float> translationCurve;
		Keyframes<float> rotationCurve;

		translationCurve.reserve(keySpan);
		rotationCurve.reserve(keySpan);

		FbxAnimCurve* translationCurveX = fbxNode->LclTranslation.GetCurve(fbxAnimLayer, KFCURVENODE_T_X);
		FbxAnimCurve* translationCurveY = fbxNode->LclTranslation.GetCurve(fbxAnimLayer, KFCURVENODE_T_Y);
		FbxAnimCurve* translationCurveZ = fbxNode->LclTranslation.GetCurve(fbxAnimLayer, KFCURVENODE_T_Z);

		if (translationCurveX && translationCurveY && translationCurveZ)
		{
			preInfType |= translationCurveX->GetPreExtrapolation() | translationCurveY->GetPreExtrapolation() | translationCurveZ->GetPreExtrapolation();
			postInfType |= translationCurveX->GetPostExtrapolation() | translationCurveY->GetPostExtrapolation() | translationCurveZ->GetPostExtrapolation();

			int xIndex = 0;
			int yIndex = 0;
			int zIndex = 0;

			for (int keyIndex = 0; keyIndex < keySpan; keyIndex++)
			{
				math::float3 key = math::float3(translationCurveX->EvaluateIndex(xIndex), translationCurveY->EvaluateIndex(yIndex), translationCurveZ->EvaluateIndex(zIndex));
				translationCurve.emplace_back(keyIndex / 30.f, key);

				if (keyIndex + 1 < translationCurveX->KeyGetCount())
					xIndex++;
				if (keyIndex + 1 < translationCurveY->KeyGetCount())
					yIndex++;
				if (keyIndex + 1 < translationCurveZ->KeyGetCount())
					zIndex++;
			}
		}
		else
		{
			math::float4 key = math::float4((float)fbxNode->LclTranslation.Get()[0], (float)fbxNode->LclTranslation.Get()[1], (float)fbxNode->LclTranslation.Get()[2], 0.0f);
			rotationCurve.emplace_back(0.0f, key);
		}

		FbxAnimCurve* rotationCurveX = fbxNode->LclRotation.GetCurve(fbxAnimLayer, KFCURVENODE_R_X);
		FbxAnimCurve* rotationCurveY = fbxNode->LclRotation.GetCurve(fbxAnimLayer, KFCURVENODE_R_Y);
		FbxAnimCurve* rotationCurveZ = fbxNode->LclRotation.GetCurve(fbxAnimLayer, KFCURVENODE_R_Z);

		if (rotationCurveX && rotationCurveY && rotationCurveZ)
		{
			preInfType |= rotationCurveX->GetPreExtrapolation() | rotationCurveY->GetPreExtrapolation() | rotationCurveZ->GetPreExtrapolation();
			postInfType |= rotationCurveX->GetPostExtrapolation() | rotationCurveY->GetPostExtrapolation() | rotationCurveZ->GetPostExtrapolation();
			
			int xIndex = 0;
			int yIndex = 0;
			int zIndex = 0;

			for (int keyIndex = 0; keyIndex < keySpan; keyIndex++)
			{
				math::float3 key = math::float3(rotationCurveX->EvaluateIndex(xIndex), rotationCurveY->EvaluateIndex(yIndex), rotationCurveZ->EvaluateIndex(zIndex));
				rotationCurve.emplace_back(keyIndex / 30.f, key);

				if (keyIndex + 1 < rotationCurveX->KeyGetCount())
					xIndex++;
				if (keyIndex + 1 < rotationCurveY->KeyGetCount())
					yIndex++;
				if (keyIndex + 1 < rotationCurveZ->KeyGetCount())
					zIndex++;
			}
		}
		else
		{
			math::float3 key = math::float3((float)fbxNode->LclRotation.Get()[0], (float)fbxNode->LclRotation.Get()[1], (float)fbxNode->LclRotation.Get()[2]);
			rotationCurve.emplace_back(0.0f, key);
		}

		clip.setCurve(fbxNode->GetName(), "LocalPosition", AnimationCurve(translationCurve));
		clip.setCurve(fbxNode->GetName(), "LocalEulerAnglesRaw", AnimationCurve(rotationCurve));
	}

	void ConstructAnimationCurvesFromSkeleton(FbxNode* fbxNode, FbxAnimLayer* fbxAnimLayer, AnimationClip& clip, int& preInfType, int& postInfType, int keySpan)
	{
		if (fbxNode->GetSkeleton())
		{
			ReadClip(fbxNode, fbxAnimLayer, clip, preInfType, postInfType, keySpan);

			int childCount = fbxNode->GetChildCount();
			for (int childIndex = 0; childIndex < childCount; childIndex++)
			{
				FbxNode* child = fbxNode->GetChild(childIndex);
				ConstructAnimationCurvesFromSkeleton(child, fbxAnimLayer, clip, preInfType, postInfType, keySpan);
			}
		}
	}

	void ParseAnimation(FbxScene* scene, FbxNode* node)
	{
		auto stackCount = scene->GetSrcObjectCount<FbxAnimStack>();

		for (int i = 0; i < stackCount; i++)
		{
			auto stack = scene->GetSrcObject<FbxAnimStack>(i);
			auto stackName = stack->GetName();
			auto takeinfo = scene->GetTakeInfo(stackName);

			auto layerCount = stack->GetMemberCount<FbxAnimLayer>();
			for (int j = 0; j < layerCount; j++)
			{
				auto layer = stack->GetMember<FbxAnimLayer>(j);
				auto layerName = layer->GetName();

				int keySpan = GetMaximumKeySpanFromSkeleton(node, layer);
				if (keySpan > 0)
				{
					int preInfType = 0;
					int postInfType = 0;
					AnimationClip clip;
					ConstructAnimationCurvesFromSkeleton(node, layer, clip, preInfType, postInfType, keySpan);
				}
			}
		}
	}

	FbxString GetTexturePath(FbxSurfaceMaterial* surfaceMaterial, const char* name)
	{
		auto fbxProperty = surfaceMaterial->FindProperty(name);
		if (fbxProperty.IsValid())
		{
			auto textureCount = fbxProperty.GetSrcObjectCount<FbxTexture>();
			if (textureCount > 0)
			{
				auto fileTexture = fbxProperty.GetSrcObject<FbxFileTexture>(0);
				if (fileTexture)
				{
					FbxString filename = FbxPathUtils::GetFileName(fileTexture->GetFileName());
					return filename;
				}
			}
		}

		return FbxString();
	}

	std::shared_ptr<Texture> LoadTexture(FbxSurfaceMaterial* surfaceMaterial, const char* name, const std::filesystem::path& fbxPath)
	{
		FbxString filename = GetTexturePath(surfaceMaterial, name);
		if (!filename.IsEmpty())
		{
			std::filesystem::path path = std::filesystem::path(fbxPath.parent_path()).append(filename.Buffer());

			if (std::filesystem::exists(path))
			{
				auto texture = AssetDatabase::instance()->loadAssetAtPath<Texture>(path);
				if (texture)
				{
					texture->apply();
					return texture;
				}
			}
		}

		return nullptr;
	}

	std::shared_ptr<Material>
	FBXImporter::LoadMaterialAttribute(AssetImporterContext& context, FbxSurfaceMaterial* surfaceMaterial, const std::filesystem::path& path)
	{
		auto material = std::make_shared<MeshStandardMaterial>();
		material->setName(surfaceMaterial->GetName());
		material->setColorMap(LoadTexture(surfaceMaterial, FbxSurfaceMaterial::sDiffuse, path));
		material->setNormalMap(LoadTexture(surfaceMaterial, FbxSurfaceMaterial::sNormalMap, path));
		material->setEmissiveMap(LoadTexture(surfaceMaterial, FbxSurfaceMaterial::sEmissive, path));

		if (!material->getNormalMap())
			material->setNormalMap(LoadTexture(surfaceMaterial, FbxSurfaceMaterial::sBump, path));

		if (material->getColorMap())
			context.addObjectToAsset(material->getColorMap()->getName(), material->getColorMap());
		if (material->getNormalMap())
			context.addObjectToAsset(material->getNormalMap()->getName(), material->getNormalMap());
		if (material->getEmissiveMap())
			context.addObjectToAsset(material->getEmissiveMap()->getName(), material->getEmissiveMap());
		if (material->getNormalMap())
			context.addObjectToAsset(material->getNormalMap()->getName(), material->getNormalMap());

		if (surfaceMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			auto phongMaterial = FbxCast<FbxSurfacePhong>(surfaceMaterial);
			auto diffuse = phongMaterial->Diffuse.Get();
			auto diffuseFactor = phongMaterial->DiffuseFactor.Get();
			auto specular = phongMaterial->Specular.Get();
			auto specularFactory = phongMaterial->SpecularFactor.Get();
			auto emissive = phongMaterial->Emissive.Get();
			auto emissiveFactor = phongMaterial->EmissiveFactor.Get();
			auto transparencyColor = phongMaterial->TransparentColor.Get();
			auto transparencyfactor = phongMaterial->TransparencyFactor.Get();
			auto shininess = phongMaterial->Shininess.Get();
			auto reflection = phongMaterial->Reflection.Get();
			auto reflectionfactor = phongMaterial->ReflectionFactor.Get();

			material->setRoughness(std::sqrt(2.0f / ((float)shininess + 2.0f)));
			material->setColor(math::float3((float)diffuse[0], (float)diffuse[1], (float)diffuse[2]) * (float)diffuseFactor);
			material->setEmissive(math::float3((float)emissive[0], (float)emissive[1], (float)emissive[2]));
			material->setEmissiveIntensity((float)emissiveFactor);
		}
		else if (surfaceMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
		{
			auto lambertMaterial = FbxCast<FbxSurfaceLambert>(surfaceMaterial);
			auto diffuse = lambertMaterial->Diffuse.Get();
			auto diffuseFactor = lambertMaterial->DiffuseFactor.Get();
			auto emissive = lambertMaterial->Emissive.Get();
			auto emissiveFactor = lambertMaterial->EmissiveFactor.Get();
			auto transparencyColor = lambertMaterial->TransparentColor.Get();
			auto transparencyfactor = lambertMaterial->TransparencyFactor.Get();

			material->setColor(math::float3((float)diffuse[0], (float)diffuse[1], (float)diffuse[2]) * (float)diffuseFactor);
			material->setEmissive(math::float3((float)emissive[0], (float)emissive[1], (float)emissive[2]));
			material->setEmissiveIntensity((float)emissiveFactor);
		}

		return material;
	}

	std::size_t
	FBXImporter::LoadMaterial(AssetImporterContext& context, FbxMesh* mesh, std::vector<std::shared_ptr<Material>>& materials, const std::filesystem::path& path)
	{
		if (mesh && mesh->GetNode())
		{
			auto node = mesh->GetNode();
			auto materialCount = mesh->GetElementMaterialCount();

			for (int materialIndex = 0; materialIndex < materialCount; materialIndex++)
				materials.push_back(LoadMaterialAttribute(context, node->GetMaterial(materialIndex), path));

			return materialCount;
		}

		return 0;
	}

	math::float3 ReadVertex(FbxMesh* mesh, int index)
	{
		auto fbxPoint = mesh->GetControlPoints()[index];

		math::float3 vertex;
		vertex.x = (float)(fbxPoint[0]);
		vertex.y = (float)(fbxPoint[1]);
		vertex.z = (float)(fbxPoint[2]);

		return vertex;
	}

	int ReadColor(FbxMesh* mesh, int pointIndex, int vertexCounter, math::float4& color)
	{
		if (mesh->GetElementVertexColorCount() < 1)
			return -1;

		auto vertexColor = mesh->GetElementVertexColor(0);
		auto mappingMode = vertexColor->GetMappingMode();
		auto index = mappingMode == FbxLayerElement::eByPolygonVertex ? vertexCounter : pointIndex;

		if (mappingMode == FbxLayerElement::eByControlPoint || mappingMode == FbxLayerElement::eByPolygonVertex)
		{
			switch (vertexColor->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:
			{
				auto fbxColor = vertexColor->GetDirectArray().GetAt(index);

				color.r = (float)(fbxColor.mRed);
				color.g = (float)(fbxColor.mGreen);
				color.b = (float)(fbxColor.mBlue);
				color.a = (float)(fbxColor.mAlpha);

				return index;
			}
			break;
			case FbxLayerElement::eIndexToDirect:
			{
				auto fbxColor = vertexColor->GetDirectArray().GetAt(index);

				color.r = (float)(fbxColor.mRed);
				color.g = (float)(fbxColor.mGreen);
				color.b = (float)(fbxColor.mBlue);
				color.a = (float)(fbxColor.mAlpha);

				return index;
			}
			break;
			}
		}

		return -1;
	}

	int ReadUv(FbxMesh* mesh, int pointIndex, int textureIndex, int layer, math::float2& uv)
	{
		if (mesh->GetElementUVCount() <= layer)
			return -1;

		auto element = mesh->GetElementUV(layer);
		auto mappingMode = element->GetMappingMode();
		auto index = mappingMode == FbxLayerElement::eByPolygonVertex ? textureIndex : pointIndex;

		if (mappingMode == FbxLayerElement::eByControlPoint || mappingMode == FbxLayerElement::eByPolygonVertex)
		{
			switch (element->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:
			{
				auto fbxTexcoord = element->GetDirectArray().GetAt(index);

				uv.x = (float)(fbxTexcoord[0]);
				uv.y = (float)(fbxTexcoord[1]);

				return index;
			}
			break;
			case FbxLayerElement::eIndexToDirect:
			{
				auto fbxTexcoord = element->GetDirectArray().GetAt(index);

				uv.x = (float)(fbxTexcoord[0]);
				uv.y = (float)(fbxTexcoord[1]);

				return index;
			}
			break;
			}
		}

		return -1;
	}

	int ReadNormal(FbxMesh* mesh, int pointIndex, int normalIndex, math::float3& normal)
	{
		if (mesh->GetElementNormalCount() < 1)
			return -1;

		auto element = mesh->GetElementNormal(0);
		auto mappingMode = element->GetMappingMode();
		auto index = mappingMode == FbxLayerElement::eByPolygonVertex ? normalIndex : pointIndex;

		if (mappingMode == FbxLayerElement::eByControlPoint || mappingMode == FbxLayerElement::eByPolygonVertex)
		{
			switch (element->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:
			{
				auto fbxNormal = element->GetDirectArray().GetAt(index);

				normal.x = (float)(fbxNormal[0]);
				normal.y = (float)(fbxNormal[1]);
				normal.z = (float)(fbxNormal[2]);

				return index;
			}
			break;
			case FbxLayerElement::eIndexToDirect:
			{
				auto id = element->GetIndexArray().GetAt(index);
				auto fbxNormal = element->GetDirectArray().GetAt(id);

				math::float3 normal;
				normal.x = (float)(fbxNormal[0]);
				normal.y = (float)(fbxNormal[1]);
				normal.z = (float)(fbxNormal[2]);

				return index;
			}
			break;
			}
		}

		return -1;
	}

	int ReadTangent(FbxMesh* mesh, int pointIndex, int tangentIndex, math::float3& tangent)
	{
		if (mesh->GetElementNormalCount() <= 1)
			return -1;

		auto element = mesh->GetElementTangent(0);
		auto mappingMode = element->GetMappingMode();
		auto index = mappingMode == FbxLayerElement::eByPolygonVertex ? tangentIndex : pointIndex;

		if (mappingMode == FbxLayerElement::eByControlPoint || mappingMode == FbxLayerElement::eByPolygonVertex)
		{
			switch (element->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:
			{
				auto fbxTangent = element->GetDirectArray().GetAt(index);

				tangent.x = (float)(fbxTangent[0]);
				tangent.y = (float)(fbxTangent[1]);
				tangent.z = (float)(fbxTangent[2]);

				return index;
			}
			break;
			case FbxLayerElement::eIndexToDirect:
			{
				auto id = element->GetIndexArray().GetAt(index);
				auto fbxTangent = element->GetDirectArray().GetAt(id);

				math::float3 tangent;
				tangent.x = (float)(fbxTangent[0]);
				tangent.y = (float)(fbxTangent[1]);
				tangent.z = (float)(fbxTangent[2]);

				return index;
			}
			break;
			}
		}

		return -1;
	}

	std::shared_ptr<GameObject>
	FBXImporter::ParseMesh(AssetImporterContext& context, FbxNode* node, const std::filesystem::path& path)
	{
		auto fbxMesh = node->GetMesh();
		if (fbxMesh)
		{
			int polygonCount = fbxMesh->GetPolygonCount();
			int polygonVertexCount = fbxMesh->GetPolygonVertexCount();
			int pointsCount = fbxMesh->GetControlPointsCount();

			math::float3s polygonVertices(pointsCount);
			math::float3s polygonNormals(polygonVertexCount);
			math::float2s polygonTexcoords(polygonVertexCount);
			std::vector<std::tuple<int, int, int>> polygonIndices(polygonVertexCount);

			for (int i = 0; i < pointsCount; i++)
				polygonVertices[i] = ReadVertex(fbxMesh, i);

			std::set<std::tuple<int, int, int>> polygonSet;
			std::map<std::tuple<int, int, int>, std::uint32_t> polygonMap;

			for (int i = 0, vertexCounter = 0; i < polygonCount; i++)
			{
				for (int j = 0; j < 3; j++, vertexCounter++)
				{
					math::float3 n;
					math::float2 uv;

					auto pointIndex = fbxMesh->GetPolygonVertex(i, j);
					auto normalIndex = ReadNormal(fbxMesh, pointIndex, vertexCounter, n);
					auto texcoordIndex = ReadUv(fbxMesh, pointIndex, fbxMesh->GetTextureUVIndex(i, j), 0, uv);
					auto tuple = std::make_tuple(pointIndex, normalIndex, texcoordIndex);

					if (normalIndex >= 0)
						polygonNormals[normalIndex] = n;

					if (texcoordIndex >= 0)
						polygonTexcoords[texcoordIndex] = uv;

					polygonIndices[vertexCounter] = tuple;
					polygonSet.insert(tuple);
				}
			}

			std::uint32_t idx = 0;

			auto vertices = math::float3s(polygonSet.size());
			auto normals = math::float3s(polygonSet.size());
			auto texcoords = math::float2s(polygonSet.size());
			auto indices = math::uint1s(polygonVertexCount);

			for (auto& index : polygonSet)
			{
				auto v = std::get<0>(index);
				auto n = std::get<1>(index);
				auto uv = std::get<2>(index);

				vertices[idx] = polygonVertices[v];
				normals[idx] = polygonNormals[n];
				texcoords[idx] = polygonTexcoords[uv];

				polygonMap[index] = idx++;
			}

			std::size_t polygonIndexCount = polygonIndices.size();
			for (std::size_t i = 0; i < polygonIndexCount; i++)
				indices[i] = polygonMap.at(polygonIndices[i]);

			auto mesh = std::make_shared<Mesh>();
			mesh->setName(node->GetName());
			mesh->setVertexArray(std::move(vertices));
			mesh->setNormalArray(std::move(normals));
			mesh->setTexcoordArray(std::move(texcoords));
			mesh->setIndicesArray(std::move(indices));

			context.addObjectToAsset(node->GetName(), mesh);

			auto gameObject = std::make_shared<GameObject>();
			gameObject->setName(node->GetName());
			gameObject->addComponent<MeshFilterComponent>(std::move(mesh));

			auto meshRenderer = gameObject->addComponent<MeshRendererComponent>();
			meshRenderer->setGlobalIllumination(true);

			if (fbxMesh->GetElementMaterialCount() > 0)
			{
				std::vector<std::shared_ptr<Material>> materials;
				LoadMaterial(context, fbxMesh, materials, path);

				for (std::size_t i = 0; i < materials.size(); i++)
				{
					if (materials[i])
					{
						auto& material = materials[i];
						context.addObjectToAsset(material->getName(), material);
						meshRenderer->setMaterial(std::move(material), i);
					}
					else
					{
						std::ostringstream buffer;
						buffer << "Standard Material";
						buffer << i;
						auto material = std::make_shared<MeshStandardMaterial>(buffer.str());
						context.addObjectToAsset(material->getName(), material);
						meshRenderer->setMaterial(std::move(material), i);
					}
				}
			}
			else
			{
				auto material = std::make_shared<MeshStandardMaterial>("Standard Material");
				context.addObjectToAsset(material->getName(), material);
				meshRenderer->setMaterial(std::move(material));
			}

			auto translation = node->LclTranslation.Get();
			auto rotation = node->LclRotation.Get();
			auto scaling = node->LclScaling.Get();

			auto transform = gameObject->getComponent<TransformComponent>();
			transform->setLocalTranslate(math::float3((float)translation[0], (float)translation[1], (float)translation[2]));
			transform->setLocalEulerAngles(math::float3((float)rotation[0], (float)rotation[1], (float)rotation[2]));
			transform->setLocalScale(math::float3((float)scaling[0], (float)scaling[1], (float)scaling[2]));

			return gameObject;
		}

		return nullptr;
	}

	std::shared_ptr<GameObject> ParseCamera(FbxScene* scene, FbxNode* node)
	{
		auto camera = node->GetCamera();
		if (camera)
		{
			auto stackCount = scene->GetSrcObjectCount<FbxAnimStack>();

			for (int i = 0; i < stackCount; i++)
			{
				auto stack = scene->GetSrcObject<FbxAnimStack>(i);
				auto stackName = stack->GetName();
				auto takeinfo = scene->GetTakeInfo(stackName);

				FbxTime start = takeinfo->mLocalTimeSpan.GetStart();
				FbxTime end = takeinfo->mLocalTimeSpan.GetStop();

				for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames30); i <= end.GetFrameCount(FbxTime::eFrames30); ++i)
				{
					FbxTime time;
					time.SetFrame(i, FbxTime::eFrames30);

					auto layerCount = stack->GetMemberCount<FbxAnimLayer>();
					for (int j = 0; j < layerCount; j++)
					{
						auto layer = stack->GetMember<FbxAnimLayer>(j);
						auto layerName = layer->GetName();

						auto pixelAspectRatio = camera->PixelAspectRatio.GetCurve(layer);
						if (pixelAspectRatio)
						{
							pixelAspectRatio->Evaluate(time);
						}
					}
				}
			}
		}

		return std::make_shared<GameObject>();
	}

	std::shared_ptr<GameObject> ParseSkeleton(FbxNode* node)
	{
		auto skeleton = node->GetSkeleton();
		if (skeleton)
		{
			auto object = std::make_shared<GameObject>();
			object->setName(skeleton->GetName());

			for (int j = 0; j < node->GetChildCount(); j++)
			{
				auto child = ParseSkeleton(node->GetChild(j));
				object->addChild(child);
			}

			return object;
		}

		return nullptr;
	}

	GameObjectPtr
	FBXImporter::ProcessNode(AssetImporterContext& context, FbxScene* scene, FbxNode* node, const std::filesystem::path& path)
	{
		GameObjectPtr object;

		auto attribute = node->GetNodeAttribute();
		if (attribute)
		{
			auto type = attribute->GetAttributeType();
			switch (type)
			{
			case FbxNodeAttribute::eCamera:
				object = ParseCamera(scene, node);
				break;
			case FbxNodeAttribute::eMesh:
				object = ParseMesh(context, node, path);
				break;
			case FbxNodeAttribute::eSkeleton:
				//ParseAnimation(scene, node);
				object = ParseSkeleton(node);
				break;
			default:
				object = std::make_shared<GameObject>();

				for (int j = 0; j < node->GetChildCount(); j++)
				{
					auto child = ProcessNode(context, scene, node->GetChild(j), path);
					context.addObjectToAsset(child->getName(), child);
					object->addChild(child);
				}
				break;
			}
		}

		return object;
	}

	void
	FBXImporter::onImportAsset(AssetImporterContext& context) noexcept(false)
	{
		auto lsdkManager = FbxManager::Create();
		if (lsdkManager)
		{
			FbxIOSettings* ios = FbxIOSettings::Create(lsdkManager, IOSROOT);
			lsdkManager->SetIOSettings(ios);

			FbxString extension = "dll";
			FbxString path = FbxGetApplicationDirectory();
			lsdkManager->LoadPluginsDirectory(path.Buffer(), extension.Buffer());

			FbxImporter* onImportAsset = FbxImporter::Create(lsdkManager, "");

			auto filepath = AssetDatabase::instance()->getAbsolutePath(context.getAssetPath());
			if (onImportAsset->Initialize((char*)filepath.u8string().c_str(), -1, lsdkManager->GetIOSettings()))
			{
				int major = 0, minor = 0, revision = 0;
				onImportAsset->GetFileVersion(major, minor, revision);

				FbxScene* scene = FbxScene::Create(lsdkManager, "myScene");
				onImportAsset->Import(scene);
				onImportAsset->Destroy();

				FbxArray<FbxString*> animStackNameArray;
				scene->FillAnimStackNameArray(animStackNameArray);

				FbxNode* rootNode = scene->GetRootNode();
				if (rootNode)
				{
					auto object = std::make_shared<GameObject>();

					for (int i = 0; i < rootNode->GetChildCount(); i++)
					{
						auto node = ProcessNode(context, scene, rootNode->GetChild(i), filepath);
						context.addObjectToAsset(node->getName(), node);
						object->addChild(std::move(node));
					}
					
					if (object->getChildCount() > 1)
						context.setMainObject(object);
					else
						context.setMainObject(object->getChild(0));
				}
			}
			else
			{
				printf("Call to FbxImporter::Initialize() failed.\n");
				printf("Error returned: %s\n\n", onImportAsset->GetStatus().GetErrorString());
			}

			lsdkManager->Destroy();
		}
	}

	void importerNode(FbxNode* node, std::vector<std::filesystem::path>& dependencies)
	{
		GameObjectPtr object;

		auto attribute = node->GetNodeAttribute();
		if (attribute)
		{
			switch (attribute->GetAttributeType())
			{
			case FbxNodeAttribute::eMesh:
			{
				auto fbxMesh = node->GetMesh();
				if (fbxMesh)
				{
					if (fbxMesh->GetElementMaterialCount() > 0)
					{
						auto node = fbxMesh->GetNode();
						auto materialCount = fbxMesh->GetElementMaterialCount();

						for (int materialIndex = 0; materialIndex < materialCount; materialIndex++)
						{
							auto fbxMaterial = node->GetMaterial(materialIndex);
							auto diffuseMap = GetTexturePath(fbxMaterial, FbxSurfaceMaterial::sDiffuse);
							if (!diffuseMap.IsEmpty())
								dependencies.push_back(diffuseMap.Buffer());
							auto normalMapMap = GetTexturePath(fbxMaterial, FbxSurfaceMaterial::sNormalMap);
							if (!normalMapMap.IsEmpty())
								dependencies.push_back(normalMapMap.Buffer());
							auto emissiveMap = GetTexturePath(fbxMaterial, FbxSurfaceMaterial::sEmissive);
							if (!emissiveMap.IsEmpty())
								dependencies.push_back(emissiveMap.Buffer());
							auto bumpMap = GetTexturePath(fbxMaterial, FbxSurfaceMaterial::sBump);
							if (!bumpMap.IsEmpty())
								dependencies.push_back(bumpMap.Buffer());
						}
					}
				}
			}
			break;
			}
		}

		for (int j = 0; j < node->GetChildCount(); j++)
			importerNode(node->GetChild(j), dependencies);
	}

	std::vector<std::filesystem::path>
	FBXImporter::getDependencies(const std::filesystem::path& filepath) noexcept(false)
	{
		std::vector<std::filesystem::path> dependencies;

		auto lsdkManager = FbxManager::Create();
		if (lsdkManager)
		{
			FbxIOSettings* ios = FbxIOSettings::Create(lsdkManager, IOSROOT);
			lsdkManager->SetIOSettings(ios);

			FbxString extension = "dll";
			FbxString path = FbxGetApplicationDirectory();
			lsdkManager->LoadPluginsDirectory(path.Buffer(), extension.Buffer());

			FbxImporter* onImportAsset = FbxImporter::Create(lsdkManager, "");

			if (onImportAsset->Initialize((char*)filepath.u8string().c_str(), -1, lsdkManager->GetIOSettings()))
			{
				int major = 0, minor = 0, revision = 0;
				onImportAsset->GetFileVersion(major, minor, revision);

				FbxScene* scene = FbxScene::Create(lsdkManager, "myScene");
				onImportAsset->Import(scene);
				onImportAsset->Destroy();

				FbxNode* rootNode = scene->GetRootNode();
				if (rootNode)
				{
					auto object = std::make_shared<GameObject>();

					for (int i = 0; i < rootNode->GetChildCount(); i++)
						importerNode(rootNode->GetChild(i), dependencies);
				}
			}
			else
			{
				printf("Call to FbxImporter::Initialize() failed.\n");
				printf("Error returned: %s\n\n", onImportAsset->GetStatus().GetErrorString());
			}

			lsdkManager->Destroy();
		}

		return dependencies;
	}
}