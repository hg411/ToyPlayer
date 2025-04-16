#pragma once

class Imgui;

class Engine {
  public:
    void Init(const WindowInfo &windowInfo);
    void Update();
    void Render();

    void ResizeWindow(int32 widht, int32 height);

  private:
    void AdjustWindowSize(); // 123
    void CreateDeviceAndSwapChain();

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

    shared_ptr<Imgui> _imgui;
};
