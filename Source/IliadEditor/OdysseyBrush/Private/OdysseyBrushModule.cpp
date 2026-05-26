// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyBrushModule.h"

#include "Modules/ModuleManager.h"

#include "Customizations/OdysseyBrushCustomization.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushModule"

void
FOdysseyBrushModule::StartupModule()
{
    RegisterBrushCustomizations();
}

void
FOdysseyBrushModule::ShutdownModule()
{
    UnregisterBrushCustomizations();
}

void
FOdysseyBrushModule::RegisterBrushCustomizations()
{
    FOdysseyBrushCustomization::Register();
}

void
FOdysseyBrushModule::UnregisterBrushCustomizations()
{

}

IMPLEMENT_MODULE(FOdysseyBrushModule, OdysseyBrush);

#undef LOCTEXT_NAMESPACE
