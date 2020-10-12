// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyMeshPaintRenderingModule.h"
#include "Interfaces/IPluginManager.h"

IMPLEMENT_MODULE(FOdysseyMeshPaintRenderingModule, OdysseyMeshPaintRendering );

void FOdysseyMeshPaintRenderingModule::StartupModule()
{
    FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("Iliad"))->GetBaseDir(),TEXT("Shaders"));
    AddShaderSourceDirectoryMapping(TEXT("/Plugins/Iliad"),PluginShaderDir);
}

void FOdysseyMeshPaintRenderingModule::ShutdownModule()
{
}
