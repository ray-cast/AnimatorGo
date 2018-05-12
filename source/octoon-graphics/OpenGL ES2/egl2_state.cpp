#include "egl2_state.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(EGL2GraphicsState, GraphicsState, "EGL2GraphicsState")

		EGL2GraphicsState::EGL2GraphicsState() noexcept
		{
		}

		EGL2GraphicsState::~EGL2GraphicsState() noexcept
		{
		}

		bool
		EGL2GraphicsState::setup(const GraphicsStateDesc& desc) noexcept
		{
			_stateDesc = desc;
			return true;
		}

		void
		EGL2GraphicsState::close() noexcept
		{
		}

		void
		EGL2GraphicsState::apply(GraphicsStateDesc& lastStateDesc) noexcept
		{
			auto& srcBlends = _stateDesc.getColorBlends();
			auto& destBlends = lastStateDesc.getColorBlends();

			std::size_t srcBlendCount = srcBlends.size();
			std::size_t destBlendCount = destBlends.size();
			for (std::size_t i = srcBlendCount; i < destBlendCount; i++)
			{
				auto& destBlend = destBlends[i];
				if (destBlend.getBlendEnable())
				{
					glDisable(GL_BLEND);
					destBlend.setBlendEnable(false);
				}
			}

			for (std::size_t i = 0; i < srcBlendCount; i++)
			{
				auto& srcBlend = srcBlends[i];
				auto& destBlend = destBlends[i];

				if (srcBlends[i].getBlendEnable())
				{
					if (!destBlend.getBlendEnable())
					{
						glEnable(GL_BLEND);
						destBlend.setBlendEnable(true);
					}

					if (destBlend.getBlendSrc() != srcBlend.getBlendSrc() ||
						destBlend.getBlendDest() != srcBlend.getBlendDest() ||
						destBlend.getBlendAlphaSrc() != srcBlend.getBlendAlphaSrc() ||
						destBlend.getBlendAlphaDest() != srcBlend.getBlendAlphaDest())
					{
						GLenum sfactorRGB = EGL2Types::asBlendFactor(srcBlend.getBlendSrc());
						GLenum dfactorRGB = EGL2Types::asBlendFactor(srcBlend.getBlendDest());
						GLenum sfactorAlpha = EGL2Types::asBlendFactor(srcBlend.getBlendAlphaSrc());
						GLenum dfactorAlpha = EGL2Types::asBlendFactor(srcBlend.getBlendAlphaDest());

						glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);

						destBlend.setBlendSrc(srcBlend.getBlendSrc());
						destBlend.setBlendDest(srcBlend.getBlendDest());
						destBlend.setBlendAlphaSrc(srcBlend.getBlendAlphaSrc());
						destBlend.setBlendAlphaDest(srcBlend.getBlendAlphaDest());
					}

					if (destBlend.getBlendOp() != srcBlend.getBlendOp() ||
						destBlend.getBlendAlphaOp() != srcBlend.getBlendAlphaOp())
					{
						GLenum modeRGB = EGL2Types::asBlendOperation(srcBlend.getBlendOp());
						GLenum modeAlpha = EGL2Types::asBlendOperation(srcBlend.getBlendAlphaOp());

						glBlendEquationSeparate(modeRGB, modeAlpha);

						destBlend.setBlendOp(srcBlend.getBlendOp());
						destBlend.setBlendAlphaOp(srcBlend.getBlendAlphaOp());
					}
				}
				else
				{
					if (destBlend.getBlendEnable())
					{
						glDisable(GL_BLEND);
						destBlend.setBlendEnable(false);
					}
				}

				if (destBlend.getColorWriteMask() != srcBlend.getColorWriteMask())
				{
					auto flags = srcBlend.getColorWriteMask();

					GLboolean r = flags & GraphicsColorMaskFlagBits::RedBit ? GL_TRUE : GL_FALSE;
					GLboolean g = flags & GraphicsColorMaskFlagBits::GreendBit ? GL_TRUE : GL_FALSE;
					GLboolean b = flags & GraphicsColorMaskFlagBits::BlurBit ? GL_TRUE : GL_FALSE;
					GLboolean a = flags & GraphicsColorMaskFlagBits::AlphaBit ? GL_TRUE : GL_FALSE;

					glColorMask(r, g, b, a);

					destBlend.setColorWriteMask(srcBlend.getColorWriteMask());
				}
			}

			if (lastStateDesc.getCullMode() != _stateDesc.getCullMode())
			{
				if (_stateDesc.getCullMode() != GraphicsCullMode::None)
				{
					GLenum mode = EGL2Types::asCullMode(_stateDesc.getCullMode());
					glEnable(GL_CULL_FACE);
					glCullFace(mode);

					lastStateDesc.setCullMode(_stateDesc.getCullMode());
				}
				else
				{
					glDisable(GL_CULL_FACE);
					lastStateDesc.setCullMode(GraphicsCullMode::None);
				}
			}

			if (lastStateDesc.getFrontFace() != _stateDesc.getFrontFace())
			{
				GLenum face = EGL2Types::asFrontFace(_stateDesc.getFrontFace());
				glFrontFace(face);
			}

			if (lastStateDesc.getScissorTestEnable() != _stateDesc.getScissorTestEnable())
			{
				if (_stateDesc.getScissorTestEnable())
					glEnable(GL_SCISSOR_TEST);
				else
					glDisable(GL_SCISSOR_TEST);
				lastStateDesc.setScissorTestEnable(_stateDesc.getScissorTestEnable());
			}

			if (lastStateDesc.getLinear2sRGBEnable() != _stateDesc.getLinear2sRGBEnable())
			{
				if (_stateDesc.getLinear2sRGBEnable())
					glEnable(GL_FRAMEBUFFER_SRGB);
				else
					glDisable(GL_FRAMEBUFFER_SRGB);
				lastStateDesc.setLinear2sRGBEnable(_stateDesc.getLinear2sRGBEnable());
			}

			if (_stateDesc.getDepthEnable())
			{
				if (!lastStateDesc.getDepthEnable())
				{
					glEnable(GL_DEPTH_TEST);
					lastStateDesc.setDepthEnable(true);
				}

				if (lastStateDesc.getDepthFunc() != _stateDesc.getDepthFunc())
				{
					GLenum func = EGL2Types::asCompareFunction(_stateDesc.getDepthFunc());
					glDepthFunc(func);
					lastStateDesc.setDepthFunc(_stateDesc.getDepthFunc());
				}
			}
			else
			{
				if (lastStateDesc.getDepthEnable())
				{
					glDisable(GL_DEPTH_TEST);
					lastStateDesc.setDepthEnable(false);
				}
			}

			if (lastStateDesc.getDepthWriteEnable() != _stateDesc.getDepthWriteEnable())
			{
				GLboolean enable = _stateDesc.getDepthWriteEnable() ? GL_TRUE : GL_FALSE;
				glDepthMask(enable);
				lastStateDesc.setDepthWriteEnable(_stateDesc.getDepthWriteEnable());
			}

			if (_stateDesc.getDepthBiasEnable())
			{
				if (!lastStateDesc.getDepthBiasEnable())
				{
					glEnable(GL_POLYGON_OFFSET_FILL);
					lastStateDesc.setDepthBiasEnable(true);
				}

				if (lastStateDesc.getDepthBias() != _stateDesc.getDepthBias() ||
					lastStateDesc.getDepthSlopeScaleBias() != _stateDesc.getDepthSlopeScaleBias())
				{
					glPolygonOffset(_stateDesc.getDepthSlopeScaleBias(), _stateDesc.getDepthBias());

					lastStateDesc.setDepthBias(_stateDesc.getDepthBias());
					lastStateDesc.setDepthSlopeScaleBias(_stateDesc.getDepthSlopeScaleBias());
				}
			}
			else
			{
				if (lastStateDesc.getDepthBiasEnable())
				{
					glDisable(GL_POLYGON_OFFSET_FILL);
					lastStateDesc.setDepthBiasEnable(false);
				}
			}

			if (_stateDesc.getStencilEnable())
			{
				if (!lastStateDesc.getStencilEnable())
				{
					glEnable(GL_STENCIL_TEST);
					lastStateDesc.setStencilEnable(true);
				}

				if (lastStateDesc.getStencilFrontFunc() != _stateDesc.getStencilFrontFunc() ||
					lastStateDesc.getStencilFrontRef() != _stateDesc.getStencilFrontRef() ||
					lastStateDesc.getStencilFrontReadMask() != _stateDesc.getStencilFrontReadMask())
				{
					GLenum frontfunc = EGL2Types::asCompareFunction(_stateDesc.getStencilFrontFunc());
					glStencilFuncSeparate(GL_FRONT, frontfunc, _stateDesc.getStencilFrontRef(), _stateDesc.getStencilFrontReadMask());

					lastStateDesc.setStencilFrontFunc(_stateDesc.getStencilFrontFunc());
					lastStateDesc.setStencilFrontRef(_stateDesc.getStencilFrontRef());
					lastStateDesc.setStencilFrontReadMask(_stateDesc.getStencilFrontReadMask());
				}

				if (lastStateDesc.getStencilBackFunc() != _stateDesc.getStencilBackFunc() ||
					lastStateDesc.getStencilBackRef() != _stateDesc.getStencilBackRef() ||
					lastStateDesc.getStencilBackReadMask() != _stateDesc.getStencilBackReadMask())
				{
					GLenum backfunc = EGL2Types::asCompareFunction(_stateDesc.getStencilBackFunc());
					glStencilFuncSeparate(GL_BACK, backfunc, _stateDesc.getStencilBackRef(), _stateDesc.getStencilBackReadMask());

					lastStateDesc.setStencilBackFunc(_stateDesc.getStencilBackFunc());
					lastStateDesc.setStencilBackRef(_stateDesc.getStencilBackRef());
					lastStateDesc.setStencilBackReadMask(_stateDesc.getStencilBackReadMask());
				}

				if (lastStateDesc.getStencilFrontFail() != _stateDesc.getStencilFrontFail() ||
					lastStateDesc.getStencilFrontZFail() != _stateDesc.getStencilFrontZFail() ||
					lastStateDesc.getStencilFrontPass() != _stateDesc.getStencilFrontPass())
				{
					GLenum frontfail = EGL2Types::asStencilOperation(_stateDesc.getStencilFrontFail());
					GLenum frontzfail = EGL2Types::asStencilOperation(_stateDesc.getStencilFrontZFail());
					GLenum frontpass = EGL2Types::asStencilOperation(_stateDesc.getStencilFrontPass());

					glStencilOpSeparate(GL_FRONT, frontfail, frontzfail, frontpass);

					lastStateDesc.setStencilFrontFail(_stateDesc.getStencilFrontFail());
					lastStateDesc.setStencilFrontZFail(_stateDesc.getStencilFrontZFail());
					lastStateDesc.setStencilFrontPass(_stateDesc.getStencilFrontPass());
				}

				if (lastStateDesc.getStencilBackFail() != _stateDesc.getStencilBackFail() ||
					lastStateDesc.getStencilBackZFail() != _stateDesc.getStencilBackZFail() ||
					lastStateDesc.getStencilBackPass() != _stateDesc.getStencilBackPass())
				{
					GLenum backfail = EGL2Types::asStencilOperation(_stateDesc.getStencilBackFail());
					GLenum backzfail = EGL2Types::asStencilOperation(_stateDesc.getStencilBackZFail());
					GLenum backpass = EGL2Types::asStencilOperation(_stateDesc.getStencilBackPass());
					glStencilOpSeparate(GL_BACK, backfail, backzfail, backpass);

					lastStateDesc.setStencilBackFail(_stateDesc.getStencilBackFail());
					lastStateDesc.setStencilBackZFail(_stateDesc.getStencilBackZFail());
					lastStateDesc.setStencilBackPass(_stateDesc.getStencilBackPass());
				}

				if (lastStateDesc.getStencilFrontWriteMask() != _stateDesc.getStencilFrontWriteMask())
				{
					glStencilMaskSeparate(GL_FRONT, _stateDesc.getStencilFrontWriteMask());
					lastStateDesc.setStencilFrontWriteMask(_stateDesc.getStencilFrontWriteMask());
				}

				if (lastStateDesc.getStencilBackWriteMask() != _stateDesc.getStencilBackWriteMask())
				{
					glStencilMaskSeparate(GL_BACK, _stateDesc.getStencilBackWriteMask());
					lastStateDesc.setStencilBackWriteMask(_stateDesc.getStencilBackWriteMask());
				}
			}
			else
			{
				if (lastStateDesc.getStencilEnable())
				{
					glDisable(GL_STENCIL_TEST);
					lastStateDesc.setStencilEnable(false);
				}
			}

			if (lastStateDesc.getLineWidth() != _stateDesc.getLineWidth())
			{
				glLineWidth(_stateDesc.getLineWidth());
				lastStateDesc.setLineWidth(_stateDesc.getLineWidth());
			}

			lastStateDesc.setPrimitiveType(_stateDesc.getPrimitiveType());
		}

		const GraphicsStateDesc&
		EGL2GraphicsState::getGraphicsStateDesc() const noexcept
		{
			return _stateDesc;
		}

		void
		EGL2GraphicsState::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		EGL2GraphicsState::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}
