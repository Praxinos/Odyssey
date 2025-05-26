// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureEditorSettings.h"

UOdysseyTextureEditorSettings::UOdysseyTextureEditorSettings( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
    , OdysseyDefaultEditorEnabled( true )
{
}

//Static
UOdysseyTextureEditorSettings* UOdysseyTextureEditorSettings::Get()
{
    return CastChecked<UOdysseyTextureEditorSettings>(UOdysseyTextureEditorSettings::StaticClass()->GetDefaultObject());
}
