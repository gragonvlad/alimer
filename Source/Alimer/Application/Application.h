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

#include <new>
#include <memory>
#include <array>
#include <vector>
#include <string>
#include <atomic>
#include "../Core/Object.h"
#include "../Core/Timer.h"
#include "../Core/PluginManager.h"
#include "../Application/Window.h"
#include "../Application/GameSystem.h"
#include "../Serialization/Serializable.h"
#include "../IO/FileSystem.h"
#include "../Resource/ResourceManager.h"
#include "../Input/Input.h"
#include "../Audio/Audio.h"
#include "../Graphics/Graphics.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneManager.h"
#include "../Renderer/RenderContext.h"
#include "../Renderer/RenderPipeline.h"
#include "../UI/Gui.h"

namespace Alimer
{
    class Logger;

    struct ApplicationSettings
    {
        GraphicsBackend preferredGraphicsBackend = GraphicsBackend::Default;

#if defined(_DEBUG)
        bool validation = true;
#else
        bool validation = false;
#endif

        GraphicsSettings graphicsSettings = {};
    };

    /// Application for main loop and all modules and OS setup.
    class ALIMER_API Application : public Object
    {
        ALIMER_OBJECT(Application, Object);

    protected:
        /// Constructor.
        Application();

    public:
        /// Destructor.
        virtual ~Application();

        /// Return the single instance of the Application.
        static Application* GetInstance();

        /// Runs main loop.
        int Run();

        /// Run one frame.
        void RunFrame();

        /// Request application to exit.
        void Exit();

        /// Pause the main execution loop.
        void Pause();

        /// Resume the main execution loop.
        void Resume();

        Timer &GetFrameTimer() { return _timer; }

        inline ResourceManager& GetResources() { return _resources; }
        inline Window* GetMainWindow() const { return _mainWindow; }
        inline Graphics* GetGraphics() const { return _graphics; }
        inline Input& GetInput() { return _input; }

    private:
        void PlatformConstruct();
        bool InitializeBeforeRun();
        void LoadPlugins();

    protected:
        /// Called after setup and engine initialization with all modules initialized.
        virtual void Initialize() { }

        /// Cleanup after the main loop. 
        virtual void OnExiting() { }

        /// Render after frame update.
        void RenderFrame(double frameTime, double elapsedTime);

        /// Called during rendering single frame.
        virtual void OnRenderFrame(double frameTime, double elapsedTime);

        std::vector<String> _args;
        std::atomic<bool> _running;
        std::atomic<bool> _paused;
        std::atomic<bool> _headless;
        ApplicationSettings _settings;

        Logger* _log;
        Timer _timer;
        ResourceManager _resources;
        Window* _mainWindow = nullptr;
        SharedPtr<Graphics> _graphics;
        Input _input;
        Audio* _audio;

        //
        EntityManager _entities;
        SystemManager _systems;
        Scene _scene;
        SharedPtr<SceneManager> _sceneManager;
        RenderContext _renderContext;
        SceneRenderPipeline* _sceneRenderPipeline = nullptr;

        // Gui
        SharedPtr<Gui> _gui;

    private:
        DISALLOW_COPY_MOVE_AND_ASSIGN(Application);
    };
}
