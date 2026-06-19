// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyToolCollectionFactory.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor.h"
#include "EditorStyleSet.h"
#include "OdysseyToolCollection.h"

UOdysseyToolCollectionFactory::UOdysseyToolCollectionFactory(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
{
    bCreateNew = true;
    SupportedClass = UOdysseyToolCollection::StaticClass();
}

bool UOdysseyToolCollectionFactory::ConfigureProperties()
{
    return true;
}

FString
UOdysseyToolCollectionFactory::GetDefaultNewAssetName() const
{
    return Super::GetDefaultNewAssetName();
}

UObject*
UOdysseyToolCollectionFactory::FactoryCreateNew(UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn)
{
    // Ensure correct flags
    iFlags |= RF_Public | RF_Standalone;

    UOdysseyToolCollection* toolCollection = NewObject<UOdysseyToolCollection>(iParent, iClass, iName, iFlags);

    toolCollection->PostEditChange();
    toolCollection->MarkPackageDirty();

    FAssetRegistryModule::AssetCreated(toolCollection);

    return toolCollection;
}
