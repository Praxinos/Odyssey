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
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Transform64");

    bHasContextMenu = true;
}

void
UArianeEditorLayerTransformTool::Init( FArianeEditor* InEditor )
{
    Super::Init( InEditor );

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

    Gizmo = GizmoManager->CreateCustomTransformGizmo( ETransformGizmoSubElements::StandardTranslateRotate
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

    CreateGizmo();
    //TransformProxy->OnTransformChanged.AddUObject( this, &UArianeEditorLayerTransformTool::OnTransformChanged )
    BindDelegates();
}

void
UArianeEditorLayerTransformTool::Inactivate()
{
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

#undef LOCTEXT_NAMESPACE
