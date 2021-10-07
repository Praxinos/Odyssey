// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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

void
UOdysseyTexture2DEditorSettings::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    const FName propertyName = PropertyChangedEvent.GetPropertyName();
    if( propertyName == GET_MEMBER_NAME_CHECKED( UOdysseyTexture2DEditorSettings, IliadDefaultEditorEnabled ) )
    {
        FOdysseyTexture2DEditorModule* odysseyTextureModule = &FModuleManager::LoadModuleChecked<FOdysseyTexture2DEditorModule>("OdysseyTexture2DEditor");
        odysseyTextureModule->UnregisterAssetTypeActions();
        odysseyTextureModule->RegisterAssetTypeActions();
    }
}

