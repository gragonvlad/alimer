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

#include "../Application/Window.h"

namespace Alimer
{
    Window::Window(const String& title, const uvec2& size, WindowFlags flags)
        : _title(title)
        , _size(size)
        , _flags(flags)
    {
        PlatformConstruct();
    }

    Window::~Window()
    {
        PlatformDestroy();
    }

    bool Window::IsFullscreen() const
    {
        return any(_flags & WindowFlags::Fullscreen);
    }

    void Window::Resize(uint32_t width, uint32_t height)
    {
        if (_size.x == width
            && _size.y == height)
        {
            return;
        }

        _size.x = width;
        _size.y = height;
        PlatformResize(width, height);
        OnSizeChanged(_size);
    }

    void Window::OnSizeChanged(const uvec2& newSize)
    {
        WindowResizeEvent evt;
        evt.size = _size;
        resizeEvent(evt);
    }
}
