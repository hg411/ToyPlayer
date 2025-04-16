#include "pch.h"
#include "ToyPlayerManager.h"
#include "ToyPlayer.h"

void ToyPlayerManager::Update() {
    if (_activeToyPlayer == nullptr)
        return;

    _activeToyPlayer->Update();
}

void ToyPlayerManager::Render() { _activeToyPlayer->Render(); }

void ToyPlayerManager::LoadSupernova() { 
    _activeToyPlayer = make_shared<ToyPlayer>();


}
