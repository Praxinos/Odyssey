// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationEditorModule.h"

#include "AssetToolsModule.h"
#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Settings/ContentBrowserSettings.h"
#include "Toolkits/AssetEditorToolkit.h"

#include "AnimationEditor/OdysseyAnimationEditor.h"
#include "OdysseyAnimationEditorToolkit.h"
#include "OdysseyAnimationAssetTypeActions.h"
#include "OdysseyAnimationAssetTypeActions.h"
#include "AnimationEditor/OdysseyAnimationEditorCommands.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationEditorModule"

/*-----------------------------------------------------------------------------
   FOdysseyAnimationEditorModule
-----------------------------------------------------------------------------*/

TSharedRef<FOdysseyAnimationEditorToolkit>
FOdysseyAnimationEditorModule::CreateOdysseyAnimationEditor( UOdysseyAnimation* iAnimation )
{
	TSharedPtr<FOdysseyPainterEditor> editor = MakeShared<FOdysseyPainterEditor>(
		LOCTEXT("WorkspaceMenu_OdysseyAnimationEditor", "Odyssey Animation Editor"),
		iAnimation,
		"OdysseyAnimationEditor_Layout"
	);

	editor->AddExtension(MakeShared<FOdysseyAnimationEditorExtension>(editor.Get()));

    TSharedPtr<FOdysseyAnimationEditorToolkit> toolkit = MakeShared<FOdysseyAnimationEditorToolkit>();
    toolkit->Initialize(iAnimation, editor);

	TSharedPtr<FOdysseyAnimationEditorSource> source = MakeShared<FOdysseyAnimationEditorSource>(iAnimation);
	editor->SetSource(source);

    return toolkit.ToSharedRef();

	/* TSharedPtr<FOdysseyAnimationEditor> editor = MakeShareable(new FOdysseyAnimationEditor());
    TSharedPtr<FOdysseyAnimationEditorToolkit> toolkit = MakeShareable( new FOdysseyAnimationEditorToolkit(editor) );
	editor->Initialize(iAnimation);
    toolkit->Initialize();

	TSharedPtr<FOdysseyAnimationEditorSource> source = MakeShared<FOdysseyAnimationEditorSource>(iAnimation);
	editor->SetSource(source);

    return toolkit.ToSharedRef(); */
}

void
FOdysseyAnimationEditorModule::StartupModule()
{
    RegisterAssetTypeActions();

	// Register Commands
	RegisterCommands();
}

void
FOdysseyAnimationEditorModule::ShutdownModule()
{
	// Unregister Commands
	UnregisterCommands();

	// Unregister Assets Type Actions
	UnregisterAssetTypeActions();
}

void
FOdysseyAnimationEditorModule::RegisterAssetTypeActions()
{
	IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	// Create Asset Categories
	EAssetTypeCategories::Type category = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("ILIAD")), LOCTEXT("IliadPainterAssetCategory", "ILIAD"));

	//Create Asset Types Actions
	mIliadTypeActions = MakeShareable(new FOdysseyAnimationAssetTypeActions(category));

	//Register created Asset Type Actions
	assetTools.RegisterAssetTypeActions(mIliadTypeActions.ToSharedRef());
}

void
FOdysseyAnimationEditorModule::UnregisterAssetTypeActions()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
		return;
	
	IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
    assetTools.UnregisterAssetTypeActions(mIliadTypeActions.ToSharedRef());
}

void
FOdysseyAnimationEditorModule::RegisterCommands()
{
	FOdysseyAnimationEditorCommands::Register();
}

void
FOdysseyAnimationEditorModule::UnregisterCommands()
{
	FOdysseyAnimationEditorCommands::Unregister();
}

IMPLEMENT_MODULE( FOdysseyAnimationEditorModule, OdysseyAnimationEditor );

#undef LOCTEXT_NAMESPACE
