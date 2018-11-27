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

#include "../Base/String.h"
#include "../Math/Math.h"
#include "../Math/Color.h"
#include "../Graphics/agpu.h"

namespace Alimer
{
    static constexpr uint32_t RemainingMipLevels  = ~0U;
    static constexpr uint32_t RemainingArrayLayers = ~0U;
    static constexpr uint32_t MaxViewportsAndScissors = 16u;
    static constexpr uint32_t MaxDescriptorSets = 4u;
    static constexpr uint32_t MaxBindingsPerSet = 14u;
    static constexpr uint32_t MaxVertexAttributes = 16u;
    static constexpr uint32_t MaxVertexBufferBindings = 4u;
    static constexpr uint32_t MaxColorAttachments = 8u;

    /// Enum describing the Graphics backend.
    enum class GraphicsBackend : uint32_t
    {
        /// Best device supported for running platform.
        Default,
        /// Empty/Headless device type.
        Empty,
        /// Vulkan backend.
        Vulkan,
        /// Direct3D 11 backend.
        D3D11,
        /// Direct3D 12 backend.
        D3D12,
        /// Metal backend.
        Metal,
        /// OpenGL backend.
        OpenGL,
        /// Count
        Count
    };

    /// Enum describing the number of samples.
    enum class SampleCount : uint32_t
    {
        /// 1 sample (no multi-sampling).
        Count1 = AGPU_SAMPLE_COUNT1,
        /// 2 Samples.
        Count2 = AGPU_SAMPLE_COUNT2,
        /// 4 Samples.
        Count4 = AGPU_SAMPLE_COUNT4,
        /// 8 Samples.
        Count8 = AGPU_SAMPLE_COUNT8,
        /// 16 Samples.
        Count16 = AGPU_SAMPLE_COUNT16,
        /// 32 Samples.
        Count32 = AGPU_SAMPLE_COUNT32,
    };

    enum class ResourceUsage : unsigned
    {
        Default,
        Immutable,
        Dynamic,
        Staging
    };

    enum class LoadAction
    {
        DontCare,
        Load,
        Clear
    };

    enum class StoreAction
    {
        DontCare,
        Store
    };

    /// Primitive topology.
    enum class PrimitiveTopology : uint32_t
    {
        PointList = AGPU_PRIMITIVE_TOPOLOGY_POINT_LIST,
        LineList = AGPU_PRIMITIVE_TOPOLOGY_LINE_LIST,
        LineStrip = AGPU_PRIMITIVE_TOPOLOGY_LINE_STRIP,
        TriangleList = AGPU_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        TriangleStrip = AGPU_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
        LineListWithAdjacency = AGPU_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
        LineStripWithAdjacency = AGPU_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
        TriangleListWithAdjacency = AGPU_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
        TriangleStripWithAdjacency = AGPU_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,
        PatchList = AGPU_PRIMITIVE_TOPOLOGY_PATCH_LIST,
        Count = AGPU_PRIMITIVE_TOPOLOGY_COUNT,
    };

    
    enum class VertexFormat : uint32_t
    {
        Unknown = AGPU_VERTEX_FORMAT_UNKNOWN,
        Float = AGPU_VERTEX_FORMAT_FLOAT,
        Float2 = AGPU_VERTEX_FORMAT_FLOAT2,
        Float3 = AGPU_VERTEX_FORMAT_FLOAT3,
        Float4 = AGPU_VERTEX_FORMAT_FLOAT4,
        Byte4 = AGPU_VERTEX_FORMAT_BYTE4,
        Byte4N = AGPU_VERTEX_FORMAT_BYTE4N,
        UByte4 = AGPU_VERTEX_FORMAT_UBYTE4,
        UByte4N = AGPU_VERTEX_FORMAT_UBYTE4N,
        Short2 = AGPU_VERTEX_FORMAT_SHORT2,
        Short2N = AGPU_VERTEX_FORMAT_SHORT2N,
        Short4 = AGPU_VERTEX_FORMAT_SHORT4,
        Short4N = AGPU_VERTEX_FORMAT_SHORT4N,
        Count = AGPU_VERTEX_FORMAT_COUNT
    };
    
    /// VertexInputRate
    enum class VertexInputRate : uint32_t
    {
        Vertex = AGPU_VERTEX_INPUT_RATE_VERTEX,
        Instance = AGPU_VERTEX_INPUT_RATE_INSTANCE
    };

    enum class ParamDataType
    {
        Unknown = 0,
        Void,
        Boolean,
        Char,
        Int,
        UInt,
        Int64,
        UInt64,
        Half,
        Float,
        Double,
        Struct
    };

    enum class ParamAccess
    {
        Read,
        Write,
        ReadWrite
    };

    enum class ResourceParamType
    {
        Input = 0,
        Output = 1,
    };

    /// Defines shader stage usage.
    enum class ShaderStageUsage : uint32_t
    {
        None = 0,
        Vertex = 1 << 0,
        TessControl = 1 << 1,
        TessEvaluation = 1 << 2,
        Geometry = 1 << 3,
        Fragment = 1 << 4,
        Compute = 1 << 5,
        AllGraphics = (Vertex | TessControl | TessEvaluation | Geometry | Fragment),
        All = (AllGraphics | Compute),
    };
    ALIMER_BITMASK(ShaderStageUsage);

    struct PipelineResource
    {
        String name;
        ShaderStageUsage stages;
        ResourceParamType resourceType;
        ParamDataType dataType;
        ParamAccess access;
        uint32_t set;
        uint32_t binding;
        uint32_t location;
        uint32_t vecSize;
        uint32_t arraySize;
        uint32_t offset;
        uint32_t size;
    };

    enum class VertexElementSemantic : uint32_t
    {
        Position = 0,
        Normal,
        Binormal,
        Tangent,
        BlendWeight,
        BlendIndices,
        Color0,
        Color1,
        Color2,
        Color3,
        Texcoord0,
        Texcoord1,
        Texcoord2,
        Texcoord3,
        Texcoord4,
        Texcoord5,
        Texcoord6,
        Texcoord7,
        Count
    };

    ALIMER_API uint32_t GetVertexFormatSize(VertexFormat format);
    ALIMER_API const char* EnumToString(ResourceUsage usage);
    ALIMER_API const char* EnumToString(VertexElementSemantic semantic);
}
