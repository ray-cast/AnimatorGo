#ifndef OCTOON_RENDER_STATE_H_
#define OCTOON_RENDER_STATE_H_

#include <octoon/hal/graphics_resource.h>

namespace octoon
{
	class OCTOON_EXPORT RenderStateDesc final
	{
	public:
		RenderStateDesc() noexcept;
		~RenderStateDesc() noexcept;

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

	private:
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
	};

	class OCTOON_EXPORT RenderState : public GraphicsResource
	{
		OctoonDeclareSubInterface(RenderState, GraphicsResource)
	public:
		RenderState() noexcept = default;
		virtual ~RenderState() = default;

		virtual const RenderStateDesc& getStateDesc() const noexcept = 0;

	private:
		RenderState(const RenderState&) noexcept = delete;
		RenderState& operator=(const RenderState&) noexcept = delete;
	};
}

#endif