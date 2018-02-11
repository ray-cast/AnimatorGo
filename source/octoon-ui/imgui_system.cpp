#include <octoon/ui/imgui_system.h>
#include <octoon/graphics/graphics_system.h>

#include <imgui.h>
#include <imgui_dock.h>

namespace octoon
{
	namespace imgui
	{
		System::System() noexcept
			: initialize_(false)
			, window_(nullptr)
		{
		}

		System::~System() noexcept
		{
			this->close();
		}

		bool
		System::open(input::WindHandle window, float dpi) except
		{
			assert(window);
			assert(!initialize_);

#if _WINDOWS
			assert(::IsWindow((HWND)window));
#endif

			imguiPath_ = "../../";
			imguiPath_ += "ui/imgui.layout";

			imguiDockPath_ = "../../";
			imguiDockPath_ += "ui/imgui_dock.layout";

			GuiStyle style;
			setStyle(style);

			ImGuiIO& io = ImGui::GetIO();
			io.ImeWindowHandle = window_ = window;
			io.IniFilename = imguiPath_.c_str();
			io.KeyMap[ImGuiKey_Tab] = input::InputKey::Tab;
			io.KeyMap[ImGuiKey_LeftArrow] = input::InputKey::ArrowLeft;
			io.KeyMap[ImGuiKey_RightArrow] = input::InputKey::ArrowRight;
			io.KeyMap[ImGuiKey_UpArrow] = input::InputKey::ArrowUp;
			io.KeyMap[ImGuiKey_DownArrow] = input::InputKey::ArrowDown;
			io.KeyMap[ImGuiKey_PageUp] = input::InputKey::PageUp;
			io.KeyMap[ImGuiKey_PageDown] = input::InputKey::PageDown;
			io.KeyMap[ImGuiKey_Home] = input::InputKey::Home;
			io.KeyMap[ImGuiKey_End] = input::InputKey::End;
			io.KeyMap[ImGuiKey_Delete] = input::InputKey::Delete;
			io.KeyMap[ImGuiKey_Backspace] = input::InputKey::Backspace;
			io.KeyMap[ImGuiKey_Enter] = input::InputKey::KP_Enter;
			io.KeyMap[ImGuiKey_Escape] = input::InputKey::Escape;
			io.KeyMap[ImGuiKey_A] = input::InputKey::A;
			io.KeyMap[ImGuiKey_C] = input::InputKey::C;
			io.KeyMap[ImGuiKey_V] = input::InputKey::V;
			io.KeyMap[ImGuiKey_X] = input::InputKey::X;
			io.KeyMap[ImGuiKey_Y] = input::InputKey::Y;
			io.KeyMap[ImGuiKey_Z] = input::InputKey::Z;

			static const ImWchar ranges[] =
			{
				0x0020, 0x00FF, // Basic Latin + Latin Supplement
				0x4e00, 0x9FAF, // CJK Ideograms
				0,
			};

			std::uint8_t* pixels;
			int width, height;

			io.Fonts->ClearFonts();
			io.Fonts->AddFontFromFileTTF("../../system/fonts/DroidSansFallback.ttf", 15.0f * dpi, 0, 0);
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

			graphics::GraphicsDeviceDesc deviceDesc;
			deviceDesc.setDeviceType(graphics::GraphicsDeviceType::GraphicsDeviceTypeOpenGL);
			graphicsDevice_ = graphics::GraphicsSystem::instance()->createDevice(deviceDesc);
			if (!graphicsDevice_)
				return false;

			graphics::GraphicsSwapchainDesc swapchainDesc;
			swapchainDesc.setWindHandle(window);
			swapchainDesc.setWidth(1);
			swapchainDesc.setHeight(1);
			swapchainDesc.setSwapInterval(graphics::GraphicsSwapInterval::GraphicsSwapIntervalVsync);
			swapchainDesc.setImageNums(2);
			swapchainDesc.setColorFormat(graphics::GraphicsFormat::GraphicsFormatB8G8R8A8UNorm);
			swapchainDesc.setDepthStencilFormat(graphics::GraphicsFormat::GraphicsFormatD24UNorm_S8UInt);
			graphicsSwapchain_ = graphicsDevice_->createSwapchain(swapchainDesc);
			if (!graphicsSwapchain_)
				return false;

			graphics::GraphicsContextDesc contextDesc;
			contextDesc.setSwapchain(graphicsSwapchain_);
			graphicsContext_ = graphicsDevice_->createDeviceContext(contextDesc);
			if (!graphicsContext_)
				return false;

			graphics::GraphicsTextureDesc fontDesc;
			fontDesc.setSize(width, height);
			fontDesc.setTexDim(graphics::GraphicsTextureDim::GraphicsTextureDim2D);
			fontDesc.setTexFormat(graphics::GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
			fontDesc.setStream(pixels);
			fontDesc.setStreamSize(width * height * sizeof(std::uint32_t));
			fontDesc.setTexTiling(graphics::GraphicsImageTiling::GraphicsImageTilingLinear);
			fontDesc.setSamplerFilter(graphics::GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapLinear, graphics::GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
			fontDesc.setSamplerWrap(graphics::GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);

			texture_ = graphicsDevice_->createTexture(fontDesc);
			if (!texture_)
				return false;

			io.Fonts->TexID = (void*)texture_.get();

			graphics::GraphicsDataDesc dataDesc;
			dataDesc.setType(graphics::GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
			dataDesc.setStream(0);
			dataDesc.setStreamSize(4096 * sizeof(ImDrawVert));
			dataDesc.setUsage(graphics::GraphicsUsageFlagBits::GraphicsUsageFlagWriteBit);

			vbo_ = graphicsDevice_->createGraphicsData(dataDesc);
			if (!vbo_)
				return false;

			graphics::GraphicsDataDesc elementDesc;
			elementDesc.setType(graphics::GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
			elementDesc.setStream(0);
			elementDesc.setStreamSize(4096 * sizeof(ImDrawIdx));
			elementDesc.setUsage(graphics::GraphicsUsageFlagBits::GraphicsUsageFlagWriteBit);

			ibo_ = graphicsDevice_->createGraphicsData(elementDesc);
			if (!ibo_)
				return false;

			initialize_ = true;
			return true;
		}

		void
		System::close() noexcept
		{
			vbo_.reset();
			ibo_.reset();
			texture_.reset();

			if (initialize_)
			{
				ImGui::ShutdownDock();
				ImGui::Shutdown();

				initialize_ = false;
			}
		}

		bool
		System::injectMouseMove(float absx, float absy) noexcept
		{
			auto& io = ImGui::GetIO();
			io.MousePos.x = absx;
			io.MousePos.y = absy;
			return true;
		}

		bool
		System::injectMousePress(float absx, float absy, input::InputButton::Code code) noexcept
		{
			auto& io = ImGui::GetIO();
			io.MouseDown[code] = true;
			io.MousePos.x = absx;
			io.MousePos.y = absy;
			return true;
		}

		bool
		System::injectMouseRelease(float absx, float absy, input::InputButton::Code code) noexcept
		{
			auto& io = ImGui::GetIO();
			io.MouseDown[code] = false;
			io.MousePos.x = absx;
			io.MousePos.y = absy;
			return true;
		}

		bool
		System::injectMouseWheel(float wheel) noexcept
		{
			auto& io = ImGui::GetIO();
			io.MouseWheel = wheel;
			return true;
		}

		bool
		System::injectKeyPress(input::InputKey::Code _key, wchar_t _char) noexcept
		{
			auto& io = ImGui::GetIO();
			if (_key != input::InputKey::Code::None)
			{
				if (_key == input::InputKey::Code::LeftControl)
					io.KeyCtrl = true;
				else if (_key == input::InputKey::Code::LeftShift)
					io.KeyShift = true;
				else if (_key == input::InputKey::Code::LeftAlt)
					io.KeyAlt = true;
				else if (_key == input::InputKey::Code::LeftSuper)
					io.KeySuper = true;

				io.KeysDown[_key] = true;
			}
			else
			{
				io.AddInputCharacter(_char);
			}

			return true;
		}

		bool
		System::injectKeyRelease(input::InputKey::Code _key) noexcept
		{
			auto& io = ImGui::GetIO();
			if (_key == input::InputKey::Code::LeftControl)
				io.KeyCtrl = false;
			else if (_key == input::InputKey::Code::LeftShift)
				io.KeyShift = false;
			else if (_key == input::InputKey::Code::LeftAlt)
				io.KeyAlt = false;
			else if (_key == input::InputKey::Code::LeftSuper)
				io.KeySuper = false;

			io.KeysDown[_key] = false;
			return true;
		}

		bool
		System::injectWindowFocus(bool focus) noexcept
		{
			if (focus)
			{
				auto& io = ImGui::GetIO();

				std::memset(io.KeysDown, 0, sizeof(io.KeysDown));
				std::memset(io.MouseDown, 0, sizeof(io.MouseDown));
			}

			return true;
		}

		void
		System::setViewport(std::uint32_t w, std::uint32_t h) noexcept
		{
			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize.x = w;
			io.DisplaySize.y = h;
		}

		void
		System::getViewport(std::uint32_t& w, std::uint32_t& h) noexcept
		{
			ImGuiIO& io = ImGui::GetIO();
			w = io.DisplaySize.x;
			h = io.DisplaySize.y;
		}

		void
		System::setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept
		{
			ImGuiIO& io = ImGui::GetIO();
			io.DisplayFramebufferScale.x = w;
			io.DisplayFramebufferScale.y = h;

			math::float4x4 project;
			project.make_ortho_lh(0, w, h, 0, 0, 1);

			// _materialProj->uniform4fmat(project);
		}

		void
		System::getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept
		{
			ImGuiIO& io = ImGui::GetIO();
			w = io.DisplayFramebufferScale.x;
			h = io.DisplayFramebufferScale.y;
		}

		void
		System::render() except
		{
			auto drawData = ImGui::GetDrawData();
			if (!drawData)
				return;

			std::size_t totalVertexSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
			std::size_t totalIndirectSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);
			if (totalVertexSize == 0 || totalIndirectSize == 0)
				return;

			if (!vbo_ || vbo_->getGraphicsDataDesc().getStreamSize() < totalVertexSize)
			{
				graphics::GraphicsDataDesc dataDesc;
				dataDesc.setType(graphics::GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
				dataDesc.setStream(0);
				dataDesc.setStreamSize(totalVertexSize);
				dataDesc.setUsage(vbo_->getGraphicsDataDesc().getUsage());

				vbo_ = graphicsDevice_->createGraphicsData(dataDesc);
				if (!vbo_)
				return;
			}

			if (!ibo_ || ibo_->getGraphicsDataDesc().getStreamSize() < totalIndirectSize)
			{
				graphics::GraphicsDataDesc elementDesc;
				elementDesc.setType(graphics::GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
				elementDesc.setStream(0);
				elementDesc.setStreamSize(totalIndirectSize);
				elementDesc.setUsage(ibo_->getGraphicsDataDesc().getUsage());

				ibo_ = graphicsDevice_->createGraphicsData(elementDesc);
				if (!ibo_)
					return;
			}

			ImDrawVert* vbo;
			ImDrawIdx* ibo;

			if (!vbo_->map(0, totalVertexSize, (void**)&vbo))
				return;

			if (!ibo_->map(0, totalIndirectSize, (void**)&ibo))
				return;

			for (int n = 0; n < drawData->CmdListsCount; n++)
			{
				const ImDrawList* cmd_list = drawData->CmdLists[n];
				std::memcpy(vbo, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.size() * sizeof(ImDrawVert));
				std::memcpy(ibo, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx));
				vbo += cmd_list->VtxBuffer.size();
				ibo += cmd_list->IdxBuffer.size();
			}

			vbo_->unmap();
			ibo_->unmap();

			auto& io = ImGui::GetIO();

			graphicsContext_->renderBegin();

			graphicsContext_->clearFramebuffer(0, graphics::GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4(0.1,0.2,0.3), 1, 0);
			graphicsContext_->setViewport(0, graphics::Viewport(0, 0, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y));
			graphicsContext_->setScissor(0, graphics::Scissor(0, 0, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y));

			graphicsContext_->setVertexBufferData(0, vbo_, 0);
			graphicsContext_->setIndexBufferData(ibo_, 0, graphics::GraphicsIndexType::GraphicsIndexTypeUInt16);

			std::uint32_t vdx_buffer_offset = 0;
			std::uint32_t idx_buffer_offset = 0;

			/*for (int n = 0; n < drawData->CmdListsCount; n++)
			{
				const ImDrawList* cmd_list = drawData->CmdLists[n];

				for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
				{
					auto texture = (graphics::GraphicsTexture*)pcmd->TextureId;
					if (texture)
						_materialDecal->uniformTexture(texture->downcast_pointer<graphics::GraphicsTexture>());
					else
						_materialDecal->uniformTexture(nullptr);

					ImVec4 scissor((int)pcmd->ClipRect.x, (int)pcmd->ClipRect.y, (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));

					graphicsContext_->setScissor(0, graphics::Scissor(scissor.x, scissor.y, scissor.z, scissor.w));

					//graphicsContext_->setMaterialPass(_materialTech->getPass(0));
					graphicsContext_->drawIndexed(pcmd->ElemCount, 1, idx_buffer_offset, vdx_buffer_offset, 0);

					idx_buffer_offset += pcmd->ElemCount;
				}

				vdx_buffer_offset += cmd_list->VtxBuffer.size();
			}*/

			graphicsContext_->renderEnd();
			graphicsContext_->present();
		}
	}
}