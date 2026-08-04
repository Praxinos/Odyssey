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
}

UArianeEditorPaintBucketTool::UArianeEditorPaintBucketTool()
{
    Icon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsTab.PaintBucket64");

    bHasContextMenu = true;
}

void
UArianeEditorPaintBucketTool::Init( FArianeEditor* InEditor )
{
    Super::Init( InEditor );
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
            UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
            UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>( LayerStack->GetCurrentLayer() );

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

                (new FArianeGraph())->Build( CameraLocation, DrawingPlane, GraphedPaths );
            }
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
