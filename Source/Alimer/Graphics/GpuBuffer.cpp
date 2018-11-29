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

#include "../Graphics/GpuBuffer.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/GraphicsImpl.h"
#include "../Debug/Log.h"

namespace Alimer
{
    GpuBuffer::GpuBuffer()
        : GraphicsResource(Object::GetSubsystem<Graphics>())
        , _impl(nullptr)
    {

    }

    GpuBuffer::~GpuBuffer()
    {
        Destroy();
    }

   
    bool GpuBuffer::Define(const BufferDescriptor* descriptor, const void* initialData)
    {
        ALIMER_ASSERT(descriptor);

        if (descriptor->usage == BufferUsage::None)
        {
            ALIMER_LOGCRITICAL("Invalid bufer usage");
        }

        Destroy();

        _usage = descriptor->usage;
        _stride = descriptor->stride;
        _size = descriptor->size;

        return Create(descriptor, initialData);
    }

    bool GpuBuffer::SetSubData(const void* pData)
    {
        return false;
    }

    bool GpuBuffer::SetSubData(uint32_t offset, uint32_t size, const void* pData)
    {
        if (offset + size > GetSize())
        {
            ALIMER_LOGERROR("Buffer subdata out of range");
            return false;
        }

        return false;
    }

    bool GpuBuffer::Create(const BufferDescriptor* descriptor, const void* initialData)
    {
        _impl = gGraphics().GetImpl()->CreateBuffer(descriptor, initialData, nullptr);
        return _impl != nullptr;
    }

    void GpuBuffer::Destroy()
    {
        SafeDelete(_impl);
    }

}
