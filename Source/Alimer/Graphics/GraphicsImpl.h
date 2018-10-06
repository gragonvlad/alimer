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

#include "../Graphics/Texture.h"
#include "../Graphics/CommandBuffer.h"
#include "../Graphics/GraphicsDevice.h"

namespace Alimer
{
    class BufferImpl
    {
    public:
        virtual ~BufferImpl() = default;

        virtual bool setSubDataImpl(uint32_t offset, uint32_t size, const void* pData) = 0;
    };

    class TextureImpl
    {
    public:
        virtual ~TextureImpl() = default;
    };

    class CommandBufferImpl
    {
    public:
        virtual ~CommandBufferImpl() = default;

        virtual void BeginRenderPass(const RenderPassDescriptor* descriptor) = 0;
        virtual void EndRenderPass() = 0;

        virtual void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;
    };

    class GraphicsImpl 
    {
    public:
        virtual ~GraphicsImpl() = default;

        virtual uint32_t GetVendorID() const = 0;
        virtual GpuVendor GetVendor() const = 0;

        virtual CommandBufferImpl* Initialize(const RenderingSettings& settings) = 0;
        virtual bool WaitIdle() = 0;

        virtual bool BeginFrame() = 0;
        virtual void EndFrame() = 0;
    };
}