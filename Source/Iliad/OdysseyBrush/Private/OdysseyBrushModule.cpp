// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyBrushModule.h"
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
