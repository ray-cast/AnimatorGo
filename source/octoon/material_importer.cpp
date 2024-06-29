#include <octoon/material_importer.h>
#include <octoon/material/mesh_standard_material.h>
#include <octoon/asset_database.h>

#include <fstream>

namespace octoon
{
	OctoonImplementSubClass(MaterialImporter, AssetImporter, "MaterialImporter")

	MaterialImporter::MaterialImporter() noexcept
	{
	}

	MaterialImporter::~MaterialImporter() noexcept
	{
	}

	void
	MaterialImporter::onImportAsset(AssetImporterContext& context) noexcept(false)
	{
		auto filepath = AssetDatabase::instance()->getAbsolutePath(context.getAssetPath());
		std::ifstream ifs(filepath);
		if (ifs)
		{
			auto mat = nlohmann::json::parse(ifs);

			auto material = std::make_shared<MeshStandardMaterial>();
			auto name = mat.find("name");
			auto colorMap = mat.find("colorMap");
			auto opacityMap = mat.find("opacityMap");
			auto normalMap = mat.find("normalMap");
			auto roughnessMap = mat.find("roughnessMap");
			auto specularMap = mat.find("specularMap");
			auto metalnessMap = mat.find("metalnessMap");
			auto emissiveMap = mat.find("emissiveMap");
			auto anisotropyMap = mat.find("anisotropyMap");
			auto clearCoatMap = mat.find("clearCoatMap");
			auto clearCoatRoughnessMap = mat.find("clearCoatRoughnessMap");
			auto subsurfaceMap = mat.find("subsurfaceMap");
			auto subsurfaceColorMap = mat.find("subsurfaceColorMap");
			auto sheenMap = mat.find("sheenMap");
			auto lightMap = mat.find("lightMap");

			if (name != mat.end() && (*name).is_string())
				material->setName((*name).get<std::string>());
			if (colorMap != mat.end() && (*colorMap).is_string())
				material->setColorMap(AssetDatabase::instance()->loadAssetAtPath<Texture>(AssetDatabase::instance()->getAssetPath(*colorMap)));
			if (opacityMap != mat.end() && (*opacityMap).is_string())
				material->setOpacityMap(AssetDatabase::instance()->loadAssetAtPath<Texture>(AssetDatabase::instance()->getAssetPath(*opacityMap)));
			if (normalMap != mat.end() && (*normalMap).is_string())
				material->setNormalMap(AssetDatabase::instance()->loadAssetAtPath<Texture>(AssetDatabase::instance()->getAssetPath(*normalMap)));
			if (roughnessMap != mat.end() && (*roughnessMap).is_string())
				material->setRoughnessMap(AssetDatabase::instance()->loadAssetAtPath<Texture>(AssetDatabase::instance()->getAssetPath(*roughnessMap)));
			if (specularMap != mat.end() && (*specularMap).is_string())
				material->setSpecularMap(AssetDatabase::instance()->loadAssetAtPath<Texture>(AssetDatabase::instance()->getAssetPath(*specularMap)));
			if (metalnessMap != mat.end() && (*metalnessMap).is_string())
				material->setMetalnessMap(AssetDatabase::instance()->loadAssetAtPath<Texture>(AssetDatabase::instance()->getAssetPath(*metalnessMap)));
			if (emissiveMap != mat.end() && (*emissiveMap).is_string())
				material->setEmissiveMap(AssetDatabase::instance()->loadAssetAtPath<Texture>(AssetDatabase::instance()->getAssetPath(*emissiveMap)));
			if (anisotropyMap != mat.end() && (*anisotropyMap).is_string())
				material->setAnisotropyMap(AssetDatabase::instance()->loadAssetAtPath<Texture>(AssetDatabase::instance()->getAssetPath(*anisotropyMap)));
			if (clearCoatMap != mat.end() && (*clearCoatMap).is_string())
				material->setClearCoatMap(AssetDatabase::instance()->loadAssetAtPath<Texture>(AssetDatabase::instance()->getAssetPath(*clearCoatMap)));
			if (clearCoatRoughnessMap != mat.end() && (*clearCoatRoughnessMap).is_string())
				material->setClearCoatRoughnessMap(AssetDatabase::instance()->loadAssetAtPath<Texture>(AssetDatabase::instance()->getAssetPath(*clearCoatRoughnessMap)));
			if (subsurfaceMap != mat.end() && (*subsurfaceMap).is_string())
				material->setSubsurfaceMap(AssetDatabase::instance()->loadAssetAtPath<Texture>(AssetDatabase::instance()->getAssetPath(*subsurfaceMap)));
			if (subsurfaceColorMap != mat.end() && (*subsurfaceColorMap).is_string())
				material->setSubsurfaceColorMap(AssetDatabase::instance()->loadAssetAtPath<Texture>(AssetDatabase::instance()->getAssetPath(*subsurfaceColorMap)));
			if (sheenMap != mat.end() && (*sheenMap).is_string())
				material->setSheenMap(AssetDatabase::instance()->loadAssetAtPath<Texture>(AssetDatabase::instance()->getAssetPath(*sheenMap)));
			if (lightMap != mat.end() && (*lightMap).is_string())
				material->setLightMap(AssetDatabase::instance()->loadAssetAtPath<Texture>(AssetDatabase::instance()->getAssetPath(*lightMap)));

			auto blendEnable = mat.find("blendEnable");
			auto blendOp = mat.find("blendOp");
			auto blendSrc = mat.find("blendSrc");
			auto blendDest = mat.find("blendDest");
			auto blendAlphaOp = mat.find("blendAlphaOp");
			auto blendAlphaSrc = mat.find("blendAlphaSrc");
			auto blendAlphaDest = mat.find("blendAlphaDest");

			if (blendEnable != mat.end() && (*blendEnable).is_boolean())
				material->setBlendEnable((*blendEnable).get<nlohmann::json::boolean_t>());
			if (blendOp != mat.end() && (*blendOp).is_number_unsigned())
				material->setBlendOp((BlendOp)(*blendOp).get<nlohmann::json::number_unsigned_t>());
			if (blendSrc != mat.end() && (*blendSrc).is_number_unsigned())
				material->setBlendSrc((BlendMode)(*blendSrc).get<nlohmann::json::number_unsigned_t>());
			if (blendDest != mat.end() && (*blendDest).is_number_unsigned())
				material->setBlendDest((BlendMode)(*blendDest).get<nlohmann::json::number_unsigned_t>());
			if (blendAlphaOp != mat.end() && (*blendAlphaOp).is_number_unsigned())
				material->setBlendAlphaOp((BlendOp)(*blendAlphaOp).get<nlohmann::json::number_unsigned_t>());
			if (blendAlphaSrc != mat.end() && (*blendAlphaSrc).is_number_unsigned())
				material->setBlendAlphaSrc((BlendMode)(*blendAlphaSrc).get<nlohmann::json::number_unsigned_t>());
			if (blendAlphaDest != mat.end() && (*blendAlphaDest).is_number_unsigned())
				material->setBlendAlphaDest((BlendMode)(*blendAlphaDest).get<nlohmann::json::number_unsigned_t>());

			auto depthEnable = mat.find("depthEnable");
			auto depthBiasEnable = mat.find("depthBiasEnable");
			auto depthBoundsEnable = mat.find("depthBoundsEnable");
			auto depthClampEnable = mat.find("depthClampEnable");
			auto depthWriteEnable = mat.find("depthWriteEnable");
			auto stencilEnable = mat.find("stencilEnable");
			auto scissorTestEnable = mat.find("scissorTestEnable");

			if (depthEnable != mat.end() && (*depthEnable).is_boolean())
				material->setDepthEnable((*depthEnable).get<nlohmann::json::boolean_t>());
			if (depthBiasEnable != mat.end() && (*depthBiasEnable).is_boolean())
				material->setDepthBiasEnable((*depthBiasEnable).get<nlohmann::json::boolean_t>());
			if (depthBoundsEnable != mat.end() && (*depthBoundsEnable).is_boolean())
				material->setDepthBoundsEnable((*depthBoundsEnable).get<nlohmann::json::boolean_t>());
			if (depthClampEnable != mat.end() && (*depthClampEnable).is_boolean())
				material->setDepthClampEnable((*depthClampEnable).get<nlohmann::json::boolean_t>());
			if (depthWriteEnable != mat.end() && (*depthWriteEnable).is_boolean())
				material->setDepthWriteEnable((*depthWriteEnable).get<nlohmann::json::boolean_t>());
			if (stencilEnable != mat.end() && (*stencilEnable).is_boolean())
				material->setStencilEnable((*stencilEnable).get<nlohmann::json::boolean_t>());
			if (scissorTestEnable != mat.end() && (*scissorTestEnable).is_boolean())
				material->setScissorTestEnable((*scissorTestEnable).get<nlohmann::json::boolean_t>());

			auto emissiveIntensity = mat.find("emissiveIntensity");
			auto opacity = mat.find("opacity");
			auto smoothness = mat.find("smoothness");
			auto roughness = mat.find("roughness");
			auto metalness = mat.find("metalness");
			auto anisotropy = mat.find("anisotropy");
			auto sheen = mat.find("sheen");
			auto specular = mat.find("specular");
			auto refractionRatio = mat.find("refractionRatio");
			auto clearCoat = mat.find("clearCoat");
			auto clearCoatRoughness = mat.find("clearCoatRoughness");
			auto subsurface = mat.find("subsurface");
			auto reflectionRatio = mat.find("reflectionRatio");
			auto transmission = mat.find("transmission");
			auto lightMapIntensity = mat.find("lightMapIntensity");
			auto gamma = mat.find("gamma");
			auto depthMin = mat.find("depthMin");
			auto depthMax = mat.find("depthMax");
			auto depthBias = mat.find("depthBias");
			auto depthSlopeScaleBias = mat.find("depthSlopeScaleBias");

			if (emissiveIntensity != mat.end() && (*emissiveIntensity).is_number_float())
				material->setEmissiveIntensity((*emissiveIntensity).get<nlohmann::json::number_float_t>());
			if (opacity != mat.end() && (*opacity).is_number_float())
				material->setOpacity((*opacity).get<nlohmann::json::number_float_t>());
			if (smoothness != mat.end() && (*smoothness).is_number_float())
				material->setSmoothness((*smoothness).get<nlohmann::json::number_float_t>());
			if (roughness != mat.end() && (*roughness).is_number_float())
				material->setRoughness((*roughness).get<nlohmann::json::number_float_t>());
			if (metalness != mat.end() && (*metalness).is_number_float())
				material->setMetalness((*metalness).get<nlohmann::json::number_float_t>());
			if (anisotropy != mat.end() && (*anisotropy).is_number_float())
				material->setAnisotropy((*anisotropy).get<nlohmann::json::number_float_t>());
			if (sheen != mat.end() && (*sheen).is_number_float())
				material->setSheen((*sheen).get<nlohmann::json::number_float_t>());
			if (specular != mat.end() && (*specular).is_number_float())
				material->setSpecular((*specular).get<nlohmann::json::number_float_t>());
			if (refractionRatio != mat.end() && (*refractionRatio).is_number_float())
				material->setRefractionRatio((*refractionRatio).get<nlohmann::json::number_float_t>());
			if (clearCoat != mat.end() && (*clearCoat).is_number_float())
				material->setClearCoat((*clearCoat).get<nlohmann::json::number_float_t>());
			if (clearCoatRoughness != mat.end() && (*clearCoatRoughness).is_number_float())
				material->setClearCoatRoughness((*clearCoatRoughness).get<nlohmann::json::number_float_t>());
			if (subsurface != mat.end() && (*subsurface).is_number_float())
				material->setSubsurface((*subsurface).get<nlohmann::json::number_float_t>());
			if (reflectionRatio != mat.end() && (*reflectionRatio).is_number_float())
				material->setReflectionRatio((*reflectionRatio).get<nlohmann::json::number_float_t>());
			if (transmission != mat.end() && (*transmission).is_number_float())
				material->setTransmission((*transmission).get<nlohmann::json::number_float_t>());
			if (lightMapIntensity != mat.end() && (*lightMapIntensity).is_number_float())
				material->setLightMapIntensity((*lightMapIntensity).get<nlohmann::json::number_float_t>());
			if (gamma != mat.end() && (*gamma).is_number_float())
				material->setGamma((*gamma).get<nlohmann::json::number_float_t>());
			if (depthMin != mat.end() && (*depthMin).is_number_float())
				material->setDepthMin((*depthMin).get<nlohmann::json::number_float_t>());
			if (depthMax != mat.end() && (*depthMax).is_number_float())
				material->setDepthMax((*depthMax).get<nlohmann::json::number_float_t>());
			if (depthBias != mat.end() && (*depthBias).is_number_float())
				material->setDepthBias((*depthBias).get<nlohmann::json::number_float_t>());
			if (depthSlopeScaleBias != mat.end() && (*depthSlopeScaleBias).is_number_float())
				material->setDepthSlopeScaleBias((*depthSlopeScaleBias).get<nlohmann::json::number_float_t>());

			auto offset = mat.find("offset");
			auto repeat = mat.find("repeat");
			auto normalScale = mat.find("normalScale");
			auto color = mat.find("color");
			auto emissive = mat.find("emissive");
			auto subsurfaceColor = mat.find("subsurfaceColor");

			if (offset != mat.end() && (*offset).is_array())
				material->setOffset(math::float2((*offset).get<std::array<float, 2>>()));
			if (repeat != mat.end() && (*repeat).is_array())
				material->setRepeat(math::float2((*repeat).get<std::array<float, 2>>()));
			if (normalScale != mat.end() && (*normalScale).is_array())
				material->setNormalScale(math::float2((*normalScale).get<std::array<float, 2>>()));
			if (color != mat.end() && (*color).is_array())
				material->setColor(math::float3((*color).get<std::array<float, 3>>()));
			if (emissive != mat.end() && (*emissive).is_array())
				material->setEmissive(math::float3((*emissive).get<std::array<float, 3>>()));
			if (subsurfaceColor != mat.end() && (*subsurfaceColor).is_array())
				material->setSubsurfaceColor(math::float3((*subsurfaceColor).get<std::array<float, 3>>()));

			context.setMainObject(material);
		}
	}
}