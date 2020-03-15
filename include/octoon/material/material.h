#ifndef OCTOON_MODEL_MATERIAL_H_
#define OCTOON_MODEL_MATERIAL_H_

#include <octoon/math/math.h>
#include <octoon/hal/graphics_state.h>

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

#define MATKEY_SHADER_VERT			"$shader.vert"
#define MATKEY_SHADER_FRAG			"$shader.frag"

namespace octoon::material
{
	enum PropertyTypeInfo
	{
		PropertyTypeInfoFloat = 0x01,
		PropertyTypeInfoString = 0x02,
		PropertyTypeInfoInt = 0x04,
		PropertyTypeInfoBuffer = 0x08,
		PropertyTypeInfoTexture = 0x16,
	};

	class OCTOON_EXPORT Material : public runtime::RttiInterface
	{
		OctoonDeclareSubClass(Material, runtime::RttiInterface);
	public:
		struct MaterialParam
		{
			std::string key;

			std::size_t length;
			std::size_t type;

			char* data;
			hal::GraphicsTexturePtr texture;
		};

	public:
		Material() noexcept;
		virtual ~Material() noexcept;

		void setName(std::string_view name) noexcept;
		const std::string& getName() const noexcept;

		void setColorBlends(hal::GraphicsColorBlends&& blends) noexcept;
		void setColorBlends(const hal::GraphicsColorBlends& blends) noexcept;
		hal::GraphicsColorBlends& getColorBlends() noexcept;
		const hal::GraphicsColorBlends& getColorBlends() const noexcept;

		void setCullMode(hal::GraphicsCullMode mode) noexcept;
		void setPolygonMode(hal::GraphicsPolygonMode mode) noexcept;
		void setPrimitiveType(hal::GraphicsVertexType type) noexcept;
		void setFrontFace(hal::GraphicsFrontFace face) noexcept;
		void setScissorTestEnable(bool enable) noexcept;
		void setLinear2sRGBEnable(bool enable) noexcept;
		void setMultisampleEnable(bool enable) noexcept;
		void setRasterizerDiscardEnable(bool enable) noexcept;
		void setLineWidth(float width) noexcept;

		void setDepthEnable(bool enable) noexcept;
		void setDepthWriteEnable(bool enable) noexcept;
		void setDepthBoundsEnable(bool enable) noexcept;
		void setDepthMin(float mix) noexcept;
		void setDepthMax(float max) noexcept;
		void setDepthFunc(hal::GraphicsCompareFunc func) noexcept;
		void setDepthBiasEnable(bool enable) noexcept;
		void setDepthBias(float bias) noexcept;
		void setDepthSlopeScaleBias(float bias) noexcept;
		void setDepthBiasClamp(bool bias) noexcept;
		void setDepthClampEnable(bool enable) noexcept;

		void setStencilEnable(bool enable) noexcept;
		void setStencilFrontRef(std::uint32_t ref) noexcept;
		void setStencilFrontFunc(hal::GraphicsCompareFunc func) noexcept;
		void setStencilFrontReadMask(std::uint32_t mask) noexcept;
		void setStencilFrontWriteMask(std::uint32_t mask) noexcept;
		void setStencilFrontFail(hal::GraphicsStencilOp stencilOp) noexcept;
		void setStencilFrontZFail(hal::GraphicsStencilOp stencilOp) noexcept;
		void setStencilFrontPass(hal::GraphicsStencilOp stencilOp) noexcept;
		void setStencilBackRef(std::uint32_t ref) noexcept;
		void setStencilBackFunc(hal::GraphicsCompareFunc func) noexcept;
		void setStencilBackReadMask(std::uint32_t mask) noexcept;
		void setStencilBackWriteMask(std::uint32_t mask) noexcept;
		void setStencilBackFail(hal::GraphicsStencilOp stencilOp) noexcept;
		void setStencilBackZFail(hal::GraphicsStencilOp stencilOp) noexcept;
		void setStencilBackPass(hal::GraphicsStencilOp stencilOp) noexcept;

		hal::GraphicsCullMode getCullMode() const noexcept;
		hal::GraphicsPolygonMode getPolygonMode() const noexcept;
		hal::GraphicsVertexType getPrimitiveType() const noexcept;
		hal::GraphicsFrontFace getFrontFace() const noexcept;
		bool getScissorTestEnable() const noexcept;
		bool getLinear2sRGBEnable() const noexcept;
		bool getMultisampleEnable() const noexcept;
		bool getRasterizerDiscardEnable() const noexcept;
		float getLineWidth() const noexcept;

		bool getDepthEnable() const noexcept;
		bool getDepthWriteEnable() const noexcept;
		bool getDepthBoundsEnable() const noexcept;
		bool getDepthBiasEnable() const noexcept;
		bool getDepthBiasClamp() const noexcept;
		bool getDepthClampEnable() const noexcept;
		float getDepthMin() const noexcept;
		float getDepthMax() const noexcept;
		float getDepthBias() const noexcept;
		float getDepthSlopeScaleBias() const noexcept;
		hal::GraphicsCompareFunc getDepthFunc() const noexcept;

		bool getStencilEnable() const noexcept;
		hal::GraphicsCompareFunc getStencilFrontFunc() const noexcept;
		std::uint32_t getStencilFrontRef() const noexcept;
		std::uint32_t getStencilFrontReadMask() const noexcept;
		std::uint32_t getStencilFrontWriteMask() const noexcept;
		hal::GraphicsStencilOp getStencilFrontFail() const noexcept;
		hal::GraphicsStencilOp getStencilFrontZFail() const noexcept;
		hal::GraphicsStencilOp getStencilFrontPass() const noexcept;
		hal::GraphicsCompareFunc getStencilBackFunc() const noexcept;
		std::uint32_t getStencilBackRef() const noexcept;
		std::uint32_t getStencilBackReadMask() const noexcept;
		std::uint32_t getStencilBackWriteMask() const noexcept;
		hal::GraphicsStencilOp getStencilBackFail() const noexcept;
		hal::GraphicsStencilOp getStencilBackZFail() const noexcept;
		hal::GraphicsStencilOp getStencilBackPass() const noexcept;

		bool set(std::string_view key, int value) noexcept;
		bool set(std::string_view key, float value) noexcept;
		bool set(std::string_view key, const math::Vector3& value) noexcept;
		bool set(std::string_view key, const math::Vector4& value) noexcept;
		bool set(std::string_view key, std::string_view value) noexcept;
		bool set(std::string_view key, const hal::GraphicsTexturePtr& value) noexcept;
		bool set(const MaterialParam& value) noexcept;

		bool get(std::string_view key, int& value) const noexcept;
		bool get(std::string_view key, float& value) const noexcept;
		bool get(std::string_view key, math::Vector3& value) const noexcept;
		bool get(std::string_view key, math::Vector4& value) const noexcept;
		bool get(std::string_view key, std::string& value) const noexcept;
		bool get(std::string_view key, hal::GraphicsTexturePtr& value) const noexcept;
		bool get(std::string_view key, MaterialParam& out) const noexcept;

		std::size_t hash() const noexcept;

		virtual std::shared_ptr<Material> clone() const noexcept;

	private:
		std::string name_;

		bool _enableScissorTest;
		bool _enableSrgb;
		bool _enableMultisample;
		bool _enableRasterizerDiscard;
		bool _enableDepth;
		bool _enableDepthWrite;
		bool _enableDepthBounds;
		bool _enableDepthClamp;
		bool _enableDepthBias;
		bool _enableDepthBiasClamp;
		bool _enableStencil;

		float _lineWidth;

		hal::GraphicsCullMode    _cullMode;
		hal::GraphicsPolygonMode _polygonMode;
		hal::GraphicsVertexType  _primitiveType;
		hal::GraphicsFrontFace   _frontFace;

		float _depthMin;
		float _depthMax;
		float _depthBias;
		float _depthSlopeScaleBias;
		hal::GraphicsCompareFunc _depthFunc;

		std::uint32_t _stencilFrontRef;
		std::uint32_t _stencilFrontReadMask;
		std::uint32_t _stencilFrontWriteMask;
		hal::GraphicsStencilOp _stencilFrontFail;
		hal::GraphicsStencilOp _stencilFrontZFail;
		hal::GraphicsStencilOp _stencilFrontPass;
		hal::GraphicsCompareFunc _stencilFrontFunc;

		std::uint32_t _stencilBackRef;
		std::uint32_t _stencilBackReadMask;
		std::uint32_t _stencilBackWriteMask;
		hal::GraphicsStencilOp _stencilBackFail;
		hal::GraphicsStencilOp _stencilBackZFail;
		hal::GraphicsStencilOp _stencilBackPass;
		hal::GraphicsCompareFunc _stencilBackFunc;

		hal::GraphicsColorBlends _blends;

		std::vector<MaterialParam> _properties;
	};

	using MaterialPtr = std::shared_ptr<Material>;
	using Materials = std::vector<MaterialPtr>;
}

#endif