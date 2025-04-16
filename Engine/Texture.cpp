#include "pch.h"
#include "Texture.h"
#include "Engine.h"
#include "D3D11Utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() {}

Texture::~Texture() {}

void Texture::Load(const wstring &path, bool useSRGB) {
    wstring ext = fs::path(path).extension();
    std::transform(ext.begin(), ext.end(), ext.begin(), [](wchar_t c) { return std::towlower(c); });

    if (ext == L".dds") {
        CreateTextureFromDDSFile(path); // dds
    } else if (ext == L".exr") {
        CreateTextureFromEXR(path); // exr
    } else {
        CreateTextureFromImage(path, useSRGB); // jpg, png ...
    }
}

void Texture::CreateTextureArrayFromFile(const vector<wstring> &paths, bool useSRGB) {
    vector<ScratchImage> imageArray;
    for (const auto &f : paths) {
        ScratchImage image;

        LoadWICFile(f, image, useSRGB);

        imageArray.push_back(std::move(image));
    }
    const TexMetadata &metadata = imageArray[0].GetMetadata();

    // Create texture.
    D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(txtDesc));
    txtDesc.Width = static_cast<UINT>(metadata.width);
    txtDesc.Height = static_cast<UINT>(metadata.height);
    txtDesc.MipLevels = 0;
    txtDesc.ArraySize = UINT(paths.size());
    txtDesc.Format = metadata.format;
    txtDesc.SampleDesc.Count = 1;
    txtDesc.SampleDesc.Quality = 0;
    txtDesc.Usage = D3D11_USAGE_DEFAULT;
    txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    txtDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; // 밉맵 사용

    // 검은색 빈 텍스쳐 생성
    DEVICE->CreateTexture2D(&txtDesc, nullptr, _texture.GetAddressOf());

    // 실제 생성된 MipLevels 값을 반영하여 올바른 서브리소스 인덱스를 계산
    _texture->GetDesc(&txtDesc);

    // StagingTexture를 만들어서 하나씩 복사
    for (size_t i = 0; i < imageArray.size(); i++) {
        auto &image = imageArray[i];

        // StagingTexture는 Texture2DArray가 아니라 Texture2D
        ComPtr<ID3D11Texture2D> stagingTexture = CreateStagingTexture(image, metadata);
        // 스테이징 텍스춰를 텍스춰 배열의 해당 위치에 복사
        UINT subresourceIndex = D3D11CalcSubresource(0, static_cast<UINT>(i), txtDesc.MipLevels);

        CONTEXT->CopySubresourceRegion(_texture.Get(), subresourceIndex, 0, 0, 0,
                                       stagingTexture.Get(), 0, nullptr);
    }

    // SRV 생성
    DEVICE->CreateShaderResourceView(_texture.Get(), nullptr, _srv.GetAddressOf());
    CONTEXT->GenerateMips(_srv.Get());
}

void Texture::CreateTextureFromDDSFile(const wstring &path) {
    ScratchImage image;
    ThrowIfFailed(LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, image));
    wcout << path << " " << image.GetMetadata().width << " " << image.GetMetadata().height << endl;

    ThrowIfFailed(CreateTextureFromScratchImage(image));

    ThrowIfFailed(CreateShaderResourceView(DEVICE.Get(), image.GetImages(), image.GetImageCount(),
                                           image.GetMetadata(), _srv.GetAddressOf()));

    // 간단하고 빠른 코드(micsFlag를 정해줘야함)
    // auto hr = CreateDDSTextureFromFileEx(
    //   DEVICE.Get(), path.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0,
    //   D3D11_RESOURCE_MISC_TEXTURECUBE, // 큐브맵용 텍스처
    //   DDS_LOADER_FLAGS(false), (ID3D11Resource **)_texture.GetAddressOf(),
    //   _textureResourceView.GetAddressOf(), nullptr);

    // if (FAILED(hr)) {
    //     std::cout << "CreateDDSTextureFromFileEx() failed" << std::endl;
    // }
}

void Texture::CreateTextureFromImage(const wstring &path, bool useSRGB) {
    vector<uint8> image;
    int width = 0, height = 0;
    DXGI_FORMAT pixelFormat =
        useSRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
    ReadImage(path, image, width, height);
    ComPtr<ID3D11Texture2D> stagingTexture =
        CreateStagingTexture(width, height, image, pixelFormat);

    D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(txtDesc));
    txtDesc.Width = width;
    txtDesc.Height = height;
    txtDesc.MipLevels = 0; // 밉맵 레벨 최대
    txtDesc.ArraySize = 1;
    txtDesc.Format = pixelFormat;
    txtDesc.SampleDesc.Count = 1;
    txtDesc.Usage = D3D11_USAGE_DEFAULT; // 스테이징 텍스춰로부터 복사 가능
    txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    txtDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; // 밉맵 사용
    txtDesc.CPUAccessFlags = 0;

    // 검은색 빈 텍스처 생성
    DEVICE->CreateTexture2D(&txtDesc, nullptr, _texture.GetAddressOf());

    // 스테이징 텍스처로부터 가장 해상도가 높은 이미지 복사
    CONTEXT->CopySubresourceRegion(_texture.Get(), 0, 0, 0, 0, stagingTexture.Get(), 0, nullptr);

    // SRV 생성
    DEVICE->CreateShaderResourceView(_texture.Get(), nullptr, _srv.GetAddressOf());

    // 해상도를 낮춰가며 밉맵 생성
    CONTEXT->GenerateMips(_srv.Get());
}

ComPtr<ID3D11Texture2D> Texture::CreateStagingTexture(const ScratchImage &image,
                                                      const TexMetadata &metadata,
                                                      const int mipLevels, const int arraySize) {
    const Image *img = image.GetImage(0, 0, 0);

    D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(txtDesc));
    txtDesc.Width = static_cast<UINT>(metadata.width);
    txtDesc.Height = static_cast<UINT>(metadata.height);
    txtDesc.MipLevels = mipLevels;
    txtDesc.ArraySize = arraySize;
    txtDesc.Format = metadata.format;
    txtDesc.SampleDesc.Count = 1;
    txtDesc.Usage = D3D11_USAGE_STAGING;
    txtDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

    ComPtr<ID3D11Texture2D> stagingTexture;
    if (FAILED(DEVICE->CreateTexture2D(&txtDesc, NULL, stagingTexture.GetAddressOf()))) {
        cout << "Failed()" << endl;
    }

    D3D11_MAPPED_SUBRESOURCE ms;
    CONTEXT->Map(stagingTexture.Get(), NULL, D3D11_MAP_WRITE, NULL, &ms);
    uint8_t *pData = static_cast<uint8_t *>(ms.pData);
    const uint8_t *srcData = static_cast<const uint8_t *>(img->pixels);
    for (UINT h = 0; h < UINT(metadata.height); h++) { // 가로줄 한 줄씩 복사
        memcpy(pData + h * ms.RowPitch, srcData + h * img->rowPitch, img->rowPitch);
    }
    CONTEXT->Unmap(stagingTexture.Get(), NULL);

    return stagingTexture;
}

ComPtr<ID3D11Texture2D> Texture::CreateStagingTexture(const int width, const int height,
                                                      const vector<uint8_t> &image,
                                                      const DXGI_FORMAT pixelFormat,
                                                      const int mipLevels, const int arraySize) {
    D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(txtDesc));
    txtDesc.Width = width;
    txtDesc.Height = height;
    txtDesc.MipLevels = mipLevels;
    txtDesc.ArraySize = arraySize;
    txtDesc.Format = pixelFormat;
    txtDesc.SampleDesc.Count = 1;
    txtDesc.Usage = D3D11_USAGE_STAGING;
    txtDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

    ComPtr<ID3D11Texture2D> stagingTexture;
    ThrowIfFailed(DEVICE->CreateTexture2D(&txtDesc, NULL, stagingTexture.GetAddressOf()));

    size_t pixelSize = GetPixelSize(pixelFormat);

    D3D11_MAPPED_SUBRESOURCE ms;
    CONTEXT->Map(stagingTexture.Get(), NULL, D3D11_MAP_WRITE, NULL, &ms);
    uint8_t *pData = (uint8_t *)ms.pData;
    for (UINT h = 0; h < UINT(height); h++) { // 가로줄 한 줄씩 복사
        memcpy(&pData[h * ms.RowPitch], &image[h * width * pixelSize], width * pixelSize);
    }
    CONTEXT->Unmap(stagingTexture.Get(), NULL);

    return stagingTexture;
}

HRESULT Texture::CreateTextureFromScratchImage(const ScratchImage &image) {

    ComPtr<ID3D11Resource> resource;
    HRESULT hr = ::CreateTexture(DEVICE.Get(), image.GetImages(), image.GetImageCount(),
                                 image.GetMetadata(), resource.GetAddressOf());
    if (FAILED(hr))
        return hr;

    // ID3D11Resource* → ID3D11Texture2D* 변환
    hr = resource.As(&_texture);
    return hr;
}

void Texture::ReadImage(const wstring &path, vector<uint8_t> &image, int &width, int &height) {

    int channels;
    string pathString = ws2s(path);
    unsigned char *img = stbi_load(pathString.c_str(), &width, &height, &channels, 0);

    image.resize(width * height * 4);

    if (channels == 1) {
        for (size_t i = 0; i < width * height; i++) {
            uint8_t g = img[i * channels + 0];
            for (size_t c = 0; c < 4; c++) {
                image[4 * i + c] = g;
            }
        }
    } else if (channels == 3) {
        for (size_t i = 0; i < width * height; i++) {
            for (size_t c = 0; c < 3; c++) {
                image[4 * i + c] = img[i * channels + c];
            }
            image[4 * i + 3] = 255;
        }
    } else if (channels == 4) {
        for (size_t i = 0; i < width * height; i++) {
            for (size_t c = 0; c < 4; c++) {
                image[4 * i + c] = img[i * channels + c];
            }
        }
    }

    stbi_image_free(img);

    wcout << path << " " << width << " " << height << endl;
}

void Texture::LoadWICFile(const wstring &path, ScratchImage &image, bool useSRGB) {

    if (useSRGB) {
        LoadFromWICFile(path.c_str(), WIC_FLAGS_DEFAULT_SRGB, nullptr, image);
    } else {
        LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, nullptr, image);
    }

    const TexMetadata &metadata = image.GetMetadata();
    wcout << path << ' ' << metadata.width << ' ' << metadata.height << endl;
}

void Texture::CreateTextureFromEXR(const wstring &path) {

    ScratchImage image;

    ThrowIfFailed(::LoadFromEXRFile(path.c_str(), nullptr, image));

    const TexMetadata &metadata = image.GetMetadata();

    ComPtr<ID3D11Texture2D> stagingTexture = CreateStagingTexture(image, metadata);

    D3D11_TEXTURE2D_DESC txtDesc = {};
    txtDesc.Width = static_cast<UINT>(metadata.width);
    txtDesc.Height = static_cast<UINT>(metadata.height);
    txtDesc.MipLevels = 0; // 밉맵 레벨 최대
    txtDesc.ArraySize = 1;
    txtDesc.Format = metadata.format;
    txtDesc.SampleDesc.Count = 1;
    txtDesc.Usage = D3D11_USAGE_DEFAULT; // DEFAULT->복사가능
    txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    txtDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; // 밉맵 사용
    txtDesc.CPUAccessFlags = 0;

    // 검은색 빈 텍스처 생성
    DEVICE->CreateTexture2D(&txtDesc, nullptr, _texture.GetAddressOf());

    // 스테이징 텍스처로부터 가장 해상도가 높은 이미지 복사
    CONTEXT->CopySubresourceRegion(_texture.Get(), 0, 0, 0, 0, stagingTexture.Get(), 0, nullptr);

    // SRV 생성
    DEVICE->CreateShaderResourceView(_texture.Get(), nullptr, _srv.GetAddressOf());

    // 해상도를 낮춰가며 밉맵 생성
    CONTEXT->GenerateMips(_srv.Get());
}

size_t Texture::GetPixelSize(DXGI_FORMAT pixelFormat) {

    switch (pixelFormat) {
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
        return sizeof(uint16_t) * 4;
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        return sizeof(uint32_t) * 4;
    case DXGI_FORMAT_R32_FLOAT:
        return sizeof(uint32_t) * 1;
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        return sizeof(uint8_t) * 4;
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return sizeof(uint8_t) * 4;
    case DXGI_FORMAT_R32_SINT:
        return sizeof(int32_t) * 1;
    case DXGI_FORMAT_R16_FLOAT:
        return sizeof(uint16_t) * 1;
    }

    cout << "PixelFormat not implemented " << pixelFormat << endl;

    return sizeof(uint8_t) * 4;
}
