// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureEditorSettings.h"

#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "OdysseyTextureEditorModule.h"

UOdysseyTextureEditorSettings::UOdysseyTextureEditorSettings( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
    , IliadDefaultEditorEnabled( true )
{
}

//Static
UOdysseyTextureEditorSettings* UOdysseyTextureEditorSettings::Get()
{
    return CastChecked<UOdysseyTextureEditorSettings>(UOdysseyTextureEditorSettings::StaticClass()->GetDefaultObject());
}

