#include "pch.h"
#include "Engine.h"
#include "Imgui.h"
#include "Timer.h"

void Engine::Init(const WindowInfo &windowInfo) {
    _windowInfo = windowInfo;
    AdjustWindowSize();
    CreateDeviceAndSwapChain();

    // Init Imgui
    _imgui = make_shared<Imgui>();
    _imgui->Init(_device, _context);

    GET_SINGLE(Timer)->Init(_device);
}

void Engine::Update() {}

void Engine::Render() {}

void Engine::ResizeWindow(int32 widht, int32 height) {}

void Engine::AdjustWindowSize() {
    RECT rect = {0, 0, _windowInfo.width, _windowInfo.height};
    ::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
    ::SetWindowPos(_windowInfo.hwnd, 0, 100, 100, rect.right - rect.left, rect.bottom - rect.top,
                   0);
}

void Engine::CreateDeviceAndSwapChain() {
    const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

    UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;

    const D3D_FEATURE_LEVEL featureLevels[2] = {
        D3D_FEATURE_LEVEL_11_0,                 // 더 높은 버전이 먼저 오도록 설정
        D3D_FEATURE_LEVEL_9_3};                 // 낮은 버전에서도 실행 가능하도록 설정
    D3D_FEATURE_LEVEL featureLevel;

    D3D11CreateDevice(nullptr,                  // Specify nullptr to use the default adapter.
                      driverType,               // Create a device using the hardware graphics driver.
                      0,                        // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
                      createDeviceFlags,        // Set debug and Direct2D compatibility flags.
                      featureLevels,            // List of feature levels this app can support.
                      ARRAYSIZE(featureLevels), // Size of the list above.
                      D3D11_SDK_VERSION,        // Always set this to D3D11_SDK_VERSION for Microsoft
                                                // Store apps.
                      device.GetAddressOf(),    // Returns the Direct3D device created.
                      &featureLevel,            // Returns feature level of device created.
                      context.GetAddressOf()    // Returns the device immediate context.
    );

    device.As(&_device);
    context.As(&_context);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferDesc.Width = _windowInfo.width;               // set the back buffer width
    sd.BufferDesc.Height = _windowInfo.height;             // set the back buffer height
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
    sd.BufferCount = 2;                                    // Double-buffering
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT |
                     DXGI_USAGE_UNORDERED_ACCESS;          // how swap chain is to be used
                                                           // : DXGI_USAGE_SHADER_INPUT |
                                                           // DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _windowInfo.hwnd;                    // the window to be used
    sd.Windowed = _windowInfo.windowed;                    // windowed/full-screen mode
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen switching
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.SampleDesc.Count = 1;                               // 멀티 샘플링 비활성화 (FLIP에서 사용불가)
    sd.SampleDesc.Quality = 0;                             // 품질 수준

    D3D11CreateDeviceAndSwapChain(0, // Default adapter
                                  driverType,
                                  0, // No software device
                                  createDeviceFlags, featureLevels, 1, D3D11_SDK_VERSION, &sd,
                                  _swapChain.GetAddressOf(), _device.GetAddressOf(), &featureLevel,
                                  _context.GetAddressOf());

    if (featureLevel != D3D_FEATURE_LEVEL_11_0) {
        cout << "D3D Feature Level 11 unsupported." << endl;
    }
}
