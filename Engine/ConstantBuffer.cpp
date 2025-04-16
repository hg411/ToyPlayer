#include "pch.h"
#include "ConstantBuffer.h"
#include "Engine.h"
#include "D3d11Utils.h"

ConstantBuffer::ConstantBuffer() {}

ConstantBuffer::~ConstantBuffer() { _buffer.Reset(); }

void ConstantBuffer::Init(uint32 bufferSize, void *initData) {
    _bufferSize = bufferSize;

    assert((_bufferSize % 16) == 0);

    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.ByteWidth = bufferSize;
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    if (initData) {
        D3D11_SUBRESOURCE_DATA bufferData;
        ZeroMemory(&bufferData, sizeof(bufferData));
        bufferData.pSysMem = initData;
        ThrowIfFailed(DEVICE->CreateBuffer(&bufferDesc, &bufferData, _buffer.GetAddressOf()));
    } else {
        ThrowIfFailed(DEVICE->CreateBuffer(&bufferDesc, NULL, _buffer.GetAddressOf()));
    }
}

void ConstantBuffer::Update(const void *bufferData, uint32 bufferSize) {
    assert((bufferSize % 16) == 0);

    D3D11_MAPPED_SUBRESOURCE ms;
    ThrowIfFailed(CONTEXT->Map(_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
    // 포인터 사이즈 사용하지않게 주의.
    memcpy(ms.pData, bufferData, bufferSize);

    CONTEXT->Unmap(_buffer.Get(), 0);
}

void ConstantBuffer::Bind(SHADER_TYPE shaderType, CBV_REGISTER reg) {
    switch (shaderType) {
    case SHADER_TYPE::VS:
        CONTEXT->VSSetConstantBuffers(static_cast<uint8>(reg), 1, _buffer.GetAddressOf());
        break;

    case SHADER_TYPE::PS:
        CONTEXT->PSSetConstantBuffers(static_cast<uint8>(reg), 1, _buffer.GetAddressOf());
        break;

    case SHADER_TYPE::HS:
        CONTEXT->HSSetConstantBuffers(static_cast<uint8>(reg), 1, _buffer.GetAddressOf());
        break;

    case SHADER_TYPE::DS:
        CONTEXT->DSSetConstantBuffers(static_cast<uint8>(reg), 1, _buffer.GetAddressOf());
        break;

    case SHADER_TYPE::GS:
        CONTEXT->GSSetConstantBuffers(static_cast<uint8>(reg), 1, _buffer.GetAddressOf());
        break;

    case SHADER_TYPE::CS:
        CONTEXT->CSSetConstantBuffers(static_cast<uint8>(reg), 1, _buffer.GetAddressOf());
        break;

    default:
        cout << "Error: Unknown Shader Type in ConstantBuffer::Bind!" << endl;
        break;
    }
}
