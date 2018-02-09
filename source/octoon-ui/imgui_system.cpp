#include <octoon/ui/imgui_system.h>

#include <imgui.h>
#include <imgui_dock.h>

namespace octoon
{
	namespace imgui
	{
		System::System() noexcept
			: _initialize(false)
		{
		}

		System::~System() noexcept
		{
			this->close();
		}

		bool
		System::open(input::WindHandle _window, float dpi) except
		{
			assert(_window);
			assert(!_initialize);

#if _WINDOWS
			assert(::IsWindow((HWND)_window));
#endif

			_imguiPath = "../../";
			_imguiPath += "ui/imgui.layout";

			_imguiDockPath = "../../";
			_imguiDockPath += "ui/imgui_dock.layout";

			GuiStyle style;
			setStyle(style);

			ImGuiIO& io = ImGui::GetIO();
			io.ImeWindowHandle = _window;
			io.IniFilename = _imguiPath.c_str();
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

			//io.Fonts->AddFontFromFileTTF("../../system/fonts/DroidSansFallback.ttf", 15.0f * dpi, 0, 0);
			io.Fonts->AddFontFromFileTTF("../../system/fonts/DroidSansFallback.ttf", 15.0f * dpi, 0, &ranges[0]);
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

			/*GraphicsTextureDesc fontDesc;
			fontDesc.setSize(width, height);
			fontDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
			fontDesc.setTexFormat(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
			fontDesc.setStream(pixels);
			fontDesc.setStreamSize(width * height * sizeof(std::uint32_t));
			fontDesc.setTexTiling(GraphicsImageTiling::GraphicsImageTilingLinear);
			fontDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapLinear, GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
			fontDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
			_texture = RenderSystem::instance()->createTexture(fontDesc);
			if (!_texture)
			return false;

			io.Fonts->TexID = (void*)_texture.get();

			GraphicsDataDesc dataDesc;
			dataDesc.setType(GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
			dataDesc.setStream(0);
			dataDesc.setStreamSize(4096 * sizeof(ImDrawVert));
			dataDesc.setUsage(GraphicsUsageFlagBits::GraphicsUsageFlagWriteBit);
			_vbo = RenderSystem::instance()->createGraphicsData(dataDesc);
			if (!_vbo)
			return false;

			GraphicsDataDesc elementDesc;
			elementDesc.setType(GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
			elementDesc.setStream(0);
			elementDesc.setStreamSize(4096 * sizeof(ImDrawIdx));
			elementDesc.setUsage(GraphicsUsageFlagBits::GraphicsUsageFlagWriteBit);
			_ibo = RenderSystem::instance()->createGraphicsData(elementDesc);
			if (!_ibo)
			return false;

			_material = RenderSystem::instance()->createMaterial("sys:fx/uilayout.fxml");
			if (!_material)
			return false;

			_materialTech = _material->getTech("IMGUI");
			_materialDecal = _material->getParameter("decal");
			_materialDecal->uniformTexture(_texture);

			_materialProj = _material->getParameter("proj");*/

			ImGui::LoadDock(_imguiDockPath.c_str());

			_initialize = true;
			return true;
		}

		void
		System::close() noexcept
		{
			/*_vbo.reset();
			_ibo.reset();
			_texture.reset();
			_material.reset();

			if (_initialize)
			{
			std::string path;
			IoServer::instance()->getAssign("sys", path);

			path += "ui/imgui_dock.layout";

			ImGui::SaveDock(path.c_str());
			ImGui::ShutdownDock();
			ImGui::Shutdown();

			_initialize = false;
			}*/
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

			/*math::float4x4 project;
			if (RenderSystem::instance()->getRenderSetting().deviceType == GraphicsDeviceType::GraphicsDeviceTypeVulkan)
			project.makeOrtho_lh(0, w, 0, h, 0, 1);
			else
			project.makeOrtho_lh(0, w, h, 0, 0, 1);

			_materialProj->uniform4fmat(project);*/
		}

		void
		System::getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept
		{
			ImGuiIO& io = ImGui::GetIO();
			w = io.DisplayFramebufferScale.x;
			h = io.DisplayFramebufferScale.y;
		}

		void
		System::render(float delta) except
		{
			/*auto renderer = RenderSystem::instance();

			auto drawData = ImGui::GetDrawData();

			std::size_t totalVertexSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
			std::size_t totalIndirectSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);
			if (totalVertexSize == 0 || totalIndirectSize == 0)
			return;

			if (!_vbo || _vbo->getGraphicsDataDesc().getStreamSize() < totalVertexSize)
			{
			GraphicsDataDesc dataDesc;
			dataDesc.setType(GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
			dataDesc.setStream(0);
			dataDesc.setStreamSize(totalVertexSize);
			dataDesc.setUsage(_vbo->getGraphicsDataDesc().getUsage());
			_vbo = renderer->createGraphicsData(dataDesc);
			if (!_vbo)
			return;
			}

			if (!_ibo || _ibo->getGraphicsDataDesc().getStreamSize() < totalIndirectSize)
			{
			GraphicsDataDesc elementDesc;
			elementDesc.setType(GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
			elementDesc.setStream(0);
			elementDesc.setStreamSize(totalIndirectSize);
			elementDesc.setUsage(_ibo->getGraphicsDataDesc().getUsage());
			_ibo = renderer->createGraphicsData(elementDesc);
			if (!_ibo)
			return;
			}

			ImDrawVert* vbo;
			ImDrawIdx* ibo;

			if (!_vbo->map(0, totalVertexSize, (void**)&vbo))
			return;

			if (!_ibo->map(0, totalIndirectSize, (void**)&ibo))
			return;

			for (int n = 0; n < drawData->CmdListsCount; n++)
			{
			const ImDrawList* cmd_list = drawData->CmdLists[n];
			std::memcpy(vbo, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.size() * sizeof(ImDrawVert));
			std::memcpy(ibo, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx));
			vbo += cmd_list->VtxBuffer.size();
			ibo += cmd_list->IdxBuffer.size();
			}

			_vbo->unmap();
			_ibo->unmap();

			auto& io = ImGui::GetIO();

			renderer->clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1, 0);
			renderer->setViewport(0, Viewport(0, 0, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y));
			renderer->setScissor(0, Scissor(0, 0, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y));

			renderer->setVertexBuffer(0, _vbo, 0);
			renderer->setIndexBuffer(_ibo, 0, GraphicsIndexType::GraphicsIndexTypeUInt16);

			std::uint32_t vdx_buffer_offset = 0;
			std::uint32_t idx_buffer_offset = 0;

			for (int n = 0; n < drawData->CmdListsCount; n++)
			{
			const ImDrawList* cmd_list = drawData->CmdLists[n];

			for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
			{
			auto texture = (GraphicsTexture*)pcmd->TextureId;
			if (texture)
			_materialDecal->uniformTexture(texture->downcast_pointer<GraphicsTexture>());
			else
			_materialDecal->uniformTexture(nullptr);

			ImVec4 scissor((int)pcmd->ClipRect.x, (int)pcmd->ClipRect.y, (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));

			renderer->setScissor(0, Scissor(scissor.x, scissor.y, scissor.z, scissor.w));
			renderer->setMaterialPass(_materialTech->getPass(0));

			renderer->drawIndexed(pcmd->ElemCount, 1, idx_buffer_offset, vdx_buffer_offset, 0);

			idx_buffer_offset += pcmd->ElemCount;
			}

			vdx_buffer_offset += cmd_list->VtxBuffer.size();
			}*/
		}
	}
}