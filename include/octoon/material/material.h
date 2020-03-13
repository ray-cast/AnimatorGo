#ifndef OCTOON_MODEL_MATERIAL_H_
#define OCTOON_MODEL_MATERIAL_H_

#include <octoon/model/modtypes.h>

#define MATKEY_NAME               "$mat.name"
#define MATKEY_PATH               "$mat.path"
#define MATKEY_TWOSIDED           "$mat.twosided"
#define MATKEY_SHADING_MODEL      "$mat.shadingm"
#define MATKEY_ENABLE_WIREFRAME   "$mat.wireframe"
#define MATKEY_BLEND_FUNC         "$mat.blend"
#define MATKEY_OPACITY            "$mat.opacity"
#define MATKEY_BUMPSCALING        "$mat.bumpscaling"
#define MATKEY_SHININESS          "$mat.shininess"
#define MATKEY_REFLECTIVITY       "$mat.reflectivity"
#define MATKEY_SHININESS_STRENGTH "$mat.shinpercent"
#define MATKEY_REFRACTI           "$mat.refracti"
#define MATKEY_EFFECT             "$mat.effect"
#define MATKEY_COLOR_DIFFUSE      "$clr.diffuse"
#define MATKEY_COLOR_AMBIENT      "$clr.ambient"
#define MATKEY_COLOR_SPECULAR     "$clr.specular"
#define MATKEY_COLOR_EMISSIVE     "$clr.emissive"
#define MATKEY_COLOR_SPHEREMAP    "$clr.spheremap"
#define MATKEY_COLOR_TRANSPARENT  "$clr.transparent"
#define MATKEY_COLOR_REFLECTIVE   "$clr.reflective"
#define MATKEY_COLOR_EDGE		  "$clr.edge"

#define MATKEY_TEXTURE_DIFFUSE	    "$tex.diffuse"
#define MATKEY_TEXTURE_SPECULAR	    "$tex.specular"
#define MATKEY_TEXTURE_AMBIENT	    "$tex.ambient"
#define MATKEY_TEXTURE_EMISSIVE	    "$tex.emissive"
#define MATKEY_TEXTURE_NORMALS	    "$tex.normals"
#define MATKEY_TEXTURE_HEIGHT	    "$tex.height"
#define MATKEY_TEXTURE_SHININESS	"$tex.shininess"
#define MATKEY_TEXTURE_OPACITY	    "$tex.opacity"
#define MATKEY_TEXTURE_DISPLACEMENT	"$tex.displacement"
#define MATKEY_TEXTURE_LIGHTMAP	    "$tex.lightmap"
#define MATKEY_TEXTURE_REFLECTION	"$tex.reflection"
#define MATKEY_TEXTURE_TOON			"$tex.toon"

namespace octoon
{
	namespace model
	{
		class OCTOON_EXPORT Material final
		{
		public:
			struct MaterialParam
			{
				std::string key;

				std::size_t length;
				std::size_t dataType;

				char* data;
			};

		public:
			Material() noexcept;
			~Material() noexcept;

			bool set(const char* key, int value) noexcept;
			bool set(const char* key, float value) noexcept;
			bool set(const char* key, const math::Vector3& value) noexcept;
			bool set(const char* key, const math::Vector4& value) noexcept;
			bool set(const char* key, const char* value) noexcept;
			bool set(const char* key, const unsigned char* value) noexcept;
			bool set(const char* key, const std::string& value) noexcept;
			bool set(const MaterialParam& value) noexcept;

			bool get(const char* key, int& value) const noexcept;
			bool get(const char* key, float& value) const noexcept;
			bool get(const char* key, math::Vector3& value) const noexcept;
			bool get(const char* key, math::Vector4& value) const noexcept;
			bool get(const char* key, std::string& value) const noexcept;
			bool get(const char* key, MaterialParam& out) const noexcept;

			std::size_t hash() const noexcept;

			MaterialPtr clone() const noexcept;

		private:
			std::vector<MaterialParam> _properties;
		};
	}
}

#endif