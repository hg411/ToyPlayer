#pragma once

inline void ThrowIfFailed(HRESULT hr) {
    if (FAILED(hr)) {
        // ������� �� ���⿡ breakpoint ����
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

  private:
    static const wstring _shaderPath;
};
