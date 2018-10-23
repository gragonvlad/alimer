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
#include "../Graphics/GraphicsDevice.h"
#include "../Math/MathUtil.h"
#include "../Core/Log.h"

namespace Alimer
{
    CommandContext::CommandContext(GraphicsDevice* device)
        : _device(device)
    {
        ALIMER_ASSERT(device);
        BeginCompute();
    }

    void CommandContext::Flush(bool waitForCompletion)
    {
        FlushImpl(waitForCompletion);
    }

    void CommandContext::BeginDefaultRenderPass(const RenderPassBeginDescriptor* descriptor)
    {
        BeginRenderPassImpl(nullptr, descriptor);
        _insideRenderPass = true;
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

    void CommandContext::SetVertexBuffer(GpuBuffer* buffer, uint32_t offset, uint32_t index)
    {
        ALIMER_ASSERT(buffer);
        ALIMER_ASSERT(index < MaxVertexBufferBindings);
        if (!any(buffer->GetUsage() & BufferUsage::Vertex))
        {
            _device->NotifyValidationError("SetVertexBuffer need buffer with Vertex usage");
            return;
        }

        SetVertexBufferImpl(buffer, offset);
    }

    void CommandContext::SetVertexBuffers(uint32_t firstBinding, uint32_t count, const GpuBuffer** buffers, const uint32_t* offsets)
    {
        ALIMER_ASSERT(firstBinding + count < MaxVertexBufferBindings);
        for (uint32_t i = firstBinding; i < count; i++)
        {
            if (!any(buffers[i]->GetUsage() & BufferUsage::Vertex))
            {
                _device->NotifyValidationError("SetVertexBuffer need buffer with Vertex usage");
                return;
            }
        }

        SetVertexBuffersImpl(firstBinding, count, buffers, offsets);
    }

    void CommandContext::SetIndexBuffer(GpuBuffer* buffer, uint32_t offset, IndexType indexType)
    {
        ALIMER_ASSERT(buffer);
        if (!any(buffer->GetUsage() & BufferUsage::Index))
        {
            _device->NotifyValidationError("SetIndexBuffer need buffer with Index usage");
            return;
        }

        SetIndexBufferImpl(buffer, offset, indexType);
    }

    void CommandContext::SetProgram(Program* program)
    {
        if (_currentProgram == program)
            return;

        _currentProgram = program;
        SetProgramImpl(program);
    }

    void CommandContext::SetProgram(const std::string &vertex, const std::string &fragment, const std::vector<std::pair<std::string, int>> &defines)
    {
        ShaderProgram* program = _device->GetShaderManager().RegisterGraphics(vertex, fragment);
        uint32_t variant = program->RegisterVariant(defines);
        SetProgram(program->GetVariant(variant));
    }

    void CommandContext::Draw(PrimitiveTopology topology, uint32_t vertexStart, uint32_t vertexCount)
    {
        ALIMER_ASSERT(_currentProgram);
        ALIMER_ASSERT(_insideRenderPass);
        ALIMER_ASSERT(!_isCompute);
        ALIMER_ASSERT(vertexCount > 0);
        DrawImpl(topology, vertexStart, vertexCount);
    }

    void CommandContext::DrawInstanced(PrimitiveTopology topology, uint32_t vertexStart, uint32_t vertexCount, uint32_t instanceCount)
    {
        ALIMER_ASSERT(_currentProgram);
        ALIMER_ASSERT(_insideRenderPass);
        ALIMER_ASSERT(!_isCompute);
        ALIMER_ASSERT(vertexCount > 1);
        ALIMER_ASSERT(instanceCount > 1);
        DrawInstancedImpl(topology, vertexStart, vertexCount, instanceCount, 0);
    }

    void CommandContext::DrawInstanced(PrimitiveTopology topology, uint32_t vertexStart, uint32_t vertexCount, uint32_t instanceCount, uint32_t baseInstance)
    {
        ALIMER_ASSERT(_currentProgram);
        ALIMER_ASSERT(_insideRenderPass);
        ALIMER_ASSERT(!_isCompute);
        DrawInstancedImpl(topology, vertexStart, vertexCount, instanceCount, baseInstance);
    }

    void CommandContext::Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
    {
        //ALIMER_ASSERT(_currentShader);
        ALIMER_ASSERT(_isCompute);
        FlushComputeState();
        DispatchImpl(groupCountX, groupCountY, groupCountZ);
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

    void CommandContext::BeginCompute()
    {
        _isCompute = true;
        BeginContext();
    }

    void CommandContext::BeginGraphics()
    {
        _isCompute = false;
        BeginContext();
    }

    void CommandContext::BeginContext()
    {
        _dirtySets = ~0u;
        _dirtyVbos = ~0u;
        _currentProgram = nullptr;
        _insideRenderPass = false;
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

    void CommandBuffer::DrawIndexed(PrimitiveTopology topology, uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex)
    {
        if (!IsInsideRenderPass())
        {
            ALIMER_LOGCRITICAL("Cannot draw outside RenderPass.");
        }

        DrawIndexedCore(topology, indexCount, instanceCount, startIndex);
    }

    /*void CommandBuffer::ExecuteCommands(uint32_t commandBufferCount, CommandBuffer* const* commandBuffers)
    {
        ALIMER_ASSERT(commandBufferCount);
        ALIMER_ASSERT(commandBuffers);

        ExecuteCommandsCore(commandBufferCount, commandBuffers);
    }*/
#endif // TODO

    }
