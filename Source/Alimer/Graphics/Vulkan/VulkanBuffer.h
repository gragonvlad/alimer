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

#include "../GpuBuffer.h"
#include "../GraphicsImpl.h"
#include "VulkanPrerequisites.h"
#include <vector>

namespace Alimer
{
	class VulkanGraphics;

	/// Vulkan Buffer.
	class VulkanBuffer final : public GpuBuffer
	{
	public:
        VulkanBuffer(VulkanGraphics* graphics, BufferUsageFlags usage, uint64_t size, uint32_t stride, ResourceUsage resourceUsage, const void* initialData);
		~VulkanBuffer() override;

        //bool SetData(uint32_t offset, uint32_t size, const void* data) override;

		inline VkBuffer GetVkHandle() const { return _vkHandle; }

	private:
		VkDevice _logicalDevice;
        VmaAllocator _allocator;
        VkBuffer _vkHandle = VK_NULL_HANDLE;
        VmaAllocation _allocation = VK_NULL_HANDLE;
	};
}
