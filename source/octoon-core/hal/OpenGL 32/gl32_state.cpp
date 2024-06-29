#include "gl32_state.h"

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL32GraphicsState, RenderState, "GL32GraphicsState")

		GL32GraphicsState::GL32GraphicsState() noexcept
		{
		}

		GL32GraphicsState::~GL32GraphicsState() noexcept
		{
		}

		bool
		GL32GraphicsState::setup(const RenderStateDesc& desc) noexcept
		{
			_stateDesc = desc;
			return true;
		}

		void
		GL32GraphicsState::close() noexcept
		{
		}

		void
		GL32GraphicsState::apply(RenderStateDesc& lastStateDesc) noexcept
		{
			if (lastStateDesc.getBlendEnable() != _stateDesc.getBlendEnable())
			{
				if (_stateDesc.getBlendEnable())
					glEnablei(GL_BLEND, 0);
				else
					glDisablei(GL_BLEND, 0);
				lastStateDesc.setScissorTestEnable(_stateDesc.getBlendEnable());
			}

			if (_stateDesc.getBlendEnable())
			{
				if (lastStateDesc.getBlendSrc() != _stateDesc.getBlendSrc() ||
					lastStateDesc.getBlendDest() != _stateDesc.getBlendDest() ||
					lastStateDesc.getBlendAlphaSrc() != _stateDesc.getBlendAlphaSrc() ||
					lastStateDesc.getBlendAlphaDest() != _stateDesc.getBlendAlphaDest())
				{
					GLenum sfactorRGB = GL32Types::asBlendFactor(_stateDesc.getBlendSrc());
					GLenum dfactorRGB = GL32Types::asBlendFactor(_stateDesc.getBlendDest());
					GLenum sfactorAlpha = GL32Types::asBlendFactor(_stateDesc.getBlendAlphaSrc());
					GLenum dfactorAlpha = GL32Types::asBlendFactor(_stateDesc.getBlendAlphaDest());

					glBlendFuncSeparatei(0, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);

					lastStateDesc.setBlendSrc(_stateDesc.getBlendSrc());
					lastStateDesc.setBlendDest(_stateDesc.getBlendDest());
					lastStateDesc.setBlendAlphaSrc(_stateDesc.getBlendAlphaSrc());
					lastStateDesc.setBlendAlphaDest(_stateDesc.getBlendAlphaDest());
				}

				if (lastStateDesc.getBlendOp() != _stateDesc.getBlendOp() ||
					lastStateDesc.getBlendAlphaOp() != _stateDesc.getBlendAlphaOp())
				{
					GLenum modeRGB = GL32Types::asBlendOperation(_stateDesc.getBlendOp());
					GLenum modeAlpha = GL32Types::asBlendOperation(_stateDesc.getBlendAlphaOp());

					glBlendEquationSeparatei(0, modeRGB, modeAlpha);

					lastStateDesc.setBlendOp(_stateDesc.getBlendOp());
					lastStateDesc.setBlendAlphaOp(_stateDesc.getBlendAlphaOp());
				}
			}

			if (lastStateDesc.getColorWriteMask() != _stateDesc.getColorWriteMask())
			{
				auto flags = _stateDesc.getColorWriteMask();

				GLboolean r = flags & ColorWriteMask::RedBit ? GL_TRUE : GL_FALSE;
				GLboolean g = flags & ColorWriteMask::GreendBit ? GL_TRUE : GL_FALSE;
				GLboolean b = flags & ColorWriteMask::BlurBit ? GL_TRUE : GL_FALSE;
				GLboolean a = flags & ColorWriteMask::AlphaBit ? GL_TRUE : GL_FALSE;

				glColorMaski(0, r, g, b, a);

				lastStateDesc.setColorWriteMask(_stateDesc.getColorWriteMask());
			}

			if (lastStateDesc.getCullMode() != _stateDesc.getCullMode())
			{
				if (_stateDesc.getCullMode() != CullMode::Off)
				{
					GLenum mode = GL32Types::asCullMode(_stateDesc.getCullMode());
					glEnable(GL_CULL_FACE);
					glCullFace(mode);

					lastStateDesc.setCullMode(_stateDesc.getCullMode());
				}
				else
				{
					glDisable(GL_CULL_FACE);
					lastStateDesc.setCullMode(CullMode::Off);
				}
			}

			if (lastStateDesc.getFrontFace() != _stateDesc.getFrontFace())
			{
				GLenum face = GL32Types::asFrontFace(_stateDesc.getFrontFace());
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

			if (_stateDesc.getDepthEnable())
			{
				if (!lastStateDesc.getDepthEnable())
				{
					glEnable(GL_DEPTH_TEST);
					lastStateDesc.setDepthEnable(true);
				}

				if (lastStateDesc.getDepthFunc() != _stateDesc.getDepthFunc())
				{
					GLenum func = GL32Types::asCompareFunction(_stateDesc.getDepthFunc());
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
					GLenum frontfunc = GL32Types::asCompareFunction(_stateDesc.getStencilFrontFunc());
					glStencilFuncSeparate(GL_FRONT, frontfunc, _stateDesc.getStencilFrontRef(), _stateDesc.getStencilFrontReadMask());

					lastStateDesc.setStencilFrontFunc(_stateDesc.getStencilFrontFunc());
					lastStateDesc.setStencilFrontRef(_stateDesc.getStencilFrontRef());
					lastStateDesc.setStencilFrontReadMask(_stateDesc.getStencilFrontReadMask());
				}

				if (lastStateDesc.getStencilBackFunc() != _stateDesc.getStencilBackFunc() ||
					lastStateDesc.getStencilBackRef() != _stateDesc.getStencilBackRef() ||
					lastStateDesc.getStencilBackReadMask() != _stateDesc.getStencilBackReadMask())
				{
					GLenum backfunc = GL32Types::asCompareFunction(_stateDesc.getStencilBackFunc());
					glStencilFuncSeparate(GL_BACK, backfunc, _stateDesc.getStencilBackRef(), _stateDesc.getStencilBackReadMask());

					lastStateDesc.setStencilBackFunc(_stateDesc.getStencilBackFunc());
					lastStateDesc.setStencilBackRef(_stateDesc.getStencilBackRef());
					lastStateDesc.setStencilBackReadMask(_stateDesc.getStencilBackReadMask());
				}

				if (lastStateDesc.getStencilFrontFail() != _stateDesc.getStencilFrontFail() ||
					lastStateDesc.getStencilFrontZFail() != _stateDesc.getStencilFrontZFail() ||
					lastStateDesc.getStencilFrontPass() != _stateDesc.getStencilFrontPass())
				{
					GLenum frontfail = GL32Types::asStencilOperation(_stateDesc.getStencilFrontFail());
					GLenum frontzfail = GL32Types::asStencilOperation(_stateDesc.getStencilFrontZFail());
					GLenum frontpass = GL32Types::asStencilOperation(_stateDesc.getStencilFrontPass());

					glStencilOpSeparate(GL_FRONT, frontfail, frontzfail, frontpass);

					lastStateDesc.setStencilFrontFail(_stateDesc.getStencilFrontFail());
					lastStateDesc.setStencilFrontZFail(_stateDesc.getStencilFrontZFail());
					lastStateDesc.setStencilFrontPass(_stateDesc.getStencilFrontPass());
				}

				if (lastStateDesc.getStencilBackFail() != _stateDesc.getStencilBackFail() ||
					lastStateDesc.getStencilBackZFail() != _stateDesc.getStencilBackZFail() ||
					lastStateDesc.getStencilBackPass() != _stateDesc.getStencilBackPass())
				{
					GLenum backfail = GL32Types::asStencilOperation(_stateDesc.getStencilBackFail());
					GLenum backzfail = GL32Types::asStencilOperation(_stateDesc.getStencilBackZFail());
					GLenum backpass = GL32Types::asStencilOperation(_stateDesc.getStencilBackPass());
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

		const RenderStateDesc&
		GL32GraphicsState::getStateDesc() const noexcept
		{
			return _stateDesc;
		}

		void
		GL32GraphicsState::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL32GraphicsState::getDevice() const noexcept
		{
			return _device.lock();
		}
	}
}