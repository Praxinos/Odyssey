// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor headers
#include "PaintBucketTool/ArianeEditorPaintBucketTool.h"
#include "ArianeEditor.h"
#include "ArianeEditorSettings.h"
#include "ArianeEditorStyle.h"
// Ariane headers
#include "ArianePainting3DComponent.h"
#include "ArianeCycle.h"
#include "ArianeGroup.h"
#include "ArianeGraph.h"
#include "ArianeLayerStack.h"
#include "ArianeDrawing.h"
#include "ArianeLayerVector.h"
#include "ArianeLayerFolder.h"
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
    // the solve ticker captures this. The graph itself is released by its last owner (tool or running task)
    if( SolveTickerHandle.IsValid() )
    {
        FTSTicker::GetCoreTicker().RemoveTicker( SolveTickerHandle );
    }

    if( CameraMoveHandle.IsValid() )
    {
        FTSTicker::GetCoreTicker().RemoveTicker( CameraMoveHandle );
    }
}

UArianeEditorPaintBucketTool::UArianeEditorPaintBucketTool()
    : GapTolerance( 50.0f )
    , SegmentExtensionScheme ( EArianeGraphSegmentExtensionScheme::Single )
    , FittingRule ( EArianeCycleFittingRule::FitToPaths )
    , LayerPolicy ( EArianeEditorPaintBucketToolLayerPolicy::Selected )
    , MaterialInterface( nullptr )
    , Graph( MakeShared<FArianeGraph>() )
    , Worker( Graph )
    , PickedCycle( nullptr )
    , bGraphNeedsUpdate ( true )
    , Generation( 0 )
    , SolvedGeneration( 0 )
    , bGeometryNeedsImport( true )
    , bNeedsPick( false )
    , bIsActive( false )
    , LastCursorViewportPosition( FVector2D::ZeroVector )
{
    Icon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsTab.PaintBucket64");

    bHasContextMenu = true;
}

void
UArianeEditorPaintBucketTool::Init( FArianeEditor* InEditor )
{
    const UArianeEditorSettings* Settings = GetDefault<UArianeEditorSettings>();

    Super::Init( InEditor );

    MaterialInterface = Settings->GetDefaultPathDrawingMaterial();
}

void
UArianeEditorPaintBucketTool::OnCameraMoved( const FVector& Location, const FRotator& Rotation, ELevelViewportType ViewportType, int32 ViewIndex )
{
    // the current graph, or the one being solved, no longer matches the view
    Generation++;
    bGraphNeedsUpdate = true;
    PickedCycle = nullptr;

    if (CameraMoveHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(CameraMoveHandle);
    }

    // solve only once the camera is idle (mousewheel has no begin/end events)
    CameraMoveHandle = FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateLambda([this](float)
        {
            CameraMoveHandle.Reset();

            RequestSolve();

            return false;
        }),
        0.1f // 100 ms
    );
}


void
UArianeEditorPaintBucketTool::OnPreUpdate( bool bInteractive )
{
}

void
UArianeEditorPaintBucketTool::OnPostUpdate( bool bInteractive )
{
    if( bInteractive == false )
    {
        RequestImport();
    }
}

void
UArianeEditorPaintBucketTool::BindDelegates()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    Editor->OnPost3DPaintingComponentSelectionChangedDelegate().AddUObject( this, &UArianeEditorPaintBucketTool::RequestImport );

    if( Painting3DComponent )
    {
        //Painting3DComponent->OnPreUpdateDelegate().AddUObject( this, &UArianeEditorPaintBucketTool::OnPreUpdate );
        Painting3DComponent->OnPostUpdateDelegate().AddUObject( this, &UArianeEditorPaintBucketTool::OnPostUpdate );
        Painting3DComponent->GetLayerStack()->OnPostSelectionChangedDelegate().AddUObject( this, &UArianeEditorPaintBucketTool::RequestImport );
        Painting3DComponent->GetLayerStack()->OnPostHierarchyChangedDelegate().AddUObject( this, &UArianeEditorPaintBucketTool::RequestImport );
    }

    FEditorDelegates::OnEditorCameraMoved.AddUObject( this, &UArianeEditorPaintBucketTool::OnCameraMoved );
}

void
UArianeEditorPaintBucketTool::UnbindDelegates()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    Editor->OnPost3DPaintingComponentSelectionChangedDelegate().RemoveAll( this );

    if( Painting3DComponent )
    {
        //Painting3DComponent->OnPreUpdateDelegate().RemoveAll( this );
        Painting3DComponent->OnPostUpdateDelegate().RemoveAll( this );
        Painting3DComponent->GetLayerStack()->OnPostSelectionChangedDelegate().RemoveAll( this );
        Painting3DComponent->GetLayerStack()->OnPostHierarchyChangedDelegate().RemoveAll( this );
    }

    FEditorDelegates::OnEditorCameraMoved.RemoveAll( this );
}

void
UArianeEditorPaintBucketTool::Activate()
{
    const UArianeEditorSettings* Settings = GetDefault<UArianeEditorSettings>();

    Super::Activate();

    bIsActive = true;

    RequestImport();

    if( MaterialInterface == nullptr )
    {
        MaterialInterface = Settings->GetDefaultPathDrawingMaterial();
    }

    BindDelegates();
}

void
UArianeEditorPaintBucketTool::Inactivate()
{
    UnbindDelegates();

    // any running solve becomes stale
    Generation++;
    bIsActive = false;
    PickedCycle = nullptr;

    // would call RequestSolve() after the tool is gone
    if( CameraMoveHandle.IsValid() )
    {
        FTSTicker::GetCoreTicker().RemoveTicker( CameraMoveHandle );
        CameraMoveHandle.Reset();
    }

    // free the graph now if possible, it is useless while another tool is active
    if( Worker.IsBusy() == false )
    {
        Graph->Reset( EArianeGraphResetPolicy::All );
    }
    // otherwise the graph is in use: the solve ticker keeps running and frees it when the worker is done

    Super::Inactivate();
}

void
UArianeEditorPaintBucketTool::RequestImport()
{
    // the graph is only touched in RequestSolve, when the worker is idle
    Generation++;
    bGraphNeedsUpdate = true;
    bGeometryNeedsImport = true;
    PickedCycle = nullptr;

    RequestSolve();
}

void
UArianeEditorPaintBucketTool::RequestSolve()
{
    // a running solve comes back to OnSolveFinished, which relaunches if needed
    if( Worker.IsBusy() )
    {
        return;
    }

    FEditorViewportClient* ViewportClient = GetActiveViewportClient();
    // the tools context camera state is stale until the user interacts with the viewport, the scene view is not
    FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( ViewportClient->Viewport
                                                                            , ViewportClient->GetScene()
                                                                            , ViewportClient->EngineShowFlags ) );
    // Note: View is not allocated, it will be destroyed by Unreal at the end of the scope
    FSceneView* View = ViewportClient->CalcSceneView( &ViewFamily );

    // game thread, worker idle: the only step that reads the drawing
    if( bGeometryNeedsImport )
    {
        TArray<FArianeObject*> GraphedObjects;

        GatherGraphedObjects( GraphedObjects );

        Graph->ImportGeometry( GraphedObjects, GapTolerance, SegmentExtensionScheme );

        bGeometryNeedsImport = false;
    }

    // points into the cycles the worker is about to delete
    PickedCycle = nullptr;
    SolvedGeneration = Generation;

    Worker.Launch( View->ViewMatrices.GetViewOrigin(), GetDrawingPlane( ViewportClient, nullptr ) );

    StartSolveTicker();
}

void
UArianeEditorPaintBucketTool::StartSolveTicker()
{
    if( SolveTickerHandle.IsValid() )
    {
        return;
    }

    SolveTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateLambda( [ this ]( float )
        {
            // still solving: keep polling every frame
            if( Worker.IsBusy() )
            {
                // the viewport is not realtime, force a redraw so the progress bar animates
                if( bIsActive )
                {
                    GetActiveViewportClient()->Invalidate();
                }

                return true;
            }

            // worker done: the graph can be read or modified again
            SolveTickerHandle.Reset();

            if( bIsActive )
            {
                // use the result, or relaunch if it is stale
                OnSolveFinished();
            }
            else
            {
                // tool was left during the solve: nobody needs the result
                Graph->Reset( EArianeGraphResetPolicy::All );
            }

            // stop ticking
            return false;
        } ) );
}

void
UArianeEditorPaintBucketTool::OnSolveFinished()
{
    if( SolvedGeneration == Generation )
    {
        bGraphNeedsUpdate = false;
        // the cursor may not move: pick at its last position on next DrawHUD
        bNeedsPick = true;

        GetActiveViewportClient()->Invalidate();
    }
    else if( CameraMoveHandle.IsValid() == false )
    {
        // stale and camera idle: relaunch, re-importing if the drawing changed meanwhile
        RequestSolve();
    }
    // stale and camera moving: the camera ticker will call RequestSolve
}

void
UArianeEditorPaintBucketTool::GatherGraphedObjects( TArray<FArianeObject*>& OutObjects )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
        UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>( LayerStack->GetCurrentLayer() );

        if( VectorLayer )
        {
            TArray<UArianeLayer*> PaintedLayers;
            uint32 MaxObjectCount = 0;

            GetPaintedLayers( PaintedLayers );

            for( UArianeLayer* PaintedLayer : PaintedLayers )
            {
                UArianeLayerVector* PaintedVectorLayer = Cast<UArianeLayerVector>(PaintedLayer);

                if( PaintedVectorLayer )
                {
                    MaxObjectCount += PaintedVectorLayer->GetDrawing()->GetInstancedObjects().Num();
                }
            }

            OutObjects.Reserve( MaxObjectCount );

            for( UArianeLayer* PaintedLayer : PaintedLayers )
            {
                UArianeLayerVector* PaintedVectorLayer = Cast<UArianeLayerVector>(PaintedLayer);

                if( PaintedVectorLayer )
                {
                    FArianeObject::Traverse( PaintedVectorLayer->GetDrawing()->GetRootGroup()
                                           , FArianeObject::ETraversalPolicy::AllChildren
                                           , [ &OutObjects ]( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
                        {
                            OutObjects.Add( Object );

                            return FArianeObject::ETraversalReturnValue::Continue;
                        } );
                }
            }
        }
    }
}

void
UArianeEditorPaintBucketTool::GetPaintedLayers( TArray<UArianeLayer*>& OutPaintedLayers )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
        UArianeLayer* CurrentLayer = LayerStack->GetCurrentLayer();
        bool bDoSelect = false;

        // Traverse goes in array order: back-most layer (bottom of the list) first
        if( LayerPolicy == EArianeEditorPaintBucketToolLayerPolicy::CurrentAndAbove )
        {
            bDoSelect = false;
        }

        if( LayerPolicy == EArianeEditorPaintBucketToolLayerPolicy::CurrentAndBelow )
        {
            bDoSelect = true;
        }

        UArianeLayerFolder::Traverse( LayerStack->GetRootFolder()
                                    , [ this
                                      , CurrentLayer
                                      , &bDoSelect
                                      , &OutPaintedLayers ]( UArianeLayer* Layer ) -> UArianeLayerFolder::ETraversalReturnValue
            {
                UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(Layer);

                if( VectorLayer )
                {
                    switch( LayerPolicy )
                    {
                        case EArianeEditorPaintBucketToolLayerPolicy::CurrentAndBelow :
                            if( bDoSelect )
                            {
                                OutPaintedLayers.Add( VectorLayer );
                            }

                            // the layers past the current one are above it
                            if( VectorLayer == CurrentLayer )
                            {
                                bDoSelect = false;
                            }
                        break;

                        case EArianeEditorPaintBucketToolLayerPolicy::CurrentAndAbove :
                            // the layers before the current one are below it
                            if( VectorLayer == CurrentLayer )
                            {
                                bDoSelect = true;
                            }

                            if( bDoSelect )
                            {
                                OutPaintedLayers.Add( VectorLayer );
                            }
                        break;

                        case EArianeEditorPaintBucketToolLayerPolicy::Selected :
                            if( VectorLayer->IsSelected() )
                            {
                                OutPaintedLayers.Add( VectorLayer );
                            }
                        break;

                        case EArianeEditorPaintBucketToolLayerPolicy::All :
                            OutPaintedLayers.Add( VectorLayer );
                        break;


                        default :
                        break;
                    }
                }

                return UArianeLayerFolder::ETraversalReturnValue::Continue;
            } );
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
    static FArianePointerState PreviousPointerState;
    FVector RayOrigin;
    FVector RayDirection;

    // used to pick when a solve finishes while the cursor does not move
    LastCursorViewportPosition = FVector2D( PointerState.ViewportX, PointerState.ViewportY );

    if( bGraphNeedsUpdate == false )
    {
        View->DeprojectFVector2D( FVector2D( PointerState.ViewportX, PointerState.ViewportY )
                                , RayOrigin
                                , RayDirection );

        PickedCycle = Graph->PickCycle( RayOrigin, RayDirection );
    }
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
    const UArianeEditorSettings* Settings = GetDefault<UArianeEditorSettings>();

    if( iKey == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
        ::ULIS::FColor color = Editor->GetPaintColor();
        ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
        FColor ueColor = FColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );

        // no cycle under the cursor, or the graph is being solved
        if( PickedCycle == nullptr )
        {
            return true;
        }

        if( Painting3DComponent )
        {
            //painting3DComponent->PrintPointers();
            UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
            UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(LayerStack->GetCurrentLayer());

            if( VectorLayer )
            {
                // choose between the root group and the selected group if any
                FArianeGroup* RootGroup = VectorLayer->GetDrawing()->GetRootGroup();
                int PathNumber = VectorLayer->GetDrawing()->GetInstancedObjects().Num();

                GetToolManager()->BeginUndoTransaction(LOCTEXT("ariane-paint-bucket-tool.add-cycle","Add Cycle"));

                VectorLayer->GetDrawing()->Modify();

                FArianeCycle* PaintedCycle = VectorLayer->GetDrawing()->AllocCycle( MaterialInterface
                                                                                 , *(FString( "Cycle_" ) + FString::FromInt( PathNumber ))
                                                                                 , EArianeAllocationModel::InstancedStruct );

                RootGroup->AppendChild( PaintedCycle, FArianeObject::ETransformPolicy::KeepRelative );

                PaintedCycle->SetColor( ueColor );
                PaintedCycle->UpdateTransform();
                // ImportGraphCycle needs the transform matrix to be set, hence the call to UpdateTransform just before
                PaintedCycle->ImportGraphCycle( Graph.Get(), PickedCycle, FittingRule );

                GetToolManager()->EndUndoTransaction();
            }

            PickedCycle = nullptr;

            Painting3DComponent->Update( false );
        }

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
UArianeEditorPaintBucketTool::Render(IToolsContextRenderAPI* RenderAPI)
{
    if  ( Editor->GetShowGrid() )
    {
        RenderDrawingGrid( RenderAPI );
    }
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
        // a section of an edge going behind the camera has no meaningful 2D image.
        // The edge is tested, not the section's nodes: after stitching they may differ from the edge's
        if( ContourSection->GetEdge()->IsProjected() )
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
                           , 2.0f );
            }

            //if( ContourSection->GetCycle(0) == this )
            if( ContourSection->GetClass() == FArianeGraph::FSectionCubic::StaticClass() )
            {
                FArianeGraph::FSectionCubic* CubicSection = static_cast<FArianeGraph::FSectionCubic*>(ContourSection);

                FVector2D* SectionBezier = CubicSection->GetBezier();
            }
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
    DrawSectionsHUD( HUDCanvas, RenderAPI, ViewportClient, View, HcColor, Cycle->ContourSections );
    DrawSectionsHUD( HUDCanvas, RenderAPI, ViewportClient, View, HcColor, Cycle->InnerSections );
}

void
UArianeEditorPaintBucketTool::DrawExtensionHUD ( FCanvas* HUDCanvas
                                               , IToolsContextRenderAPI* RenderAPI
                                               , FEditorViewportClient* ViewportClient
                                               , FSceneView* View
                                               , const FLinearColor& HcColor )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
    const FPlane& ProjectionPlane =  Graph->GetProjectionPlane();
    FQuat RotationQuat = FQuat::FindBetweenNormals( FVector::UpVector, ProjectionPlane.GetNormal() );
    FVector ProjectionPlaneOrigin = ProjectionPlane.GetOrigin();

    if( Painting3DComponent )
    {
        UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
        UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>( LayerStack->GetCurrentLayer() );

        if( VectorLayer )
        {
            for( FArianeGraph::FNode* ExtendedNode : Graph->GetExtendedNodes() )
            {
                FArianeGraph::FEdge* ExtendedEdge = ExtendedNode->Edges[0];

                // an extension going behind the camera has no meaningful 2D image
                if( ExtendedEdge->IsProjected() )
                {
                    FArianeGraph::FNode* OtherNode = ( ExtendedEdge->Nodes[0] == ExtendedNode ) ? ExtendedEdge->Nodes[1]
                                                                                                : ExtendedEdge->Nodes[0];
                    FVector P0Local = FVector( ExtendedNode->Position.X, ExtendedNode->Position.Y, 0.0f );
                    FVector P1Local = FVector( OtherNode->Position.X   , OtherNode->Position.Y   , 0.0f );
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
                               , VectorLayer->GetDrawing()->GetRootGroup()->GetHUDForegroundColor() //FgColor
                               , 2.0f );
                }
            }
        }
    }
}

void
UArianeEditorPaintBucketTool::DrawProgressBarHUD( FCanvas* HUDCanvas
                                                , FEditorViewportClient* ViewportClient
                                                , const FLinearColor& BgColor
                                                , const FLinearColor& HcColor )
{
    const FIntPoint ViewportSize = ViewportClient->Viewport->GetSizeXY();
    const float DPIScale = HUDCanvas->GetDPIScale();
    const float Width = ViewportSize.X / DPIScale;
    const float Height = ViewportSize.Y / DPIScale;
    const float BarHeight = 3.0f;
    // written by the worker between the pipeline steps
    const float Progress = Graph->GetProgress();

    FCanvasTileItem Background( FVector2D( 0.0f, Height - BarHeight )
                              , FVector2D( Width, BarHeight )
                              , BgColor.CopyWithNewOpacity( 0.5f ) );
    Background.BlendMode = SE_BLEND_Translucent;
    HUDCanvas->DrawItem( Background );

    FCanvasTileItem Filled( FVector2D( 0.0f, Height - BarHeight )
                          , FVector2D( Width * Progress, BarHeight )
                          , HcColor );
    Filled.BlendMode = SE_BLEND_Translucent;
    HUDCanvas->DrawItem( Filled );
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

    if( Worker.IsBusy() )
    {
        DrawProgressBarHUD( HUDCanvas, ViewportClient, BgColor, HcColor );
    }

    // a new graph arrived: pick with the current view at the last known cursor position
    if( bNeedsPick && ( bGraphNeedsUpdate == false ) )
    {
        FVector RayOrigin;
        FVector RayDirection;

        View->DeprojectFVector2D( LastCursorViewportPosition, RayOrigin, RayDirection );

        PickedCycle = Graph->PickCycle( RayOrigin, RayDirection );
        bNeedsPick = false;
    }

    if( CanDraw() && ( bGraphNeedsUpdate == false ) )
    {
        DrawExtensionHUD( HUDCanvas, RenderAPI, ViewportClient, View, HcColor );

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
    const UArianeEditorSettings* Settings = GetDefault<UArianeEditorSettings>();

    Super::PostEditChangeProperty(PropertyChangedEvent);
    FName PropertyName = PropertyChangedEvent.GetPropertyName();
    FName MemberPropertyName = PropertyChangedEvent.GetMemberPropertyName();

    // import settings: the geometry must be imported again
    if( PropertyName == GET_MEMBER_NAME_CHECKED( UArianeEditorPaintBucketTool, GapTolerance ) )
    {
        RequestImport();
    }

    if( PropertyName == GET_MEMBER_NAME_CHECKED( UArianeEditorPaintBucketTool, SegmentExtensionScheme ) )
    {
        RequestImport();
    }

    if( PropertyName == GET_MEMBER_NAME_CHECKED( UArianeEditorPaintBucketTool, LayerPolicy ) )
    {
        RequestImport();
    }

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPaintBucketTool, MaterialInterface) )
    {
        if( MaterialInterface == nullptr )
        {
            MaterialInterface = Settings->GetDefaultPathDrawingMaterial();
        }
    }
}

#undef LOCTEXT_NAMESPACE
