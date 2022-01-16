// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTexture2DEditorToolkit.h"

#include "OdysseyTexture2DEditorGUI.h"

#include "IOdysseyTexture2DEditorModule.h"

#define LOCTEXT_NAMESPACE "OdysseyTexture2DEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyTexture2DEditorToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTexture2DEditorToolkit::~FOdysseyTexture2DEditorToolkit()
{
}

FOdysseyTexture2DEditorToolkit::FOdysseyTexture2DEditorToolkit(TSharedPtr<FOdysseyTexture2DEditor> iEditor) :
	FOdysseyAssetEditorToolkit( TEXT( "OdysseyTexture2DEditorApp" ), iEditor )
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------- FAssetEditorToolkit override

void
FOdysseyTexture2DEditorToolkit::OpenAsset(UObject* iObject)
{
	UTexture2D* texture = Cast<UTexture2D>(iObject);
	IOdysseyTexture2DEditorModule* odysseyTexture2DEditorModule = &FModuleManager::GetModuleChecked<IOdysseyTexture2DEditorModule>("OdysseyTexture2DEditor");
	odysseyTexture2DEditorModule->CreateOdysseyTexture2DEditor(texture);
}

FText
FOdysseyTexture2DEditorToolkit::GetBaseToolkitName() const
{
    return LOCTEXT( "AppLabel", "Odyssey Texture Editor" );
}

FName
FOdysseyTexture2DEditorToolkit::GetToolkitFName() const
{
    return FName( "OdysseyTexture2DEditor" );
}

FString
FOdysseyTexture2DEditorToolkit::GetWorldCentricTabPrefix() const
{
    return LOCTEXT( "WorldCentricTabPrefix", "Texture" ).ToString();
}

#undef LOCTEXT_NAMESPACE