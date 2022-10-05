// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTexture2DEditorSettings.h"

#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "OdysseyTexture2DEditorModule.h"

UOdysseyTexture2DEditorSettings::UOdysseyTexture2DEditorSettings( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
    , IliadDefaultEditorEnabled( true )
{
}

//Static
UOdysseyTexture2DEditorSettings* UOdysseyTexture2DEditorSettings::Get()
{
    return CastChecked<UOdysseyTexture2DEditorSettings>(UOdysseyTexture2DEditorSettings::StaticClass()->GetDefaultObject());
}

