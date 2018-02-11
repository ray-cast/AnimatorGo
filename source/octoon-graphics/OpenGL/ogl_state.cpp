// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#include "ogl_state.h"
#include "ogl_device.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(OGLGraphicsState, GraphicsState, "OGLGraphicsState")

		OGLGraphicsState::OGLGraphicsState() noexcept
		{
		}

		OGLGraphicsState::~OGLGraphicsState() noexcept
		{
			this->close();
		}

		bool
		OGLGraphicsState::setup(const GraphicsStateDesc& desc) noexcept
		{
			_stateDesc = desc;
			return true;
		}

		void
		OGLGraphicsState::close() noexcept
		{
		}

		void
		OGLGraphicsState::apply(GraphicsStateDesc& lastStateDesc) noexcept
		{
			auto& srcBlends = _stateDesc.getColorBlends();
			auto& destBlends = lastStateDesc.getColorBlends();

			GLuint srcBlendCount = static_cast<GLuint>(srcBlends.size());
			GLuint destBlendCount = static_cast<GLuint>(destBlends.size());
			for (GLuint i = srcBlendCount; i < destBlendCount; i++)
			{
				auto& destBlend = destBlends[i];
				if (destBlend.getBlendEnable())
				{
					glDisablei(GL_BLEND, i);
					destBlend.setBlendEnable(false);
				}

				if (destBlend.getColorWriteMask() != GraphicsColorMaskFlagBits::GraphicsColorMaskFlagRGBABit)
				{
					glColorMaski(i, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
					destBlend.setColorWriteMask(GraphicsColorMaskFlagBits::GraphicsColorMaskFlagRGBABit);
				}
			}

			for (GLuint i = 0; i < srcBlendCount; i++)
			{
				auto& srcBlend = srcBlends[i];
				auto& destBlend = destBlends[i];

				if (srcBlends[i].getBlendEnable())
				{
					if (!destBlend.getBlendEnable())
					{
						glEnablei(GL_BLEND, i);
						destBlend.setBlendEnable(true);
					}

					if (destBlend.getBlendSrc() != srcBlend.getBlendSrc() ||
						destBlend.getBlendDest() != srcBlend.getBlendDest() ||
						destBlend.getBlendAlphaSrc() != srcBlend.getBlendAlphaSrc() ||
						destBlend.getBlendAlphaDest() != srcBlend.getBlendAlphaDest())
					{
						GLenum sfactorRGB = OGLTypes::asBlendFactor(srcBlend.getBlendSrc());
						GLenum dfactorRGB = OGLTypes::asBlendFactor(srcBlend.getBlendDest());
						GLenum sfactorAlpha = OGLTypes::asBlendFactor(srcBlend.getBlendAlphaSrc());
						GLenum dfactorAlpha = OGLTypes::asBlendFactor(srcBlend.getBlendAlphaDest());
		#if _DEBUG
						if (sfactorRGB == GL_INVALID_ENUM || dfactorRGB == GL_INVALID_ENUM || sfactorAlpha == GL_INVALID_ENUM || dfactorAlpha == GL_INVALID_ENUM)
						{
							this->getDevice()->downcast<OGLDevice>()->message("Invalid blend factor");
							return;
						}
		#endif

						glBlendFuncSeparatei(i, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);

						destBlend.setBlendSrc(srcBlend.getBlendSrc());
						destBlend.setBlendDest(srcBlend.getBlendDest());
						destBlend.setBlendAlphaSrc(srcBlend.getBlendAlphaSrc());
						destBlend.setBlendAlphaDest(srcBlend.getBlendAlphaDest());
					}

					if (destBlend.getBlendOp() != srcBlend.getBlendOp() ||
						destBlend.getBlendAlphaOp() != srcBlend.getBlendAlphaOp())
					{
						GLenum modeRGB = OGLTypes::asBlendOperation(srcBlend.getBlendOp());
						GLenum modeAlpha = OGLTypes::asBlendOperation(srcBlend.getBlendAlphaOp());
		#if _DEBUG
						if (modeRGB == GL_INVALID_ENUM || modeAlpha == GL_INVALID_ENUM)
						{
							this->getDevice()->downcast<OGLDevice>()->message("Invalid blend operation");
							return;
						}
		#endif

						glBlendEquationSeparatei(i, modeRGB, modeAlpha);

						destBlend.setBlendOp(srcBlend.getBlendOp());
						destBlend.setBlendAlphaOp(srcBlend.getBlendAlphaOp());
					}
				}
				else
				{
					if (destBlend.getBlendEnable())
					{
						glDisablei(GL_BLEND, i);
						destBlend.setBlendEnable(false);
					}
				}

				if (destBlend.getColorWriteMask() != srcBlend.getColorWriteMask())
				{
					auto flags = srcBlend.getColorWriteMask();

					GLboolean r = flags & GraphicsColorMaskFlagBits::GraphicsColorMaskFlagRedBit ? GL_TRUE : GL_FALSE;
					GLboolean g = flags & GraphicsColorMaskFlagBits::GraphicsColorMaskFlagGreendBit ? GL_TRUE : GL_FALSE;
					GLboolean b = flags & GraphicsColorMaskFlagBits::GraphicsColorMaskFlagBlurBit ? GL_TRUE : GL_FALSE;
					GLboolean a = flags & GraphicsColorMaskFlagBits::GraphicsColorMaskFlagAlphaBit ? GL_TRUE : GL_FALSE;

					glColorMaski(i, r, g, b, a);

					destBlend.setColorWriteMask(srcBlend.getColorWriteMask());
				}
			}

			if (lastStateDesc.getCullMode() != _stateDesc.getCullMode())
			{
				if (_stateDesc.getCullMode() != GraphicsCullMode::GraphicsCullModeNone)
				{
					GLenum mode = OGLTypes::asCullMode(_stateDesc.getCullMode());
		#if _DEBUG
					if (mode == GL_INVALID_ENUM)
					{
						this->getDevice()->downcast<OGLDevice>()->message("Invalid cull mode");
						return;
					}
		#endif

					glEnable(GL_CULL_FACE);
					glCullFace(mode);

					lastStateDesc.setCullMode(_stateDesc.getCullMode());
				}
				else
				{
					glDisable(GL_CULL_FACE);
					lastStateDesc.setCullMode(GraphicsCullMode::GraphicsCullModeNone);
				}
			}

			if (lastStateDesc.getFrontFace() != _stateDesc.getFrontFace())
			{
				GLenum face = OGLTypes::asFrontFace(_stateDesc.getFrontFace());
		#if _DEBUG
				if (face == GL_INVALID_ENUM)
				{
					this->getDevice()->downcast<OGLDevice>()->message("invalid front face");
					return;
				}
		#endif
				glFrontFace(face);
			}

			if (lastStateDesc.getPolygonMode() != _stateDesc.getPolygonMode())
			{
				GLenum mode = OGLTypes::asFillMode(_stateDesc.getPolygonMode());
		#if _DEBUG
				if (mode == GL_INVALID_ENUM)
				{
					this->getDevice()->downcast<OGLDevice>()->message("invalid FillMode");
					return;
				}
		#endif

				glPolygonMode(GL_FRONT_AND_BACK, mode);
				lastStateDesc.setPolygonMode(_stateDesc.getPolygonMode());
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
					GLenum func = OGLTypes::asCompareFunction(_stateDesc.getDepthFunc());
					if (func == GL_INVALID_ENUM)
					{
						this->getDevice()->downcast<OGLDevice>()->message("Invalid compare function");
						return;
					}

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
					GLenum frontfunc = OGLTypes::asCompareFunction(_stateDesc.getStencilFrontFunc());
					if (frontfunc == GL_INVALID_ENUM)
					{
						this->getDevice()->downcast<OGLDevice>()->message("Invalid compare function");
						return;
					}

					glStencilFuncSeparate(GL_FRONT, frontfunc, _stateDesc.getStencilFrontRef(), _stateDesc.getStencilFrontReadMask());

					lastStateDesc.setStencilFrontFunc(_stateDesc.getStencilFrontFunc());
					lastStateDesc.setStencilFrontRef(_stateDesc.getStencilFrontRef());
					lastStateDesc.setStencilFrontReadMask(_stateDesc.getStencilFrontReadMask());
				}

				if (lastStateDesc.getStencilBackFunc() != _stateDesc.getStencilBackFunc() ||
					lastStateDesc.getStencilBackRef() != _stateDesc.getStencilBackRef() ||
					lastStateDesc.getStencilBackReadMask() != _stateDesc.getStencilBackReadMask())
				{
					GLenum backfunc = OGLTypes::asCompareFunction(_stateDesc.getStencilBackFunc());
					if (backfunc == GL_INVALID_ENUM)
					{
						this->getDevice()->downcast<OGLDevice>()->message("Invalid compare function");
						return;
					}

					glStencilFuncSeparate(GL_BACK, backfunc, _stateDesc.getStencilBackRef(), _stateDesc.getStencilBackReadMask());

					lastStateDesc.setStencilBackFunc(_stateDesc.getStencilBackFunc());
					lastStateDesc.setStencilBackRef(_stateDesc.getStencilBackRef());
					lastStateDesc.setStencilBackReadMask(_stateDesc.getStencilBackReadMask());
				}

				if (lastStateDesc.getStencilFrontFail() != _stateDesc.getStencilFrontFail() ||
					lastStateDesc.getStencilFrontZFail() != _stateDesc.getStencilFrontZFail() ||
					lastStateDesc.getStencilFrontPass() != _stateDesc.getStencilFrontPass())
				{
					GLenum frontfail = OGLTypes::asStencilOperation(_stateDesc.getStencilFrontFail());
					GLenum frontzfail = OGLTypes::asStencilOperation(_stateDesc.getStencilFrontZFail());
					GLenum frontpass = OGLTypes::asStencilOperation(_stateDesc.getStencilFrontPass());
		#if _DEBUG
					if (frontfail == GL_INVALID_ENUM || frontzfail == GL_INVALID_ENUM || frontpass == GL_INVALID_ENUM)
					{
						this->getDevice()->downcast<OGLDevice>()->message("invalid StencilOperation");
						return;
					}
		#endif

					glStencilOpSeparate(GL_FRONT, frontfail, frontzfail, frontpass);

					lastStateDesc.setStencilFrontFail(_stateDesc.getStencilFrontFail());
					lastStateDesc.setStencilFrontZFail(_stateDesc.getStencilFrontZFail());
					lastStateDesc.setStencilFrontPass(_stateDesc.getStencilFrontPass());
				}

				if (lastStateDesc.getStencilBackFail() != _stateDesc.getStencilBackFail() ||
					lastStateDesc.getStencilBackZFail() != _stateDesc.getStencilBackZFail() ||
					lastStateDesc.getStencilBackPass() != _stateDesc.getStencilBackPass())
				{
					GLenum backfail = OGLTypes::asStencilOperation(_stateDesc.getStencilBackFail());
					GLenum backzfail = OGLTypes::asStencilOperation(_stateDesc.getStencilBackZFail());
					GLenum backpass = OGLTypes::asStencilOperation(_stateDesc.getStencilBackPass());
		#if _DEBUG
					if (backfail == GL_INVALID_ENUM || backzfail == GL_INVALID_ENUM || backpass == GL_INVALID_ENUM)
					{
						this->getDevice()->downcast<OGLDevice>()->message("invalid StencilOperation");
						return;
					}
		#endif

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
		OGLGraphicsState::getGraphicsStateDesc() const noexcept
		{
			return _stateDesc;
		}

		void
		OGLGraphicsState::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		OGLGraphicsState::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}