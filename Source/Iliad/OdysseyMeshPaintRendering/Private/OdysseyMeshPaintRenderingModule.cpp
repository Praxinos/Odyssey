// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyMeshPaintRenderingModule.h"
#include "Interfaces/IPluginManager.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FOdysseyMeshPaintRenderingModule, OdysseyMeshPaintRendering );

void FOdysseyMeshPaintRenderingModule::StartupModule()
{
    FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("Odyssey"))->GetBaseDir(),TEXT("Shaders"));
    AddShaderSourceDirectoryMapping(TEXT("/Plugins/Odyssey"),PluginShaderDir);
}

void FOdysseyMeshPaintRenderingModule::ShutdownModule()
{
}
