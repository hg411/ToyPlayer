#include "pch.h"
#include "D3D11Utils.h"

const wstring D3D11Utils::_shaderPath = L"../Resources/Shader/";

void CheckResult(HRESULT hr, ID3DBlob *errorBlob) {
    if (FAILED(hr)) {
        // 파일이 없을 경우
        if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0) {
            cout << "File not found." << endl;
        }

        // 에러 메시지가 있으면 출력
        if (errorBlob) {
            cout << "Shader compile error\n" << (char *)errorBlob->GetBufferPointer() << endl;
        }
    }
}

void D3D11Utils::CreateVertexShaderAndInputLayout(
    ComPtr<ID3D11Device> &device, const wstring &filename,
    const vector<D3D11_INPUT_ELEMENT_DESC> &inputElements, ComPtr<ID3D11VertexShader> &vertexShader,
    ComPtr<ID3D11InputLayout> &inputLayout) {

    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    // 쉐이더의 시작점의 이름이 "main"인 함수로 지정
    // D3D_COMPILE_STANDARD_FILE_INCLUDE 추가: 쉐이더에서 include 사용

    HRESULT hr =
        D3DCompileFromFile((_shaderPath + filename).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                           "main", "vs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

    CheckResult(hr, errorBlob.Get());

    device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL,
                               &vertexShader);

    device->CreateInputLayout(inputElements.data(), UINT(inputElements.size()),
                              shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
                              &inputLayout);
}

void D3D11Utils::CreatePixelShader(ComPtr<ID3D11Device> &device, const wstring &filename,
                                   ComPtr<ID3D11PixelShader> &pixelShader) {

    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    // 쉐이더의 시작점의 이름이 "main"인 함수로 지정
    // D3D_COMPILE_STANDARD_FILE_INCLUDE 추가: 쉐이더에서 include 사용
    HRESULT hr =
        D3DCompileFromFile((_shaderPath + filename).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                           "main", "ps_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

    CheckResult(hr, errorBlob.Get());

    device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL,
                              &pixelShader);
}