#pragma once
#include "ConfigSet.h"
#include "PyGame.h"
#include "PyImguiInterface.h"
#include "Python.h"
#include "ScriptManager.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "windows.h"
#include <boost/python.hpp>
#include <chrono>
#include <d3d11_2.h>
#include <dcomp.h>
#include <dinput.h>
#include <dxgi1_3.h>
#include <list>
#include <map>

using namespace std::chrono;
using namespace boost::python;

/// Manages the overlay of the cheat. Also manages the executing scripts.
class Overlay {

public:
    Overlay();
    void Init();
    void GameStart(MemSnapShot& memSnapshot);

    void StartFrame();
    void Update(MemSnapShot& memSnapshot);
    void RenderFrame();

    bool IsVisible();
    void Hide();
    void Show();
    void ToggleTransparent();

    static ID3D11Device* GetDxDevice();

private:
    void DrawUI(PyGame& state, MemSnapShot& memSnapshot);
    void ExecScripts(PyGame& state);

    void DrawOverlayWindows(PyGame& state);
    void DrawScriptSettings(PyGame& state, MemSnapShot& memSnapshot);

    void DrawScriptError(std::shared_ptr<Script>& script);
    void DrawScriptCommonSettings(std::shared_ptr<Script>& script, int id);

    static bool CreateDeviceD3D(HWND hWnd);
    static void CleanupDeviceD3D();
    static void CreateRenderTarget();
    static void CleanupRenderTarget();
    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    HWND hWindow;
    bool isWindowVisible = true;

    // DirectX stuff
    static ID3D11Device* dxDevice;
    static ID3D11DeviceContext* dxDeviceContext;
    static IDXGISwapChain1* dxSwapChain;
    static ID3D11RenderTargetView* dxRenderTarget;

    ConfigSet& configs;

    ScriptManager scriptManager;
    PyImguiInterface imguiInterface;
};