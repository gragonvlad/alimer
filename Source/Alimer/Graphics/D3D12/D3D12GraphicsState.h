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

#include "../Types.h"
#include "D3D12DescriptorAllocator.h"

namespace Alimer
{
	class D3D12Graphics;

	class D3D12GraphicsState final 
	{
	public:
		/// Constructor.
        D3D12GraphicsState(D3D12Graphics* graphics);

        void Reset();

        void SetPrimitiveTopology(PrimitiveTopology primitiveTopology);
        PrimitiveTopology GetPrimitiveTopology() const { return _primitiveTopology; }

        bool IsDirty() const { return _dirty; }
        void ClearDirty() { _dirty = false; }
        void SetDirty() { _dirty = true; }

	private:
        D3D12Graphics* _graphics;
        PrimitiveTopology _primitiveTopology;
        bool _dirty;
	};
}
