// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureEditorToolkit.h"

#include "TextureEditor/OdysseyTextureEditorGUI.h"

#include "IOdysseyTextureEditorModule.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorToolkit::~FOdysseyTextureEditorToolkit()
{
}

FOdysseyTextureEditorToolkit::FOdysseyTextureEditorToolkit()
    : FOdysseyAssetEditorToolkit( TEXT( "OdysseyTextureEditorApp" ) )
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------- FAssetEditorToolkit override

void
FOdysseyTextureEditorToolkit::OpenAsset(UObject* iObject)
{
    UTexture2D* texture = Cast<UTexture2D>(iObject);
    IOdysseyTextureEditorModule* OdysseyTextureEditorModule = &FModuleManager::GetModuleChecked<IOdysseyTextureEditorModule>("OdysseyTextureEditor");
    OdysseyTextureEditorModule->CreateOdysseyTextureEditor( { texture } );
}

FText
FOdysseyTextureEditorToolkit::GetBaseToolkitName() const
{
    return LOCTEXT( "editor.name", "Odyssey Texture Editor" );
}

FName
FOdysseyTextureEditorToolkit::GetToolkitFName() const
{
    return FName( "OdysseyTextureEditor" );
}

FString
FOdysseyTextureEditorToolkit::GetWorldCentricTabPrefix() const
{
    return LOCTEXT( "editor.world-centric-tab-prefix", "Texture" ).ToString();
}

#undef LOCTEXT_NAMESPACE
