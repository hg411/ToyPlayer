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
    _shadertoyParams.useToneMapping = true;
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

    D3D11Utils::CreateVertexBuffer(device, vertices, _vertexBuffer);
    D3D11Utils::CreateIndexBuffer(device, indices, _indexBuffer);
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
