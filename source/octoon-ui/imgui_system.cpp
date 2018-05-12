#include <octoon/ui/imgui_system.h>

#include <imgui.h>
#include <imgui_dock.h>
#include <imgui_internal.h>

#include <cstring> // std::memcpy

using namespace octoon::graphics;

namespace octoon
{
	namespace imgui
	{
		const char* vert =
			"#version 330\n"
			"uniform mat4 proj;\n"
			"layout(location  = 0) in vec4 POSITION0;\n"
			"layout(location  = 1) in vec4 COLOR0;\n"
			"layout(location  = 2) in vec4 TEXCOORD0;\n"
			"out vec4 oTexcoord0;\n"
			"out vec4 oTexcoord1;\n"
			"void main()\n"
			"{\n"
			"oTexcoord0 = COLOR0;\n"
			"oTexcoord1 = TEXCOORD0;\n"
			"gl_Position = proj * POSITION0;\n"
			"}\n";

		const char* frag =
			"#version 330\n"
			"uniform sampler2D decal;\n"
			"layout(location  = 0) out vec4 oColor;\n"
			"in vec4 oTexcoord0;\n"
			"in vec4 oTexcoord1;\n"
			"void main()\n"
			"{\n"
			"	oColor = texture(decal, oTexcoord1.rg) * oTexcoord0;\n"
			"}";

		System::System() noexcept
			: initialize_(false)
			, imguiPath_("../../ui/imgui.layout")
			, imguiDockPath_("../../ui/imgui_dock.layout")
			, ui_context_(nullptr)
		{
		}

		System::~System() noexcept
		{
			this->close();
		}

		bool
		System::open(input::WindHandle window, const GraphicsDevicePtr& device) except
		{
			assert(!initialize_);

			ui_context_ = ImGui::CreateContext();
			ImGui::SetCurrentContext(ui_context_);

			GuiStyle style;
			set_style(style);

			ImGuiIO& io = ImGui::GetIO();
			io.ImeWindowHandle = window;
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

			device_ = device;

			GraphicsDataDesc dataDesc;
			dataDesc.setType(GraphicsDataType::StorageVertexBuffer);
			dataDesc.setStreamSize(0xFFF * sizeof(ImDrawVert));
			dataDesc.setUsage(GraphicsUsageFlagBits::WriteBit);

			vbo_ = device->createGraphicsData(dataDesc);
			if (!vbo_)
				return false;

			GraphicsDataDesc elementDesc;
			elementDesc.setType(GraphicsDataType::StorageIndexBuffer);
			elementDesc.setStreamSize(0xFFF * sizeof(ImDrawIdx));
			elementDesc.setUsage(GraphicsUsageFlagBits::WriteBit);

			ibo_ = device->createGraphicsData(elementDesc);
			if (!ibo_)
				return false;

			GraphicsProgramDesc programDesc;
			programDesc.addShader(device->createShader(GraphicsShaderDesc(GraphicsShaderStageFlagBits::VertexBit, vert, "main", graphics::GraphicsShaderLang::GLSL)));
			programDesc.addShader(device->createShader(GraphicsShaderDesc(GraphicsShaderStageFlagBits::FragmentBit, frag, "main", graphics::GraphicsShaderLang::GLSL)));
			auto program = device->createProgram(programDesc);

			GraphicsInputLayoutDesc layoutDesc;
			layoutDesc.addVertexLayout(GraphicsVertexLayout(0, "POSITION", 0, GraphicsFormat::R32G32SFloat));
			layoutDesc.addVertexLayout(GraphicsVertexLayout(0, "TEXCOORD", 0, GraphicsFormat::R32G32SFloat));
			layoutDesc.addVertexLayout(GraphicsVertexLayout(0, "COLOR", 0, GraphicsFormat::R8G8B8A8UNorm));
			layoutDesc.addVertexBinding(GraphicsVertexBinding(0, layoutDesc.getVertexSize()));

			GraphicsDescriptorSetLayoutDesc descriptor_set_layout;
			descriptor_set_layout.setUniformComponents(program->getActiveParams());

			GraphicsColorBlend blend;
			blend.setBlendEnable(true);
			blend.setBlendSrc(GraphicsBlendFactor::SrcAlpha);
			blend.setBlendDest(GraphicsBlendFactor::OneMinusSrcAlpha);

			std::vector<GraphicsColorBlend> blends;
			blends.push_back(blend);

			GraphicsStateDesc stateDesc;
			stateDesc.setColorBlends(std::move(blends));
			stateDesc.setScissorTestEnable(true);
			stateDesc.setPrimitiveType(GraphicsVertexType::TriangleList);
			stateDesc.setCullMode(GraphicsCullMode::None);
			stateDesc.setDepthEnable(false);

			GraphicsPipelineDesc pipeline;
			pipeline.setGraphicsInputLayout(device->createInputLayout(layoutDesc));
			pipeline.setGraphicsState(device->createRenderState(stateDesc));
			pipeline.setGraphicsProgram(std::move(program));
			pipeline.setGraphicsDescriptorSetLayout(device->createDescriptorSetLayout(descriptor_set_layout));

			pipeline_ = device->createRenderPipeline(pipeline);
			if (!pipeline_)
				return false;

			GraphicsDescriptorSetDesc descriptor_set;
			descriptor_set.setGraphicsDescriptorSetLayout(pipeline.getDescriptorSetLayout());

			descriptor_set_ = device->createDescriptorSet(descriptor_set);
			if (!descriptor_set_)
				return false;

			auto begin = descriptor_set_->getGraphicsUniformSets().begin();
			auto end = descriptor_set_->getGraphicsUniformSets().end();

			proj_ = *std::find_if(begin, end, [](const GraphicsUniformSetPtr& set) {return set->getName() == "proj"; });
			decal_ = *std::find_if(begin, end, [](const GraphicsUniformSetPtr& set) {return set->getName() == "decal"; });

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
				ImGui::Shutdown(ui_context_);

				initialize_ = false;
			}

			if (ui_context_)
			{
				ImGui::DestroyContext(ui_context_);
				ui_context_ = nullptr;
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
		System::injectKeyPress(input::InputKey::Code key, wchar_t char_) noexcept
		{
			auto& io = ImGui::GetIO();
			if (key != input::InputKey::Code::None)
			{
				if (key == input::InputKey::Code::LeftControl)
					io.KeyCtrl = true;
				else if (key == input::InputKey::Code::LeftShift)
					io.KeyShift = true;
				else if (key == input::InputKey::Code::LeftAlt)
					io.KeyAlt = true;
				else if (key == input::InputKey::Code::LeftSuper)
					io.KeySuper = true;

				io.KeysDown[key] = true;
			}
			else
			{
				io.AddInputCharacter(char_);
			}

			return true;
		}

		bool
		System::injectKeyRelease(input::InputKey::Code key) noexcept
		{
			auto& io = ImGui::GetIO();
			if (key == input::InputKey::Code::LeftControl)
				io.KeyCtrl = false;
			else if (key == input::InputKey::Code::LeftShift)
				io.KeyShift = false;
			else if (key == input::InputKey::Code::LeftAlt)
				io.KeyAlt = false;
			else if (key == input::InputKey::Code::LeftSuper)
				io.KeySuper = false;

			io.KeysDown[key] = false;
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
			io.DisplaySize.x = (float)w;
			io.DisplaySize.y = (float)h;
		}

		void
		System::getViewport(std::uint32_t& w, std::uint32_t& h) noexcept
		{
			ImGuiIO& io = ImGui::GetIO();
			w = (std::uint32_t)io.DisplaySize.x;
			h = (std::uint32_t)io.DisplaySize.y;
		}

		void
		System::setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept
		{
			ImGuiIO& io = ImGui::GetIO();
			io.DisplayFramebufferScale.x = (float)w;
			io.DisplayFramebufferScale.y = (float)h;

			proj_->uniform4fmat(math::makeOrthoLH<float>(0.0f, (float)w, (float)h, 0.0f, 0.0f, 1.0f));
		}

		void
		System::getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept
		{
			ImGuiIO& io = ImGui::GetIO();
			w = (std::uint32_t)io.DisplayFramebufferScale.x;
			h = (std::uint32_t)io.DisplayFramebufferScale.y;
		}

		bool
		System::loadFont(const char* path, float font_size) noexcept
		{
			static const ImWchar ranges[] =
			{
				0x0020, 0x00FF, // Basic Latin + Latin Supplement
				0x4e00, 0x9FAF, // CJK Ideograms
				0,
			};

			std::uint8_t* pixels;
			int width, height;

			auto io = ImGui::GetIO();
			io.Fonts->ClearFonts();
			io.Fonts->AddFontFromFileTTF(path, font_size, 0, 0);
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

			GraphicsTextureDesc fontDesc;
			fontDesc.setSize(width, height);
			fontDesc.setTexDim(GraphicsTextureDim::Texture2D);
			fontDesc.setTexFormat(GraphicsFormat::R8G8B8A8UNorm);
			fontDesc.setStream(pixels);
			fontDesc.setStreamSize(width * height * sizeof(std::uint32_t));
			fontDesc.setTexTiling(GraphicsImageTiling::Linear);

			texture_ = device_->createTexture(fontDesc);
			if (!texture_)
				return false;

			io.Fonts->TexID = (void*)texture_.get();

			return true;
		}

		void
		System::newFrame() noexcept
		{
			ImGui::SetCurrentContext(ui_context_);
			ImGui::NewFrame();
		}

		void
		System::render(graphics::GraphicsContext& context) noexcept
		{
			assert(vbo_ && ibo_);

			ImGui::Render();

			auto drawData = ImGui::GetDrawData();
			if (!drawData)
				return;

			std::size_t totalVertexSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
			std::size_t totalIndirectSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);
			if (totalVertexSize != 0 || totalIndirectSize != 0)
			{
				if (vbo_->getGraphicsDataDesc().getStreamSize() < totalVertexSize)
				{
					GraphicsDataDesc dataDesc;
					dataDesc.setType(GraphicsDataType::StorageVertexBuffer);
					dataDesc.setStream(0);
					dataDesc.setStreamSize(totalVertexSize);
					dataDesc.setUsage(vbo_->getGraphicsDataDesc().getUsage());

					vbo_ = device_->createGraphicsData(dataDesc);
					if (!vbo_)
						return;
				}

				if (ibo_->getGraphicsDataDesc().getStreamSize() < totalIndirectSize)
				{
					GraphicsDataDesc elementDesc;
					elementDesc.setType(GraphicsDataType::StorageIndexBuffer);
					elementDesc.setStream(0);
					elementDesc.setStreamSize(totalIndirectSize);
					elementDesc.setUsage(ibo_->getGraphicsDataDesc().getUsage());

					ibo_ = device_->createGraphicsData(elementDesc);
					if (!ibo_)
						return;
				}

				ImDrawVert* vbo;
				ImDrawIdx* ibo;

				if (!vbo_->map(0, totalVertexSize, (void**)&vbo))
					return;

				if (!ibo_->map(0, totalIndirectSize, (void**)&ibo))
				{
					vbo_->unmap();
					return;
				}

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
			}

			auto& io = ImGui::GetIO();

			context.setViewport(0, float4(0, 0, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y));
			context.setScissor(0, uint4(0, 0, (std::uint32_t)io.DisplayFramebufferScale.x, (std::uint32_t)io.DisplayFramebufferScale.y));

			if (totalVertexSize != 0 || totalIndirectSize != 0)
			{
				context.setVertexBufferData(0, vbo_, 0);
				context.setIndexBufferData(ibo_, 0, GraphicsIndexType::UInt16);

				std::uint32_t vdx_buffer_offset = 0;
				std::uint32_t idx_buffer_offset = 0;

				for (int n = 0; n < drawData->CmdListsCount; n++)
				{
					const ImDrawList* cmd_list = drawData->CmdLists[n];

					for (auto cmd = cmd_list->CmdBuffer.begin(); cmd != cmd_list->CmdBuffer.end(); cmd++)
					{
						auto texture = (GraphicsTexture*)cmd->TextureId;
						if (texture)
							decal_->uniformTexture(texture->downcast_pointer<GraphicsTexture>());
						else
							decal_->uniformTexture(nullptr);

						uint4 scissor((int)cmd->ClipRect.x, (int)cmd->ClipRect.y, (int)(cmd->ClipRect.z - cmd->ClipRect.x), (int)(cmd->ClipRect.w - cmd->ClipRect.y));

						context.setScissor(0, scissor);

						context.setRenderPipeline(pipeline_);
						context.setDescriptorSet(descriptor_set_);

						context.drawIndexed(cmd->ElemCount, 1, idx_buffer_offset, vdx_buffer_offset, 0);

						idx_buffer_offset += cmd->ElemCount;
					}

					vdx_buffer_offset += cmd_list->VtxBuffer.size();
				}
			}
		}
	}
}