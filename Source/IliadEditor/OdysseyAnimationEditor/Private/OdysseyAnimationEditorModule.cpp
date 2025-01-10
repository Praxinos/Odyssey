// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAnimationEditorModule.h"

#include "AssetToolsModule.h"
#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Settings/ContentBrowserSettings.h"
#include "Subsystems/PlacementSubsystem.h"
#include "Toolkits/AssetEditorToolkit.h"

#include "OdysseyAnimationActorFactory.h"
#include "OdysseyAnimationEditorToolkit.h"
#include "OdysseyAnimationAssetTypeActions.h"
#include "OdysseyAnimationAssetTypeActions.h"
#include "AnimationEditor/OdysseyAnimationEditorCommands.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "AnimationEditor/OdysseyAnimationEditorGUI.h"
#include "AnimationEditor/OdysseyAnimationEditorProjectSettings.h"
#include "AnimationEditor/OdysseyAnimationEditorUserSettings.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyAnimation.h"
#include "AnimationEditor/OdysseyAnimationEditorSource.h"
#include "OdysseyAnimationEditorFlipSystem.h"
#include "Tools/OutOfPegsTool/OdysseyAnimationEditorOutOfPegsTool.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "LayerStack/Cells/OdysseyAnimationCellThumbnailRenderer.h"
#include "ISequencerModule.h"
#include "ILevelSequenceModule.h"


#define LOCTEXT_NAMESPACE "AnimationEditor"

/*-----------------------------------------------------------------------------
   FOdysseyAnimationEditorModule
-----------------------------------------------------------------------------*/

TSharedRef<FOdysseyAnimationEditorToolkit>
FOdysseyAnimationEditorModule::CreateOdysseyAnimationEditor( UOdysseyAnimation* iAnimation )
{
    TSharedPtr<FOdysseyPainterEditor> editor = MakeShared<FOdysseyPainterEditor>(
        TEXT("OdysseyAnimationEditor"),
        LOCTEXT("main-menu.category", "Odyssey Animation Editor"),
        iAnimation,
        "OdysseyAnimationEditor_Layout"
    );

    TSharedRef<FOdysseyAnimationEditorExtension> animationExtension = MakeShared<FOdysseyAnimationEditorExtension>(editor.Get());
    editor->AddExtension(animationExtension);

    TSharedPtr<FOdysseyAnimationEditorToolkit> toolkit = MakeShared<FOdysseyAnimationEditorToolkit>();
    toolkit->Initialize(iAnimation, editor);
    //-----

    TSharedPtr<FOdysseyAnimationEditorSource> source = MakeShared<FOdysseyAnimationEditorSource>(iAnimation);
    editor->SetSource(source);

    return toolkit.ToSharedRef();
}

void
FOdysseyAnimationEditorModule::StartupModule()
{
    RegisterAssetTypeActions();

    // Register Commands
    RegisterCommands();

    RegisterSettings();

    RegisterLevelEditorLayoutExtensions();

    RegisterDetailCustomizations();

    RegisterThumbnailRenderers();

    RegisterPlacementFactories();
}

void
FOdysseyAnimationEditorModule::ShutdownModule()
{
    // Unregister Commands
    UnregisterCommands();

    UnregisterSettings();

    // Unregister Assets Type Actions
    UnregisterAssetTypeActions();

    UnregisterLevelEditorLayoutExtensions();

    UnregisterDetailCustomization();

    UnregisterThumbnailRenderers();

    UnregisterPlacementFactories();
}

void
FOdysseyAnimationEditorModule::RegisterAssetTypeActions()
{
    IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

    // Create Asset Categories
    EAssetTypeCategories::Type category = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("Odyssey")), LOCTEXT("asset-category.name", "Odyssey"));

    //Create Asset Types Actions
    mOdysseyTypeActions = MakeShareable(new FOdysseyAnimationAssetTypeActions(category));

    //Register created Asset Type Actions
    assetTools.RegisterAssetTypeActions(mOdysseyTypeActions.ToSharedRef());
}

void
FOdysseyAnimationEditorModule::UnregisterAssetTypeActions()
{
    if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
        return;

    IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
    assetTools.UnregisterAssetTypeActions(mOdysseyTypeActions.ToSharedRef());
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

void
FOdysseyAnimationEditorModule::RegisterDetailCustomizations()
{
    FOdysseyAnimationEditorFlipSystem::RegisterDetailCustomization();

    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout(UOdysseyAnimationEditorOutOfPegsTool::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FOdysseyAnimationEditorOutOfPegsToolDetails::MakeInstance));
}

void
FOdysseyAnimationEditorModule::UnregisterDetailCustomization()
{
    FOdysseyAnimationEditorFlipSystem::UnregisterDetailCustomization();
}

void
FOdysseyAnimationEditorModule::RegisterLevelEditorLayoutExtensions()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
    mExtendLevelEditorLayout = LevelEditorModule.OnRegisterLayoutExtensions().AddStatic(&FOdysseyAnimationEditorGUI::ExtendLevelEditorLayout);
}

void
FOdysseyAnimationEditorModule::UnregisterLevelEditorLayoutExtensions()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
    LevelEditorModule.OnRegisterLayoutExtensions().Remove(mExtendLevelEditorLayout);
}

void
FOdysseyAnimationEditorModule::RegisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

    settingsModule->RegisterSettings( "Project", "Plugins", "OdysseyAnimationEditor"
                                        , LOCTEXT( "settings.name", "2D Animation Editor" )
                                        , LOCTEXT( "settings.tooltip", "Configure the look and feel of the 2D Animation Editor." )
                                        , GetMutableDefault<UOdysseyAnimationEditorProjectSettings>() );

    settingsModule->RegisterSettings( "Editor", "Plugins", "OdysseyAnimationEditorUserSettings"
                                        , LOCTEXT( "settings.name", "2D Animation Editor" )
                                        , LOCTEXT( "settings.tooltip", "Configure the look and feel of the 2D Animation Editor." )
                                        , GetMutableDefault<UOdysseyAnimationEditorUserSettings>() );
}

void
FOdysseyAnimationEditorModule::UnregisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

    settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyAnimationEditor" );
    settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyAnimationEditorUserSettings" );
}

void
FOdysseyAnimationEditorModule::RegisterThumbnailRenderers()
{
    UThumbnailManager::Get().RegisterCustomRenderer(UOdysseyAnimationCell::StaticClass(), UOdysseyAnimationCellThumbnailRenderer::StaticClass());
}

void
FOdysseyAnimationEditorModule::UnregisterThumbnailRenderers()
{
    //UThumbnailManager::Get().UnregisterCustomRenderer(UOdysseyAnimationCellImageRaster::StaticClass());
}

void
FOdysseyAnimationEditorModule::RegisterPlacementFactories()
{
    // This assumes that this delegate is called AFTER the one registered in UPlacementSubsystem::Initialize()
    FCoreDelegates::OnPostEngineInit.AddRaw( this, &FOdysseyAnimationEditorModule::RegisterFactoryDelayed );
}

void
FOdysseyAnimationEditorModule::UnregisterPlacementFactories()
{
    FCoreDelegates::OnPostEngineInit.RemoveAll( this );

    //check( GEditor );
    //UPlacementSubsystem* placementSubsystem = GEditor->GetEditorSubsystem<UPlacementSubsystem>();
    //placementSubsystem->OnPlacementFactoriesRegistered().RemoveAll( this );
}

void
FOdysseyAnimationEditorModule::RegisterFactoryDelayed()
{
    // It's not possible to use the first delegate FCoreDelegates::OnPostEngineInit and then add a new delegate on placementSubsystem->OnPlacementFactoriesRegistered()
    // because PlacementFactoriesRegistered is called during OnPostEngineInit ( UPlacementSubsystem::Initialize() -> OnPostEngineInit -> UPlacementSubsystem::RegisterPlacementFactories() -> PlacementFactoriesRegistered.Broadcast() )
    // so placementSubsystem->OnPlacementFactoriesRegistered() is always called BEFORE our own further registration
    // ( furthermore, GEditor is not available in FOdysseyAnimationEditorModule::StartupModule in case we want to direclty called FOdysseyAnimationEditorModule::RegisterFactoryDelayed() (without indirectly attach to FCoreDelegates::OnPostEngineInit )
    //check( GEditor );
    //UPlacementSubsystem* placementSubsystem = GEditor->GetEditorSubsystem<UPlacementSubsystem>();
    //placementSubsystem->OnPlacementFactoriesRegistered().AddRaw( this, &FOdysseyAnimationEditorModule::ReorderFactories );

    // So we call directly ReorderFactories() and still assume that our delegate in FCoreDelegates::OnPostEngineInit() is called after the one in UPlacementSubsystem::Initialize()
    ReorderFactories();
}

void
FOdysseyAnimationEditorModule::ReorderFactories()
{
    UPlacementSubsystem* placementSubsystem = GEditor->GetEditorSubsystem<UPlacementSubsystem>();
    TArray<TScriptInterface<IAssetFactoryInterface>> factories_to_move;

    UOdysseyAnimation* animation = NewObject<UOdysseyAnimation>( GetTransientPackage(), UOdysseyAnimation::StaticClass(), FName( TEXT( "Transient_Animation" ) ), RF_Transient );
    FAssetData asset_data( animation, false );

    // Find all the factories which can create UOdysseyAnimationActor
    // Unregister all the ones which are not UOdysseyAnimationActorFactory
    // Then register them again but at the end
    // So the first registered factory which can create UOdysseyAnimationActor is the UOdysseyAnimationActorFactory
    while( true )
    {
        TScriptInterface<IAssetFactoryInterface> factory = placementSubsystem->FindAssetFactoryFromAssetData( asset_data );
        UActorFactory* actorFactory = Cast<UActorFactory>( factory.GetObject() );
        if( !actorFactory )
            break;

        if( actorFactory->IsA<UOdysseyAnimationActorFactory>() )
            break;

        factories_to_move.Add( factory );
        placementSubsystem->UnregisterAssetFactory( factory );
    }

    for( TScriptInterface<IAssetFactoryInterface> factory_to_move : factories_to_move )
        placementSubsystem->RegisterAssetFactory( factory_to_move );
}

IMPLEMENT_MODULE( FOdysseyAnimationEditorModule, OdysseyAnimationEditor );

#undef LOCTEXT_NAMESPACE
