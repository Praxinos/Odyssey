// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor headers
#include "PaintBucketTool/ArianeEditorPaintBucketTool.h"
#include "ArianeEditor.h"
#include "ArianeEditorStyle.h"
// Ariane headers
#include "ArianePainting3DComponent.h"
#include "ArianePath.h"
#include "ArianeGroup.h"
#include "ArianeGraph.h"
#include "ArianeVertex.h"
#include "ArianeLayerStack.h"
#include "ArianeLayerDrawing.h"
#include "ArianeSegmentCubic.h"

// Unreal headers
#include "Subsystems/EditorActorSubsystem.h"
#include "SceneView.h"
#include "Math/UnrealMathUtility.h"
#include "IStylusState.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Engine/Canvas.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "InteractiveToolManager.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

UArianeEditorPaintBucketTool::~UArianeEditorPaintBucketTool()
{
    delete Graph;
}

UArianeEditorPaintBucketTool::UArianeEditorPaintBucketTool()
    : Graph( new FArianeGraph() )
{
    Icon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsTab.PaintBucket64");

    bHasContextMenu = true;
}

void
UArianeEditorPaintBucketTool::Init( FArianeEditor* InEditor )
{
    Super::Init( InEditor );
}

void
UArianeEditorPaintBucketTool::Activate()
{
    Reset();
}

void
UArianeEditorPaintBucketTool::Reset()
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();
    IToolsContextQueriesAPI* QueriesAPI = GetToolManager()->GetContextQueriesAPI();
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>( LayerStack->GetCurrentLayer() );
        FViewCameraState CameraState;

        QueriesAPI->GetCurrentViewState( CameraState );

        if( DrawingLayer )
        {
            FVector4 DrawingPlane = GetDrawingPlane( ViewportClient, DrawingLayer );
            TArray<FArianeObject*> GraphedPaths;
            FVector CameraLocation = CameraState.Position;

            GraphedPaths.Reserve( DrawingLayer->GetInstancedObjects().Num() );

            DrawingLayer->GetRootGroup()->Traverse( [ &GraphedPaths ]( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
                {
                    GraphedPaths.Add( Object );

                    return FArianeObject::ETraversalReturnValue::Continue;
                } );

            Graph->Build( CameraLocation, DrawingPlane, GraphedPaths );
        }
    }
}

bool
UArianeEditorPaintBucketTool::OnMouseDown( FEditorViewportClient* ViewportClient
                                         , FSceneView* View
                                         , const FKey& iKey
                                         , const FArianePointerState& PointerState
                                         , bool bRepeat )
{
    IToolsContextQueriesAPI* QueriesAPI = GetToolManager()->GetContextQueriesAPI();
    FViewCameraState CameraState;

    QueriesAPI->GetCurrentViewState( CameraState );

    if( iKey == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        if( Painting3DComponent )
        {

        }

        return true;
    }

    return false;
}

void
UArianeEditorPaintBucketTool::OnMouseHover( FEditorViewportClient* iViewportClient
                                          , FSceneView* View
                                          , const FArianePointerState& PointerState )
{
    FVector RayOrigin;
    FVector RayDirection;

    View->DeprojectFVector2D( FVector2D( PointerState.ViewportX, PointerState.ViewportY )
                            , RayOrigin
                            , RayDirection );

    PickedCycle = Graph->PickCycle( RayOrigin, RayDirection );
}

bool
UArianeEditorPaintBucketTool::OnMouseDrag( FEditorViewportClient* iViewportClient
                                         , FSceneView* View
                                         , const FKey& iKey
                                         , const FArianePointerState& PointerState )
{
    if( iKey == EKeys::LeftMouseButton )
    {

    }

    return false;
}

bool
UArianeEditorPaintBucketTool::OnMouseUp( FEditorViewportClient* ViewportClient
                                       , FSceneView* View
                                       , const FKey& iKey
                                       , const FArianePointerState& PointerState )
{
    if( iKey == EKeys::LeftMouseButton )
    {
        //UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        return true;
    }

    if( iKey == EKeys::RightMouseButton )
    {
        return false;
    }

    return false;
}

void
UArianeEditorPaintBucketTool::ExtendContextMenu( FMenuBuilder& menu )
{
}

void
UArianeEditorPaintBucketTool::DrawSectionsHUD ( FCanvas* HUDCanvas
                                              , IToolsContextRenderAPI* RenderAPI
                                              , FEditorViewportClient* ViewportClient
                                              , FSceneView* View
                                              , const FLinearColor& HcColor
                                              , const TArray<FArianeGraph::FSection*>& Sections )
{
    const FPlane& ProjectionPlane =  Graph->GetProjectionPlane();
    FQuat RotationQuat = FQuat::FindBetweenNormals( FVector::UpVector, ProjectionPlane.GetNormal() );
    FVector ProjectionPlaneOrigin = ProjectionPlane.GetOrigin();

    for( FArianeGraph::FSection* ContourSection : Sections )
    {
        if( ContourSection->GetClass() == FArianeGraph::FSectionLinear::StaticClass() )
        {
            FArianeGraph::FSectionLinear* LinearSection = static_cast<FArianeGraph::FSectionLinear*>(ContourSection);
            FVector P0Local = FVector( LinearSection->Nodes[0]->Position.X, LinearSection->Nodes[0]->Position.Y, 0.0f );
            FVector P1Local = FVector( LinearSection->Nodes[1]->Position.X, LinearSection->Nodes[1]->Position.Y, 0.0f );
            FVector P0World = ( RotationQuat * P0Local ) + ProjectionPlaneOrigin;
            FVector P1World = ( RotationQuat * P1Local ) + ProjectionPlaneOrigin;
            FVector2D P0HUD;
            FVector2D P1HUD;

            WorldToHUD( ViewportClient, View, P0World, P0HUD );
            WorldToHUD( ViewportClient, View, P1World, P1HUD );

            DrawLineHUD( HUDCanvas
                        , ViewportClient
                        , View
                        , P0HUD
                        , P1HUD
                        , HcColor
                        , 1.0f );
        }

        //if( ContourSection->GetCycle(0) == this )
        if( ContourSection->GetClass() == FArianeGraph::FSectionCubic::StaticClass() )
        {
            FArianeGraph::FSectionCubic* CubicSection = static_cast<FArianeGraph::FSectionCubic*>(ContourSection);

            FVector2D* SectionBezier = CubicSection->GetBezier();
        }
    }
}

void
UArianeEditorPaintBucketTool::DrawCycleHUD ( FCanvas* HUDCanvas
                                           , IToolsContextRenderAPI* RenderAPI
                                           , FEditorViewportClient* ViewportClient
                                           , FSceneView* View
                                           , const FLinearColor& HcColor
                                           , FArianeGraph::FCycle* Cycle )
{
    DrawSectionsHUD( HUDCanvas, RenderAPI, ViewportClient, View, HcColor, Cycle->GetContourSections() );
    DrawSectionsHUD( HUDCanvas, RenderAPI, ViewportClient, View, HcColor, Cycle->GetInnerSections() );
}

void
UArianeEditorPaintBucketTool::DrawHUD ( FCanvas* HUDCanvas, IToolsContextRenderAPI* RenderAPI )
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();
    FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( ViewportClient->Viewport
                                                                            , ViewportClient->GetScene()
                                                                            , ViewportClient->EngineShowFlags ) );
    // Note: View is not allocated, it will be destroyed by Unreal at the end of the scope
    FSceneView* View = ViewportClient->CalcSceneView( &ViewFamily );
    FLinearColor FgColor = GetForegroundColor();
    FLinearColor BgColor = GetBackgroundColor();
    FLinearColor HcColor = GetHighlightColor();

    if( CanDraw() )
    {
        if( PickedCycle )
        {
            DrawCycleHUD( HUDCanvas, RenderAPI, ViewportClient, View, HcColor, PickedCycle );
        }

        //FVector2D HUDPosition = ScreenToHUD( ViewportClient, MousePosition );


        //DrawCircleHUD ( HUDCanvas, ViewportClient, View, HUDPosition, ( double ) Size * 0.5f, HcColor, 1.0f );
    }
}

void
UArianeEditorPaintBucketTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    FName PropertyName = PropertyChangedEvent.GetPropertyName();
    FName MemberPropertyName = PropertyChangedEvent.GetMemberPropertyName();

    //if( PropertyName == GET_MEMBER_NAME_CHECKED( UArianeEditorPaintBucketTool, Size ) )
    {

    }
}

#undef LOCTEXT_NAMESPACE
