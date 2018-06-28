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

#include "Graphics/Texture.h"
#include "D3D11Prerequisites.h"
#include <map>

namespace Alimer
{
    class D3D11Graphics;

    /// D3D11 Texture implementation.
    class D3D11Texture final : public Texture
    {
    public:
        /// Constructor.
        D3D11Texture(D3D11Graphics* graphics);

        /// Constructor.
        D3D11Texture(D3D11Graphics* graphics, ID3D11Texture2D* nativeTexture);

        /// Destructor.
        ~D3D11Texture() override;

        void Destroy() override;

        ID3D11RenderTargetView* GetRenderTargetView(uint32_t level, uint32_t slice);

    private:
        ID3D11Device1* _d3dDevice;

        union {
            ID3D11Resource* _resource;
            ID3D11Texture1D* _texture1D;
            ID3D11Texture2D* _texture2D;
            ID3D11Texture3D* _texture3D;
        };

        DXGI_FORMAT _dxgiFormat;
        D3D11_RTV_DIMENSION _viewDimension;

        struct ViewDesc
        {
            uint32_t level;
            uint32_t slice;

            bool operator<(const ViewDesc& other) const
            {
                return std::tie(level, slice) < std::tie(other.level, other.slice);
            }
        };

        std::map<ViewDesc, ID3D11RenderTargetView*> _views;
    };
}
