#include <octoon/ui/imgui_system.h>

#include <imgui.h>
#include <imgui_dock.h>

using namespace octoon::graphics;

namespace octoon
{
	namespace imgui
	{
		const char* vert ="\
		uniform float4x4 proj;\
		void main(\
			in float4 Position : POSITION0,\
			in float4 Diffuse : COLOR0,\
			in float4 Texcoord : TEXCOORD0,\
			out float4 oDiffuse : TEXCOORD0,\
			out float4 oTexcoord : TEXCOORD1,\
			out float4 oPosition : SV_Position)\
		{\
			oDiffuse = Diffuse;\
			oTexcoord = Texcoord;\
			oPosition = mul(proj, float4(Position.xy, 0, 1));\
		}";

		const char* frag = "\
		uniform texture2D decal;\
		SamplerState LinearClamp {Filter = MIN_MAG_MIP_POINT; AddressU = CLAMP; AddressV = CLAMP;};\
		float4 main(\
			in float4 diffuse : TEXCOORD0,\
			in float4 texcoord : TEXCOORD1) : SV_Target\
		{\
			return decal.Sample(LinearClamp, texcoord.xy) * diffuse;\
		}";

		System::System() noexcept
			: initialize_(false)
			, window_(nullptr)
			, imguiPath_("../../ui/imgui.layout")
			, imguiDockPath_("../../ui/imgui_dock.layout")
		{
		}

		System::~System() noexcept
		{
			this->close();
		}

		bool
		System::open(input::WindHandle window) except
		{
			assert(window);
			assert(!initialize_);
#if _WINDOWS
			assert(::IsWindow((HWND)window));
#endif

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

			GraphicsDeviceDesc deviceDesc;
			deviceDesc.setDeviceType(GraphicsDeviceType::OpenGL);
			device_ = GraphicsSystem::instance()->createDevice(deviceDesc);
			if (!device_)
				return false;

			GraphicsSwapchainDesc swapchainDesc;
			swapchainDesc.setWindHandle(window);
			swapchainDesc.setWidth(1);
			swapchainDesc.setHeight(1);
			swapchainDesc.setSwapInterval(GraphicsSwapInterval::Vsync);
			swapchainDesc.setImageNums(2);
			swapchainDesc.setColorFormat(GraphicsFormat::B8G8R8A8UNorm);
			swapchainDesc.setDepthStencilFormat(GraphicsFormat::D24UNorm_S8UInt);
			swapchain_ = device_->createSwapchain(swapchainDesc);
			if (!swapchain_)
				return false;

			GraphicsContextDesc contextDesc;
			contextDesc.setSwapchain(swapchain_);
			context_ = device_->createDeviceContext(contextDesc);
			if (!context_)
				return false;

			GraphicsDataDesc dataDesc;
			dataDesc.setType(GraphicsDataType::StorageVertexBuffer);
			dataDesc.setStreamSize(0xFFF * sizeof(ImDrawVert));
			dataDesc.setUsage(GraphicsUsageFlagBits::GraphicsUsageFlagWriteBit);

			vbo_ = device_->createGraphicsData(dataDesc);
			if (!vbo_)
				return false;

			GraphicsDataDesc elementDesc;
			elementDesc.setType(GraphicsDataType::StorageIndexBuffer);
			elementDesc.setStreamSize(0xFFF * sizeof(ImDrawIdx));
			elementDesc.setUsage(GraphicsUsageFlagBits::GraphicsUsageFlagWriteBit);

			ibo_ = device_->createGraphicsData(elementDesc);
			if (!ibo_)
				return false;

			GraphicsProgramDesc programDesc;
			programDesc.addShader(device_->createShader(GraphicsShaderDesc(GraphicsShaderStageVertexBit, vert)));
			programDesc.addShader(device_->createShader(GraphicsShaderDesc(GraphicsShaderStageFragmentBit, frag)));
			auto program = device_->createProgram(programDesc);

			GraphicsInputLayoutDesc layoutDesc;
			layoutDesc.addVertexLayout(GraphicsVertexLayout(0, "POSITION", 0, GraphicsFormat::R32G32B32SFloat, 0));
			layoutDesc.addVertexLayout(GraphicsVertexLayout(1, "COLOR", 0, GraphicsFormat::R8G8B8A8UNorm, 0));
			layoutDesc.addVertexLayout(GraphicsVertexLayout(2, "TEXCOORD", 0, GraphicsFormat::R32G32SFloat, 0));

			GraphicsDescriptorSetLayoutDesc descriptor_set_layout;
			descriptor_set_layout.setUniformComponents(program->getActiveParams());

			GraphicsColorBlend blend;
			blend.setBlendSrc(GraphicsBlendFactor::SrcAlpha);
			blend.setBlendDest(GraphicsBlendFactor::OneMinusSrcAlpha);

			std::vector<GraphicsColorBlend> blends;
			blends.push_back(blend);

			GraphicsStateDesc stateDesc;
			stateDesc.setColorBlends(std::move(blends));
			stateDesc.setScissorTestEnable(false);
			stateDesc.setPrimitiveType(GraphicsVertexType::TriangleList);

			GraphicsPipelineDesc pipeline;
			pipeline.setGraphicsInputLayout(device_->createInputLayout(layoutDesc));
			pipeline.setGraphicsState(device_->createRenderState(stateDesc));
			pipeline.setGraphicsProgram(std::move(program));
			pipeline.setGraphicsDescriptorSetLayout(device_->createDescriptorSetLayout(descriptor_set_layout));

			pipeline_ = device_->createRenderPipeline(pipeline);
			if (!pipeline_)
				return false;

			GraphicsDescriptorSetDesc descriptor_set;
			descriptor_set.setGraphicsDescriptorSetLayout(pipeline.getGraphicsDescriptorSetLayout());

			descriptor_set_ = device_->createDescriptorSet(descriptor_set);

			initialize_ = true;
			return true;
		}

		void
		System::close() noexcept
		{
			vbo_.reset();
			ibo_.reset();
			texture_.reset();

			context_.reset();
			swapchain_.reset();
			device_.reset();

			if (initialize_)
			{
				ImGui::ShutdownDock();
				ImGui::Shutdown();

				initialize_ = false;
			}
		}

		bool
		System::inject_mouse_move(float absx, float absy) noexcept
		{
			auto& io = ImGui::GetIO();
			io.MousePos.x = absx;
			io.MousePos.y = absy;
			return true;
		}

		bool
		System::inject_mouse_press(float absx, float absy, input::InputButton::Code code) noexcept
		{
			auto& io = ImGui::GetIO();
			io.MouseDown[code] = true;
			io.MousePos.x = absx;
			io.MousePos.y = absy;
			return true;
		}

		bool
		System::inject_mouse_release(float absx, float absy, input::InputButton::Code code) noexcept
		{
			auto& io = ImGui::GetIO();
			io.MouseDown[code] = false;
			io.MousePos.x = absx;
			io.MousePos.y = absy;
			return true;
		}

		bool
		System::inject_mouse_wheel(float wheel) noexcept
		{
			auto& io = ImGui::GetIO();
			io.MouseWheel = wheel;
			return true;
		}

		bool
		System::inject_key_press(input::InputKey::Code key, wchar_t char_) noexcept
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
		System::inject_key_release(input::InputKey::Code key) noexcept
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
		System::inject_window_focus(bool focus) noexcept
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
		System::set_viewport(std::uint32_t w, std::uint32_t h) noexcept
		{
			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize.x = w;
			io.DisplaySize.y = h;
		}

		void
		System::get_viewport(std::uint32_t& w, std::uint32_t& h) noexcept
		{
			ImGuiIO& io = ImGui::GetIO();
			w = io.DisplaySize.x;
			h = io.DisplaySize.y;
		}

		void
		System::set_framebuffer_scale(std::uint32_t w, std::uint32_t h) noexcept
		{
			ImGuiIO& io = ImGui::GetIO();
			io.DisplayFramebufferScale.x = w;
			io.DisplayFramebufferScale.y = h;

			math::float4x4 project;
			project.make_ortho_lh(0, w, h, 0, 0, 1);

			descriptor_set_->getGraphicsUniformSets()[1]->uniform4fmat(project);
		}

		void
		System::get_framebuffer_scale(std::uint32_t& w, std::uint32_t& h) noexcept
		{
			ImGuiIO& io = ImGui::GetIO();
			w = io.DisplayFramebufferScale.x;
			h = io.DisplayFramebufferScale.y;
		}

		bool
		System::load_font(const char* path, float font_size) noexcept
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
			fontDesc.setSamplerFilter(GraphicsSamplerFilter::LinearMipmapLinear, GraphicsSamplerFilter::Linear);
			fontDesc.setSamplerWrap(GraphicsSamplerWrap::ClampToEdge);

			texture_ = device_->createTexture(fontDesc);
			if (!texture_)
				return false;

			io.Fonts->TexID = (void*)texture_.get();

			return true;
		}

		void
		System::render() noexcept
		{
			assert(vbo_ && ibo_);

			auto drawData = ImGui::GetDrawData();
			if (!drawData)
				return;

			std::size_t totalVertexSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
			std::size_t totalIndirectSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);
			if (totalVertexSize == 0 || totalIndirectSize == 0)
				return;

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

			context_->renderBegin();

			context_->setViewport(0, Viewport(0, 0, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y));
			context_->setScissor(0, Scissor(0, 0, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y));

			context_->clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::One, 1, 0);

			context_->setVertexBufferData(0, vbo_, 0);
			context_->setIndexBufferData(ibo_, 0, GraphicsIndexType::UInt16);

			std::uint32_t vdx_buffer_offset = 0;
			std::uint32_t idx_buffer_offset = 0;

			for (int n = 0; n < drawData->CmdListsCount; n++)
			{
				const ImDrawList* cmd_list = drawData->CmdLists[n];

				for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
				{
					auto texture = (GraphicsTexture*)pcmd->TextureId;
					if (texture)
						descriptor_set_->getGraphicsUniformSets()[0]->uniformTexture(texture->downcast_pointer<GraphicsTexture>());
					else
						descriptor_set_->getGraphicsUniformSets()[0]->uniformTexture(nullptr);

					ImVec4 scissor((int)pcmd->ClipRect.x, (int)pcmd->ClipRect.y, (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));

					context_->setScissor(0, Scissor(scissor.x, scissor.y, scissor.z, scissor.w));
					context_->setRenderPipeline(pipeline_);
					context_->setDescriptorSet(descriptor_set_);

					context_->drawIndexed(pcmd->ElemCount, 1, idx_buffer_offset, vdx_buffer_offset, 0);

					idx_buffer_offset += pcmd->ElemCount;
				}

				vdx_buffer_offset += cmd_list->VtxBuffer.size();
			}

			context_->renderEnd();
			context_->present();
		}
	}
}