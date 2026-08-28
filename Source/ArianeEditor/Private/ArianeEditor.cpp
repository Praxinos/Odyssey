// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// ArianeEditor headers
#include "ArianeEditor.h"
#include "ArianeEditorStyle.h"
#include "ArianeEditorTool.h"
#include "ArianeEditorSettings.h"
#include "ArianeEditorViewportToolkit.h"
#include "ArianeEditorColorSelectorTab.h"
#include "ArianeEditorLayerStackTab.h"
#include "ArianeEditorSceneTreeViewTab.h"
#include "PathEditTool/ArianeEditorPathEditTool.h"
#include "PathEditTool/ArianeEditorPathEditToolBuilder.h"
#include "PathDrawingTool/ArianeEditorPathDrawingTool.h"
#include "PathDrawingTool/ArianeEditorPathDrawingToolBuilder.h"
#include "PrimitiveDrawingTool/ArianeEditorPrimitiveDrawingTool.h"
#include "PrimitiveDrawingTool/ArianeEditorPrimitiveDrawingToolBuilder.h"
#include "EraserTool/ArianeEditorEraserTool.h"
#include "EraserTool/ArianeEditorEraserToolBuilder.h"
#include "PaintBucketTool/ArianeEditorPaintBucketTool.h"
#include "PaintBucketTool/ArianeEditorPaintBucketToolBuilder.h"
#include "LayerTransformTool/ArianeEditorLayerTransformTool.h"
#include "LayerTransformTool/ArianeEditorLayerTransformToolBuilder.h"
// Ariane headers
#include "ArianePrimitive.h"
#include "ArianeGroup.h"
#include "ArianePainting3DComponent.h"
#include "ArianeLayerStack.h"
#include "ArianeLayerDrawing.h"
#include "ArianeLayerFolder.h"
#include "ArianePainting3DActor.h"
// Unreal
#include "Toolkits/BaseToolkit.h"
#include "UObject/Object.h"
#include "EdMode.h"
#include "LevelEditor.h"
#include "FileHelpers.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Selection.h"
#include "EditorModeManager.h"
#include "Tools/EdModeInteractiveToolsContext.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "SEnumCombo.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"


FArianeEditor::FClipboard&
FArianeEditor::GetClipboard()
{
    static FArianeEditor::FClipboard Clipboard;

    return Clipboard;
}

FArianeEditor::~FArianeEditor()
{
    USelection::SelectionChangedEvent.RemoveAll( this );
}

FArianeEditor::FArianeEditor( FArianeEditorViewportToolkit* iToolkit )
    : Toolkit( iToolkit )
    , Name("ArianeEditor")
    , ColorType ( EOdysseyPainterEditorColorType::Raw )
    , DrawingOrientation ( EArianeEditorDrawingOrientation::View )
    , DrawingCoordinateSystem ( EArianeEditorDrawingCoordinateSystem::Local )
{
    HUDDrawingFlags.Mode = FArianeEditorHUD::EMode::Object;

    // Component selection is managed by ArianeEditor in order to emulate a Pre/Post Selection event behavior
    USelection::SelectionChangedEvent.AddRaw( this, &FArianeEditor::OnEditorSelectionChanged );

}

const FArianeEditorHUD::FDrawingFlags&
FArianeEditor::GetHUDDrawingFlags()
{
    return HUDDrawingFlags;
}

void
FArianeEditor::PostInit()
{
    // Set the CurrentPainting3DComponent
    // It will also trigger an event which will allow our widget to refresh at start
    OnEditorSelectionChanged( GEditor->GetSelectedActors() );
}

FArianeEditorViewportToolkit*
FArianeEditor::GetToolkit()
{
    return Toolkit;
}

void
FArianeEditor::ExtendToolbarSaveAssetButton( UToolMenu* iToolMenu )
{
    FToolMenuSection& assetSection = iToolMenu->AddSection("Asset");

    assetSection.AddEntry(
        FToolMenuEntry::InitToolBarButton(
            NAME_None,
            FUIAction(
                FExecuteAction::CreateLambda(
                    [this]()
                    {
                        FEditorFileUtils::SaveMap( GetWorld(), GetWorld()->GetPathName() );
                    }
                )
            ),
            FText(),
            LOCTEXT("top-tab.save-asset", "Saves the painted asset"),
            FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.Save32"),
            EUserInterfaceActionType::Button
        )
    );
}

EArianeEditorDrawingCoordinateSystem
FArianeEditor::GetDrawingCoordinateSystem()
{
    return DrawingCoordinateSystem;
}

EArianeLayerDrawingOrientation
FArianeEditor::GetLayerDrawingOrientation( UArianeLayerDrawing* DrawingLayer )
{
    if( ( DrawingOrientation == EArianeEditorDrawingOrientation::XY )
     || ( ( DrawingOrientation == EArianeEditorDrawingOrientation::LayerDefined )
       && ( DrawingLayer->GetDrawingOrientation() ==  EArianeLayerDrawingOrientation::XY ) ) )
    {
        return EArianeLayerDrawingOrientation::XY;
    }

    if( ( DrawingOrientation == EArianeEditorDrawingOrientation::YZ )
     || ( ( DrawingOrientation == EArianeEditorDrawingOrientation::LayerDefined )
       && ( DrawingLayer->GetDrawingOrientation() ==  EArianeLayerDrawingOrientation::YZ ) ) )
    {
        return EArianeLayerDrawingOrientation::YZ;
    }

    if( ( DrawingOrientation == EArianeEditorDrawingOrientation::ZX )
     || ( ( DrawingOrientation == EArianeEditorDrawingOrientation::LayerDefined )
       && ( DrawingLayer->GetDrawingOrientation() ==  EArianeLayerDrawingOrientation::ZX ) ) )
    {
        return EArianeLayerDrawingOrientation::ZX;
    }

    if( ( DrawingOrientation == EArianeEditorDrawingOrientation::View )
     || ( ( DrawingOrientation == EArianeEditorDrawingOrientation::LayerDefined )
       && ( DrawingLayer->GetDrawingOrientation() ==  EArianeLayerDrawingOrientation::View ) ) )
    {
        return EArianeLayerDrawingOrientation::View;
    }

    // should not de reached anyways
    return EArianeLayerDrawingOrientation::View;
}

void
FArianeEditor::SetDrawingOrientation( EArianeEditorDrawingOrientation InDrawingOrientation )
{
    DrawingOrientation = InDrawingOrientation;
}

const FSlateBrush*
FArianeEditor::GetDrawingOrientationBackgroundBrush( EArianeEditorDrawingOrientation InDrawingOrientation ) const
{
    static FSlateColorBrush Selected = FSlateColorBrush( FStyleColors::Select );

    return ( DrawingOrientation == InDrawingOrientation ) ? &Selected : nullptr;
}

TSharedRef<SWidget>
FArianeEditor::CreateDrawingCoordinateSystemComboBox()
{
    return SNew(SEnumComboBox, StaticEnum<EArianeEditorDrawingCoordinateSystem>())
          .OnEnumSelectionChanged(this, &FArianeEditor::OnDrawingCoordinateSystemChanged)
          .CurrentValue_Lambda([this]() { return (int32)DrawingCoordinateSystem; } );
}

void
FArianeEditor::OnDrawingCoordinateSystemChanged( int32 EnumID, ESelectInfo::Type )
{
    DrawingCoordinateSystem = static_cast<EArianeEditorDrawingCoordinateSystem>(EnumID);
}

TSharedRef<SWidget>
FArianeEditor::CreateDrawingOrientationSegmentControl()
{
    return SNew(SSegmentedControl<EArianeEditorDrawingOrientation>)
           .Value_Lambda( [this]{ return DrawingOrientation; } )
           .SupportsEmptySelection( false )
           .SupportsMultiSelection( false )
           .UniformPadding( FMargin( 2, 0, 2, 0 ) )
           .OnValueChanged( SSegmentedControl<EArianeEditorDrawingOrientation>::FOnValueChanged::CreateSP( this, &FArianeEditor::SetDrawingOrientation ) )
           + SSegmentedControl<EArianeEditorDrawingOrientation>::Slot( EArianeEditorDrawingOrientation::View )
           .ToolTip( LOCTEXT("ariane-editor.drawing-orientation.View.name", "Use Camera's view") )
           [
               SNew(SBorder)
               .BorderImage_Raw( this, &FArianeEditor::GetDrawingOrientationBackgroundBrush, EArianeEditorDrawingOrientation::View  )
               [
                   SNew(SImage)
                   .Image( FArianeEditorStyle::Get().GetBrush( "ArianeEditor.DrawingOrientation.View20") )
               ]
           ]
           + SSegmentedControl<EArianeEditorDrawingOrientation>::Slot( EArianeEditorDrawingOrientation::XY )
           .ToolTip( LOCTEXT("ariane-editor.drawing-orientation.LayerXY.name", "Use XY Plane") )
           [
               SNew(SBorder)
               .BorderImage_Raw( this, &FArianeEditor::GetDrawingOrientationBackgroundBrush, EArianeEditorDrawingOrientation::XY  )
               [
                   SNew(SImage)
                   .Image( FArianeEditorStyle::Get().GetBrush( "ArianeEditor.DrawingOrientation.LayerXY20") )
               ]
           ]
           + SSegmentedControl<EArianeEditorDrawingOrientation>::Slot( EArianeEditorDrawingOrientation::YZ )
           .ToolTip( LOCTEXT("ariane-editor.drawing-orientation.LayerYZ.name", "Use YZ Plane") )
           [
               SNew(SBorder)
               .BorderImage_Raw( this, &FArianeEditor::GetDrawingOrientationBackgroundBrush, EArianeEditorDrawingOrientation::YZ  )
               [
                   SNew(SImage)
                   .Image( FArianeEditorStyle::Get().GetBrush( "ArianeEditor.DrawingOrientation.LayerYZ20") )
               ]
           ]
           + SSegmentedControl<EArianeEditorDrawingOrientation>::Slot( EArianeEditorDrawingOrientation::ZX )
           .ToolTip( LOCTEXT("ariane-editor.drawing-orientation.LayerZX.name", "Use ZX Plane") )
           [
               SNew(SBorder)
               .BorderImage_Raw( this, &FArianeEditor::GetDrawingOrientationBackgroundBrush, EArianeEditorDrawingOrientation::ZX  )
               [
                   SNew(SImage)
                   .Image( FArianeEditorStyle::Get().GetBrush( "ArianeEditor.DrawingOrientation.LayerZX20") )
               ]
           ]
           + SSegmentedControl<EArianeEditorDrawingOrientation>::Slot( EArianeEditorDrawingOrientation::LayerDefined )
           .ToolTip( LOCTEXT("ariane-editor.drawing-orientation.LayerDefined.name", "Use Layer-defined drawing orientation") )
           [
               SNew(SBorder)
               .BorderImage_Raw( this, &FArianeEditor::GetDrawingOrientationBackgroundBrush, EArianeEditorDrawingOrientation::LayerDefined  )
               [
                   SNew(SImage)
                   .Image( FArianeEditorStyle::Get().GetBrush( "ArianeEditor.DrawingOrientation.LayerDefined20") )
               ]
           ];
}

void
FArianeEditor::ClearPainting3DComponents()
{
    UEditorActorSubsystem* editorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();

    for( AActor* actor : editorActorSubsystem->GetSelectedLevelActors() )
    {
        UArianePainting3DComponent* Painting3DComponent = Cast<UArianePainting3DComponent>(actor->GetComponentByClass( UArianePainting3DComponent::StaticClass() ));

        if( Painting3DComponent )
        {
            UArianeLayerDrawing* CurrentDrawingLayer = Cast<UArianeLayerDrawing>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

            if( CurrentDrawingLayer )
            {
                if( GEditor->IsTransactionActive() )
                {
                    CurrentDrawingLayer->Modify();
                }

                CurrentDrawingLayer->ResetHierarchy();
            }

            Painting3DComponent->Update( false );
        }
    }
}

void
FArianeEditor::ExtendToolbarToolParameters( UToolMenu* iToolMenu )
{
    FToolMenuSection& undoRedoSection = iToolMenu->AddSection("UndoRedo");

    undoRedoSection.AddEntry(
        FToolMenuEntry::InitToolBarButton(
            "Undo",
            FUIAction(
                FExecuteAction::CreateLambda( [this]()
                                              {
                                                  GEditor->UndoTransaction(true);
                                              } )
            ),
            FText(),
            LOCTEXT("top-tab.undo", "Undo the previous action."),
            FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.Undo32"),
            EUserInterfaceActionType::Button
        )
    );

    undoRedoSection.AddEntry(
        FToolMenuEntry::InitToolBarButton(
            "Redo",
            FUIAction(
                FExecuteAction::CreateLambda( [this]()
                                              {
                                                  GEditor->RedoTransaction();
                                              } )
            ),
            FText(),
            LOCTEXT("top-tab.redo", "Redo the next action."),
            FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.Redo32"),
            EUserInterfaceActionType::Button
        )
    );

    FToolMenuSection& clearCanvasSection = iToolMenu->AddSection("ClearPainting3DActors");

    clearCanvasSection.AddEntry(
        FToolMenuEntry::InitToolBarButton(
            "ClearPainting3DActors",
            FUIAction(
                FExecuteAction::CreateLambda(
                    [this]()
                    {
                        GEditor->BeginTransaction(FText::FromString("Clear selected Painting3D actors"));

                        ClearPainting3DComponents();

                        GEditor->EndTransaction();
                    }
                )
            ),
            FText(),
            LOCTEXT("ariane-top-tab-.clear", "Clear selected Painting3D actors."),
            FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.Clear32"),
            EUserInterfaceActionType::Button
        )
    );

    FToolMenuSection& DrawingOrientationSection = iToolMenu->AddSection("DrawingOrientation");

    DrawingOrientationSection.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            CreateDrawingOrientationSegmentControl(),
            FText()
        )
    );

    FToolMenuSection& DrawingCoordinateSystemSection = iToolMenu->AddSection("DrawingCoordinateSystem");

    DrawingCoordinateSystemSection.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            CreateDrawingCoordinateSystemComboBox(),
            FText()
        )
    );

    UArianeEditorTool* CurrentTool = GetCurrentTool();
    if (CurrentTool)
    {
        CurrentTool->ExtendToolbar(iToolMenu);
    }
}

void
FArianeEditor::ExtendLevelEditorToolbar( UToolMenu* iToolbar )
{
    ToolbarMenuName = iToolbar->GetMenuName();

    iToolbar->AddDynamicSection(
        "ToolParameters",
        FNewToolMenuDelegate::CreateLambda(
            [this](UToolMenu* iToolMenu)
            {
                ExtendToolbarSaveAssetButton(iToolMenu);
                ExtendToolbarToolParameters(iToolMenu);
            }
        )
    );
}

// Tools ------------------------------

UArianeEditorTool*
FArianeEditor::GetTool( const FString& ToolType )
{
    for( UArianeEditorTool* Tool : Tools )
    {
        if( Tool->GetType() == ToolType )
        {
            return Tool;
        }
    }

    return nullptr;
}

bool
FArianeEditor::IsCurrentTool( const FString& ToolType )
{
    return ( GetCurrentTool() == GetTool( ToolType ) );
}

UArianeEditorTool*
FArianeEditor::GetCurrentTool()
{
    // Note: EToolSide::Left means "the mouse"
    return Cast<UArianeEditorTool>(GetToolManager()->GetActiveTool( EToolSide::Left ));
}

void
FArianeEditor::SetCurrentTool( const FString& ToolType
                             , EToolShutdownType PreviousToolShutdownType
                             , bool TriggerEvent  )
{
    SetCurrentTool( GetTool( ToolType ), PreviousToolShutdownType, TriggerEvent );
}

void
FArianeEditor::SetCurrentTool( UArianeEditorTool* Tool
                             , EToolShutdownType PreviousToolShutdownType
                             , bool TriggerEvent )
{
    if( TriggerEvent )
        OnPreCurrentToolChanged.Broadcast();

    if( Tool )
    {
        // Note: EToolSide::Left means the mouse
        GetToolManager()->SelectActiveToolType( EToolSide::Left, Tool->GetType() );
        GetToolManager()->ActivateTool( EToolSide::Left );
    }
    else
    {
        GetToolManager()->DeactivateTool( EToolSide::Left, PreviousToolShutdownType );
    }

    if( TriggerEvent )
        OnPostCurrentToolChanged.Broadcast();

    UToolMenus::Get()->RefreshMenuWidget(ToolbarMenuName);
}

UInteractiveToolManager*
FArianeEditor::GetToolManager()
{
    return Toolkit->GetEditorMode()->GetEditorModeTools()->GetInteractiveToolsContext()->ToolManager.Get();
}

const TArray<UArianeEditorTool*>&
FArianeEditor::GetTools()
{
    return Tools;
}

void
FArianeEditor::AddToolBuilder( UArianeEditorToolBuilder* ToolBuilder )
{
    // We create the tool here and not in UArianeEditorToolBuilder::BuildTool so that
    // the tool collection is ready for the tool selector widget because GetToolManager()->RegisterToolType
    // does not create the tool right away
    UArianeEditorTool* Tool = ToolBuilder->CreateTool( GetToolManager() );

    Tool->Init( this );

    Tools.Add( Tool );

    GetToolManager()->RegisterToolType( ToolBuilder->GetType(), ToolBuilder );
}

void
FArianeEditor::RegisterTools()
{
    AddToolBuilder( NewObject<UArianeEditorPathDrawingToolBuilder>() );
    AddToolBuilder( NewObject<UArianeEditorPrimitiveDrawingToolBuilder>() );
    AddToolBuilder( NewObject<UArianeEditorPathEditToolBuilder>() );
    AddToolBuilder( NewObject<UArianeEditorEraserToolBuilder>() );
    AddToolBuilder( NewObject<UArianeEditorPaintBucketToolBuilder>() );
    AddToolBuilder( NewObject<UArianeEditorLayerTransformToolBuilder>() );

    SetCurrentTool( Tools[0], EToolShutdownType::Accept, true );
}

void FArianeEditor::UnregisterTools()
{
    TObjectPtr<UInteractiveToolManager> ToolManager = GetToolManager();

    // will deactivate the current tool.
    SetCurrentTool( nullptr, EToolShutdownType::Completed, true );

    for( UArianeEditorTool* Tool : Tools )
    {
        ToolManager->UnregisterToolType( Tool->GetType() );
    }

/*
    if (ToolManager->HasAnyActiveTool(EToolSide::Left))
    {
            ToolManager->DeactivateTool(EToolSide::Left, EToolShutdownType::Completed);
    }
*/


}

// --------------------- Tabs

void
FArianeEditor::RemoveTab( TSharedPtr<FArianeEditorTab> iTab )
{
    Tabs.Remove( iTab );
}

void
FArianeEditor::AddTab( TSharedPtr<FArianeEditorTab> iTab)
{
    Tabs.Add( iTab );
}

void
FArianeEditor::InitTabs()
{
    AddTab( MakeShared<FArianeEditorColorSelectorTab>(this) );
    AddTab( MakeShared<FArianeEditorLayerStackTab>(this) );
    AddTab( MakeShared<FArianeEditorSceneTreeViewTab>(this) );

    for (const TSharedPtr<FArianeEditorTab> tab : Tabs)
    {
        tab->Init();
    }
}

const TArray<TSharedPtr<FArianeEditorTab>>&
FArianeEditor::GetTabs() const
{
    return Tabs;
}

void
FArianeEditor::CloseAllTabs()
{
    for (TSharedPtr<FArianeEditorTab> tab : Tabs)
    {
        if ( tab->IsOpen() )
        {
            tab->Close();
        }
    }
}

void
FArianeEditor::RegisterTabSpawners()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
    TSharedPtr<FTabManager> TabManager = LevelEditorModule.GetLevelEditorTabManager()->AsShared();
    TSharedPtr<FWorkspaceItem> workspaceMenuCategory = TabManager->AddLocalWorkspaceMenuCategory(FText::FromName( Name ));
    TSharedRef<FWorkspaceItem> workspaceMenuCategoryRef = workspaceMenuCategory.ToSharedRef();

    for ( TSharedPtr<FArianeEditorTab> tab : Tabs )
    {
        tab->Register( workspaceMenuCategoryRef );
    }
}

void
FArianeEditor::UnregisterTabSpawners()
{
    for ( TSharedPtr<FArianeEditorTab> tab : Tabs )
    {
        tab->Unregister();
    }
}

// --------------------------

const FName&
FArianeEditor::GetId() const
{
    return Name;
}

bool
FArianeEditor::GetCursor( EMouseCursor::Type& OutCursor )
{
    UArianeEditorTool* CurrentTool = GetCurrentTool();

    if( CurrentTool )
    {
        return CurrentTool->GetCursor( OutCursor );
    }

    return false;
}

void
FArianeEditor::Tick( float DeltaTime )
{
    UArianePainting3DComponent* Painting3DComponent = GetCurrentPainting3DComponent();
    uint64 UniqueKey = (uint64)this;

    if ( Painting3DComponent == nullptr )
    {
        // Display an error message on the viewport
        GEngine->AddOnScreenDebugMessage ( UniqueKey // use the pointer as the ID
                                         , 0.1f // Short duration
                                         , FColor::Red
                                         , LOCTEXT("ariane-editor.no-actor-selected","No Painting3D Actor selected").ToString() );
    }
    else
    {
        UArianeLayer* CurrentLayer = Painting3DComponent->GetLayerStack()->GetCurrentLayer();

        if( CurrentLayer && CurrentLayer->IsLocked( true ) )
        {
            // Display an error message on the viewport
            GEngine->AddOnScreenDebugMessage ( UniqueKey // use the pointer as the ID
                                             , 0.1f // Short duration
                                             , FColor::Red
                                             , LOCTEXT("ariane-editor.layer-locked","The selected layer is locked").ToString() );
        }
    }
}

void
FArianeEditor::Init()
{
    InitTabs();
}

UWorld*
FArianeEditor::GetWorld()
{
    return Toolkit->GetEditorMode()->GetWorld();
}

AArianePainting3DActor*
FArianeEditor::AddPainting3DActor()
{
    const UArianeEditorSettings* Settings = GetDefault<UArianeEditorSettings>();
    FEditorViewportClient* ViewportClient = static_cast<FEditorViewportClient*>(GEditor->GetActiveViewport()->GetClient());
    FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( ViewportClient->Viewport
                                                                            , ViewportClient->GetScene()
                                                                            , ViewportClient->EngineShowFlags ) );
    // Note: View is not allocated, it will be destroyed by Unreal at the end of the scope
    FSceneView* View = ViewportClient->CalcSceneView( &ViewFamily );
    // Create the actor at some distance away from the camera
    FVector Location = View ? View->ViewLocation + ( View->GetViewDirection() * Settings->GetDistanceToNewActor() )
                            : FVector();

    GetWorld()->MarkPackageDirty();

    return Cast<AArianePainting3DActor>(GetWorld()->SpawnActor( AArianePainting3DActor::StaticClass(), &Location ) );
}

void
FArianeEditor::AddPainting3DComponent( const TArray<class AActor *> iActors )
{
    for( AActor* actor : iActors )
    {
        actor->AddComponentByClass( UArianePainting3DComponent::StaticClass(), false, FTransform(), false );
    }
}

FSimpleDelegate&
FArianeEditor::OnRegenerateToolbarAndMenusDelegate()
{
    return OnRegenerateToolbarAndMenus;
}

void
FArianeEditor::OnEditorSelectionChanged( UObject* NewSelection )
{
    USelection* SelectionSet = Cast<USelection>(NewSelection);

    OnPre3DPaintingComponentSelectionChanged.Broadcast();

    CurrentPainting3DComponent = nullptr;

    if( SelectionSet )
    {
        TArray<UObject*> SelectedObjects;

        SelectionSet->GetSelectedObjects( SelectedObjects );

        if( SelectedObjects.Num() )
        {
            for( UObject* SelectedObject : SelectedObjects )
            {
                // Filter calls to the world we are interested in, because USelection::SelectionChangedEvent is a static
                // delegate and could be called by other Editor Tabs.
                if( SelectedObject->GetWorld() == GetWorld() )
                {
                    if( SelectedObject->GetClass() == AArianePainting3DActor::StaticClass() )
                    {
                        AArianePainting3DActor* Painting3DActor = Cast<AArianePainting3DActor>(SelectedObject);

                        CurrentPainting3DComponent = Cast<UArianePainting3DComponent>( Painting3DActor->GetComponentByClass( UArianePainting3DComponent::StaticClass() ) );
                    }

                    if( SelectedObject->GetClass() == UArianePainting3DComponent::StaticClass() )
                    {
                        CurrentPainting3DComponent = Cast<UArianePainting3DComponent>( SelectedObject );
                    }
                }
            }
        }
    }

    // allow the component to communicate with the Editor to retrieve Environnement Settings
    if( CurrentPainting3DComponent.Get() )
    {
        CurrentPainting3DComponent->SetEditorInterface( this );
    }

    OnPost3DPaintingComponentSelectionChanged.Broadcast();
}

void
FArianeEditor::SetCurrentPainting3DComponent( UArianePainting3DComponent* InPainting3DComponent )
{
    OnPre3DPaintingComponentSelectionChanged.Broadcast();

    CurrentPainting3DComponent = InPainting3DComponent;

    OnPost3DPaintingComponentSelectionChanged.Broadcast();
}

FColor
FArianeEditor::GetHUDForegroundColor()
{
    const UArianeEditorSettings* Settings = GetDefault<UArianeEditorSettings>();

    return Settings->GetHUDForegroundColor();
}

UArianePainting3DComponent*
FArianeEditor::GetCurrentPainting3DComponent()
{
    return CurrentPainting3DComponent.Get();
}

FArianeEditor::FOnCurrentToolChanged&
FArianeEditor::OnPreCurrentToolChangedDelegate()
{
    return OnPreCurrentToolChanged;
}

FArianeEditor::FOnCurrentToolChanged&
FArianeEditor::OnPostCurrentToolChangedDelegate()
{
    return OnPostCurrentToolChanged;
}

FArianeEditor::FOn3DPaintingComponentSelectionChanged&
FArianeEditor::OnPre3DPaintingComponentSelectionChangedDelegate()
{
    return OnPre3DPaintingComponentSelectionChanged;
}

FArianeEditor::FOn3DPaintingComponentSelectionChanged&
FArianeEditor::OnPost3DPaintingComponentSelectionChangedDelegate()
{
    return OnPost3DPaintingComponentSelectionChanged;
}

::ULIS::FColor
FArianeEditor::GetPaintColor()
{
    return PaintColor;
}

void
FArianeEditor::SetPaintColor( const ::ULIS::FColor& InPaintColor )
{
    PaintColor = InPaintColor;
}

EOdysseyPainterEditorColorType
FArianeEditor::GetColorType()
{
    return ColorType;
}

void
FArianeEditor::SetColorType( EOdysseyPainterEditorColorType& InColorType )
{
    ColorType = InColorType;
}

void
FArianeEditor::AddReferencedObjects( FReferenceCollector& Collector )
{

}

FString
FArianeEditor::GetReferencerName() const
{
    return "FArianeEditor";
}

void
FArianeEditor::UngroupSelectedGroups()
{
    UArianePainting3DComponent* Painting3DComponent = GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( DrawingLayer )
        {
            FArianeGroup* RootGroup = DrawingLayer->GetRootGroup();

            // for undos in case a transaction is opened by the caller
            DrawingLayer->Modify();

            for( FArianeObject* SelectedObject : DrawingLayer->GetSelectedObjects() )
            {
                if( SelectedObject != RootGroup )
                {
                    if( SelectedObject->GetClass() == FArianeGroup::StaticClass() )
                    {
                        FArianeGroup* SelectedGroup = static_cast<FArianeGroup*>(SelectedObject);
                        // we work on a copy of the array because the hierarchy will be modified
                        TArray<FArianeObjectID> ChildrenIDs = SelectedGroup->GetChildren();

                        for( FArianeObjectID& ChildID : ChildrenIDs )
                        {
                            FArianeObject* Child = ChildID.GetObject();

                            SelectedGroup->GetParent()->TransferChild( Child, nullptr );
                        }
                    }
                }
            }

            Painting3DComponent->Update( false );
        }
    }
}

void
FArianeEditor::GroupSelectedObjects( const FName& NewGroupName )
{
    UArianePainting3DComponent* Painting3DComponent = GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( DrawingLayer )
        {
            // for undos in case a transaction is opened by the caller
            DrawingLayer->Modify();

            FArianeGroup* RootGroup = DrawingLayer->GetRootGroup();
            FArianeObject* FosterParent = RootGroup;
            FArianeGroup* NewGroup = DrawingLayer->AllocGroup( NewGroupName, EArianeAllocationModel::InstancedStruct );
            TArray<FArianeObject*> ObjectsToRegroup;

            if( RootGroup->IsSelected() == false )
            {
                DrawingLayer->GetSelectedTrees( ObjectsToRegroup );

                // Check if they all belong to the same parent
                if( ObjectsToRegroup.Num() >= 2 )
                {
                    FosterParent = ObjectsToRegroup[0]->GetParent();

                    for( int32 i = 1; i < ObjectsToRegroup.Num(); i++ )
                    {
                        FArianeObject* SelectedObject = ObjectsToRegroup[i];

                        if( SelectedObject->GetParent() != FosterParent )
                        {
                            // if they don't belong to the same parent, use the RootGroup as the new group's parent
                            FosterParent = RootGroup;

                            break;
                        }
                    }
                }

                // Final step
                FosterParent->AppendChild( NewGroup );

                for( FArianeObject* ObjectToRegroup : ObjectsToRegroup )
                {
                    NewGroup->TransferChild( ObjectToRegroup, nullptr );
                }

                Painting3DComponent->Update( false );
            }
        }
    }
}

void
FArianeEditor::DeleteSelectedObjects()
{
    UArianePainting3DComponent* Painting3DComponent = GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( DrawingLayer )
        {
            FArianeGroup* RootGroup = DrawingLayer->GetRootGroup();
            TArray<FArianeObject*> ObjectsToDelete;

            // for undos in case a transaction is opened by the caller
            DrawingLayer->Modify();

            if( RootGroup->IsSelected() == false )
            {
                DrawingLayer->GetSelectedTrees( ObjectsToDelete );

                // We need to clear the selection because ObjectsToDelete may not contain all selected objects
                DrawingLayer->ClearObjectSelection();

                for( FArianeObject* ObjectToDelete : ObjectsToDelete )
                {
                    ObjectToDelete->GetParent()->RemoveChild( ObjectToDelete, true );
                }

                Painting3DComponent->Update( false );
            }
        }
    }
}

void
FArianeEditor::ConvertSelectedPrimitives()
{
    UArianePainting3DComponent* Painting3DComponent = GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( DrawingLayer )
        {
            TArray<FArianePrimitive*> PrimitivesToConvert;

            PrimitivesToConvert.Reserve( DrawingLayer->GetSelectedObjects().Num() );

            for( FArianeObject* SelectedObject : DrawingLayer->GetSelectedObjects() )
            {
                if( SelectedObject->HasBaseClass( FArianePrimitive::StaticClass() ) )
                {
                    FArianePrimitive* SelectedPrimitive = static_cast<FArianePrimitive*>(SelectedObject);

                    PrimitivesToConvert.Add( SelectedPrimitive );
                }
            }

            for( FArianePrimitive* SelectedPrimitive : PrimitivesToConvert )
            {
                TArray<FArianeObjectID> Children;
                FArianePath* Path;

                Children = SelectedPrimitive->GetChildren();

                // first step: remove the children first or else the conversion will convert all children primitives
                // even the one that were not selected
                for( FArianeObjectID& ChildID : Children )
                {
                    SelectedPrimitive->RemoveChild( ChildID.GetObject(), false ); // remove but do not free the child
                }

                // second step: convert
                Path = SelectedPrimitive->Convert( FArianePrimitive::EConversionFlags::Bezier );

                // third step: append children
                for( FArianeObjectID& ChildID : Children )
                {
                    Path->AppendChild( ChildID.GetObject() );
                }

                // final step: append the converted path and remove the primitive
                SelectedPrimitive->GetParent()->AppendChild( Path );

                Path->UpdateTransform();

                SelectedPrimitive->GetParent()->RemoveChild( SelectedPrimitive, true );
            }

            Painting3DComponent->Update( false );
        }
    }
}

void
FArianeEditor::CopySelectedObjects()
{
    UArianePainting3DComponent* Painting3DComponent = GetCurrentPainting3DComponent();
    FClipboard& Clipboard = GetClipboard();

    if( Painting3DComponent )
    {
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( DrawingLayer )
        {
            TArray<FArianeObject*> ObjectsToCopy;

            DrawingLayer->GetSelectedTrees( ObjectsToCopy );

            if( ObjectsToCopy.Num() )
            {
                FArianeObject::FCopyArgs CopyArgs;

                CopyArgs.DrawingLayer = DrawingLayer;
                CopyArgs.AllocationModel = EArianeAllocationModel::OperatingSystem;

                // free copied objects
                for( FArianeObject* ObjectToCopy : Clipboard.CopiedObjects )
                {
                    delete ObjectToCopy;
                }

                Clipboard.CopiedObjects.Empty();

                for( FArianeObject* ObjectToCopy : ObjectsToCopy )
                {
                    // We alloc with new because we don't want this object to be saved by the serialization
                    // that saves all objects allocated as instanced structs. We want independent objects to store in
                    // a clipboard
                    Clipboard.CopiedObjects.Add ( ObjectToCopy->Copy( CopyArgs ) );
                }
            }
        }
    }
}

void
FArianeEditor::PasteObjects()
{
    UArianePainting3DComponent* Painting3DComponent = GetCurrentPainting3DComponent();
    FClipboard& Clipboard = GetClipboard();

    if( Painting3DComponent )
    {
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( DrawingLayer )
        {
            if( Clipboard.CopiedObjects.Num() )
            {
                TArray<FArianeObject*> SelectedTrees;
                FArianeObject* Destination = DrawingLayer->GetRootGroup();
                FArianeObject::FCopyArgs CopyArgs = FArianeObject::FCopyArgs();

                // ECopyFlags::AllocByLayer means the allocation will be made as FInstancedstruct, thus saved by
                // Unreal Engine's serialization
                CopyArgs.Flags = FArianeObject::ECopyFlags::Rename;
                CopyArgs.AllocationModel = EArianeAllocationModel::InstancedStruct;
                CopyArgs.DrawingLayer = DrawingLayer;

                DrawingLayer->GetSelectedTrees( SelectedTrees );

                Destination = ( SelectedTrees.Num() == 1 ) ? SelectedTrees[0]
                                                           : DrawingLayer->GetRootGroup();

                // for undos in case a transaction is opened by the caller
                DrawingLayer->Modify();

                DrawingLayer->ClearObjectSelection();

                for( FArianeObject* CopiedObject : Clipboard.CopiedObjects )
                {
                    FArianeObject* PasteObject = CopiedObject->Copy( CopyArgs );

                    Destination->AppendChild( PasteObject );

                    DrawingLayer->SelectObject( PasteObject );
                }

                Painting3DComponent->Update( false );
            }
        }
    }
}

void
FArianeEditor::CopySelectedLayers()
{
    UArianePainting3DComponent* Painting3DComponent = GetCurrentPainting3DComponent();
    FClipboard& Clipboard = GetClipboard();

    if( Painting3DComponent )
    {
        UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
        TArray<UArianeLayer*> SelectedTrees;

        LayerStack->GetSelectedTrees( SelectedTrees );

        Clipboard.CopiedLayers.Empty();

        for( UArianeLayer* Layer : SelectedTrees )
        {
            UArianeLayer* LayerCopy = DuplicateObject<UArianeLayer>(Layer, GetTransientPackage());

            Clipboard.CopiedLayers.Add( LayerCopy );
        }

        //Painting3DComponent.Get()->CopySelectedLayers();
    }
}

void
FArianeEditor::PasteLayers()
{
    UArianePainting3DComponent* Painting3DComponent = GetCurrentPainting3DComponent();
    FClipboard& Clipboard = GetClipboard();

    if( Painting3DComponent )
    {
        UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
        TArray<UArianeLayer*> SelectedTrees;
        TArray<UArianeLayer*> LayerCopies;
        UArianeLayerFolder* ReceiverFolder = LayerStack->GetRootFolder();

        LayerStack->GetSelectedTrees( SelectedTrees );

        if( SelectedTrees.Num() == 1 )
        {
            UArianeLayer* SelectedLayer = SelectedTrees[0];
            UArianeLayerFolder* SelectedFolder = Cast<UArianeLayerFolder>(SelectedLayer);

            if( SelectedFolder )
            {
                ReceiverFolder = SelectedFolder;
            }
        }

        for( UArianeLayer* Layer : Clipboard.CopiedLayers )
        {
                                                                           // The outer must be the AActor or else the TEDS system could crash
            UArianeLayer* LayerCopy = DuplicateObject<UArianeLayer>( Layer, Painting3DComponent->GetOwner() );

            LayerCopy->RegisterComponent();

            LayerCopies.Add( LayerCopy );
        }

        LayerStack->AddLayers( ReceiverFolder, LayerCopies, true );

        Painting3DComponent->Update( false );
    }
}

#undef LOCTEXT_NAMESPACE
