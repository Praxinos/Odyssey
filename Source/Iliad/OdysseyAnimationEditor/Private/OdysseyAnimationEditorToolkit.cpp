// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationEditorToolkit.h"

#include "AnimationEditor/OdysseyAnimationEditorGUI.h"

#include "IOdysseyAnimationEditorModule.h"
#include "OdysseyAnimation.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

/////////////////////////////////////////////////////
// FOdysseyAnimationEditorToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyAnimationEditorToolkit::~FOdysseyAnimationEditorToolkit()
{
}

FOdysseyAnimationEditorToolkit::FOdysseyAnimationEditorToolkit() :
    FOdysseyAssetEditorToolkit( TEXT( "OdysseyAnimationEditorApp" ) )
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------- FAssetEditorToolkit override

void
FOdysseyAnimationEditorToolkit::OpenAsset(UObject* iObject)
{
    UOdysseyAnimation* animation = Cast<UOdysseyAnimation>(iObject);
    IOdysseyAnimationEditorModule* OdysseyAnimationEditorModule = &FModuleManager::GetModuleChecked<IOdysseyAnimationEditorModule>("OdysseyAnimationEditor");
    OdysseyAnimationEditorModule->CreateOdysseyAnimationEditor(animation);
}

FText
FOdysseyAnimationEditorToolkit::GetBaseToolkitName() const
{
    return LOCTEXT( "editor.name", "Odyssey Animation Editor" );
}

FName
FOdysseyAnimationEditorToolkit::GetToolkitFName() const
{
    return FName( "OdysseyAnimationEditor" );
}

FString
FOdysseyAnimationEditorToolkit::GetWorldCentricTabPrefix() const
{
    return LOCTEXT( "editor.world-centric-tab-prefix", "Animation " ).ToString();
}

#undef LOCTEXT_NAMESPACE
