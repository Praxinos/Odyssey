// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "EposNamingConventionModule.h"

#include "PropertyEditorModule.h"

#include "EposSequenceModule.h"
#include "NamingFormatter.h"
#include "Settings/NamingConventionSettings.h"
#include "Settings/NamingConventionSettingsCustomization.h"

#define LOCTEXT_NAMESPACE "FEposNamingConventionModule"

//---

void
FEposNamingConventionModule::StartupModule()
{
    RegisterPropertyCustomizations();
    RegisterNamingFormatter();
}

void
FEposNamingConventionModule::ShutdownModule()
{
    UnregisterPropertyCustomizations();
    UnregisterNamingFormatter();
}

void
FEposNamingConventionModule::AddReferencedObjects( FReferenceCollector& Collector )
{
    if( mNamingFormatterBoard )
        Collector.AddReferencedObject( mNamingFormatterBoard );
    if( mNamingFormatterShot )
        Collector.AddReferencedObject( mNamingFormatterShot );
}

FString
FEposNamingConventionModule::GetReferencerName() const //override
{
    return "FEposNamingConventionModule";
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

    PropertyModule.RegisterCustomPropertyTypeLayout(
        FNamingConventionCamera::StaticStruct()->GetFName(),
        FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FNamingConventionCameraCustomization::MakeInstance ) );

    PropertyModule.RegisterCustomPropertyTypeLayout(
        FNamingConventionShot::StaticStruct()->GetFName(),
        FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FNamingConventionShotCustomization::MakeInstance ) );

    PropertyModule.RegisterCustomPropertyTypeLayout(
        FNamingConventionBoard::StaticStruct()->GetFName(),
        FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FNamingConventionBoardCustomization::MakeInstance ) );

    PropertyModule.NotifyCustomizationModuleChanged();
}

void
FEposNamingConventionModule::UnregisterPropertyCustomizations()
{
    if( FModuleManager::Get().IsModuleLoaded( "PropertyEditor" ) )
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>( "PropertyEditor" );
        PropertyModule.UnregisterCustomPropertyTypeLayout( FNamingConventionPlane::StaticStruct()->GetFName() );
        PropertyModule.UnregisterCustomPropertyTypeLayout( FNamingConventionCamera::StaticStruct()->GetFName() );
        PropertyModule.UnregisterCustomPropertyTypeLayout( FNamingConventionShot::StaticStruct()->GetFName() );
        PropertyModule.UnregisterCustomPropertyTypeLayout( FNamingConventionBoard::StaticStruct()->GetFName() );

        PropertyModule.NotifyCustomizationModuleChanged();
    }
}

void
FEposNamingConventionModule::RegisterNamingFormatter()
{
    FEposSequenceModule& module = FModuleManager::LoadModuleChecked<FEposSequenceModule>( "EposSequence" );

    mNamingFormatterBoard = NewObject<UDefaultNamingFormatterBoard>();
    module.RegisterNamingFormatter( mNamingFormatterBoard );

    mNamingFormatterShot = NewObject<UDefaultNamingFormatterShot>();
    module.RegisterNamingFormatter( mNamingFormatterShot );
}

void
FEposNamingConventionModule::UnregisterNamingFormatter()
{
    FEposSequenceModule& module = FModuleManager::LoadModuleChecked<FEposSequenceModule>( "EposSequence" );

    module.UnregisterNamingFormatter( mNamingFormatterBoard );
    module.UnregisterNamingFormatter( mNamingFormatterShot );
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEposNamingConventionModule, EposNamingConvention )
