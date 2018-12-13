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

#include "../Application/Application.h"
#include "../Scene/Systems/CameraSystem.h"
#include "../IO/Path.h"
#include "../Core/Platform.h"
#include "../Debug/Log.h"

namespace Alimer
{
    static Application* __appInstance = nullptr;

    Application::Application(int argc, char** argv)
        : _running(false)
        , _paused(false)
        , _headless(false)
        , _settings{}
        , _entities{}
        , _systems(_entities)
        , _scene(_entities)
    {
        for (int i = 0; i < argc; ++i)
        {
            _args.Push(argv[i]);
        }

        PlatformConstruct();
        AddSubsystem(this);
        _log = new Logger();
        _sceneManager = new SceneManager();

        // Register modules
        Graphics::RegisterObject();
        ResourceManager::RegisterObject();

        SceneManager::Register();

        __appInstance = this;
    }

    Application::~Application()
    {
        Shutdown();
        SafeDelete(_log);
        __appInstance = nullptr;
    }

    Application* Application::GetInstance()
    {
        return __appInstance;
    }

    void Application::Shutdown()
    {
        if (!_running)
            return;

        _paused = true;
        _running = false;

        _gui.Reset();
        Graphics::Shutdown();
        Audio::Shutdown();
        PluginManager::Shutdown();
    }

    bool Application::InitializeBeforeRun()
    {
        SetCurrentThreadName("Main");
        _log->Open("Alimer.log");

        ALIMER_LOGINFOF("Initializing engine %s...", ALIMER_VERSION_STR);

        // Init Window and Gpu.
        if (!_headless)
        {
            Graphics* graphics = Graphics::Create(_settings.preferredGraphicsBackend, _settings.validation);
            if (!graphics->Initialize(&_settings.mainWindowDescriptor))
            {
                ALIMER_LOGERROR("Failed to initialize Graphics.");
                return false;
            }

            _mainWindow = graphics->GetMainWindow();

            // Create imgui system.
            _gui = new Gui();
        }

        // Load plugins
        LoadPlugins();

        // Create per platform Audio module.
        Audio* audio = Audio::Create();
        audio->Initialize();

        // Initialize this instance and all systems.
        Initialize();

        // Setup and configure all systems.
        _systems.Add<CameraSystem>();

        ALIMER_LOGINFO("Engine initialized with success.");
        _running = true;
        //BeginRun();

        // Reset timer.
        _timer.Reset();

        // Run the first time an update
        //InternalUpdate();

        return true;
    }

    void Application::LoadPlugins()
    {
        PluginManager::GetInstance()->LoadPlugins(FileSystem::GetExecutableFolder());
    }

    void Application::RunFrame()
    {
        if (!_paused)
        {
            // Tick timer.
            double frameTime = _timer.Frame();
            double deltaTime = _timer.GetElapsed();

            // Update all systems.
            _systems.Update(deltaTime);

            // Render single frame if window is not minimzed.
            if (!_headless 
                && !_mainWindow->IsMinimized())
            {
                RenderFrame(frameTime, deltaTime);
            }
        }

        // Update input, even when paused.
        _input.Update();
    }

    void Application::RenderFrame(double frameTime, double elapsedTime)
    {
        if (_headless)
            return;

        /*auto context = _graphicsDevice->GetContext();

        RenderPassBeginDescriptor renderPass = {};
        renderPass.colors[0].clearColor = Color4(0.0f, 0.2f, 0.4f, 1.0f);
        context->BeginDefaultRenderPass(&renderPass);
        */

        // Call OnRenderFrame for custom rendering frame logic.
        OnRenderFrame(frameTime, elapsedTime);

        // Render scene to default command buffer.
        /*if (_sceneRenderPipeline)
        {
            //auto camera = _scene.GetActiveCamera()->GetComponent<CameraComponent>()->camera;
            //_renderPipeline->Render(_renderContext, { camera });
        }

        // End swap chain render pass.
        context->EndRenderPass();*/

        // Present rendering frame.
        _mainWindow->SwapBuffers();

        // Advance to next frame.
        gGraphics().Frame();
    }

    void Application::OnRenderFrame(double frameTime, double elapsedTime)
    {
        ALIMER_UNUSED(frameTime);
        ALIMER_UNUSED(elapsedTime);

        Color4 clearColor(0.0f, 0.2f, 0.4f, 1.0f);
        CommandContext& context = gGraphics().GetImmediateContext();
        context.BeginDefaultRenderPass(clearColor);
        //context.Draw(3, 0);
        context.EndRenderPass();
        context.Flush();
    }

    void Application::Exit()
    {
        _paused = true;

        if (_running)
        {
            // TODO: Fire event.
            _running = false;
        }
    }

    void Application::Pause()
    {
        if (_running && !_paused)
        {
            // TODO: Fire event.
            _paused = true;
        }
    }

    void Application::Resume()
    {
        if (_running && _paused)
        {
            // TODO: Fire event.
            _paused = false;
        }
    }
}
