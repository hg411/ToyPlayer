#pragma once

class ConstantBuffer;

struct ShadertoyParams {
    float dx;
    float dy;
    float threshold;
    float strength;
    float iTime;
    float padding[3];
};

class ToyPlayer {
  public:
    void Init(ComPtr<ID3D11Device> device, const wstring &pixelShaderName, const float width, const float height);

    void Update();
    void Render();

    void SetChannel(int index, ComPtr<ID3D11ShaderResourceView> srv);

  private:
    void CreateMesh(ComPtr<ID3D11Device> device);
    void CreateVertexBuffer(ComPtr<ID3D11Device> device, const vector<Vertex> &vertices);
    void CreateIndexBuffer(ComPtr<ID3D11Device> device, const vector<uint32> &indices);
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
