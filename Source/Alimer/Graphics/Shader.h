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

#include "../Graphics/ShaderModule.h"

namespace Alimer
{
    /// Defines a shader resource.
    class ALIMER_API Shader final : public GraphicsResource
    {
        friend class Graphics;
        ALIMER_OBJECT(Shader, GraphicsResource);

    protected:
        /// Constructor.
        Shader(Graphics* graphics);

    public:
        /// Destructor.
        ~Shader() override;

        /// Unconditionally destroy the GPU resource.
        void Destroy();

        bool Define(ShaderModule* vertex, ShaderModule* fragment);

        inline const ShaderModule* GetShader(ShaderStage stage) const
        {
            return _shaders[static_cast<unsigned>(stage)].Get();
        }

    private:
        bool BeginLoad(Stream& source) override;
        bool EndLoad() override;

        /// Register object factory.
        static void RegisterObject();

        SharedPtr<ShaderModule> _shaders[static_cast<unsigned>(ShaderStage::Count)] = {};
    };
}
