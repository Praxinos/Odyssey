// Copyright � 2018-2019 Praxinos, Inc. All Rights Reserved.
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyMeshPaintRenderingModule.h"
#include "Interfaces/IPluginManager.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FOdysseyMeshPaintRenderingModule, OdysseyMeshPaintRendering );

void FOdysseyMeshPaintRenderingModule::StartupModule()
{
    FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("Iliad"))->GetBaseDir(),TEXT("Shaders"));
    AddShaderSourceDirectoryMapping(TEXT("/Plugins/Iliad"),PluginShaderDir);
}

void FOdysseyMeshPaintRenderingModule::ShutdownModule()
{
}
