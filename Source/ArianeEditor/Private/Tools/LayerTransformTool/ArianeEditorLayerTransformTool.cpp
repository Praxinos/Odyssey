// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "LayerTransformTool/ArianeEditorLayerTransformTool.h"
#include "ArianeEditor.h"
#include "ArianePainting3DComponent.h"
#include "ArianeLayerStack.h"
#include "ArianeLayerDrawing.h"
// Odyssey
#include "OdysseyStyle.h"
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
{
    Icon = FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Transform64");

    bHasContextMenu = true;
}

void
UArianeEditorLayerTransformTool::Init( FArianeEditor* InEditor )
{
    Super::Init( InEditor );

    CreateOverlayWidget();
}

void
UArianeEditorLayerTransformTool::BindDelegates()
{
    UArianeLayerStack* LayerStack = Editor->GetCurrentPainting3DComponent()->GetLayerStack();

    LayerStack->OnPreCurrentLayerChangedDelegate().AddUObject( this, &UArianeEditorLayerTransformTool::ClearGizmo );
    LayerStack->OnPostCurrentLayerChangedDelegate().AddUObject( this, &UArianeEditorLayerTransformTool::CreateGizmo );
}

void
UArianeEditorLayerTransformTool::UnbindDelegates()
{
    UArianeLayerStack* LayerStack = Editor->GetCurrentPainting3DComponent()->GetLayerStack();

    LayerStack->OnPreCurrentLayerChangedDelegate().RemoveAll( this );
    LayerStack->OnPostCurrentLayerChangedDelegate().RemoveAll( this );
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
}

void
UArianeEditorLayerTransformTool::CreateGizmo()
{
    const TArray<UArianeLayer*>& SelectedLayers = Editor->GetCurrentPainting3DComponent()->GetLayerStack()->GetSelectedLayers();
    UInteractiveGizmoManager* GizmoManager = GetToolManager()->GetPairedGizmoManager();

    // We create a new proxy because we cannot empty it from its components. The old one will be Garbage Collected... I guess.
    TransformProxy = NewObject<UTransformProxy>(this);

    Gizmo = GizmoManager->CreateCustomTransformGizmo( ETransformGizmoSubElements::FullTranslateRotateScale
                                                    , TransformProxy
                                                    , TEXT("ArianeLayerTransform"));

    for( UArianeLayer* SelectedLayer : SelectedLayers )
    {
        TransformProxy->AddComponent( SelectedLayer );
    }

    Gizmo->SetActiveTarget( TransformProxy );
}

void
UArianeEditorLayerTransformTool::Activate()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");

    CreateGizmo();
    //TransformProxy->OnTransformChanged.AddUObject( this, &UArianeEditorLayerTransformTool::OnTransformChanged )
    BindDelegates();
    // display the overlay widget
    LevelEditorModule.GetFirstLevelEditor()->AddViewportOverlayWidget( OverlayWidget.ToSharedRef() );
}

void
UArianeEditorLayerTransformTool::Inactivate()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
    // Remove the overlay widget
    LevelEditorModule.GetFirstLevelEditor()->RemoveViewportOverlayWidget( OverlayWidget.ToSharedRef() );

    UnbindDelegates();

    ClearGizmo();
}

/*
void
UArianeEditorLayerTransformTool::OnTransformChanged( UTransformProxy* Proxy, FTransform NewTransform )
{
}
*/

bool
UArianeEditorLayerTransformTool::OnMouseDown( FEditorViewportClient* iViewportClient
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
                                             , const FArianePointerState& State )
{

}

bool
UArianeEditorLayerTransformTool::OnMouseDrag( FEditorViewportClient* iViewportClient
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

void
UArianeEditorLayerTransformTool::DrawHUD ( FEditorViewportClient* ViewportClient
                                         , FViewport* Viewport
                                         , const FSceneView* View
                                         , FCanvas* HUDCanvas )
{
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
    return FReply::Handled();
}

FReply
UArianeEditorLayerTransformTool::OnCancel()
{
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
