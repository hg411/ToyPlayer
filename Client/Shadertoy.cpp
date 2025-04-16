#include "pch.h"
#include "Shadertoy.h"
#include "Engine.h"

void Shadertoy::Init(const WindowInfo& windowInfo)
{
	GEngine->Init(windowInfo);
}

void Shadertoy::Update()
{
	GEngine->Update();
}
