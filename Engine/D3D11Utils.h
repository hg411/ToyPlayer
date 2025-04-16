#pragma once

inline void ThrowIfFailed(HRESULT hr) {
    if (FAILED(hr)) {
        // 디버깅할 때 여기에 breakpoint 설정
        throw std::exception();
    }
}

class D3D11Utils {
  public:
    static void
    CreateVertexShaderAndInputLayout(ComPtr<ID3D11Device> &device, const wstring &filename,
                                     const vector<D3D11_INPUT_ELEMENT_DESC> &inputElements,
                                     ComPtr<ID3D11VertexShader> &vertexShader,
                                     ComPtr<ID3D11InputLayout> &inputLayout);

    static void CreatePixelShader(ComPtr<ID3D11Device> &device, const wstring &filename,
                                  ComPtr<ID3D11PixelShader> &pixelShader);

    static void CreateVertexBuffer(ComPtr<ID3D11Device> device, const vector<Vertex> &vertices,
                                   ComPtr<ID3D11Buffer> &vertexBuffer);

    static void CreateIndexBuffer(ComPtr<ID3D11Device> device, const vector<uint32> &indices,
                                  ComPtr<ID3D11Buffer> &indexBuffer);

  private:
    static const wstring _shaderPath;
};
