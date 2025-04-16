#pragma once
class Timer {
    DECLARE_SINGLE(Timer)

  public:
    void Init(ComPtr<ID3D11Device> device);
    void Update();

    float GetDeltaTime() const { return _deltaTime; }

  private:
    uint64 _frequency = 0;
    uint64 _prevCount = 0;
    float _deltaTime = 0.0f;

    uint32 _frameCount = 0;
    float _frameTime = 0.0f;
    uint32 _fps = 0;

    ComPtr<ID3D11Query> _startQuery, _stopQuery, _disjointQuery;
    double _elapsedTimeCPU = 0.0;
    double _elapsedTimeGPU = 0.0;
    decltype(std::chrono::high_resolution_clock::now()) _startTimeCPU;
    bool _measureGPU = false;
};
