// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor headers
#include "ArianeEditorModule.h"
#include "ArianeEditorCommands.h"
#include "ArianeEditorViewportToolkit.h"
#include "ArianeEditor.h"
#include "ArianeEditorSettings.h"
#include "ArianeEditorColorSelectorTab.h"
#include "ArianeEditorLayerStackTab.h"
#include "ArianeEditorSceneTreeViewTab.h"
#include "ArianePainting3DComponentCustomization.h"
#include "ArianeEditorImageMovieSceneTrack.h"
#include "ArianeEditorStyle.h"
#include "ArianeEditorViewportEdMode.h"

// Ariane headers
#include "ArianePainting3DActor.h"
#include "ArianePainting3DComponent.h"
#include "ArianePainting3DStaticMeshComponent.h"
#include "ArianeLayer.h"
#include "ArianeLayerDrawing.h"
#include "ArianeLayerFolder.h"
#include "ArianeLayerStack.h"
#include "ArianeImageMovieSceneSection.h"

// Unreal headers
#include "AssetToolsModule.h"
#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "PropertyEditorModule.h"
#include "Settings/ContentBrowserSettings.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "ActorFactories/ActorFactory.h"
#include "EditorModeRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "EditorModeManager.h"
#include "Framework/Docking/LayoutExtender.h"
#include "Framework/Docking/TabManager.h"
#include "Selection.h"
#include "MeshUtilities.h" // for conversion to static mesh
#include "ISequencerModule.h"
#include "SequencerChannelInterface.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

/*-----------------------------------------------------------------------------
   FArianeEditorModule
-----------------------------------------------------------------------------*/

void
FArianeEditorModule::OpenStandaloneEditorForAsset( UObject* iAsset )
{
    if (!iAsset)
        return;

/* Gary
    TSharedRef<FArianeEditorStandaloneToolkit> toolkit = MakeShared<FArianeEditorStandaloneToolkit>(iAsset);
    toolkit->Open();
*/
}

/* Gary
void
FArianeEditorModule::AddOpenedEditor(FOdysseyPainterEditor* iEditor)
{
    mOpenedEditors.AddUnique(iEditor);
}


void
FArianeEditorModule::RemoveOpenedEditor(FOdysseyPainterEditor* iEditor)
{
    mOpenedEditors.Remove(iEditor);
}

FOdysseyPainterEditor*
FArianeEditorModule::GetOpenedEditorForAsset(UObject* iObject)
{
    for (FOdysseyPainterEditor* editor : mOpenedEditors)
    {
        UObject* editedObject = editor->GetEditedObject();
        if (editedObject == iObject)
            return editor;

        TArray<UObject*> additionalEditedObjects = editor->GetAdditionalEditedObjects();
        if (additionalEditedObjects.Contains(iObject))
            return editor;
    }

    return nullptr;
}
*/

void
FArianeEditorModule::StartupModule()
{
    FArianeEditorStyle::Register(); // Must be done before Commands(), as it uses style

    RegisterEditorMode();

    UToolMenus::RegisterStartupCallback( FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FArianeEditorModule::RegisterMenus ) );

    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
    LevelEditorModule.OnLevelEditorCreated().AddRaw(this, &FArianeEditorModule::RegisterToolbarButton);

    //FCoreDelegates::OnPostEngineInit.AddRaw(this, &FArianeEditorModule::OnEngineInit );
    RegisterCustomizations();

    // color Selector
    LevelEditorModule.OnRegisterLayoutExtensions().AddLambda([](FLayoutExtender& InExtender)
    {
        FTabManager::FTab ColorSelectorTab = FTabManager::FTab(FTabId(FArianeEditorColorSelectorTab::StaticId()), ETabState::OpenedTab);

        InExtender.ExtendLayout(
            FTabId("LevelEditorSceneOutliner"),
            ELayoutExtensionPosition::Below,
            ColorSelectorTab
        );
    });

    // Scene Treeview
    LevelEditorModule.OnRegisterLayoutExtensions().AddLambda([](FLayoutExtender& InExtender)
    {
        FTabManager::FTab SceneTreeViewTab = FTabManager::FTab(FTabId(FArianeEditorSceneTreeViewTab::StaticId()), ETabState::OpenedTab);

        InExtender.ExtendLayout(
            FTabId("TopLeftModeTab"),
            ELayoutExtensionPosition::Below,
            SceneTreeViewTab
        );
    });

    // Layer Stack
    LevelEditorModule.OnRegisterLayoutExtensions().AddLambda([](FLayoutExtender& InExtender)
    {
        FTabManager::FTab LayerStackTab = FTabManager::FTab(FTabId(FArianeEditorLayerStackTab::StaticId()), ETabState::OpenedTab);

        InExtender.ExtendLayout(
            FTabId("TopLeftModeTab"),
            ELayoutExtensionPosition::Below,
            LayerStackTab
        );
    });

    RegisterSettings();


    // for Ariane Image tracks in the sequencer
    ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>("Sequencer");
    ImageTrackHandle = SequencerModule.RegisterTrackEditor(FOnCreateTrackEditor::CreateStatic( &FArianeEditorImageMovieSceneTrack::CreateTrackEditor ) );

    SequencerModule.RegisterChannelInterface<FArianeImageMovieSceneChannel>();

/* Gary
    RegisterBrushOverrides(); //First thing to do, as it modifies the Brush CDO
    RegisterCommands();
    RegisterLevelEditorLayoutExtensions();
    RegisterDetailCustomizations();
    RegisterThumbnailRenderers();
    RegisterPropertyModuleCustomizations();

    FOdysseyVectorBrushCustomization::Register();
    FOdysseyVectorObjectViewPaletteCustomization::Register();
*/
}

void
FArianeEditorModule::OnEngineInit()
{
    //UToolMenus::RegisterStartupCallback( FSimpleMulticastDelegate::FDelegate::CreateRaw( this, &FArianeEditorModule::RegisterToolbarButton ) );
}

void
FArianeEditorModule::ShutdownModule()
{
    UToolMenus::UnRegisterStartupCallback( this );

    UnregisterCustomization();
    UnregisterEditorMode();
    UnregisterSettings();

    FArianeEditorStyle::Unregister();


    // for Ariane Image tracks in the sequencer
    ISequencerModule& SequencerModule = FModuleManager::GetModuleChecked<ISequencerModule>("Sequencer");
    SequencerModule.UnRegisterTrackEditor( ImageTrackHandle );

/* Gary
    UnregisterBrushOverrides();
    UnregisterCommands();
    UnregisterLevelEditorLayoutExtensions();
    UnregisterDetailCustomization();
    UnregisterThumbnailRenderers();
    UnregisterPropertyModuleCustomizations();

    FOdysseyVectorBrushCustomization::Unregister();
    FOdysseyVectorObjectViewPaletteCustomization::Unregister();
*/
}

FArianeEditorViewportEdMode*
FArianeEditorModule::GetArianeEditorViewportEdMode()
{
    FEditorModeTools& ModeTools = GLevelEditorModeTools();

    ModeTools.ActivateMode( FArianeEditorViewportEdMode::EM_ArianeEditorViewportEdModeId );

    FEdMode* EdMode = ModeTools.GetActiveMode( FArianeEditorViewportEdMode::EM_ArianeEditorViewportEdModeId );

    return static_cast<FArianeEditorViewportEdMode*>( EdMode );
}
/*
void
FArianeEditorModule::AddToolbarButton( FToolBarBuilder &builder )
{
    builder.AddToolBarButton( FArianeEditorCommands::Get().LaunchAriane
                            , NAME_None
                            , LOCTEXT("WorldProperties_Override", "My Button")
                            , LOCTEXT("WorldProperties_ToolTipOverride", "Click me to display a message")
                            , TAttribute<FSlateIcon>(), "LevelToolbarWorldSettings" );
}
*/

void
FArianeEditorModule::ActivateEdMode( AArianePainting3DActor* Painting3DActor )
{
    FEditorModeTools& ModeTools = GLevelEditorModeTools();
    FEdMode* CurrentEdMode = ModeTools.GetActiveMode( FArianeEditorViewportEdMode::EM_ArianeEditorViewportEdModeId );

    if( CurrentEdMode == nullptr )
    {
        ModeTools.ActivateMode( FArianeEditorViewportEdMode::EM_ArianeEditorViewportEdModeId );

        CurrentEdMode = ModeTools.GetActiveMode( FArianeEditorViewportEdMode::EM_ArianeEditorViewportEdModeId );

        if( CurrentEdMode )
        {
            FArianeEditorViewportEdMode* ArianeEdMode = static_cast<FArianeEditorViewportEdMode*>( CurrentEdMode );
            TSharedPtr<FModeToolkit> Toolkit = ArianeEdMode->GetToolkit();
            TSharedPtr<FArianeEditorViewportToolkit> ArianeToolkit = StaticCastSharedPtr<FArianeEditorViewportToolkit>(Toolkit);

            if( Painting3DActor == nullptr )
            {
                Painting3DActor = ArianeToolkit->GetEditor().AddPainting3DActor();

                UArianeLayerStack* LayerStack = Painting3DActor->GetPainting3DComponent()->GetLayerStack();
                UArianeLayerFolder* RootFolder = LayerStack->GetRootFolder();
                UArianeLayerDrawing* NewDrawingLayer = LayerStack->CreateDrawingLayer( RootFolder, true );

                NewDrawingLayer->Rename( TEXT("Drawing Layer") );

                LayerStack->SelectLayer( NewDrawingLayer, true );
            }

            // Select the actor
            GEditor->SelectNone( true, true );
            GEditor->SelectActor( Painting3DActor, true, true );
        }
    }
}

void
FArianeEditorModule::RegisterToolbarButton(TSharedPtr<ILevelEditor> InLevelEditor)
{
    // Extend the "File" section of the main toolbar
    UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu( "LevelEditor.LevelEditorToolBar.AssetsToolBar");
    FToolMenuSection& ToolbarSection = ToolbarMenu->FindOrAddSection("Content");

    FToolMenuEntry ArianeLauncherEntry = FToolMenuEntry::InitToolBarButton( TEXT("Launch Ariane")
                                                                            , FExecuteAction::CreateLambda( [this]()
                                                                            {
                                                                                ActivateEdMode( nullptr );
                                                                                //GetModeManager()->ActivateMode( EM_ArianeEditorViewportEdModeId );
                                                                            } )
                                                                            , LOCTEXT( "arianeeditor-launch.label", "Launch Ariane" )
                                                                            , LOCTEXT( "arianeeditor-launch.tooltip", "Launch Ariane" )
                                                                            , FSlateIcon( FArianeEditorStyle::Get().GetStyleSetName(), "ArianeEditor.EdMode24" ) );

    ToolbarSection.AddEntry( ArianeLauncherEntry );
}

void
FArianeEditorModule::UnregisterToolbarButton()
{
}

void
FArianeEditorModule::RegisterActorMenu()
{
    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.ActorContextMenu");
    FToolMenuSection& Section = Menu->FindOrAddSection("ActorOptions");

    FToolMenuEntry Entry = FToolMenuEntry::InitMenuEntry(
        "ConvertToStaticMesh",
        LOCTEXT("arianeeditor-convert-to-static-mesh.label", "[Ariane] Convert to Static Mesh"),
        LOCTEXT("arianeeditor-convert-to-static-mesh.tooltip", ""),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "GenericEditor.Bake"),
        FUIAction(FExecuteAction::CreateRaw(this, &FArianeEditorModule::ConvertToStaticMesh))
    );

    Section.AddEntry(Entry);
}

void
FArianeEditorModule::ConvertToStaticMesh()
{
    IMeshUtilities& MeshUtilities = FModuleManager::Get().LoadModuleChecked<IMeshUtilities>("MeshUtilities");
    USelection* SelectionSet = GEditor->GetSelectedActors();

    if( SelectionSet )
    {
        TArray<AActor*> ActorsToConvert;

        SelectionSet->GetSelectedObjects<AActor>(ActorsToConvert);

        if( ActorsToConvert.Num() )
        {
            TArray<UMeshComponent*> MeshComponentsToConvert;

            // first step retrieve all mesh components
            //FMeshUtilities::GetSkinnedAndStaticMeshComponentsFromActors( ActorsToConvert, MeshComponentsToConvert );

            for( AActor* ActorToConvert : ActorsToConvert )
            {
                TInlineComponentArray<UMeshComponent*> MeshComponents( ActorToConvert );

                for (UMeshComponent* MeshComponent : MeshComponents)
                {
                    if ( MeshComponent->IsA( UStaticMeshComponent::StaticClass() ) )
                    {
                        MeshComponentsToConvert.Add( MeshComponent );
                    }
                }
            }

            for (UMeshComponent* MeshComponent : MeshComponentsToConvert )
            {
                if ( MeshComponent->IsA( UArianePainting3DStaticMeshComponent::StaticClass() ) )
                {
                    //UArianePainting3DComponent* Painting3DComponent = Cast<UArianePainting3DComponent>(MeshComponent);
                    UArianePainting3DStaticMeshComponent* Painting3DStaticMeshComponent = Cast<UArianePainting3DStaticMeshComponent>(MeshComponent);

                    // if the component is invisible, it will not be converted to
                    // a static mesh by MeshUtilities.ConvertMeshesToStaticMesh
                    Painting3DStaticMeshComponent->SetVisibility(true);
                    Painting3DStaticMeshComponent->SetHiddenInGame(false);

                    Painting3DStaticMeshComponent->ConvertToStaticMesh();
                }
            }

            MeshUtilities.ConvertMeshesToStaticMesh( MeshComponentsToConvert
                                                   , ActorsToConvert.Num() == 1 ? ActorsToConvert[0]->GetTransform()
                                                                                : FTransform::Identity );

            for (UMeshComponent* MeshComponent : MeshComponentsToConvert )
            {
                if ( MeshComponent->IsA( UArianePainting3DStaticMeshComponent::StaticClass() ) )
                {
                    //UArianePainting3DComponent* Painting3DComponent = Cast<UArianePainting3DComponent>(MeshComponent);
                    UArianePainting3DStaticMeshComponent* Painting3DStaticMeshComponent = Cast<UArianePainting3DStaticMeshComponent>(MeshComponent);

                    Painting3DStaticMeshComponent->SetStaticMesh( nullptr );
                    Painting3DStaticMeshComponent->SetVisibility(false);
                    Painting3DStaticMeshComponent->SetHiddenInGame(true);
                }
            }
        }
    }
}

void
FArianeEditorModule::RegisterMenus()
{
    //UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu.AssetActionsSubMenu");
    //FToolMenuSection& Section = Menu->FindOrAddSection("AssetContextMoveActions");

    RegisterActorMenu();
}

void
FArianeEditorModule::RegisterEditorMode()
{
    FEditorModeRegistry::Get().RegisterMode<FArianeEditorViewportEdMode>(
        FArianeEditorViewportEdMode::EM_ArianeEditorViewportEdModeId,
        LOCTEXT("ariane-editor-mode.name", "Ariane"),
        FSlateIcon(FArianeEditorStyle::Get().GetStyleSetName(), "ArianeEditor.EdMode24" ),
        true );
}

void
FArianeEditorModule::UnregisterEditorMode()
{
    FEditorModeRegistry::Get().UnregisterMode(FArianeEditorViewportEdMode::EM_ArianeEditorViewportEdModeId);
}

void
FArianeEditorModule::RegisterPropertyModuleCustomizations()
{
}

void
FArianeEditorModule::UnregisterPropertyModuleCustomizations()
{
}

void
FArianeEditorModule::RegisterSettings()
{
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !SettingsModule )
        return;

    SettingsModule->RegisterSettings( "Editor", "Plugins", "ArianeEditor"
        , LOCTEXT( "user-settings.ariane-editor.name", "Ariane Editor" )
        , LOCTEXT( "user-settings.ariane-editor.tooltip", "Configure the look and feel of the Ariane Editor." )
        , GetMutableDefault<UArianeEditorSettings>() );
}

void
FArianeEditorModule::UnregisterSettings()
{
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !SettingsModule )
        return;

    SettingsModule->UnregisterSettings( "Editor", "Plugins", "ArianeEditor" );
}

void
FArianeEditorModule::RegisterCommands()
{
    FArianeEditorCommands::Register();
/* Gary
    FOdysseyPainterEditorAnimationCommands::Register();
    FOdysseyPainterEditorFlipbookCommands::Register();
    FOdysseyViewportDrawingEditorCommands::Register();
*/
}

void
FArianeEditorModule::UnregisterCommands()
{
/* Gary
    FOdysseyPainterEditorCommands::Unregister();
    FOdysseyPainterEditorAnimationCommands::Unregister();
    FOdysseyPainterEditorFlipbookCommands::Unregister();
    FOdysseyViewportDrawingEditorCommands::Unregister();
*/
}

void
FArianeEditorModule::RegisterThumbnailRenderers()
{
/* Gary
    UThumbnailManager::Get().RegisterCustomRenderer(UOdysseyAnimationCell::StaticClass(), UOdysseyAnimationCellThumbnailRenderer::StaticClass());
*/
}

void
FArianeEditorModule::UnregisterThumbnailRenderers()
{
    //UThumbnailManager::Get().UnregisterCustomRenderer(UOdysseyAnimationCellImageRaster::StaticClass());
}

void
FArianeEditorModule::RegisterLevelEditorLayoutExtensions()
{
/* Gary
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
    mExtendLevelEditorLayout = LevelEditorModule.OnRegisterLayoutExtensions().AddStatic(&FOdysseyPainterEditorLevelEditorLayout::ExtendLevelEditorLayout);
*/
}

void
FArianeEditorModule::UnregisterLevelEditorLayoutExtensions()
{
/* Gary
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
    LevelEditorModule.OnRegisterLayoutExtensions().Remove(mExtendLevelEditorLayout);
*/
}

void
FArianeEditorModule::RegisterCustomizations()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    PropertyModule.RegisterCustomClassLayout(
        UArianePainting3DComponent::StaticClass()->GetFName(),
        FOnGetDetailCustomizationInstance::CreateStatic(&FArianePainting3DComponentCustomization::MakeInstance)
    );
}

void
FArianeEditorModule::UnregisterCustomization()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    PropertyModule.UnregisterCustomPropertyTypeLayout( UArianePainting3DComponent::StaticClass()->GetFName() );
}

IMPLEMENT_MODULE( FArianeEditorModule, ArianeEditor );

#undef LOCTEXT_NAMESPACE
