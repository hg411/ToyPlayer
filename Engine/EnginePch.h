#pragma once

// std::byte ������� ����
#define _HAS_STD_BYTE 0

// ���� include
#include <windows.h>
#include <tchar.h>
#include <algorithm>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include <iostream>
#include <array>
#include <list>
#include <map>
#include <unordered_set>
#include <assert.h>
#include <execution>
#include <random>
#include <cwctype>

using namespace std;

#include <filesystem>
namespace fs = std::filesystem;

#include <d3d11.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi.h>    // DXGIFactory
#include <dxgi1_4.h> // DXGIFactory4
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXTex.h>
#include <DirectXTexEXr.h> // EXR ���� HDRI �б�
#include <fp16.h>
#include <DirectXColors.h>
#include <directxtk/DDSTextureLoader.h> // ť��� ���� �� �ʿ�
#include <directxtk/WICTextureLoader.h>
#include <DirectXMesh.h> // Update tangent
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

// imgui ,SimpleMath �߰�
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <directxtk/SimpleMath.h>

// ���� lib
#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

// ���� typedef
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using Vec2 = DirectX::SimpleMath::Vector2;
using Vec3 = DirectX::SimpleMath::Vector3;
using Vec4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;
using Quaternion = DirectX::SimpleMath::Quaternion;
using Ray = DirectX::SimpleMath::Ray;
using Plane = DirectX::SimpleMath::Plane;

enum class CBV_REGISTER : uint8 {
    b0,
    b1,
    b2,
    b3,
    b4,
    b5,
    b6,
    b7,
    b8,
    b9,
    b10,

    END
};

enum class SRV_REGISTER : uint8 {
    t0,
    t1,
    t2,
    t3,
    t4,
    t5,
    t6,
    t7,
    t8,
    t9,
    t10,
    t11,
    t12,
    t13,

    END
};

enum class UAV_REGISTER : uint8 {
    u0,
    u1,
    u2,
    u3,
    u4,
    u5,
    u6,
    u7,
    u8,
    u9,
    u10,

    END,
};

enum class SHADER_TYPE : uint8 {
    VS,
    PS,
    HS,
    DS,
    GS,
    CS,

    END,
};

struct WindowInfo {
    HWND hwnd;     // ��� ������
    int32 width;   // �ʺ�
    int32 height;  // ����
    bool windowed; // â��� or ��üȭ��
};

struct Vertex {
    Vertex() {}

    Vertex(Vec3 p, Vec2 u = Vec2(0.0f), Vec3 n = Vec3(0.0f), Vec3 t = Vec3(0.0f))
        : position(p), uv(u), normal(n), tangent(t) {}

    Vec3 position = Vec3(0.0f);
    Vec2 uv = Vec2(0.0f);
    Vec3 normal = Vec3(0.0f);
    Vec3 tangent = Vec3(0.0f);
};

#define DECLARE_SINGLE(type)                                                                       \
  private:                                                                                         \
    type() {}                                                                                      \
    ~type() {}                                                                                     \
                                                                                                   \
  public:                                                                                          \
    static type *GetInstance() {                                                                   \
        static type instance;                                                                      \
        return &instance;                                                                          \
    }

#define GET_SINGLE(type) type::GetInstance()
#define DEVICE GEngine->GetDevice()
#define CONTEXT GEngine->GetContext()

#define CONST_BUFFER(type) GEngine->GetConstantBuffer(type)
#define RESOURCES GET_SINGLE(Resources)
#define INPUT GET_SINGLE(Input)
#define DELTA_TIME GET_SINGLE(Timer)->GetDeltaTime()

std::string WStringToString(const std::wstring &wstr);

wstring s2ws(const string &s);
string ws2s(const wstring &s);

extern unique_ptr<class Engine> GEngine;