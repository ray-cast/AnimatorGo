#ifndef OCTOON_GRAPHICS_STATE_H_
#define OCTOON_GRAPHICS_STATE_H_

#include <octoon/graphics/graphics_child.h>

namespace octoon
{
	namespace graphics
	{
		class OCTOON_EXPORT GraphicsColorBlend final
		{
		public:
			GraphicsColorBlend() noexcept;
			~GraphicsColorBlend() noexcept;

			void setBlendEnable(bool enable) noexcept;
			void setBlendOp(GraphicsBlendOp blendop) noexcept;
			void setBlendSrc(GraphicsBlendFactor factor) noexcept;
			void setBlendDest(GraphicsBlendFactor factor) noexcept;
			void setBlendAlphaOp(GraphicsBlendOp blendop) noexcept;
			void setBlendAlphaSrc(GraphicsBlendFactor factor) noexcept;
			void setBlendAlphaDest(GraphicsBlendFactor factor) noexcept;

			bool getBlendEnable() const noexcept;
			GraphicsBlendOp getBlendOp() const noexcept;
			GraphicsBlendFactor getBlendSrc() const noexcept;
			GraphicsBlendFactor getBlendDest() const noexcept;
			GraphicsBlendOp getBlendAlphaOp() const noexcept;
			GraphicsBlendFactor getBlendAlphaSrc() const noexcept;
			GraphicsBlendFactor getBlendAlphaDest() const noexcept;

			void setColorWriteMask(GraphicsColorMaskFlags mask) noexcept;
			GraphicsColorMaskFlags getColorWriteMask() const noexcept;

		private:
			bool _enable;

			GraphicsBlendOp _blendOp;
			GraphicsBlendOp _blendAlphaOp;

			GraphicsBlendFactor _blendSrc;
			GraphicsBlendFactor _blendDest;
			GraphicsBlendFactor _blendAlphaSrc;
			GraphicsBlendFactor _blendAlphaDest;

			GraphicsColorMaskFlags _colorWriteMask;
		};

		class OCTOON_EXPORT GraphicsStateDesc final
		{
		public:
			GraphicsStateDesc() noexcept;
			~GraphicsStateDesc() noexcept;

			void setColorBlends(const GraphicsColorBlends& blends) noexcept;
			GraphicsColorBlends& getColorBlends() noexcept;
			const GraphicsColorBlends& getColorBlends() const noexcept;

			void setCullMode(GraphicsCullMode mode) noexcept;
			void setPolygonMode(GraphicsPolygonMode mode) noexcept;
			void setPrimitiveType(GraphicsVertexType type) noexcept;
			void setFrontFace(GraphicsFrontFace face) noexcept;
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
			void setDepthFunc(GraphicsCompareFunc func) noexcept;
			void setDepthBiasEnable(bool enable) noexcept;
			void setDepthBias(float bias) noexcept;
			void setDepthSlopeScaleBias(float bias) noexcept;
			void setDepthBiasClamp(bool bias) noexcept;
			void setDepthClampEnable(bool enable) noexcept;

			void setStencilEnable(bool enable) noexcept;
			void setStencilFrontRef(std::uint32_t ref) noexcept;
			void setStencilFrontFunc(GraphicsCompareFunc func) noexcept;
			void setStencilFrontReadMask(std::uint32_t mask) noexcept;
			void setStencilFrontWriteMask(std::uint32_t mask) noexcept;
			void setStencilFrontFail(GraphicsStencilOp stencilOp) noexcept;
			void setStencilFrontZFail(GraphicsStencilOp stencilOp) noexcept;
			void setStencilFrontPass(GraphicsStencilOp stencilOp) noexcept;
			void setStencilBackRef(std::uint32_t ref) noexcept;
			void setStencilBackFunc(GraphicsCompareFunc func) noexcept;
			void setStencilBackReadMask(std::uint32_t mask) noexcept;
			void setStencilBackWriteMask(std::uint32_t mask) noexcept;
			void setStencilBackFail(GraphicsStencilOp stencilOp) noexcept;
			void setStencilBackZFail(GraphicsStencilOp stencilOp) noexcept;
			void setStencilBackPass(GraphicsStencilOp stencilOp) noexcept;

			GraphicsCullMode getCullMode() const noexcept;
			GraphicsPolygonMode getPolygonMode() const noexcept;
			GraphicsVertexType getPrimitiveType() const noexcept;
			GraphicsFrontFace getFrontFace() const noexcept;
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
			GraphicsCompareFunc getDepthFunc() const noexcept;

			bool getStencilEnable() const noexcept;
			GraphicsCompareFunc getStencilFrontFunc() const noexcept;
			std::uint32_t getStencilFrontRef() const noexcept;
			std::uint32_t getStencilFrontReadMask() const noexcept;
			std::uint32_t getStencilFrontWriteMask() const noexcept;
			GraphicsStencilOp getStencilFrontFail() const noexcept;
			GraphicsStencilOp getStencilFrontZFail() const noexcept;
			GraphicsStencilOp getStencilFrontPass() const noexcept;
			GraphicsCompareFunc getStencilBackFunc() const noexcept;
			std::uint32_t getStencilBackRef() const noexcept;
			std::uint32_t getStencilBackReadMask() const noexcept;
			std::uint32_t getStencilBackWriteMask() const noexcept;
			GraphicsStencilOp getStencilBackFail() const noexcept;
			GraphicsStencilOp getStencilBackZFail() const noexcept;
			GraphicsStencilOp getStencilBackPass() const noexcept;

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

			float _lineWidth;

			GraphicsCullMode    _cullMode;
			GraphicsPolygonMode _polygonMode;
			GraphicsVertexType  _primitiveType;
			GraphicsFrontFace   _frontFace;

			float _depthMin;
			float _depthMax;
			float _depthBias;
			float _depthSlopeScaleBias;
			GraphicsCompareFunc _depthFunc;

			std::uint32_t _stencilFrontRef;
			std::uint32_t _stencilFrontReadMask;
			std::uint32_t _stencilFrontWriteMask;
			GraphicsStencilOp _stencilFrontFail;
			GraphicsStencilOp _stencilFrontZFail;
			GraphicsStencilOp _stencilFrontPass;
			GraphicsCompareFunc _stencilFrontFunc;

			std::uint32_t _stencilBackRef;
			std::uint32_t _stencilBackReadMask;
			std::uint32_t _stencilBackWriteMask;
			GraphicsStencilOp _stencilBackFail;
			GraphicsStencilOp _stencilBackZFail;
			GraphicsStencilOp _stencilBackPass;
			GraphicsCompareFunc _stencilBackFunc;

			GraphicsColorBlends _blends;
		};

		class OCTOON_EXPORT GraphicsState : public GraphicsChild
		{
			OctoonDeclareSubInterface(GraphicsState, GraphicsChild)
		public:
			GraphicsState() noexcept = default;
			virtual ~GraphicsState() = default;

			virtual const GraphicsStateDesc& getGraphicsStateDesc() const noexcept = 0;

		private:
			GraphicsState(const GraphicsState&) noexcept = delete;
			GraphicsState& operator=(const GraphicsState&) noexcept = delete;
		};
	}
}

#endif