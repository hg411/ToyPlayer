#include "pch.h"
#include "ToyPlayer.h"
#include "Timer.h"
#include "ConstantBuffer.h"
#include "D3D11Utils.h"
#include "Engine.h"

void ToyPlayer::Init(ComPtr<ID3D11Device> device, const wstring &pixelShaderName, const float width, const float height) {
    // Create ConstantBuffer
    _constantBuffer = make_shared<ConstantBuffer>();
    _constantBuffer->Init(sizeof(_shadertoyParams));

    // Rectangle Mesh
    CreateMesh(device);

    InitShader(device, pixelShaderName);

    _shadertoyParams.dx = 1.0f / width;
    _shadertoyParams.dy = 1.0f / height;
}

void ToyPlayer::Update() {
    _shadertoyParams.iTime += DELTA_TIME;
    _constantBuffer->Update(&_shadertoyParams, sizeof(_shadertoyParams));
}

void ToyPlayer::Render() {
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    CONTEXT->IASetInputLayout(_inputLayout.Get());
    CONTEXT->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
    CONTEXT->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    CONTEXT->VSSetShader(_vertexShader.Get(), 0, 0);
    CONTEXT->PSSetShader(_pixelShader.Get(), 0, 0);
    ID3D11ShaderResourceView *srvs[4] = {
        _iChannels[0].Get(),
        _iChannels[1].Get(),
        _iChannels[2].Get(),
        _iChannels[3].Get(),
    };
    CONTEXT->PSSetShaderResources(0, 4, srvs);
    CONTEXT->PSSetConstantBuffers(0, 1, _constantBuffer->GetBuffer().GetAddressOf());
    CONTEXT->DrawIndexed(6, 0, 0);
}

void ToyPlayer::SetChannel(int index, ComPtr<ID3D11ShaderResourceView> srv) {
    if (index >= 0 && index < 4)
        _iChannels[index] = srv;
}

void ToyPlayer::CreateMesh(ComPtr<ID3D11Device> device) {
    vector<Vertex> vertices = {
        {{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // Top Left
        {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},   // Top Right
        {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // Bottom Left
        {{1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},  // Bottom Right
    };

    vector<uint32> indices = {0, 1, 2, 2, 1, 3};

    CreateVertexBuffer(device, vertices);
    CreateIndexBuffer(device, indices);
}

void ToyPlayer::CreateVertexBuffer(ComPtr<ID3D11Device> device, const vector<Vertex> &vertices) {
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // 초기화 후 변경X
    bufferDesc.ByteWidth = UINT(sizeof(Vertex) * vertices.size());
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
    bufferDesc.StructureByteStride = sizeof(Vertex);

    D3D11_SUBRESOURCE_DATA vertexBufferData = {0}; // MS 예제에서 초기화하는 방식
    vertexBufferData.pSysMem = vertices.data();
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;

    device->CreateBuffer(&bufferDesc, &vertexBufferData, _vertexBuffer.GetAddressOf());
}

void ToyPlayer::CreateIndexBuffer(ComPtr<ID3D11Device> device, const vector<uint32> &indices) {
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // 초기화 후 변경X
    bufferDesc.ByteWidth = UINT(sizeof(uint32) * indices.size());
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
    bufferDesc.StructureByteStride = sizeof(uint32);

    D3D11_SUBRESOURCE_DATA indexBufferData = {0};
    indexBufferData.pSysMem = indices.data();
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;

    device->CreateBuffer(&bufferDesc, &indexBufferData, _indexBuffer.GetAddressOf());
}

void ToyPlayer::InitShader(ComPtr<ID3D11Device> device, const wstring &pixelShaderName) {
    vector<D3D11_INPUT_ELEMENT_DESC> samplingIED = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"SamplingVS.hlsl", samplingIED,
                                                 _vertexShader, _inputLayout);
    D3D11Utils::CreatePixelShader(device, pixelShaderName, _pixelShader);
}
