#include <octoon/hal/render_state.h>

namespace octoon
{
	OctoonImplementSubInterface(RenderState, GraphicsResource, "RenderState")

	RenderStateDesc::RenderStateDesc() noexcept
		: _enableScissorTest(false)
		, _enableSrgb(false)
		, _enableMultisample(false)
		, _enableRasterizerDiscard(false)
		, _enableDepth(true)
		, _enableDepthBounds(false)
		, _enableDepthWrite(true)
		, _enableDepthBias(false)
		, _enableDepthClamp(false)
		, _enableDepthBiasClamp(false)
		, _enableStencil(false)
		, _enableBlend(false)
		, _lineWidth(1.0f)
		, _cullMode(CullMode::Back)
		, _polygonMode(PolygonMode::Solid)
		, _primitiveType(VertexType::TriangleList)
		, _frontFace(FrontFace::CW)
		, _colorWriteMask(ColorWriteMask::RGBABit)
		, _blendOp(BlendOp::Add)
		, _blendAlphaOp(BlendOp::Add)
		, _blendSrc(BlendMode::SrcAlpha)
		, _blendDest(BlendMode::OneMinusSrcAlpha)
		, _blendAlphaSrc(BlendMode::SrcAlpha)
		, _blendAlphaDest(BlendMode::OneMinusSrcAlpha)
		, _depthMin(0.0)
		, _depthMax(1.0)
		, _depthSlopeScaleBias(0)
		, _depthBias(0)
		, _depthFunc(CompareFunction::Lequal)
		, _stencilFrontRef(0)
		, _stencilBackRef(0)
		, _stencilFrontReadMask(0xFFFFFFFF)
		, _stencilFrontWriteMask(0xFFFFFFFF)
		, _stencilBackReadMask(0xFFFFFFFF)
		, _stencilBackWriteMask(0xFFFFFFFF)
		, _stencilFrontFunc(CompareFunction::Always)
		, _stencilBackFunc(CompareFunction::Always)
		, _stencilFrontFail(StencilOp::Keep)
		, _stencilFrontZFail(StencilOp::Keep)
		, _stencilFrontPass(StencilOp::Keep)
		, _stencilBackFail(StencilOp::Keep)
		, _stencilBackZFail(StencilOp::Keep)
		, _stencilBackPass(StencilOp::Keep)
	{
	}

	RenderStateDesc::~RenderStateDesc() noexcept
	{
	}

	void
	RenderStateDesc::setCullMode(CullMode mode) noexcept
	{
		_cullMode = mode;
	}

	void
	RenderStateDesc::setPolygonMode(PolygonMode mode) noexcept
	{
		_polygonMode = mode;
	}

	void
	RenderStateDesc::setPrimitiveType(VertexType type) noexcept
	{
		_primitiveType = type;
	}

	void
	RenderStateDesc::setFrontFace(FrontFace face) noexcept
	{
		_frontFace = face;
	}

	void
	RenderStateDesc::setScissorTestEnable(bool enable) noexcept
	{
		_enableScissorTest = enable;
	}

	void
	RenderStateDesc::setLinear2sRGBEnable(bool enable) noexcept
	{
		_enableSrgb = enable;
	}

	void
	RenderStateDesc::setMultisampleEnable(bool enable) noexcept
	{
		_enableMultisample = enable;
	}

	void
	RenderStateDesc::setRasterizerDiscardEnable(bool enable) noexcept
	{
		_enableRasterizerDiscard = enable;
	}

	void
	RenderStateDesc::setLineWidth(float width) noexcept
	{
		_lineWidth = width;
	}

	void
	RenderStateDesc::setBlendEnable(bool enable) noexcept
	{
		_enableBlend = enable;
	}

	void
	RenderStateDesc::setBlendOp(BlendOp blendOp) noexcept
	{
		_blendOp = blendOp;
	}

	void
	RenderStateDesc::setBlendSrc(BlendMode factor) noexcept
	{
		_blendSrc = factor;
	}

	void
	RenderStateDesc::setBlendDest(BlendMode factor) noexcept
	{
		_blendDest = factor;
	}

	void
	RenderStateDesc::setBlendAlphaOp(BlendOp blendOp) noexcept
	{
		_blendAlphaOp = blendOp;
	}

	void
	RenderStateDesc::setBlendAlphaSrc(BlendMode factor) noexcept
	{
		_blendAlphaSrc = factor;
	}

	void
	RenderStateDesc::setBlendAlphaDest(BlendMode factor) noexcept
	{
		_blendAlphaDest = factor;
	}

	void
	RenderStateDesc::setColorWriteMask(ColorWriteMaskFlags mask) noexcept
	{
		_colorWriteMask = mask;
	}

	void
	RenderStateDesc::setDepthEnable(bool enable) noexcept
	{
		_enableDepth = enable;
	}

	void
	RenderStateDesc::setDepthWriteEnable(bool enable) noexcept
	{
		_enableDepthWrite = enable;
	}

	void
	RenderStateDesc::setDepthBoundsEnable(bool enable) noexcept
	{
		_enableDepthBounds = enable;
	}

	void
	RenderStateDesc::setDepthMin(float min) noexcept
	{
		_depthMin = min;
	}

	void
	RenderStateDesc::setDepthMax(float max) noexcept
	{
		_depthMax = max;
	}

	void
	RenderStateDesc::setDepthFunc(CompareFunction func) noexcept
	{
		_depthFunc = func;
	}

	void
	RenderStateDesc::setDepthBiasEnable(bool enable) noexcept
	{
		_enableDepthBias = enable;
	}

	void
	RenderStateDesc::setDepthBias(float bias) noexcept
	{
		_depthBias = bias;
	}

	void
	RenderStateDesc::setDepthSlopeScaleBias(float bias) noexcept
	{
		_depthSlopeScaleBias = bias;
	}

	void
	RenderStateDesc::setDepthBiasClamp(bool bias) noexcept
	{
		_enableDepthBiasClamp = bias;
	}

	void
	RenderStateDesc::setDepthClampEnable(bool enable) noexcept
	{
		_enableDepthClamp = enable;
	}

	void
	RenderStateDesc::setStencilEnable(bool enable) noexcept
	{
		_enableStencil = enable;
	}

	void
	RenderStateDesc::setStencilFrontRef(std::uint32_t ref) noexcept
	{
		_stencilFrontRef = ref;
	}

	void
	RenderStateDesc::setStencilFrontFunc(CompareFunction func) noexcept
	{
		_stencilFrontFunc = func;
	}

	void
	RenderStateDesc::setStencilFrontReadMask(std::uint32_t mask) noexcept
	{
		_stencilFrontReadMask = mask;
	}

	void
	RenderStateDesc::setStencilFrontWriteMask(std::uint32_t mask) noexcept
	{
		_stencilFrontWriteMask = mask;
	}

	void
	RenderStateDesc::setStencilFrontFail(StencilOp stencilOp) noexcept
	{
		_stencilFrontFail = stencilOp;
	}

	void
	RenderStateDesc::setStencilFrontZFail(StencilOp stencilOp) noexcept
	{
		_stencilFrontZFail = stencilOp;
	}

	void
	RenderStateDesc::setStencilFrontPass(StencilOp stencilOp) noexcept
	{
		_stencilFrontPass = stencilOp;
	}

	void
	RenderStateDesc::setStencilBackRef(std::uint32_t ref) noexcept
	{
		_stencilBackRef = ref;
	}

	void
	RenderStateDesc::setStencilBackFunc(CompareFunction func) noexcept
	{
		_stencilBackFunc = func;
	}

	void
	RenderStateDesc::setStencilBackReadMask(std::uint32_t mask) noexcept
	{
		_stencilBackReadMask = mask;
	}

	void
	RenderStateDesc::setStencilBackWriteMask(std::uint32_t mask) noexcept
	{
		_stencilBackWriteMask = mask;
	}

	void
	RenderStateDesc::setStencilBackFail(StencilOp stencilOp) noexcept
	{
		_stencilBackFail = stencilOp;
	}

	void
	RenderStateDesc::setStencilBackZFail(StencilOp stencilOp) noexcept
	{
		_stencilBackZFail = stencilOp;
	}

	void
	RenderStateDesc::setStencilBackPass(StencilOp stencilOp) noexcept
	{
		_stencilBackPass = stencilOp;
	}

	CullMode
	RenderStateDesc::getCullMode() const noexcept
	{
		return _cullMode;
	}

	PolygonMode
	RenderStateDesc::getPolygonMode() const noexcept
	{
		return _polygonMode;
	}

	VertexType
	RenderStateDesc::getPrimitiveType() const noexcept
	{
		return _primitiveType;
	}

	FrontFace
	RenderStateDesc::getFrontFace() const noexcept
	{
		return _frontFace;
	}

	bool
	RenderStateDesc::getScissorTestEnable() const noexcept
	{
		return _enableScissorTest;
	}

	bool
	RenderStateDesc::getLinear2sRGBEnable() const noexcept
	{
		return _enableSrgb;
	}

	bool
	RenderStateDesc::getMultisampleEnable() const noexcept
	{
		return _enableMultisample;
	}

	bool
	RenderStateDesc::getRasterizerDiscardEnable() const noexcept
	{
		return _enableRasterizerDiscard;
	}

	float
	RenderStateDesc::getLineWidth() const noexcept
	{
		return _lineWidth;
	}

	bool
	RenderStateDesc::getBlendEnable() const noexcept
	{
		return _enableBlend;
	}

	BlendOp
	RenderStateDesc::getBlendOp() const noexcept
	{
		return _blendOp;
	}

	BlendMode
	RenderStateDesc::getBlendSrc() const noexcept
	{
		return _blendSrc;
	}

	BlendMode
	RenderStateDesc::getBlendDest() const noexcept
	{
		return _blendDest;
	}

	BlendOp
	RenderStateDesc::getBlendAlphaOp() const noexcept
	{
		return _blendAlphaOp;
	}

	BlendMode
	RenderStateDesc::getBlendAlphaSrc() const noexcept
	{
		return _blendAlphaSrc;
	}

	BlendMode
	RenderStateDesc::getBlendAlphaDest() const noexcept
	{
		return _blendAlphaDest;
	}

	ColorWriteMaskFlags
	RenderStateDesc::getColorWriteMask() const noexcept
	{
		return _colorWriteMask;
	}

	bool
	RenderStateDesc::getDepthEnable() const noexcept
	{
		return _enableDepth;
	}

	bool
	RenderStateDesc::getDepthWriteEnable() const noexcept
	{
		return _enableDepthWrite;
	}

	bool
	RenderStateDesc::getDepthBoundsEnable() const noexcept
	{
		return _enableDepthBounds;
	}

	float
	RenderStateDesc::getDepthMin() const noexcept
	{
		return _depthMin;
	}

	float
	RenderStateDesc::getDepthMax() const noexcept
	{
		return _depthMax;
	}

	CompareFunction
	RenderStateDesc::getDepthFunc() const noexcept
	{
		return _depthFunc;
	}

	bool
	RenderStateDesc::getDepthBiasEnable() const noexcept
	{
		return _enableDepthBias;
	}

	float
	RenderStateDesc::getDepthBias() const noexcept
	{
		return _depthBias;
	}

	float
	RenderStateDesc::getDepthSlopeScaleBias() const noexcept
	{
		return _depthSlopeScaleBias;
	}

	bool
	RenderStateDesc::getDepthBiasClamp() const noexcept
	{
		return _enableDepthBiasClamp;
	}

	bool
	RenderStateDesc::getDepthClampEnable() const noexcept
	{
		return _enableDepthClamp;
	}

	bool
	RenderStateDesc::getStencilEnable() const noexcept
	{
		return _enableStencil;
	}

	std::uint32_t
	RenderStateDesc::getStencilFrontRef() const noexcept
	{
		return _stencilFrontRef;
	}

	CompareFunction
	RenderStateDesc::getStencilFrontFunc() const noexcept
	{
		return _stencilFrontFunc;
	}

	std::uint32_t
	RenderStateDesc::getStencilFrontReadMask() const noexcept
	{
		return _stencilFrontReadMask;
	}

	std::uint32_t
	RenderStateDesc::getStencilFrontWriteMask() const noexcept
	{
		return _stencilFrontWriteMask;
	}

	StencilOp
	RenderStateDesc::getStencilFrontFail() const noexcept
	{
		return _stencilFrontFail;
	}

	StencilOp
	RenderStateDesc::getStencilFrontZFail() const noexcept
	{
		return _stencilFrontZFail;
	}

	StencilOp
	RenderStateDesc::getStencilFrontPass() const noexcept
	{
		return _stencilFrontPass;
	}

	std::uint32_t
	RenderStateDesc::getStencilBackRef() const noexcept
	{
		return _stencilBackRef;
	}

	CompareFunction
	RenderStateDesc::getStencilBackFunc() const noexcept
	{
		return _stencilBackFunc;
	}

	std::uint32_t
	RenderStateDesc::getStencilBackReadMask() const noexcept
	{
		return _stencilBackReadMask;
	}

	std::uint32_t
	RenderStateDesc::getStencilBackWriteMask() const noexcept
	{
		return _stencilBackWriteMask;
	}

	StencilOp
	RenderStateDesc::getStencilBackFail() const noexcept
	{
		return _stencilBackFail;
	}

	StencilOp
	RenderStateDesc::getStencilBackZFail() const noexcept
	{
		return _stencilBackZFail;
	}

	StencilOp
	RenderStateDesc::getStencilBackPass() const noexcept
	{
		return _stencilBackPass;
	}
}