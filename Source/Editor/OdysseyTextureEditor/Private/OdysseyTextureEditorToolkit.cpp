// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditorToolkit.h"

#include "OdysseyTextureEditorGUI.h"

#include "IOdysseyTextureEditorModule.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorToolkit::~FOdysseyTextureEditorToolkit()
{
}

FOdysseyTextureEditorToolkit::FOdysseyTextureEditorToolkit() :
	FOdysseyPainterEditorToolkit( TEXT( "OdysseyTextureEditorApp" ) )
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------- FAssetEditorToolkit override

void
FOdysseyTextureEditorToolkit::OpenAsset(UObject* iObject)
{
	UTexture2D* texture = Cast<UTexture2D>(iObject);
	IOdysseyTextureEditorModule* odysseyTextureEditorModule = &FModuleManager::GetModuleChecked<IOdysseyTextureEditorModule>("OdysseyTextureEditor");
	odysseyTextureEditorModule->CreateOdysseyTextureEditor(texture);
}

FText
FOdysseyTextureEditorToolkit::GetBaseToolkitName() const
{
    return LOCTEXT( "AppLabel", "Odyssey Texture Editor" );
}

FName
FOdysseyTextureEditorToolkit::GetToolkitFName() const
{
    return FName( "OdysseyTextureEditor" );
}

FString
FOdysseyTextureEditorToolkit::GetWorldCentricTabPrefix() const
{
    return LOCTEXT( "WorldCentricTabPrefix", "Texture" ).ToString();
}

#undef LOCTEXT_NAMESPACE