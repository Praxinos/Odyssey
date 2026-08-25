// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor headers
#include "LayerTransformTool/ArianeEditorLayerTransformTool.h"
#include "ArianeEditor.h"
#include "ArianeEditorStyle.h"
// Ariane headers
#include "ArianePainting3DComponent.h"
#include "ArianeLayerStack.h"
#include "ArianeLayerDrawing.h"
#include "ArianeLayerFolder.h"

// Unreal headers
#include "Subsystems/EditorActorSubsystem.h"
#include "SceneView.h"
#include "IStylusState.h"
#include "BaseGizmos/TransformProxy.h"
#include "BaseGizmos/CombinedTransformGizmo.h"
#include "Widgets/SCompoundWidget.h"
#include "LevelEditor.h"
#include "Editor/Transactor.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

UArianeEditorLayerTransformTool::~UArianeEditorLayerTransformTool()
{
    //UInteractiveGizmoManager* GizmoManager = GetToolManager()->GetPairedGizmoManager();

    //GizmoManager->DestroyAllGizmosByOwner(this);
}

UArianeEditorLayerTransformTool::UArianeEditorLayerTransformTool()
    : TransformProxy ( nullptr )
    , Gizmo ( nullptr )
    , PreviousTool ( nullptr )
{
    Icon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsTab.Transform64");

    bHasContextMenu = true;
}

void
UArianeEditorLayerTransformTool::Init( FArianeEditor* InEditor )
{
    Super::Init( InEditor );

    CreateOverlayWidget();
}

void
UArianeEditorLayerTransformTool::BindComponentDelegates()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();

        //LayerStack->OnPreSelectionChangedDelegate().AddUObject( this, &UArianeEditorLayerTransformTool::ClearGizmo );
        LayerStack->OnPostSelectionChangedDelegate().AddUObject( this, &UArianeEditorLayerTransformTool::ResetGizmo );

        // Refresh the tool's Gizmo when the layer Transform is updated (e.g via a widget)
        Painting3DComponent->OnPostUpdateDelegate().AddUObject( this, &UArianeEditorLayerTransformTool::OnRootFolderUpdate );
    }
}

void
UArianeEditorLayerTransformTool::OnRootFolderUpdate( bool Interactive )
{
   if( Interactive == false )
   {
       ResetGizmo();
   }
}

void
UArianeEditorLayerTransformTool::OnPre3DPaintingComponentSelection()
{
    ClearGizmo();

    UnbindComponentDelegates();
}

void
UArianeEditorLayerTransformTool::OnPost3DPaintingComponentSelection()
{
    BindComponentDelegates();

    CreateGizmo();
}

void
UArianeEditorLayerTransformTool::UnbindComponentDelegates()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();

        //LayerStack->OnPreSelectionChangedDelegate().RemoveAll( this );
        LayerStack->OnPostSelectionChangedDelegate().RemoveAll( this );

        Painting3DComponent->OnPostUpdateDelegate().RemoveAll( this );
    }
}

void
UArianeEditorLayerTransformTool::ResetGizmo()
{
    ClearGizmo();
    CreateGizmo();
}

void
UArianeEditorLayerTransformTool::ClearGizmo()
{
    UInteractiveGizmoManager* GizmoManager = GetToolManager()->GetPairedGizmoManager();

    if( Gizmo )
    {
        GizmoManager->DestroyGizmo( Gizmo );

        Gizmo = nullptr;
    }

    TransformSnapshots.Empty();
}

void
UArianeEditorLayerTransformTool::CreateGizmo()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        const TArray<UArianeLayer*>& SelectedLayers = Painting3DComponent->GetLayerStack()->GetSelectedLayers();
        UInteractiveGizmoManager* GizmoManager = GetToolManager()->GetPairedGizmoManager();

        // We create a new proxy because we cannot empty it from its components. The old one will be Garbage Collected... I guess.
        TransformProxy = NewObject<UTransformProxy>(this);

        Gizmo = GizmoManager->CreateCustomTransformGizmo( ETransformGizmoSubElements::FullTranslateRotateScale
                                                        , TransformProxy
                                                        , TEXT("ArianeLayerTransform"));

        TransformSnapshots.Empty();
        TransformSnapshots.Reserve( SelectedLayers.Num() );

        for( UArianeLayer* SelectedLayer : SelectedLayers )
        {
            TransformSnapshots.Emplace( SelectedLayer, SelectedLayer->GetRelativeTransform() );
            TransformProxy->AddComponent( SelectedLayer );
        }

        Gizmo->SetActiveTarget( TransformProxy );
    }
}

void
UArianeEditorLayerTransformTool::Activate()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
    TSharedPtr<ILevelEditor> LevelEditor = LevelEditorModule.GetFirstLevelEditor();

    // this must not be put in BindComponentDelegates
    Editor->OnPre3DPaintingComponentSelectionChangedDelegate().AddUObject( this, &UArianeEditorLayerTransformTool::OnPre3DPaintingComponentSelection );
    Editor->OnPost3DPaintingComponentSelectionChangedDelegate().AddUObject( this, &UArianeEditorLayerTransformTool::OnPost3DPaintingComponentSelection );

    CreateGizmo();
    //TransformProxy->OnTransformChanged.AddUObject( this, &UArianeEditorLayerTransformTool::OnTransformChanged )
    BindComponentDelegates();

    // display the overlay widget

    // LevelEditor can be null when closing the editor (closing it will inactivate the tool).
    if( LevelEditor )
    {
        LevelEditor->AddViewportOverlayWidget( OverlayWidget.ToSharedRef() );
    }
}

void
UArianeEditorLayerTransformTool::Inactivate()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
    // Remove the overlay widget
    TSharedPtr<ILevelEditor> LevelEditor = LevelEditorModule.GetFirstLevelEditor();

    // LevelEditor can be null when closing the editor (closing it will inactivate the tool).
    if( LevelEditor )
    {
        LevelEditor->RemoveViewportOverlayWidget( OverlayWidget.ToSharedRef() );
    }

    UnbindComponentDelegates();

    ClearGizmo();

    // this must not be put in UnbindComponentDelegates
    Editor->OnPre3DPaintingComponentSelectionChangedDelegate().RemoveAll( this );
    Editor->OnPost3DPaintingComponentSelectionChangedDelegate().RemoveAll( this );

}

/*
void
UArianeEditorLayerTransformTool::OnTransformChanged( UTransformProxy* Proxy, FTransform NewTransform )
{
}
*/

bool
UArianeEditorLayerTransformTool::OnMouseDown( FEditorViewportClient* iViewportClient
                                            , FSceneView* View
                                            , const FKey& iKey
                                            , const FArianePointerState& PointerState
                                            , bool iRepeat )
{
    if( iKey == EKeys::LeftMouseButton )
    {
        return true;
    }

    return false;
}

void
UArianeEditorLayerTransformTool::OnMouseHover( FEditorViewportClient* iViewportClient
                                             , FSceneView* View
                                             , const FArianePointerState& State )
{

}

bool
UArianeEditorLayerTransformTool::OnMouseDrag( FEditorViewportClient* iViewportClient
                                            , FSceneView* View
                                            , const FKey& iKey
                                            , const FArianePointerState& PointerState )
{
    if( iViewportClient->Viewport->KeyState( EKeys::LeftMouseButton ) )
    {
    }

    return false;
}

bool
UArianeEditorLayerTransformTool::OnMouseUp( FEditorViewportClient* ViewportClient
                                          , FSceneView* View
                                          , const FKey& iKey
                                          , const FArianePointerState& PointerState )
{
    if( iKey == EKeys::LeftMouseButton )
    {
        return true;
    }

    if( iKey == EKeys::RightMouseButton )
    {
        return false;
    }

    return false;
}

void
UArianeEditorLayerTransformTool::ExtendContextMenu( FMenuBuilder& menu )
{
}

void UArianeEditorLayerTransformTool::SetPreviousTool( UArianeEditorTool* InPreviousTool )
{
    PreviousTool = InPreviousTool;
}

void
UArianeEditorLayerTransformTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    FName PropertyName = PropertyChangedEvent.GetPropertyName();
    FName MemberPropertyName = PropertyChangedEvent.GetMemberPropertyName();

/*
    if( PropertyName == GET_MEMBER_NAME_CHECKED( UArianeEditorLayerTransformTool, Size ) )
    {

    }
*/
}

void
UArianeEditorLayerTransformTool::CreateOverlayWidget()
{
    static FSlateRoundedBoxBrush RoundedBrush = FSlateRoundedBoxBrush( FLinearColor(0.01f, 0.01f, 0.01f, 0.8f)
                                                                     , 4.0f
                                                                     , FLinearColor(0.01f, 0.01f, 0.01f, 0.8f)
                                                                     , 1.0f );

    OverlayWidget = SNew(SOverlay)
                    +SOverlay::Slot()
                    .VAlign( VAlign_Bottom ) // Position at the viewport's bottom
                    .HAlign( HAlign_Center )
                    .Padding( FMargin(0, 0, 0, 20 ) ) // 40 pixels margin to the Viewport's bottom
                    [
                        SNew(SBorder)
                        .BorderImage(&RoundedBrush)
                        //.BorderImage(FAppStyle::GetBrush("RoundedSelectionBackground"))
                        //.BorderBackgroundColor(FLinearColor(0.02f, 0.02f, 0.02f, 0.7f))
                        //.Padding(FMargin(12, 8))
                        [
                            SNew(SHorizontalBox)
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            .VAlign( VAlign_Center )
                            .Padding(5)
                            [
                                SNew(SImage)
                                .Image( Icon )
                                .DesiredSizeOverride(FVector2D(24.f, 24.f))
                            ]
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            .VAlign( VAlign_Center )
                            .Padding(5)
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("ariane-layer-transform-tool.name", "Layer Transform Tool"))
                            ]
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            .VAlign( VAlign_Center )
                            .Padding(5)
                            [
                                SNew(SButton)
                                .Text(FText::FromString("Accept"))
                                .OnClicked_UObject( this, &UArianeEditorLayerTransformTool::OnAccept )
                                //.ButtonStyle( FAppStyle::Get(), "PrimaryButton")
                            ]
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            .VAlign( VAlign_Center )
                            .Padding(5)
                            [
                                SNew(SButton)
                                .Text(FText::FromString("Cancel"))
                                .OnClicked_UObject( this, &UArianeEditorLayerTransformTool::OnCancel )
                            ]
                        ]
                    ];
}

FReply
UArianeEditorLayerTransformTool::OnAccept()
{
    Editor->SetCurrentTool( PreviousTool, EToolShutdownType::Accept, true );

    return FReply::Handled();
}

FReply
UArianeEditorLayerTransformTool::OnCancel()
{
    // Restore Transforms
    for( FTransformSnapshot& TransformSnapshot : TransformSnapshots )
    {
        TransformSnapshot.Layer->SetRelativeTransform( TransformSnapshot.Transform );
    }

    Editor->SetCurrentTool( PreviousTool, EToolShutdownType::Accept, true );

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
