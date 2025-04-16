#include "pch.h"
#include "Timer.h"
#include "D3D11Utils.h"

void Timer::Init(ComPtr<ID3D11Device> device) {
    ::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER *>(&_frequency));
    ::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&_prevCount)); // CPU Å¬·°

    D3D11_QUERY_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Query = D3D11_QUERY_TIMESTAMP;
    desc.MiscFlags = 0;
    ThrowIfFailed(device->CreateQuery(&desc, &_startQuery));
    ThrowIfFailed(device->CreateQuery(&desc, &_stopQuery));
    desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
    ThrowIfFailed(device->CreateQuery(&desc, &_disjointQuery));
}

void Timer::Update() {
    uint64 currentCount;
    ::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&currentCount));

    _deltaTime = (currentCount - _prevCount) / static_cast<float>(_frequency);
    _prevCount = currentCount;

    _frameCount++;
    _frameTime += _deltaTime;

    if (_frameTime > 1.f) {
        _fps = static_cast<uint32>(_frameCount / _frameTime);

        _frameTime = 0.f;
        _frameCount = 0;
    }
}
