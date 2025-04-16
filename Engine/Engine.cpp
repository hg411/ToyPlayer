#include "pch.h"
#include "Engine.h"
#include "Imgui.h"
#include "Timer.h"
#include "ToyPlayer.h"
#include "Texture.h"

void Engine::Init(const WindowInfo &windowInfo) {
    _windowInfo = windowInfo;

    AdjustWindowSize();
    CreateDeviceAndSwapChain();

    // Init Imgui
    _imgui = make_shared<Imgui>();
    _imgui->Init(_device, _context);

    GET_SINGLE(Timer)->Init(_device);

    SetViewport();
    CreateRTV();
    SetSamplerStates();
    InitRasterizerStates();

    // ToyPlayer
    _toyPlayer = make_shared<ToyPlayer>();
    _toyPlayer->Init(_device, L"SupernovaPS.hlsl", static_cast<float>(_windowInfo.width),
                     static_cast<float>(_windowInfo.height));
    shared_ptr<Texture> texture1 = make_shared<Texture>();
    texture1->Load(L"../Resources/Texture/rgba-medium-noise.png", false);
    _toyPlayer->SetChannel(0, texture1->GetSRV());
    shared_ptr<Texture> texture2 = make_shared<Texture>();
    texture2->Load(L"../Resources/Texture/gray-small-noise.png", true);
    _toyPlayer->SetChannel(1, texture2->GetSRV());
}

void Engine::Update() {
    _imgui->Update();

    GET_SINGLE(Timer)->Update();

    _toyPlayer->Update();
    Render();
}

void Engine::Render() {
    float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    //_context->ClearRenderTargetView(_floatRTV.Get(), clearColor);

    //_context->OMSetRenderTargets(1, _floatRTV.GetAddressOf(), nullptr);

    _context->ClearRenderTargetView(_backBufferRTV.Get(), clearColor);
    _context->OMSetRenderTargets(1, _backBufferRTV.GetAddressOf(), nullptr);
    _context->RSSetState(_rasterizerState.Get());

    _toyPlayer->Render();

    _imgui->Render();

    _swapChain->Present(1, 0);
}

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
        D3D_FEATURE_LEVEL_11_0, // 더 높은 버전이 먼저 오도록 설정
        D3D_FEATURE_LEVEL_9_3}; // 낮은 버전에서도 실행 가능하도록 설정
    D3D_FEATURE_LEVEL featureLevel;

    D3D11CreateDevice(nullptr,    // Specify nullptr to use the default adapter.
                      driverType, // Create a device using the hardware graphics driver.
                      0,          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
                      createDeviceFlags,        // Set debug and Direct2D compatibility flags.
                      featureLevels,            // List of feature levels this app can support.
                      ARRAYSIZE(featureLevels), // Size of the list above.
                      D3D11_SDK_VERSION,     // Always set this to D3D11_SDK_VERSION for Microsoft
                                             // Store apps.
                      device.GetAddressOf(), // Returns the Direct3D device created.
                      &featureLevel,         // Returns feature level of device created.
                      context.GetAddressOf() // Returns the device immediate context.
    );

    device.As(&_device);
    context.As(&_context);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferDesc.Width = _windowInfo.width;           // set the back buffer width
    sd.BufferDesc.Height = _windowInfo.height;         // set the back buffer height
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 64-bit color
    sd.BufferCount = 2;                                // Double-buffering
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT |
                     DXGI_USAGE_UNORDERED_ACCESS;      // how swap chain is to be used
                                                       // : DXGI_USAGE_SHADER_INPUT |
                                                       // DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _windowInfo.hwnd;                // the window to be used
    sd.Windowed = _windowInfo.windowed;                // windowed/full-screen mode
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.SampleDesc.Count = 1;   // 멀티 샘플링 비활성화 (FLIP에서 사용불가)
    sd.SampleDesc.Quality = 0; // 품질 수준

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

void Engine::SetViewport() {
    ZeroMemory(&_viewport, sizeof(D3D11_VIEWPORT));
    _viewport.TopLeftX = 0.0f;
    _viewport.TopLeftY = 0.0f;
    _viewport.Width = static_cast<float>(_windowInfo.width);
    _viewport.Height = static_cast<float>(_windowInfo.height);
    _viewport.MinDepth = 0.0f;
    _viewport.MaxDepth = 1.0f; // Note: important for depth buffering

    _scissorRect = CD3D11_RECT(0, 0, _windowInfo.width, _windowInfo.height);

    _context->RSSetViewports(1, &_viewport);
}

void Engine::CreateRTV() {
    ComPtr<ID3D11Texture2D> backBuffer;
    _swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));

    if (backBuffer) {
        _device->CreateRenderTargetView(backBuffer.Get(), nullptr, _backBufferRTV.GetAddressOf());
    } else {
        std::cout << "CreateRTV() failed." << std::endl;
    }
}

void Engine::SetSamplerStates() {
    ComPtr<ID3D11SamplerState> pointWrapSS;
    ComPtr<ID3D11SamplerState> pointClampSS;
    ComPtr<ID3D11SamplerState> linearWrapSS;
    ComPtr<ID3D11SamplerState> linearClampSS;

    // Linear Wrap Sampler State
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    _device->CreateSamplerState(&sampDesc, linearWrapSS.GetAddressOf());

    // Point Wrap Sampler State
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    _device->CreateSamplerState(&sampDesc, pointWrapSS.GetAddressOf());

    // Linear Clamp Sampler State
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    _device->CreateSamplerState(&sampDesc, linearClampSS.GetAddressOf());

    // Point Clamp Sampler State
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    _device->CreateSamplerState(&sampDesc, pointClampSS.GetAddressOf());

    ID3D11SamplerState *sampleStates[4] = {pointWrapSS.Get(), pointClampSS.Get(),
                                           linearWrapSS.Get(), linearClampSS.Get()};

    _context->PSSetSamplers(0, 4, sampleStates);
}

void Engine::InitRasterizerStates() {
    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = false;

    _device->CreateRasterizerState(&rastDesc, _rasterizerState.GetAddressOf());
}
