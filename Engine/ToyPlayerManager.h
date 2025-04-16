#pragma once

class ToyPlayer;

class ToyPlayerManager {
    DECLARE_SINGLE(ToyPlayerManager)

  public:
    void Update();
    void Render();

    void LoadSupernova();

  private:
    shared_ptr<ToyPlayer> _activeToyPlayer;
};
