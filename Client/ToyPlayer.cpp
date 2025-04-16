#include "pch.h"
#include "ToyPlayer.h"
#include "Engine.h"

void ToyPlayer::Init(const WindowInfo& windowInfo)
{
	GEngine->Init(windowInfo);
}

void ToyPlayer::Update()
{
	GEngine->Update();
}
