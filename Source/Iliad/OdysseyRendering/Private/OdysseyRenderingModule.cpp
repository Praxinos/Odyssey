// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyRenderingModule.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

void
FOdysseyRenderingModule::StartupModule()
{
    RegisterShaders();
}

void
FOdysseyRenderingModule::ShutdownModule()
{
    UnregisterShaders();
}

void
FOdysseyRenderingModule::RegisterShaders()
{
    FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("Odyssey"))->GetBaseDir(), TEXT("Shaders"));
    AddShaderSourceDirectoryMapping(TEXT("/OdysseyShaders"), PluginShaderDir);
}

void
FOdysseyRenderingModule::UnregisterShaders()
{
    //No method available to unregister Shaders directories
}

IMPLEMENT_MODULE(FOdysseyRenderingModule, OdysseyRendering);
