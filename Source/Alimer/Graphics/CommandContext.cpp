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

#include "../Graphics/CommandContext.h"
#include "../Graphics/Graphics.h"
#include "../Math/MathUtil.h"
#include "../Core/Log.h"

namespace Alimer
{
    CommandContext::CommandContext(Graphics* graphics)
        : _graphics(graphics)
        , _insideRenderPass(false)
    {
    }

    CommandContext& CommandContext::Begin(const String name)
    {
        CommandContext* newContext = nullptr;
        newContext->SetName(name);
        if (!name.IsEmpty())
        {
            //GpuProfiler::BeginBlock(name, newContext);
        }

        return *newContext;
    }

    uint64_t CommandContext::Flush(bool waitForCompletion)
    {
        return FlushImpl(waitForCompletion);
    }

    void CommandContext::BeginDefaultRenderPass(const Color4& clearColor, float clearDepth, uint8_t clearStencil)
    {
        RenderPassBeginDescriptor descriptor = {};
        descriptor.colors[0].loadAction = LoadAction::Clear;
        descriptor.colors[0].storeAction = StoreAction::Store;
        descriptor.colors[0].clearColor = clearColor;

        descriptor.depthStencil.depthLoadAction = LoadAction::Clear;
        descriptor.depthStencil.depthStoreAction = StoreAction::Store;
        descriptor.depthStencil.stencilLoadAction = LoadAction::DontCare;
        descriptor.depthStencil.stencilStoreAction = StoreAction::DontCare;
        descriptor.depthStencil.clearDepth = clearDepth;
        descriptor.depthStencil.clearStencil = clearStencil;

        BeginRenderPass(_graphics->GetMainWindow()->GetCurrentFramebuffer(), &descriptor);
    }

    void CommandContext::BeginDefaultRenderPass(const RenderPassBeginDescriptor* descriptor)
    {
        ALIMER_ASSERT_MSG(descriptor, "Invalid descriptor");
        BeginRenderPass(_graphics->GetMainWindow()->GetCurrentFramebuffer(), descriptor);
    }

    void CommandContext::BeginRenderPass(Framebuffer* framebuffer, const RenderPassBeginDescriptor* descriptor)
    {
        ALIMER_ASSERT(framebuffer);

        BeginRenderPassImpl(framebuffer, descriptor);
        _insideRenderPass = true;
    }

    void CommandContext::EndRenderPass()
    {
        EndRenderPassImpl();
        _insideRenderPass = false;
    }

    void CommandContext::SetPipeline(Pipeline* pipeline)
    {
        ALIMER_ASSERT(pipeline);
        _currentPipeline = pipeline;
        //SetPipelineImpl(pipeline);
    }

    void CommandContext::SetVertexBuffer(uint32_t binding, GpuBuffer* buffer, uint32_t offset)
    {
        ALIMER_ASSERT(buffer);
        ALIMER_ASSERT(binding < MaxVertexBufferBindings);
#if defined(ALIMER_DEV)
        if (!any(buffer->GetUsage() & BufferUsage::Vertex))
        {
            Graphics::NotifyValidationError("SetVertexBuffer need buffer with Vertex usage");
            return;
        }
#endif
        //SetVertexBufferImpl(buffer, offset);
    }

    void CommandContext::SetVertexBuffers(uint32_t firstBinding, uint32_t count, const GpuBuffer** buffers, const uint32_t* offsets)
    {
        ALIMER_ASSERT(firstBinding + count < MaxVertexBufferBindings);
        for (uint32_t i = firstBinding; i < count; i++)
        {
            if (!any(buffers[i]->GetUsage() & BufferUsage::Vertex))
            {
                Graphics::NotifyValidationError("SetVertexBuffer need buffer with Vertex usage");
                return;
            }
        }

        //SetVertexBuffersImpl(firstBinding, count, buffers, offsets);
    }

    void CommandContext::SetIndexBuffer(GpuBuffer* buffer, uint32_t offset, IndexType indexType)
    {
        ALIMER_ASSERT(buffer);
#if defined(ALIMER_DEV)
        if (!any(buffer->GetUsage() & BufferUsage::Index))
        {
            Graphics::NotifyValidationError("SetIndexBuffer need buffer with Index usage");
            return;
        }
#endif
        
        SetIndexBufferImpl(buffer, offset, indexType);
    }

    void CommandContext::SetPrimitiveTopology(PrimitiveTopology topology)
    {
        //SetPrimitiveTopologyImpl(topology);
    }

    void CommandContext::Draw(uint32_t vertexCount, uint32_t firstVertex)
    {
        DrawInstanced(vertexCount, 1, firstVertex, 0);
    }

    void CommandContext::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
    {
#if defined(ALIMER_DEV)
        //ALIMER_ASSERT(_currentPipeline && !_currentPipeline->IsCompute());
        ALIMER_ASSERT_MSG(_insideRenderPass, "Cannot draw outside render pass");
        ALIMER_ASSERT(vertexCount > 0);
        ALIMER_ASSERT(instanceCount >= 1);
#endif

        //DrawImpl(vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void CommandContext::DrawIndexed(PrimitiveTopology topology, uint32_t indexCount, uint32_t startIndexLocation, int32_t baseVertexLocation)
    {
        ALIMER_ASSERT(_currentPipeline && !_currentPipeline->IsCompute());
        ALIMER_ASSERT(_insideRenderPass);
        ALIMER_ASSERT(indexCount > 1);
        //DrawIndexedImpl(topology, indexCount, startIndexLocation, baseVertexLocation);
    }

    void CommandContext::DrawIndexedInstanced(PrimitiveTopology topology, uint32_t indexCount, uint32_t instanceCount, uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation)
    {
        ALIMER_ASSERT(_currentPipeline && !_currentPipeline->IsCompute());
        ALIMER_ASSERT(_insideRenderPass);
        ALIMER_ASSERT(indexCount > 1);
        //DrawIndexedInstancedImpl(topology, indexCount, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
    }

    void CommandContext::Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
    {
        ALIMER_ASSERT(_currentPipeline && _currentPipeline->IsCompute());
        FlushComputeState();
        //DispatchImpl(groupCountX, groupCountY, groupCountZ);
    }

    void CommandContext::Dispatch1D(uint32_t threadCountX, uint32_t groupSizeX)
    {
        Dispatch(DivideByMultiple(threadCountX, groupSizeX), 1, 1);
    }

    void CommandContext::Dispatch2D(uint32_t threadCountX, uint32_t threadCountY, uint32_t groupSizeX, uint32_t groupSizeY)
    {
        Dispatch(
            DivideByMultiple(threadCountX, groupSizeX),
            DivideByMultiple(threadCountY, groupSizeY),
            1);
    }

    void CommandContext::Dispatch3D(uint32_t threadCountX, uint32_t threadCountY, uint32_t threadCountZ, uint32_t groupSizeX, uint32_t groupSizeY, uint32_t groupSizeZ)
    {
        Dispatch(
            DivideByMultiple(threadCountX, groupSizeX),
            DivideByMultiple(threadCountY, groupSizeY),
            DivideByMultiple(threadCountZ, groupSizeZ));
    }

    void CommandContext::FlushComputeState()
    {

    }


#if TODO


    void CommandBuffer::BindBuffer(GpuBuffer* buffer, uint32_t set, uint32_t binding)
    {
        ALIMER_ASSERT(buffer);

        BindBuffer(buffer, 0, buffer->GetSize(), set, binding);
    }

    void CommandBuffer::BindBuffer(GpuBuffer* buffer, uint32_t offset, uint32_t range, uint32_t set, uint32_t binding)
    {
        ALIMER_ASSERT(buffer);
        ALIMER_ASSERT(any(buffer->GetUsage() & BufferUsage::Uniform) || any(buffer->GetUsage() & BufferUsage::Storage));
        ALIMER_ASSERT(set < MaxDescriptorSets);
        ALIMER_ASSERT(binding < MaxBindingsPerSet);

        BindBufferImpl(buffer, offset, range, set, binding);
    }

    void CommandBuffer::BindTexture(Texture* texture, uint32_t set, uint32_t binding)
    {
        ALIMER_ASSERT(texture);
        ALIMER_ASSERT(
            any(texture->GetUsage() & TextureUsage::ShaderRead)
            || any(texture->GetUsage() & TextureUsage::ShaderWrite)
        );
        ALIMER_ASSERT(set < MaxDescriptorSets);
        ALIMER_ASSERT(binding < MaxBindingsPerSet);

        BindTextureImpl(texture, set, binding);
    }
#endif // TODO

    }
