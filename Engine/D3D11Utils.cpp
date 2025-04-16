#include "pch.h"
#include "D3D11Utils.h"

const wstring D3D11Utils::_shaderPath = L"../Resources/Shader/";

void CheckResult(HRESULT hr, ID3DBlob *errorBlob) {
    if (FAILED(hr)) {
        // ������ ���� ���
        if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0) {
            cout << "File not found." << endl;
        }

        // ���� �޽����� ������ ���
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

    // ���̴��� �������� �̸��� "main"�� �Լ��� ����
    // D3D_COMPILE_STANDARD_FILE_INCLUDE �߰�: ���̴����� include ���

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

    // ���̴��� �������� �̸��� "main"�� �Լ��� ����
    // D3D_COMPILE_STANDARD_FILE_INCLUDE �߰�: ���̴����� include ���
    HRESULT hr =
        D3DCompileFromFile((_shaderPath + filename).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                           "main", "ps_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

    CheckResult(hr, errorBlob.Get());

    device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL,
                              &pixelShader);
}

void D3D11Utils::CreateVertexBuffer(ComPtr<ID3D11Device> device, const vector<Vertex> &vertices,
                                    ComPtr<ID3D11Buffer> &vertexBuffer) {
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // �ʱ�ȭ �� ����X
    bufferDesc.ByteWidth = UINT(sizeof(Vertex) * vertices.size());
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
    bufferDesc.StructureByteStride = sizeof(Vertex);

    D3D11_SUBRESOURCE_DATA vertexBufferData = {0}; // MS �������� �ʱ�ȭ�ϴ� ���
    vertexBufferData.pSysMem = vertices.data();
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;

    device->CreateBuffer(&bufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
}

void D3D11Utils::CreateIndexBuffer(ComPtr<ID3D11Device> device, const vector<uint32> &indices,
                                   ComPtr<ID3D11Buffer> &indexBuffer) {
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // �ʱ�ȭ �� ����X
    bufferDesc.ByteWidth = UINT(sizeof(uint32) * indices.size());
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
    bufferDesc.StructureByteStride = sizeof(uint32);

    D3D11_SUBRESOURCE_DATA indexBufferData = {0};
    indexBufferData.pSysMem = indices.data();
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;

    device->CreateBuffer(&bufferDesc, &indexBufferData, indexBuffer.GetAddressOf());
}
