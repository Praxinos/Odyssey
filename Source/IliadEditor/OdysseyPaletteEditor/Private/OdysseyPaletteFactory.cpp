// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPaletteFactory.h"

#include "Editor.h"
#include "EditorStyleSet.h"
#include "OdysseyPalette.h"

#include "ULISLoaderModule.h"

#include <ULIS>

UOdysseyPaletteFactory::UOdysseyPaletteFactory( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
{
    bCreateNew = true;
    SupportedClass = UOdysseyPalette::StaticClass();
}

bool UOdysseyPaletteFactory::ConfigureProperties()
{
    return true;
}

FString
UOdysseyPaletteFactory::GetDefaultNewAssetName() const
{
    return Super::GetDefaultNewAssetName();
}

UObject*
UOdysseyPaletteFactory::FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn )
{
    UOdysseyPalette* palette = NewObject<UOdysseyPalette>(iParent, iClass, iName, iFlags);
    return palette;
}
