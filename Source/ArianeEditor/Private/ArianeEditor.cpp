// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeEditor.h"
#include "ArianeEditorTool.h"
#include "ArianeEditorViewportToolkit.h"
#include "ArianeEditorToolTab.h"
#include "ArianePainting3DComponent.h"
#include "ArianeLayerStack.h"
#include "ArianeLayerDrawing.h"
#include "ArianePainting3DActor.h"
#include "PathDrawingTool/ArianeEditorPathDrawingTool.h"
#include "EraserTool/ArianeEditorEraserTool.h"
#include "LayerTransformTool/ArianeEditorLayerTransformTool.h"
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

#define LOCTEXT_NAMESPACE "ArianeEditor"

FArianeEditor::~FArianeEditor()
{
    USelection::SelectionChangedEvent.RemoveAll( this );
}

FArianeEditor::FArianeEditor( FArianeEditorViewportToolkit* iToolkit )
    : Toolkit( iToolkit )
    , Name("ArianeEditor")
    , ColorType ( EOdysseyPainterEditorColorType::Raw )
{
    // Component selection is managed by ArianeEditor in order to emulate a Pre/Post Selection event behavior
    USelection::SelectionChangedEvent.AddRaw( this, &FArianeEditor::OnEditorSelectionChanged );

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

void
FArianeEditor::ClearPainting3DComponents()
{
    UEditorActorSubsystem* editorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();

    for( AActor* actor : editorActorSubsystem->GetSelectedLevelActors() )
    {
        UArianePainting3DComponent* painting3DComponent = Cast<UArianePainting3DComponent>(actor->GetComponentByClass( UArianePainting3DComponent::StaticClass() ));

        if( painting3DComponent )
        {
            UArianeLayerDrawing* CurrentDrawingLayer = painting3DComponent->GetLayerStack()->GetFirstSelectedDrawingLayer();

            if( CurrentDrawingLayer )
            {
                if( GEditor->IsTransactionActive() )
                    CurrentDrawingLayer->Modify();

                CurrentDrawingLayer->ResetHierarchy();
            }
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

/*
    UOdysseyPainterEditorTool* currentTool = GetCurrentTool();
    if (currentTool)
    {
        currentTool->ExtendToolbar(iToolMenu);
    }
*/
}

void
FArianeEditor::ExtendLevelEditorToolbar( UToolMenu* iToolbar )
{
    //mToolbarMenuName = iToolbar->GetMenuName();

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
    // Note: EToolSide::Left means the mouse
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
    // the tool collection is ready for the tool selector widget
    UArianeEditorTool* Tool = ToolBuilder->CreateTool( GetToolManager() );

    Tool->Init( this );

    Tools.Add( Tool );

    GetToolManager()->RegisterToolType( ToolBuilder->GetType(), ToolBuilder );
}

void
FArianeEditor::RegisterTools()
{
    AddToolBuilder( NewObject<UArianeEditorPathDrawingToolBuilder>() );
    AddToolBuilder( NewObject<UArianeEditorEraserToolBuilder>() );
    AddToolBuilder( NewObject<UArianeEditorLayerTransformToolBuilder>() );

    SetCurrentTool( Tools[0], EToolShutdownType::Accept, true );
}

void FArianeEditor::UnregisterTools()
{
    TObjectPtr<UInteractiveToolManager> ToolManager = GetToolManager();

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
    AddTab( MakeShared<FArianeEditorToolTab>(this) );

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
    return Cast<AArianePainting3DActor>(GetWorld()->SpawnActor( AArianePainting3DActor::StaticClass() ));
}

void
FArianeEditor::AddPainting3DComponent( const TArray<class AActor *> iActors )
{
    for( AActor* actor : iActors )
    {
        actor->AddComponentByClass( UArianePainting3DComponent::StaticClass(), false, FTransform(), false );
    }
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

    OnPost3DPaintingComponentSelectionChanged.Broadcast();
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

#undef LOCTEXT_NAMESPACE
