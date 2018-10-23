//
// Copyright (c) 2018 Amer Koleci and contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "../CommandContext.h"
#include "D3D11Prerequisites.h"

namespace Alimer
{
    class D3D11Shader;
    class D3D11Framebuffer;
    class D3D11GraphicsDevice;

    class D3D11CommandContext final : public CommandContext
    {
    public:
        /// Constructor.
        D3D11CommandContext(D3D11GraphicsDevice* device, ID3D11DeviceContext1* context);

        /// Destructor.
        ~D3D11CommandContext() override;

        void FlushImpl(bool waitForCompletion) override;
        void BeginRenderPassImpl(Framebuffer* framebuffer, const RenderPassBeginDescriptor* descriptor) override;
        void EndRenderPassImpl() override;

        void SetProgramImpl(Program* program) override;

        void SetVertexDescriptor(const VertexDescriptor* descriptor) override;
        void SetVertexBufferImpl(GpuBuffer* buffer, uint32_t offset) override;
        void SetVertexBuffersImpl(uint32_t firstBinding, uint32_t count, const GpuBuffer** buffers, const uint32_t* offsets) override;
        void SetIndexBufferImpl(GpuBuffer* buffer, uint32_t offset, IndexType indexType) override;

        void DrawImpl(PrimitiveTopology topology, uint32_t vertexStart, uint32_t vertexCount) override;
        void DrawInstancedImpl(PrimitiveTopology topology, uint32_t vertexStart, uint32_t vertexCount, uint32_t instanceCount, uint32_t baseInstance) override;

        void SetViewport(const rect& viewport) override;
        void SetScissor(const irect& scissor) override;

        void DispatchImpl(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;

    private:
        void BeginContext() override;
        bool PrepareDraw(PrimitiveTopology topology);
        void FlushDescriptorSet(uint32_t set);
        void FlushDescriptorSets();

    private:
        ID3D11DeviceContext1* _context;
        bool _immediate;
        bool _needWorkaround = false;

        const D3D11Framebuffer* _currentFramebuffer = nullptr;
        uint32_t _currentColorAttachmentsBound;
        PrimitiveTopology _currentTopology;
        D3D11Shader* _currentShader;
        ID3D11RasterizerState1* _currentRasterizerState;
        ID3D11DepthStencilState* _currentDepthStencilState;
        ID3D11BlendState1* _currentBlendState;

        struct BufferBindingInfo {
            GpuBuffer*  buffer;
            uint32_t    offset;
            uint32_t    range;
        };

        struct ResourceBinding
        {
            union {
                BufferBindingInfo buffer;
            };
        };

        struct ResourceBindings
        {
            ResourceBinding bindings[MaxDescriptorSets][MaxBindingsPerSet];
            uint8_t push_constant_data[MaxDescriptorSets];
        };

        ID3D11Buffer* _currentVertexBuffers[MaxVertexBufferBindings];
        UINT _vboStrides[MaxVertexBufferBindings];
        UINT _vboOffsets[MaxVertexBufferBindings];

        bool _inputLayoutDirty;
        ResourceBindings _bindings;
        uint32_t _dirtySets = 0;
        uint32_t _dirtyVbos = 0;

        /// Current input layout: vertex buffers' element mask and vertex shader's element mask combined.
        InputLayoutDesc _currentInputLayout;
    };
}
