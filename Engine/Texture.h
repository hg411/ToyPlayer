#pragma once

class Texture {
  public:
    Texture();
    ~Texture();

    ComPtr<ID3D11ShaderResourceView> GetSRV() { return _srv; }

    void Load(const wstring &path, bool useSRGB);

    void CreateTextureArrayFromFile(const vector<wstring> &paths, bool useSRGB);
    void CreateTextureFromDDSFile(const wstring &path);
    void CreateTextureFromImage(const wstring &path, bool useSRGB);

  private:
    ComPtr<ID3D11Texture2D> CreateStagingTexture(const ScratchImage &image,
                                                 const TexMetadata &metadata,
                                                 const int mipLevels = 1, const int arraySize = 1);
    ComPtr<ID3D11Texture2D> CreateStagingTexture(const int width, const int height,
                                                 const vector<uint8_t> &image,
                                                 const DXGI_FORMAT pixelFormat,
                                                 const int mipLevels = 1, const int arraySize = 1);

    HRESULT CreateTextureFromScratchImage(const ScratchImage &image);

    void ReadImage(const wstring &path, vector<uint8_t> &image, int &width, int &height);
    void LoadWICFile(const wstring &path, ScratchImage &image, bool useSRGB);
    void CreateTextureFromEXR(const wstring &path);
    size_t GetPixelSize(DXGI_FORMAT pixelFormat);

  private:
    ComPtr<ID3D11Texture2D> _texture;
    ComPtr<ID3D11ShaderResourceView> _srv;
};
