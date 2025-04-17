#pragma once

class ConstantBuffer;

struct ShadertoyParams {
    float dx;
    float dy;
    float threshold;
    float strength;
    float iTime;
    int useToneMapping;
    float padding[2];
};

class ToyPlayer {
  public:
    void Init(ComPtr<ID3D11Device> device, const wstring &pixelShaderName, const float width,
              const float height);

    void Update();
    void Render();

    void SetChannel(int index, ComPtr<ID3D11ShaderResourceView> srv);

  public:
    // IMGUI
    int GetUseToneMapping() { return _shadertoyParams.useToneMapping; }
    void SetUseToneMapping(int flag) { _shadertoyParams.useToneMapping = flag; }

  private:
    void CreateMesh(ComPtr<ID3D11Device> device);
    void InitShader(ComPtr<ID3D11Device> device, const wstring &pixelShaderName);

  private:
    ShadertoyParams _shadertoyParams;
    shared_ptr<ConstantBuffer> _constantBuffer;

    ComPtr<ID3D11Buffer> _vertexBuffer;
    ComPtr<ID3D11Buffer> _indexBuffer;

    ComPtr<ID3D11VertexShader> _vertexShader;
    ComPtr<ID3D11InputLayout> _inputLayout;
    ComPtr<ID3D11PixelShader> _pixelShader;

    array<ComPtr<ID3D11ShaderResourceView>, 4> _iChannels;
};
