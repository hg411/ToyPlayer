#pragma once

inline void ThrowIfFailed(HRESULT hr) {
    if (FAILED(hr)) {
        // ������� �� ���⿡ breakpoint ����
        throw std::exception();
    }
}

class D3D11Utils {};
