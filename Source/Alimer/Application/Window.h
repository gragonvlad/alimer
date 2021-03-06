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
#include "../Core/Platform.h"
#include "../Core/Object.h"
#include "../Math/Math.h"
#include <string>

namespace Alimer
{
    enum class WindowFlags : uint32_t
    {
        None = 0,
        Resizable = 1 << 0,
        Fullscreen = 1 << 1,
        Visible = 1 << 2,
        Default = Resizable | Visible
    };
    ALIMER_BITMASK(WindowFlags);

    /// Window resized event.
    class ALIMER_API WindowResizeEvent 
    {
    public:
        /// New window size.
        uvec2 size;
    };

    /// OS Window class.
    class ALIMER_API Window : public Object
    {
        ALIMER_OBJECT(Window, Object);

    public:
        /// Constructor.
        Window(const String& title, const uvec2& size, WindowFlags flags = WindowFlags::Default);

        /// Destructor.
        ~Window() override;

        /// Set window title.
        void SetTitle(const String& newTitle);

        /// Show the window.
        void Show();
        /// Hide the window.
        void Hide();
        /// Minimize the window.
        void Minimize();
        /// Maximize the window.
        void Maximize();
        /// Restore window size.
        void Restore();
        /// Close the window.
        void Close();
        /// Resize the window.
        void Resize(uint32_t width, uint32_t height);

        /// Return whether is visible.
        bool IsVisible() const { return _visible; }

        /// Return whether is currently minimized.
        bool IsMinimized() const;

        /// Return whether is fullscreen.
        bool IsFullscreen() const;

        /// Return window title.
        const String& GetTitle() const { return _title; }

        /// Return window client area size.
        const uvec2& GetSize() const { return _size; }
        uint32_t GetWidth() const { return _size.x; }
        uint32_t GetHeight() const { return _size.y; }

        float GetAspectRatio() const { return static_cast<float>(_size.x) / _size.y; }
        WindowFlags GetFlags() const { return _flags; }

        /// Get if window is open and valid.
        bool IsOpen() const;

        /// Is cursor visible.
        bool IsCursorVisible() const;

        /// Set cursor visibility.
        void SetCursorVisible(bool visible);

        /// Get native window handle.
        void* GetNativeHandle() const;

        /// Size changed event.
        Event<void(const WindowResizeEvent&)> resizeEvent;

    protected:
        virtual void OnSizeChanged(const uvec2& newSize);

    private:
        void PlatformConstruct();
        void PlatformDestroy();
        void PlatformResize(uint32_t width, uint32_t height);

        /// Window title.
        String _title;
        /// Window size.
        uvec2 _size;
        /// Flags
        WindowFlags _flags = WindowFlags::Default;
        /// Visibility flag.
        bool _visible = true;
        /// Visibility flag.
        bool _focused = false;
        /// Window implementation
        void* _window = nullptr;

    private:
        DISALLOW_COPY_MOVE_AND_ASSIGN(Window);
    };
}
