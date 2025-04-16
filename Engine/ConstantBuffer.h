#pragma once

class ConstantBuffer {
  public:
    ConstantBuffer();
    ~ConstantBuffer();

    void Init(uint32 buffersize, void *initData = nullptr);
    void Update(const void *bufferData, uint32 bufferSize);
    void Bind(SHADER_TYPE shaderType, CBV_REGISTER reg);

  public:
    ComPtr<ID3D11Buffer> GetBuffer() { return _buffer; }

  private:
    ComPtr<ID3D11Buffer> _buffer;
    uint32 _bufferSize = 0;
};