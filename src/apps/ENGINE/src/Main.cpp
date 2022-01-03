#include <thread>
#include "LifecycleDiagnosticsService.hpp"
#include "logging.hpp"

#include "SteamApi.hpp"
#include "compiler.h"
#include "file_service.h"
#ifdef _WIN32 // FIX_LINUX s_debug.h
#include "s_debug.h"
#else
#include "core.h"
#endif
#include "storm/fs.h"
#include "watermark.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <OSWindow.hpp>
#include <SDL.h>

VFILE_SERVICE *fio = nullptr;
S_DEBUG *CDebug = nullptr;
CORE core;

namespace
{

constexpr char defaultLoggerName[] = "system";
bool isRunning = false;
bool bActive = true;

storm::diag::LifecycleDiagnosticsService lifecycleDiagnostics;

void RunFrame()
{
    if (!core.Run())
    {
        isRunning = false;
    }

    lifecycleDiagnostics.notifyAfterRun();
}

#ifdef _WIN32
void RunFrameWithOverflowCheck()
{
    __try
    {
        RunFrame();
    }
    __except ([](unsigned code, struct _EXCEPTION_POINTERS *ep) {
        return code == EXCEPTION_STACK_OVERFLOW;
    }(GetExceptionCode(), GetExceptionInformation()))
    {
        _resetstkoflw();
        throw std::runtime_error("Stack overflow");
    }
}
#else
#define RunFrameWithOverflowCheck RunFrame
#endif

} // namespace

void HandleWindowEvent(const storm::OSWindow::Event &event)
{
    if (event == storm::OSWindow::Closed)
    {
        isRunning = false;
        core.Event("DestroyWindow", nullptr);
    }
    else if (event == storm::OSWindow::FocusGained)
    {
        bActive = true;
        core.AppState(bActive);
    }
    else if (event == storm::OSWindow::FocusLost)
    {
        bActive = false;
        core.AppState(bActive);
    }
}

#ifdef _WIN32 // FIX_LINUX
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
#else
int main(int argc, char *argv[])
#endif
{
    // Prevent multiple instances
#ifdef _WIN32 // FIX_LINUX
    if (!CreateEventA(nullptr, false, false, "Global\\FBBD2286-A9F1-4303-B60C-743C3D7AA7BE") ||
        GetLastError() == ERROR_ALREADY_EXISTS)
#else
    if (false)
#endif
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Another instance is already running!", nullptr);
        return EXIT_SUCCESS;
    }

    SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

    // Init FS
    FILE_SERVICE File_Service;
    fio = &File_Service;

    // Init diagnostics
    const auto lifecycleDiagnosticsGuard =
#ifdef STORM_ENABLE_CRASH_REPORTS
        lifecycleDiagnostics.initialize(true);
#else
        lifecycleDiagnostics.initialize(false);
#endif
    if (!lifecycleDiagnosticsGuard)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", "Unable to initialize lifecycle service!", nullptr);
        
    }
    else
    {
        lifecycleDiagnostics.setCrashInfoCollector([]() { core.collectCrashInfo(); });
    }

    // Init stash
    create_directories(fs::GetSaveDataPath());

    // Init logging
    spdlog::set_default_logger(storm::logging::getOrCreateLogger(defaultLoggerName));
    spdlog::info("Logging system initialized. Running on {}", STORM_BUILD_WATERMARK_STRING);

    // Init core
    core.Init();

    // Init script debugger
#ifdef _WIN32 // FIX_LINUX s_debug.h
    S_DEBUG debug;
    debug.Init();
    CDebug = &debug;
#endif

    // Read config
    auto ini = fio->OpenIniFile(fs::ENGINE_INI_FILE_NAME);

    uint32_t dwMaxFPS = 0;
    bool bSteam = false;
    int width = 1024, height = 768;
    bool fullscreen = false;

    if (ini)
    {
        dwMaxFPS = static_cast<uint32_t>(ini->GetInt(nullptr, "max_fps", 0));
        auto bDebugWindow = ini->GetInt(nullptr, "DebugWindow", 0) == 1;
        auto bAcceleration = ini->GetInt(nullptr, "Acceleration", 0) == 1;
        if (ini->GetInt(nullptr, "logs", 1) == 0) // disable logging
        {
            spdlog::set_level(spdlog::level::off);
        }
        width = ini->GetInt(nullptr, "screen_x", 1024);
        height = ini->GetInt(nullptr, "screen_y", 768);
        fullscreen = ini->GetInt(nullptr, "full_screen", false) ? true : false;
        bSteam = ini->GetInt(nullptr, "Steam", 1) != 0;
    }

    // initialize SteamApi through evaluating its singleton
    try
    {
        steamapi::SteamApi::getInstance(!bSteam);
    }
    catch (const std::exception &e)
    {
        spdlog::critical(e.what());
        return EXIT_FAILURE;
    }

    std::shared_ptr<storm::OSWindow> window = storm::OSWindow::Create(width, height, fullscreen);
    window->SetTitle("Sea Dogs");
    core.Set_Hwnd(static_cast<HWND>(window->OSHandle()));
    window->Subscribe(HandleWindowEvent);
    window->Show();

    // Init core
    core.InitBase();

#ifdef _WIN32 // FIX_LINUX MaxFPS
    // Message loop
    auto dwOldTime = GetTickCount();
#endif

    isRunning = true;
    while (isRunning)
    {
        SDL_PumpEvents();
        SDL_FlushEvents(0, SDL_LASTEVENT);

        if (bActive)
        {
#ifdef _WIN32 // FIX_LINUX MaxFPS
            if (dwMaxFPS)
            {
                const auto dwMS = 1000u / dwMaxFPS;
                const auto dwNewTime = GetTickCount();
                if (dwNewTime - dwOldTime < dwMS)
                    continue;
                dwOldTime = dwNewTime;
            }
#endif

            RunFrameWithOverflowCheck();
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    // Release
    core.Event("ExitApplication", nullptr);
    core.CleanUp();
    core.ReleaseBase();
#ifdef _WIN32 // FIX_LINUX
    ClipCursor(nullptr);
#endif
    SDL_Quit();

    return EXIT_SUCCESS;
}
