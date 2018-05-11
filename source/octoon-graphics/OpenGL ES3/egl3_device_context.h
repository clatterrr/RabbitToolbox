#ifndef OCTOON_EGL3_DEVICE_CONTEXT_H_
#define OCTOON_EGL3_DEVICE_CONTEXT_H_

#include "egl3_types.h"

namespace octoon
{
	namespace graphics
	{
		class EGL3DeviceContext final : public GraphicsContext
		{
			OctoonDeclareSubClass(EGL3DeviceContext, GraphicsContext)
		public:
			EGL3DeviceContext() noexcept;
			~EGL3DeviceContext() noexcept;

			bool setup(const GraphicsContextDesc& desc) noexcept;
			void close() noexcept;

			void renderBegin() noexcept override;
			void renderEnd() noexcept override;

			void setViewport(std::uint32_t i, const float4& viewport) noexcept override;
			const float4& getViewport(std::uint32_t i) const noexcept override;

			void setScissor(std::uint32_t i, const uint4& scissor) noexcept override;
			const uint4& getScissor(std::uint32_t i) const noexcept override;

			void setStencilCompareMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept override;
			std::uint32_t getStencilCompareMask(GraphicsStencilFaceFlags face) noexcept override;

			void setStencilReference(GraphicsStencilFaceFlags face, std::uint32_t reference) noexcept override;
			std::uint32_t getStencilReference(GraphicsStencilFaceFlags face) noexcept override;

			void setStencilWriteMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept override;
			std::uint32_t getStencilWriteMask(GraphicsStencilFaceFlags face) noexcept override;

			void setRenderPipeline(const GraphicsPipelinePtr& pipeline) noexcept override;
			GraphicsPipelinePtr getRenderPipeline() const noexcept override;

			void setDescriptorSet(const GraphicsDescriptorSetPtr& descriptorSet) noexcept override;
			GraphicsDescriptorSetPtr getDescriptorSet() const noexcept override;

			void setVertexBufferData(std::uint32_t i, const GraphicsDataPtr& data, std::intptr_t offset) noexcept override;
			GraphicsDataPtr getVertexBufferData(std::uint32_t i) const noexcept override;

			void setIndexBufferData(const GraphicsDataPtr& data, std::intptr_t offset, GraphicsIndexType indexType) noexcept override;
			GraphicsDataPtr getIndexBufferData() const noexcept override;

			void generateMipmap(const GraphicsTexturePtr& texture) noexcept override;

			void setFramebuffer(const GraphicsFramebufferPtr& target) noexcept override;
			void clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept override;
			void discardFramebuffer(std::uint32_t i) noexcept override;
			void blitFramebuffer(const GraphicsFramebufferPtr& src, const float4& v1, const GraphicsFramebufferPtr& dest, const float4& v2) noexcept override;
			void readFramebuffer(std::uint32_t i, const GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept override;
			void readFramebufferToCube(std::uint32_t i, std::uint32_t face, const GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept override;
			GraphicsFramebufferPtr getFramebuffer() const noexcept override;

			void draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept override;
			void drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept override;
			void drawIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept override;
			void drawIndexedIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept override;

			void present() noexcept override;

			void enableDebugControl(bool enable) noexcept;
			void startDebugControl() noexcept;
			void stopDebugControl() noexcept;

		private:
			bool initStateSystem() noexcept;

			static void GL_APIENTRY debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept;

		private:
			friend class EGL3Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			EGL3DeviceContext(const EGL3DeviceContext&) noexcept = delete;
			EGL3DeviceContext& operator=(const EGL3DeviceContext&) noexcept = delete;

		private:

			GLfloat _clearDepth;
			GLint   _clearStencil;
			std::vector<float4> _clearColor;

			float4 _viewport;
			uint4 _scissor;

			GLuint _inputLayout;

			GLenum  _indexType;
			GLintptr _indexOffset;

			EGL3PipelinePtr _pipeline;
			EGL3DescriptorSetPtr _descriptorSet;
			EGL3FramebufferPtr _framebuffer;
			EGL3VertexBuffers _vertexBuffers;
			EGL3GraphicsDataPtr _indexBuffer;
			EGL3ProgramPtr _program;
			EGL3SwapchainPtr _glcontext;
			EGL3GraphicsStatePtr _state;
			GraphicsStateDesc _stateCaptured;

			bool _needUpdatePipeline;
			bool _needUpdateDescriptor;
			bool _needUpdateVertexBuffers;
			bool _needEnableDebugControl;
			bool _needDisableDebugControl;

			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif