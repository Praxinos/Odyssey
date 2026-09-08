// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyFlipbookFactory.h"

#include "Materials/MaterialInterface.h"
#include "OdysseyTelemetry.h"
#include "PaperFlipbook.h"

//---

/////////////////////////////////////////////////////
// UOdysseyFlipbookFactory
UOdysseyFlipbookFactory::UOdysseyFlipbookFactory( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
{
    SupportedClass = UOdysseyFlipbook::StaticClass();
}

UObject*
UOdysseyFlipbookFactory::FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn )
{
    {
        using FAssetAddedFields = FAssetAdded_TelemetryFields;

        TArray<FAnalyticsEventAttribute> Attributes;
        Attributes.Emplace( FAssetAddedFields::AssetClassPath_KeyName_AsString, iClass->GetPathName() );

        FOdysseyTelemetry::Get().RecordEvent( FAssetAddedFields::KeyName, Attributes );
    }

    UPaperFlipbook* flipbook = Cast<UPaperFlipbook>(UPaperFlipbookFactory::FactoryCreateNew( UPaperFlipbook::StaticClass(), iParent, iName, iFlags, iContext, iWarn ));
    FScopedFlipbookMutator mutator(flipbook);
    mutator.FramesPerSecond = 24.0f;
    // mutator.GetSourceFlipbook()->DefaultMaterial = ;

    UClass* flipbookClass = flipbook->StaticClass();
    FObjectProperty* defaultMaterialProperty = FindFProperty<FObjectProperty>(flipbookClass, "DefaultMaterial");
    defaultMaterialProperty->SetObjectPropertyValue(defaultMaterialProperty->ContainerPtrToValuePtr<UPaperFlipbook>(flipbook), LoadObject<UMaterialInterface>(nullptr, TEXT("/Odyssey/Animation2D/DefaultFlipbookMaterialInstance.DefaultFlipbookMaterialInstance")));
    return flipbook;
}
