#pragma once

class Imgui;
class ToyPlayer;

class Engine {
  public:
    void Init(const WindowInfo &windowInfo);
    void Update();
    void Render();

    void ResizeWindow(int32 widht, int32 height);

  private:
    void AdjustWindowSize();
    void CreateDeviceAndSwapChain();
    void SetViewport();
    void CreateRTV();
    void SetSamplerStates();
    void InitRasterizerStates();

  public:
    const WindowInfo &GetWindowInfo() { return _windowInfo; }
    ComPtr<ID3D11Device> GetDevice() { return _device; }
    ComPtr<ID3D11DeviceContext> GetContext() { return _context; }
    ComPtr<IDXGISwapChain> GetSwapChain() { return _swapChain; }

  private:
    WindowInfo _windowInfo;

    ComPtr<ID3D11Device> _device;
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<IDXGISwapChain> _swapChain;

    D3D11_VIEWPORT _viewport;
    D3D11_RECT _scissorRect;

    ComPtr<ID3D11RenderTargetView> _backBufferRTV;

    //ComPtr<ID3D11Texture2D> _floatBuffer; // ªË¡¶
    //ComPtr<ID3D11RenderTargetView> _floatRTV;
    //ComPtr<ID3D11ShaderResourceView> _floatSRV;

    ComPtr<ID3D11RasterizerState> _rasterizerState;

    shared_ptr<Imgui> _imgui;
    shared_ptr<ToyPlayer> _toyPlayer;
};
