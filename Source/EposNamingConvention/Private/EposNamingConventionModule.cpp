// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "EposNamingConventionModule.h"

#include "PropertyEditorModule.h"

#include "Settings/NamingConventionSettings.h"
#include "Settings/NamingConventionSettingsCustomization.h"

#define LOCTEXT_NAMESPACE "FEposNamingConventionModule"

void
FEposNamingConventionModule::StartupModule()
{
    RegisterPropertyCustomizations();
}

void
FEposNamingConventionModule::ShutdownModule()
{
    UnregisterPropertyCustomizations();
}

//---

void
FEposNamingConventionModule::RegisterPropertyCustomizations()
{
    // import the PropertyEditor module...
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>( "PropertyEditor" );
    // to register our custom property
    PropertyModule.RegisterCustomPropertyTypeLayout(
        // This is the name of the Struct
        // this tells the property editor which is the struct property our customization will applied on.
        FNamingConventionPlane::StaticStruct()->GetFName(),
        // this is where our MakeInstance() method is usefull
        FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FNamingConventionPlaneCustomization::MakeInstance ) );

    PropertyModule.NotifyCustomizationModuleChanged();
}

void
FEposNamingConventionModule::UnregisterPropertyCustomizations()
{
    if( FModuleManager::Get().IsModuleLoaded( "PropertyEditor" ) )
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>( "PropertyEditor" );
        PropertyModule.UnregisterCustomPropertyTypeLayout( FNamingConventionPlane::StaticStruct()->GetFName() );

        PropertyModule.NotifyCustomizationModuleChanged();
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEposNamingConventionModule, EposNamingConvention )
