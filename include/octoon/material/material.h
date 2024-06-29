#ifndef OCTOON_MATERIAL_H_
#define OCTOON_MATERIAL_H_

#include <octoon/math/math.h>
#include <octoon/material/shader.h>
#include <octoon/texture/texture.h>
#include <octoon/hal/render_state.h>
#include <octoon/hal/graphics_texture.h>

namespace octoon
{
	enum PropertyTypeInfo
	{
		PropertyTypeInfoFloat = 1 << 0,
		PropertyTypeInfoFloat2 = 1 << 1,
		PropertyTypeInfoFloat3 = 1 << 2,
		PropertyTypeInfoFloat4 = 1 << 3,
		PropertyTypeInfoString = 1 << 4,
		PropertyTypeInfoBool = 1 << 5,
		PropertyTypeInfoInt = 1 << 6,
		PropertyTypeInfoBuffer = 1 << 7,
		PropertyTypeInfoTexture = 1 << 8,
		PropertyTypeInfoRenderTexture = 1 << 9,
	};

	struct MaterialParam
	{
		std::string key;

		std::size_t length;
		std::size_t type;

		char* data;
		std::shared_ptr<Texture> texture;
		std::shared_ptr<GraphicsTexture> renderTexture;
	};

	class OCTOON_EXPORT Material : public Object
	{
		OctoonDeclareSubClass(Material, Object);
	public:
		Material() noexcept;
		Material(std::string_view name) noexcept;
		Material(const std::shared_ptr<Shader>& shader) noexcept;
		virtual ~Material() noexcept;

		void setName(std::string_view name) noexcept;
		const std::string& getName() const noexcept;

		void setShader(const std::shared_ptr<Shader>& shader) noexcept;
		std::shared_ptr<Shader> getShader() const noexcept;

		void setCullMode(CullMode mode) noexcept;
		void setPolygonMode(PolygonMode mode) noexcept;
		void setPrimitiveType(VertexType type) noexcept;
		void setFrontFace(FrontFace face) noexcept;
		void setScissorTestEnable(bool enable) noexcept;
		void setLinear2sRGBEnable(bool enable) noexcept;
		void setMultisampleEnable(bool enable) noexcept;
		void setRasterizerDiscardEnable(bool enable) noexcept;
		void setLineWidth(float width) noexcept;
		void setColorWriteMask(ColorWriteMaskFlags mask) noexcept;

		void setBlendEnable(bool enable) noexcept;
		void setBlendOp(BlendOp blendop) noexcept;
		void setBlendSrc(BlendMode factor) noexcept;
		void setBlendDest(BlendMode factor) noexcept;
		void setBlendAlphaOp(BlendOp blendop) noexcept;
		void setBlendAlphaSrc(BlendMode factor) noexcept;
		void setBlendAlphaDest(BlendMode factor) noexcept;

		void setDepthEnable(bool enable) noexcept;
		void setDepthWriteEnable(bool enable) noexcept;
		void setDepthBoundsEnable(bool enable) noexcept;
		void setDepthMin(float mix) noexcept;
		void setDepthMax(float max) noexcept;
		void setDepthFunc(CompareFunction func) noexcept;
		void setDepthBiasEnable(bool enable) noexcept;
		void setDepthBias(float bias) noexcept;
		void setDepthSlopeScaleBias(float bias) noexcept;
		void setDepthBiasClamp(bool bias) noexcept;
		void setDepthClampEnable(bool enable) noexcept;

		void setStencilEnable(bool enable) noexcept;
		void setStencilFrontRef(std::uint32_t ref) noexcept;
		void setStencilFrontFunc(CompareFunction func) noexcept;
		void setStencilFrontReadMask(std::uint32_t mask) noexcept;
		void setStencilFrontWriteMask(std::uint32_t mask) noexcept;
		void setStencilFrontFail(StencilOp stencilOp) noexcept;
		void setStencilFrontZFail(StencilOp stencilOp) noexcept;
		void setStencilFrontPass(StencilOp stencilOp) noexcept;
		void setStencilBackRef(std::uint32_t ref) noexcept;
		void setStencilBackFunc(CompareFunction func) noexcept;
		void setStencilBackReadMask(std::uint32_t mask) noexcept;
		void setStencilBackWriteMask(std::uint32_t mask) noexcept;
		void setStencilBackFail(StencilOp stencilOp) noexcept;
		void setStencilBackZFail(StencilOp stencilOp) noexcept;
		void setStencilBackPass(StencilOp stencilOp) noexcept;

		CullMode getCullMode() const noexcept;
		PolygonMode getPolygonMode() const noexcept;
		VertexType getPrimitiveType() const noexcept;
		FrontFace getFrontFace() const noexcept;
		bool getScissorTestEnable() const noexcept;
		bool getLinear2sRGBEnable() const noexcept;
		bool getMultisampleEnable() const noexcept;
		bool getRasterizerDiscardEnable() const noexcept;
		float getLineWidth() const noexcept;
		ColorWriteMaskFlags getColorWriteMask() const noexcept;

		bool getBlendEnable() const noexcept;
		BlendOp getBlendOp() const noexcept;
		BlendMode getBlendSrc() const noexcept;
		BlendMode getBlendDest() const noexcept;
		BlendOp getBlendAlphaOp() const noexcept;
		BlendMode getBlendAlphaSrc() const noexcept;
		BlendMode getBlendAlphaDest() const noexcept;

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
		CompareFunction getDepthFunc() const noexcept;

		bool getStencilEnable() const noexcept;
		CompareFunction getStencilFrontFunc() const noexcept;
		std::uint32_t getStencilFrontRef() const noexcept;
		std::uint32_t getStencilFrontReadMask() const noexcept;
		std::uint32_t getStencilFrontWriteMask() const noexcept;
		StencilOp getStencilFrontFail() const noexcept;
		StencilOp getStencilFrontZFail() const noexcept;
		StencilOp getStencilFrontPass() const noexcept;
		CompareFunction getStencilBackFunc() const noexcept;
		std::uint32_t getStencilBackRef() const noexcept;
		std::uint32_t getStencilBackReadMask() const noexcept;
		std::uint32_t getStencilBackWriteMask() const noexcept;
		StencilOp getStencilBackFail() const noexcept;
		StencilOp getStencilBackZFail() const noexcept;
		StencilOp getStencilBackPass() const noexcept;

		bool set(std::string_view key, bool value) noexcept;
		bool set(std::string_view key, int value) noexcept;
		bool set(std::string_view key, float value) noexcept;
		bool set(std::string_view key, const math::Vector2& value) noexcept;
		bool set(std::string_view key, const math::Vector3& value) noexcept;
		bool set(std::string_view key, const math::Vector4& value) noexcept;
		bool set(std::string_view key, std::string_view value) noexcept;
		bool set(std::string_view key, const std::shared_ptr<Texture>& value) noexcept;
		bool set(std::string_view key, const std::shared_ptr<GraphicsTexture>& value) noexcept;
		bool set(const MaterialParam& value) noexcept;

		bool get(std::string_view key, bool& value) const noexcept;
		bool get(std::string_view key, int& value) const noexcept;
		bool get(std::string_view key, float& value) const noexcept;
		bool get(std::string_view key, math::Vector2& value) const noexcept;
		bool get(std::string_view key, math::Vector3& value) const noexcept;
		bool get(std::string_view key, math::Vector4& value) const noexcept;
		bool get(std::string_view key, std::string& value) const noexcept;
		bool get(std::string_view key, std::shared_ptr<Texture>& value) const noexcept;
		bool get(std::string_view key, std::shared_ptr<GraphicsTexture>& value) const noexcept;
		bool get(std::string_view key, MaterialParam& out) const noexcept;

		template<typename T>
		T get(std::string_view key) const noexcept
		{
			T value;
			this->get(key, value);
			return value;
		}

		void setDirty(bool dirty) noexcept;
		bool isDirty() const noexcept;

		const std::vector<MaterialParam>& getMaterialParams() const noexcept;

		std::size_t hash() const noexcept;

		virtual void copy(const Material& material) noexcept;
		virtual std::shared_ptr<Material> clone() const noexcept;

	private:
		std::string name_;

		bool dirty_;

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
		bool _enableBlend;

		BlendOp _blendOp;
		BlendOp _blendAlphaOp;

		BlendMode _blendSrc;
		BlendMode _blendDest;
		BlendMode _blendAlphaSrc;
		BlendMode _blendAlphaDest;

		ColorWriteMaskFlags _colorWriteMask;

		float _lineWidth;

		CullMode    _cullMode;
		PolygonMode _polygonMode;
		VertexType  _primitiveType;
		FrontFace   _frontFace;

		float _depthMin;
		float _depthMax;
		float _depthBias;
		float _depthSlopeScaleBias;
		CompareFunction _depthFunc;

		std::uint32_t _stencilFrontRef;
		std::uint32_t _stencilFrontReadMask;
		std::uint32_t _stencilFrontWriteMask;
		StencilOp _stencilFrontFail;
		StencilOp _stencilFrontZFail;
		StencilOp _stencilFrontPass;
		CompareFunction _stencilFrontFunc;

		std::uint32_t _stencilBackRef;
		std::uint32_t _stencilBackReadMask;
		std::uint32_t _stencilBackWriteMask;
		StencilOp _stencilBackFail;
		StencilOp _stencilBackZFail;
		StencilOp _stencilBackPass;
		CompareFunction _stencilBackFunc;

		std::shared_ptr<Shader> _shader;

		std::vector<MaterialParam> _properties;
	};

	using MaterialPtr = std::shared_ptr<Material>;
	using Materials = std::vector<MaterialPtr>;
}

#endif