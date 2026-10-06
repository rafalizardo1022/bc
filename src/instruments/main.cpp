/*   Bridge Command 5.0 Ship Simulator
     Copyright (C) 2026

     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License version 2 as
     published by the Free Software Foundation

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY Or FITNESS For A PARTICULAR PURPOSE.  See the
     GNU General Public License For more details.

     You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "irrlicht.h"

#include <clocale>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <asio.hpp>

#include "EventReceiver.hpp"
#include "InstrumentGUI.hpp"
#include "Network.hpp"

#include "../IniFile.hpp"
#include "../Utilities.hpp"

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef _MSC_VER
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

namespace IniFile {
    irr::ILogger* irrlichtLogger = 0;
}

#ifdef _WIN32
static LRESULT CALLBACK CustomWndProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

struct cMonitorsVec
{
    std::vector<int>       iMonitors;
    std::vector<HMONITOR>  hMonitors;
    std::vector<HDC>       hdcMonitors;
    std::vector<RECT>      rcMonitors;

    static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
    {
        cMonitorsVec* pThis = reinterpret_cast<cMonitorsVec*>(pData);

        pThis->hMonitors.push_back(hMon);
        pThis->hdcMonitors.push_back(hdc);
        pThis->rcMonitors.push_back(*lprcMonitor);
        pThis->iMonitors.push_back(pThis->hdcMonitors.size());
        return TRUE;
    }

    cMonitorsVec()
    {
        EnumDisplayMonitors(0, 0, MonitorEnum, (LPARAM)this);
    }
};
#endif

namespace
{
    InstrumentDisplayMode parseDisplayMode(std::string modeText)
    {
        Utilities::to_lower(modeText);
        modeText = Utilities::trim(modeText);
        if (modeText == "nav" || modeText == "navigation" || modeText == "environment" || modeText == "weather") {
            return InstrumentDisplayNavigation;
        }
        return InstrumentDisplayVessel;
    }
}

int main(int argc, char ** argv)
{
#ifndef _WIN32
    setlocale(LC_ALL, "");
#endif

#ifdef FOR_DEB
    chdir("/usr/share/bridgecommand");
#endif

#ifdef __APPLE__
    char exePath[1024];
    uint32_t pathSize = sizeof(exePath);
    std::string exeFolderPath = "";
    if (_NSGetExecutablePath(exePath, &pathSize) == 0) {
        std::string exePathString(exePath);
        size_t pos = exePathString.find_last_of("\\/");
        if (std::string::npos != pos) {
            exeFolderPath = exePathString.substr(0, pos);
        }
    }
    exeFolderPath.append("/../../../../Resources");
    chdir(exeFolderPath.c_str());
#endif

    std::string userFolder = Utilities::getUserDir();
    std::string iniFilename = "instruments.ini";
    std::string displayModeArgument = "";

    if (Utilities::pathExists(userFolder + iniFilename)) {
        iniFilename = userFolder + iniFilename;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            iniFilename = std::string(argv[++i]);
            std::cout << "Using Ini file >" << iniFilename << "<" << std::endl;
        } else if (strncmp(argv[i], "--display=", 10) == 0) {
            displayModeArgument = std::string(argv[i]).substr(10);
        } else if (strcmp(argv[i], "--display") == 0 && i + 1 < argc) {
            displayModeArgument = std::string(argv[++i]);
        }
    }

    irr::u32 graphicsWidth = IniFile::iniFileTou32(iniFilename, "graphics_width");
    irr::u32 graphicsHeight = IniFile::iniFileTou32(iniFilename, "graphics_height");
    irr::u32 graphicsDepth = IniFile::iniFileTou32(iniFilename, "graphics_depth");
    bool fullScreen = (IniFile::iniFileTou32(iniFilename, "graphics_mode") == 1);
    bool fakeFullScreen = (IniFile::iniFileTou32(iniFilename, "graphics_mode") == 3);
    if (fakeFullScreen) {
        fullScreen = true;
    }

    float fontScale = IniFile::iniFileTof32(iniFilename, "font_scale");
    if (fontScale <= 0) {
        fontScale = 1.0f;
    }
    int fontSize = (int)(15 * fontScale + 0.5f);

    if (graphicsWidth == 0 || graphicsHeight == 0) {
        irr::core::dimension2d<irr::u32> deskres;
#ifdef _WIN32
        deskres.Width = GetSystemMetrics(SM_CXSCREEN);
        deskres.Height = GetSystemMetrics(SM_CYSCREEN);
#else
        irr::IrrlichtDevice *nulldevice = irr::createDevice(irr::video::EDT_NULL);
        deskres = nulldevice->getVideoModeList()->getDesktopResolution();
        nulldevice->drop();
#endif

        if (graphicsWidth == 0) {
            if (fullScreen || fakeFullScreen) {
                graphicsWidth = deskres.Width;
            } else {
                graphicsWidth = (irr::u32)(1280 * fontScale);
                if (graphicsWidth > deskres.Width * 0.90f) {
                    graphicsWidth = (irr::u32)(deskres.Width * 0.90f);
                }
            }
        }
        if (graphicsHeight == 0) {
            if (fullScreen || fakeFullScreen) {
                graphicsHeight = deskres.Height;
            } else {
                graphicsHeight = (irr::u32)(720 * fontScale);
                if (graphicsHeight > deskres.Height * 0.90f) {
                    graphicsHeight = (irr::u32)(deskres.Height * 0.90f);
                }
            }
        }
    }

    if (graphicsDepth == 0) {
        graphicsDepth = 32;
    }

    irr::u32 udpPort = IniFile::iniFileTou32(iniFilename, "udp_send_port");
    if (udpPort == 0) {
        udpPort = 18304;
    }

    std::string displayModeString = IniFile::iniFileToString(iniFilename, "display_mode");
    if (!displayModeArgument.empty()) {
        displayModeString = displayModeArgument;
    }
    InstrumentDisplayMode displayMode = parseDisplayMode(displayModeString);

    irr::SIrrlichtCreationParameters deviceParameters;

#ifdef _WIN32
    HWND hWnd;
    HINSTANCE hInstance = 0;
    const char* Win32ClassName = "BridgeCommandInstruments";
    WNDCLASSEX wcex;

    if (fakeFullScreen) {
        int requestedMonitor = IniFile::iniFileTou32(iniFilename, "monitor") - 1;

        DWORD style = WS_VISIBLE | WS_POPUP;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = (WNDPROC)CustomWndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = DLGWINDOWEXTRA;
        wcex.hInstance = hInstance;
        wcex.hIcon = NULL;
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
        wcex.lpszMenuName = 0;
        wcex.lpszClassName = Win32ClassName;
        wcex.hIconSm = 0;
        RegisterClassEx(&wcex);

        cMonitorsVec monitors;

        if (requestedMonitor > -1 && monitors.iMonitors.size() > (unsigned int)requestedMonitor) {
            int x = monitors.rcMonitors[requestedMonitor].left;
            int y = monitors.rcMonitors[requestedMonitor].top;
            graphicsWidth = monitors.rcMonitors[requestedMonitor].right - monitors.rcMonitors[requestedMonitor].left;
            graphicsHeight = monitors.rcMonitors[requestedMonitor].bottom - monitors.rcMonitors[requestedMonitor].top;

            hWnd = CreateWindowA(Win32ClassName, "Bridge Command Instruments",
                style, x, y, graphicsWidth, graphicsHeight,
                NULL, NULL, hInstance, NULL);

            deviceParameters.WindowId = hWnd;
        } else {
            POINT p;
            int x = 0;
            int y = 0;
            if (GetCursorPos(&p))
            {
                HMONITOR monitor = MonitorFromPoint(p, MONITOR_DEFAULTTOPRIMARY);
                MONITORINFO mi;
                mi.cbSize = sizeof(mi);
                GetMonitorInfo(monitor, &mi);
                x = mi.rcMonitor.left;
                y = mi.rcMonitor.top;
                graphicsWidth = mi.rcMonitor.right - mi.rcMonitor.left;
                graphicsHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;
            }

            hWnd = CreateWindowA(Win32ClassName, "Bridge Command Instruments",
                style, x, y, graphicsWidth, graphicsHeight,
                NULL, NULL, hInstance, NULL);

            deviceParameters.WindowId = hWnd;
        }
    }
#endif

    deviceParameters.DriverType = irr::video::EDT_OPENGL;
    deviceParameters.WindowSize = irr::core::dimension2d<irr::u32>(graphicsWidth, graphicsHeight);
    deviceParameters.Bits = graphicsDepth;
    deviceParameters.Fullscreen = fullScreen;

    irr::IrrlichtDevice* device = createDeviceEx(deviceParameters);
    if (device == 0) {
        std::cerr << "Could not start - please check your graphics options." << std::endl;
        exit(EXIT_FAILURE);
    }

    irr::video::IVideoDriver* driver = device->getVideoDriver();

#ifdef __APPLE__
    irr::io::IFileSystem* fileSystem = device->getFileSystem();
    if (fileSystem == 0) {
        exit(EXIT_FAILURE);
    }
    fileSystem->changeWorkingDirectoryTo(exeFolderPath.c_str());
#endif

    std::string fontName = IniFile::iniFileToString(iniFilename, "font");
    std::string fontPath = "media/fonts/" + fontName + "/" + fontName + "-" + std::to_string(fontSize) + ".xml";
    irr::gui::IGUIFont *font = device->getGUIEnvironment()->getFont(fontPath.c_str());
    if (font != NULL) {
        device->getGUIEnvironment()->getSkin()->setFont(font);
    }

    Network network(udpPort);
    std::cout << "Instrument display address: " << asio::ip::host_name() << ":" << network.getPort() << std::endl;

    InstrumentGUI gui(device, displayMode, network.getPort());
    EventReceiver receiver(device, &gui);
    device->setEventReceiver(&receiver);

    InstrumentData instrumentData;
    irr::u32 timer = device->getTimer()->getRealTime();

    while (device->run()) {
        driver->beginScene(true, false, irr::video::SColor(255, 5, 14, 18));

        bool shutdownRequested = network.update(instrumentData);
        if (shutdownRequested) {
            device->closeDevice();
        }

        gui.draw(instrumentData, network.hasReceivedData());

        driver->endScene();

        irr::u32 newTimer = device->getTimer()->getRealTime();
        if (newTimer - timer < 33) {
            device->sleep(33 - (newTimer - timer));
        }
        timer = newTimer;
    }

    return 0;
}
