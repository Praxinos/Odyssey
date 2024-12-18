// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyShapesModule.h"

#include "OdysseyShape.h"

#define LOCTEXT_NAMESPACE "OdysseyShapes"

void
FOdysseyShapesModule::StartupModule()
{
    RegisterDetailCustomizations();
}

void
FOdysseyShapesModule::ShutdownModule()
{
    UnregisterDetailCustomization();
}

void
FOdysseyShapesModule::RegisterDetailCustomizations()
{
    FOdysseyShapes::RegisterDetailCustomization();
}

void
FOdysseyShapesModule::UnregisterDetailCustomization()
{
    FOdysseyShapes::UnregisterDetailCustomization();
}

IMPLEMENT_MODULE( FOdysseyShapesModule, OdysseyShapes );

#undef LOCTEXT_NAMESPACE
