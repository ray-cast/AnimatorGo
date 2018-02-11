#include <octoon/graphics/graphics_state.h>

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubInterface(GraphicsState, GraphicsChild, "GraphicsState")

		GraphicsColorBlend::GraphicsColorBlend() noexcept
			: _enable(false)
			, _blendOp(GraphicsBlendOp::GraphicsBlendOpAdd)
			, _blendAlphaOp(GraphicsBlendOp::GraphicsBlendOpAdd)
			, _blendSrc(GraphicsBlendFactor::GraphicsBlendFactorSrcAlpha)
			, _blendDest(GraphicsBlendFactor::GraphicsBlendFactorOneMinusSrcAlpha)
			, _blendAlphaSrc(GraphicsBlendFactor::GraphicsBlendFactorSrcAlpha)
			, _blendAlphaDest(GraphicsBlendFactor::GraphicsBlendFactorOneMinusSrcAlpha)
			, _colorWriteMask(GraphicsColorMaskFlagBits::GraphicsColorMaskFlagRGBABit)
		{
		}

		GraphicsColorBlend::~GraphicsColorBlend() noexcept
		{
		}

		void
		GraphicsColorBlend::setBlendEnable(bool enable) noexcept
		{
			_enable = enable;
		}

		void
		GraphicsColorBlend::setBlendOp(GraphicsBlendOp blendOp) noexcept
		{
			_blendOp = blendOp;
		}

		void
		GraphicsColorBlend::setBlendSrc(GraphicsBlendFactor factor) noexcept
		{
			_blendSrc = factor;
		}

		void
		GraphicsColorBlend::setBlendDest(GraphicsBlendFactor factor) noexcept
		{
			_blendDest = factor;
		}

		void
		GraphicsColorBlend::setBlendAlphaOp(GraphicsBlendOp blendOp) noexcept
		{
			_blendAlphaOp = blendOp;
		}

		void
		GraphicsColorBlend::setBlendAlphaSrc(GraphicsBlendFactor factor) noexcept
		{
			_blendAlphaSrc = factor;
		}

		void
		GraphicsColorBlend::setBlendAlphaDest(GraphicsBlendFactor factor) noexcept
		{
			_blendAlphaDest = factor;
		}

		void
		GraphicsColorBlend::setColorWriteMask(GraphicsColorMaskFlags mask) noexcept
		{
			_colorWriteMask = mask;
		}

		bool
		GraphicsColorBlend::getBlendEnable() const noexcept
		{
			return _enable;
		}

		GraphicsBlendOp
		GraphicsColorBlend::getBlendOp() const noexcept
		{
			return _blendOp;
		}

		GraphicsBlendFactor
		GraphicsColorBlend::getBlendSrc() const noexcept
		{
			return _blendSrc;
		}

		GraphicsBlendFactor
		GraphicsColorBlend::getBlendDest() const noexcept
		{
			return _blendDest;
		}

		GraphicsBlendOp
		GraphicsColorBlend::getBlendAlphaOp() const noexcept
		{
			return _blendAlphaOp;
		}

		GraphicsBlendFactor
		GraphicsColorBlend::getBlendAlphaSrc() const noexcept
		{
			return _blendAlphaSrc;
		}

		GraphicsBlendFactor
		GraphicsColorBlend::getBlendAlphaDest() const noexcept
		{
			return _blendAlphaDest;
		}

		GraphicsColorMaskFlags
		GraphicsColorBlend::getColorWriteMask() const noexcept
		{
			return _colorWriteMask;
		}

		GraphicsStateDesc::GraphicsStateDesc() noexcept
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
			, _lineWidth(1.0f)
			, _cullMode(GraphicsCullMode::GraphicsCullModeBack)
			, _polygonMode(GraphicsPolygonMode::GraphicsPolygonModeSolid)
			, _primitiveType(GraphicsVertexType::GraphicsVertexTypeTriangleList)
			, _frontFace(GraphicsFrontFace::GraphicsFrontFaceCW)
			, _depthMin(0.0)
			, _depthMax(1.0)
			, _depthSlopeScaleBias(0)
			, _depthBias(0)
			, _depthFunc(GraphicsCompareFunc::GraphicsCompareFuncLequal)
			, _stencilFrontRef(0)
			, _stencilBackRef(0)
			, _stencilFrontReadMask(0xFFFFFFFF)
			, _stencilFrontWriteMask(0xFFFFFFFF)
			, _stencilBackReadMask(0xFFFFFFFF)
			, _stencilBackWriteMask(0xFFFFFFFF)
			, _stencilFrontFunc(GraphicsCompareFunc::GraphicsCompareFuncAlways)
			, _stencilBackFunc(GraphicsCompareFunc::GraphicsCompareFuncAlways)
			, _stencilFrontFail(GraphicsStencilOp::GraphicsStencilOpKeep)
			, _stencilFrontZFail(GraphicsStencilOp::GraphicsStencilOpKeep)
			, _stencilFrontPass(GraphicsStencilOp::GraphicsStencilOpKeep)
			, _stencilBackFail(GraphicsStencilOp::GraphicsStencilOpKeep)
			, _stencilBackZFail(GraphicsStencilOp::GraphicsStencilOpKeep)
			, _stencilBackPass(GraphicsStencilOp::GraphicsStencilOpKeep)
		{
		}

		GraphicsStateDesc::~GraphicsStateDesc() noexcept
		{
		}

		void
		GraphicsStateDesc::setColorBlends(const GraphicsColorBlends& blends) noexcept
		{
			_blends = blends;
		}

		GraphicsColorBlends&
		GraphicsStateDesc::getColorBlends() noexcept
		{
			return _blends;
		}

		const GraphicsColorBlends&
		GraphicsStateDesc::getColorBlends() const noexcept
		{
			return _blends;
		}

		void
		GraphicsStateDesc::setCullMode(GraphicsCullMode mode) noexcept
		{
			_cullMode = mode;
		}

		void
		GraphicsStateDesc::setPolygonMode(GraphicsPolygonMode mode) noexcept
		{
			_polygonMode = mode;
		}

		void
		GraphicsStateDesc::setPrimitiveType(GraphicsVertexType type) noexcept
		{
			_primitiveType = type;
		}

		void
		GraphicsStateDesc::setFrontFace(GraphicsFrontFace face) noexcept
		{
			_frontFace = face;
		}

		void
		GraphicsStateDesc::setScissorTestEnable(bool enable) noexcept
		{
			_enableScissorTest = enable;
		}

		void
		GraphicsStateDesc::setLinear2sRGBEnable(bool enable) noexcept
		{
			_enableSrgb = enable;
		}

		void
		GraphicsStateDesc::setMultisampleEnable(bool enable) noexcept
		{
			_enableMultisample = enable;
		}

		void
		GraphicsStateDesc::setRasterizerDiscardEnable(bool enable) noexcept
		{
			_enableRasterizerDiscard = enable;
		}

		void
		GraphicsStateDesc::setLineWidth(float width) noexcept
		{
			_lineWidth = width;
		}

		void
		GraphicsStateDesc::setDepthEnable(bool enable) noexcept
		{
			_enableDepth = enable;
		}

		void
		GraphicsStateDesc::setDepthWriteEnable(bool enable) noexcept
		{
			_enableDepthWrite = enable;
		}

		void
		GraphicsStateDesc::setDepthBoundsEnable(bool enable) noexcept
		{
			_enableDepthBounds = enable;
		}

		void
		GraphicsStateDesc::setDepthMin(float min) noexcept
		{
			_depthMin = min;
		}

		void
		GraphicsStateDesc::setDepthMax(float max) noexcept
		{
			_depthMax = max;
		}

		void
		GraphicsStateDesc::setDepthFunc(GraphicsCompareFunc func) noexcept
		{
			_depthFunc = func;
		}

		void
		GraphicsStateDesc::setDepthBiasEnable(bool enable) noexcept
		{
			_enableDepthBias = enable;
		}

		void
		GraphicsStateDesc::setDepthBias(float bias) noexcept
		{
			_depthBias = bias;
		}

		void
		GraphicsStateDesc::setDepthSlopeScaleBias(float bias) noexcept
		{
			_depthSlopeScaleBias = bias;
		}

		void
		GraphicsStateDesc::setDepthBiasClamp(bool bias) noexcept
		{
			_enableDepthBiasClamp = bias;
		}

		void
		GraphicsStateDesc::setDepthClampEnable(bool enable) noexcept
		{
			_enableDepthClamp = enable;
		}

		void
		GraphicsStateDesc::setStencilEnable(bool enable) noexcept
		{
			_enableStencil = enable;
		}

		void
		GraphicsStateDesc::setStencilFrontRef(std::uint32_t ref) noexcept
		{
			_stencilFrontRef = ref;
		}

		void
		GraphicsStateDesc::setStencilFrontFunc(GraphicsCompareFunc func) noexcept
		{
			_stencilFrontFunc = func;
		}

		void
		GraphicsStateDesc::setStencilFrontReadMask(std::uint32_t mask) noexcept
		{
			_stencilFrontReadMask = mask;
		}

		void
		GraphicsStateDesc::setStencilFrontWriteMask(std::uint32_t mask) noexcept
		{
			_stencilFrontWriteMask = mask;
		}

		void
		GraphicsStateDesc::setStencilFrontFail(GraphicsStencilOp stencilOp) noexcept
		{
			_stencilFrontFail = stencilOp;
		}

		void
		GraphicsStateDesc::setStencilFrontZFail(GraphicsStencilOp stencilOp) noexcept
		{
			_stencilFrontZFail = stencilOp;
		}

		void
		GraphicsStateDesc::setStencilFrontPass(GraphicsStencilOp stencilOp) noexcept
		{
			_stencilFrontPass = stencilOp;
		}

		void
		GraphicsStateDesc::setStencilBackRef(std::uint32_t ref) noexcept
		{
			_stencilBackRef = ref;
		}

		void
		GraphicsStateDesc::setStencilBackFunc(GraphicsCompareFunc func) noexcept
		{
			_stencilBackFunc = func;
		}

		void
		GraphicsStateDesc::setStencilBackReadMask(std::uint32_t mask) noexcept
		{
			_stencilBackReadMask = mask;
		}

		void
		GraphicsStateDesc::setStencilBackWriteMask(std::uint32_t mask) noexcept
		{
			_stencilBackWriteMask = mask;
		}

		void
		GraphicsStateDesc::setStencilBackFail(GraphicsStencilOp stencilOp) noexcept
		{
			_stencilBackFail = stencilOp;
		}

		void
		GraphicsStateDesc::setStencilBackZFail(GraphicsStencilOp stencilOp) noexcept
		{
			_stencilBackZFail = stencilOp;
		}

		void
		GraphicsStateDesc::setStencilBackPass(GraphicsStencilOp stencilOp) noexcept
		{
			_stencilBackPass = stencilOp;
		}

		GraphicsCullMode
		GraphicsStateDesc::getCullMode() const noexcept
		{
			return _cullMode;
		}

		GraphicsPolygonMode
		GraphicsStateDesc::getPolygonMode() const noexcept
		{
			return _polygonMode;
		}

		GraphicsVertexType
		GraphicsStateDesc::getPrimitiveType() const noexcept
		{
			return _primitiveType;
		}

		GraphicsFrontFace
		GraphicsStateDesc::getFrontFace() const noexcept
		{
			return _frontFace;
		}

		bool
		GraphicsStateDesc::getScissorTestEnable() const noexcept
		{
			return _enableScissorTest;
		}

		bool
		GraphicsStateDesc::getLinear2sRGBEnable() const noexcept
		{
			return _enableSrgb;
		}

		bool
		GraphicsStateDesc::getMultisampleEnable() const noexcept
		{
			return _enableMultisample;
		}

		bool
		GraphicsStateDesc::getRasterizerDiscardEnable() const noexcept
		{
			return _enableRasterizerDiscard;
		}

		float
		GraphicsStateDesc::getLineWidth() const noexcept
		{
			return _lineWidth;
		}

		bool
		GraphicsStateDesc::getDepthEnable() const noexcept
		{
			return _enableDepth;
		}

		bool
		GraphicsStateDesc::getDepthWriteEnable() const noexcept
		{
			return _enableDepthWrite;
		}

		bool
		GraphicsStateDesc::getDepthBoundsEnable() const noexcept
		{
			return _enableDepthBounds;
		}

		float
		GraphicsStateDesc::getDepthMin() const noexcept
		{
			return _depthMin;
		}

		float
		GraphicsStateDesc::getDepthMax() const noexcept
		{
			return _depthMax;
		}

		GraphicsCompareFunc
		GraphicsStateDesc::getDepthFunc() const noexcept
		{
			return _depthFunc;
		}

		bool
		GraphicsStateDesc::getDepthBiasEnable() const noexcept
		{
			return _enableDepthBias;
		}

		float
		GraphicsStateDesc::getDepthBias() const noexcept
		{
			return _depthBias;
		}

		float
		GraphicsStateDesc::getDepthSlopeScaleBias() const noexcept
		{
			return _depthSlopeScaleBias;
		}

		bool
		GraphicsStateDesc::getDepthBiasClamp() const noexcept
		{
			return _enableDepthBiasClamp;
		}

		bool
		GraphicsStateDesc::getDepthClampEnable() const noexcept
		{
			return _enableDepthClamp;
		}

		bool
		GraphicsStateDesc::getStencilEnable() const noexcept
		{
			return _enableStencil;
		}

		std::uint32_t
		GraphicsStateDesc::getStencilFrontRef() const noexcept
		{
			return _stencilFrontRef;
		}

		GraphicsCompareFunc
		GraphicsStateDesc::getStencilFrontFunc() const noexcept
		{
			return _stencilFrontFunc;
		}

		std::uint32_t
		GraphicsStateDesc::getStencilFrontReadMask() const noexcept
		{
			return _stencilFrontReadMask;
		}

		std::uint32_t
		GraphicsStateDesc::getStencilFrontWriteMask() const noexcept
		{
			return _stencilFrontWriteMask;
		}

		GraphicsStencilOp
		GraphicsStateDesc::getStencilFrontFail() const noexcept
		{
			return _stencilFrontFail;
		}

		GraphicsStencilOp
		GraphicsStateDesc::getStencilFrontZFail() const noexcept
		{
			return _stencilFrontZFail;
		}

		GraphicsStencilOp
		GraphicsStateDesc::getStencilFrontPass() const noexcept
		{
			return _stencilFrontPass;
		}

		std::uint32_t
		GraphicsStateDesc::getStencilBackRef() const noexcept
		{
			return _stencilBackRef;
		}

		GraphicsCompareFunc
		GraphicsStateDesc::getStencilBackFunc() const noexcept
		{
			return _stencilBackFunc;
		}

		std::uint32_t
		GraphicsStateDesc::getStencilBackReadMask() const noexcept
		{
			return _stencilBackReadMask;
		}

		std::uint32_t
		GraphicsStateDesc::getStencilBackWriteMask() const noexcept
		{
			return _stencilBackWriteMask;
		}

		GraphicsStencilOp
		GraphicsStateDesc::getStencilBackFail() const noexcept
		{
			return _stencilBackFail;
		}

		GraphicsStencilOp
		GraphicsStateDesc::getStencilBackZFail() const noexcept
		{
			return _stencilBackZFail;
		}

		GraphicsStencilOp
		GraphicsStateDesc::getStencilBackPass() const noexcept
		{
			return _stencilBackPass;
		}
	}
}