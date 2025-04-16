#pragma once

class Imgui {
  public:
    Imgui();
    ~Imgui();

    void Init(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    void Update();
    void Render();

  private:
};
