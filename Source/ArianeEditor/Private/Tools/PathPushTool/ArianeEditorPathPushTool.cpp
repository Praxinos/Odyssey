// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor headers
#include "PathPushTool/ArianeEditorPathPushTool.h"
#include "ArianeEditor.h"
#include "ArianeEditorStyle.h"
// Ariane headers
#include "ArianePainting3DComponent.h"
#include "ArianeDrawing.h"
#include "ArianeLayerVector.h"
#include "ArianeLayerFolder.h"
#include "ArianePath.h"
#include "ArianeGroup.h"
#include "ArianeCore.h"
#include "ArianeVertex.h"
#include "ArianeLayerStack.h"
#include "ArianeSegmentCubic.h"
// Odyssey Widgets
#include "SOdysseySinglePropertyView.h"
// Unreal headers
#include "Subsystems/EditorActorSubsystem.h"
#include "SceneView.h"
#include "Math/UnrealMathUtility.h"
#include "IStylusState.h"
#include "InteractiveToolManager.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "Math/Plane.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

UArianeEditorPathPushTool::FPointDisplacement::~FPointDisplacement()
{

}

UArianeEditorPathPushTool::FPointDisplacement::FPointDisplacement( FArianePoint* InPoint
                                                                 , double InScreenDistance
                                                                 , const FVector& InWorldPlaneNormal
                                                                 , const FTransform& Transform
                                                                 , const FVector& RayOrigin
                                                                 , const FVector& RayDirection )
{
    Point = InPoint;
    LocalPosition = Point->GetPosition();
    WorldPosition = Transform.TransformPosition( LocalPosition );

    WorldPlane = FPlane( WorldPosition, InWorldPlaneNormal );

    FArianeCore::IntersectPlane( WorldPlane, RayOrigin, RayDirection, WorldRayPositionAtDown );
}

UArianeEditorPathPushTool::~UArianeEditorPathPushTool()
{

}

UArianeEditorPathPushTool::UArianeEditorPathPushTool()
    : PickingRadius( 25.0f )
    , bMouseButtonDown ( false )
{
    Icon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsTab.PathPush64");

    bHasContextMenu = true;
}

void
UArianeEditorPathPushTool::OnPostUpdate( bool bInteractive, UArianePainting3DComponent* Painting3DComponent )
{
    if( bInteractive == false )
    {
        Reset();
    }
}

void
UArianeEditorPathPushTool::ResetQuadTree()
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();

    if( ViewportClient )
    {

        FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( ViewportClient->Viewport
                                                                                , ViewportClient->GetScene()
                                                                                , ViewportClient->EngineShowFlags ) );
        // Note: View is not allocated, it will be destroyed by Unreal at the end of the scope
        FSceneView* View = ViewportClient->CalcSceneView( &ViewFamily );

        RebuildQuadTree( ViewportClient, View );
    }
}

void
UArianeEditorPathPushTool::Reset()
{
    UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(GetCurrentLayer());

    SelectedTrees.Empty();

    if( VectorLayer )
    {
        VectorLayer->GetDrawing()->GetSelectedTrees( SelectedTrees );

        // if nothing is selected, select the whole root group
        if( SelectedTrees.IsEmpty() )
        {
            SelectedTrees.Add( VectorLayer->GetDrawing()->GetRootGroup() );
        }
    }

    ResetQuadTree();
}

void
UArianeEditorPathPushTool::OnPostDrawingChanged( UArianeLayerVector* VectorLayer )
{
    Reset();
}

void
UArianeEditorPathPushTool::OnPreLayerStackSelectionChanged( UArianeLayerStack* LayerStack )
{
    UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(LayerStack->GetCurrentLayer());

    if( VectorLayer )
    {
        UnbindVectorLayerDelegates( VectorLayer );
    }
}

void
UArianeEditorPathPushTool::OnPostLayerStackSelectionChanged( UArianeLayerStack* LayerStack )
{
    UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(LayerStack->GetCurrentLayer());

    if( VectorLayer )
    {
        BindVectorLayerDelegates( VectorLayer );
    }
}

void
UArianeEditorPathPushTool::BindVectorLayerDelegates( UArianeLayerVector* VectorLayer )
{
    // commented-out: nothing to do on Pre. Left there for consistency
    //VectorLayer->OnPreImageChangedDelegate().AddSP( this, &UArianeEditorPathPushTool::OnPreImageChanged );
    VectorLayer->OnPostDrawingChangedDelegate().AddUObject( this, &UArianeEditorPathPushTool::OnPostDrawingChanged, VectorLayer );
}

void
UArianeEditorPathPushTool::UnbindVectorLayerDelegates( UArianeLayerVector* VectorLayer )
{
    // commented-out: nothing to do on Pre. Left there for consistency
    //VectorLayer->OnPreImageChangedDelegate().RemoveAll( this );
    VectorLayer->OnPostDrawingChangedDelegate().RemoveAll( this );
}

void
UArianeEditorPathPushTool::BindLayerStackDelegates( UArianeLayerStack* LayerStack )
{
    UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(LayerStack->GetCurrentLayer());

    LayerStack->OnPreSelectionChangedDelegate().AddUObject( this, &UArianeEditorPathPushTool::OnPreLayerStackSelectionChanged, LayerStack );
    LayerStack->OnPostSelectionChangedDelegate().AddUObject( this, &UArianeEditorPathPushTool::OnPostLayerStackSelectionChanged, LayerStack );

    if( VectorLayer )
    {
        BindVectorLayerDelegates ( VectorLayer );
    }
}

void
UArianeEditorPathPushTool::UnbindLayerStackDelegates( UArianeLayerStack* LayerStack )
{
    UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(LayerStack->GetCurrentLayer());

    LayerStack->OnPreSelectionChangedDelegate().RemoveAll( this );
    LayerStack->OnPostSelectionChangedDelegate().RemoveAll( this );

    if( VectorLayer )
    {
        BindVectorLayerDelegates ( VectorLayer );
    }
}

void
UArianeEditorPathPushTool::BindComponentDelegates( UArianePainting3DComponent* Painting3DComponent )
{
    Painting3DComponent->OnPostUpdateDelegate().AddUObject( this, &UArianeEditorPathPushTool::OnPostUpdate, Painting3DComponent );

    BindLayerStackDelegates ( Painting3DComponent->GetLayerStack() );
}

void
UArianeEditorPathPushTool::UnbindComponentDelegates( UArianePainting3DComponent* Painting3DComponent )
{
    Painting3DComponent->OnPostUpdateDelegate().RemoveAll( this );

    UnbindLayerStackDelegates ( Painting3DComponent->GetLayerStack() );
}

void
UArianeEditorPathPushTool::BindEditorDelegates()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    Editor->OnPost3DPaintingComponentSelectionChangedDelegate().AddUObject( this, &UArianeEditorPathPushTool::Reset );

    if( Painting3DComponent )
    {
        BindComponentDelegates ( Painting3DComponent );
    }
}

void
UArianeEditorPathPushTool::UnbindEditorDelegates()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    Editor->OnPost3DPaintingComponentSelectionChangedDelegate().RemoveAll( this );

    if( Painting3DComponent )
    {
        UnbindComponentDelegates( Painting3DComponent );
    }
}

void
UArianeEditorPathPushTool::Activate()
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();

    Super::Activate();

    // ViewportClient can be nullptr when closing the editor
    if( ViewportClient )
    {
        ViewportClient->EngineShowFlags.SetSelectionOutline(false);
        ViewportClient->Invalidate();

        GEditor->RedrawAllViewports();
    }

    Reset();

    BindEditorDelegates();
}

void
UArianeEditorPathPushTool::Inactivate()
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();

    UnbindEditorDelegates();

    // ViewportClient can be nullptr when closing the editor
    if( ViewportClient )
    {
        ViewportClient->EngineShowFlags.SetSelectionOutline(true);
        ViewportClient->Invalidate();

        GEditor->RedrawAllViewports();
    }

    Super::Inactivate();
}

bool
UArianeEditorPathPushTool::OnKeyDownGlobal( const FKeyEvent& InKeyEvent )
{
    return Super::OnKeyDownGlobal( InKeyEvent );
}

bool
UArianeEditorPathPushTool::OnKeyUpGlobal( const FKeyEvent& InKeyEvent )
{
    return Super::OnKeyUpGlobal( InKeyEvent );
}

UArianeEditorPathPushTool::FPointDisplacement*
UArianeEditorPathPushTool::GetPointDisplacement( TArray<FPointDisplacement>& PointDisplacements, FArianePoint* Point )
{
    return PointDisplacements.FindByPredicate( [Point] ( FPointDisplacement& PointDisplacement )
                                               {
                                                   return ( PointDisplacement.Point == Point );
                                               } );
}

// static
UArianeEditorPathPushTool::FPointDisplacement*
UArianeEditorPathPushTool::CreateOrGetPointDisplacement( FSceneView* View
                                                       , const FVector2D& MouseScreenPosition
                                                       , TArray<FPointDisplacement>& PointDisplacements
                                                       , const FPlane& DrawingPlane
                                                       , const FTransform& OwnerTransform
                                                       , const FVector& RayOrigin
                                                       , const FVector& RayDirection
                                                       , FArianePoint* Point )
{
     FPointDisplacement* PointDisplacement = GetPointDisplacement( PointDisplacements, Point );

    if( PointDisplacement == nullptr )
    {
        FVector2D PointScreenPosition;
        bool bMapped = WorldToScreen( View, OwnerTransform.TransformPosition( Point->GetPosition() ), PointScreenPosition );

        if( bMapped )
        {
            double ScreenDistanceToMouse = bMapped ? ( PointScreenPosition - MouseScreenPosition ).Length() : -1.0f;

            PointDisplacement = &PointDisplacements.Emplace_GetRef( Point
                                                                  , ScreenDistanceToMouse
                                                                  , DrawingPlane.GetNormal()
                                                                  , OwnerTransform
                                                                  , RayOrigin
                                                                  , RayDirection );
        }
    }

    return PointDisplacement;
}

void
UArianeEditorPathPushTool::ComputeRatios( TArray<FPointDisplacement>& PointDisplacements, double MaxScreenDistance )
{
/*
    // Ratios for vertices
    for( FPointDisplacement& PointDisplacement : PointDisplacements )
    {
        if( PointDisplacement.ScreenDistance >= 0.0f )
        {
            if( PointDisplacement.Point->GetClass() == FArianeVertex::StaticClass() )
            {
                PointDisplacement.Ratio = MaxScreenDistance ? ( PointDisplacement.ScreenDistance / MaxScreenDistance ) : 0.0f;
            }
        }
    }

    // ratios for handles (need the ratios for vertices to be computed first)
    for( FPointDisplacement& PointDisplacement : PointDisplacements )
    {
        if( PointDisplacement.ScreenDistance >= 0.0f )
        {
            if( PointDisplacement.Point->GetClass() == FArianeHandleSegment::StaticClass() )
            {
                FArianeHandleSegment* Handle = static_cast<FArianeHandleSegment*>( PointDisplacement.Point );

                PointDisplacement.Ratio = MaxScreenDistance ? ( PointDisplacement.ScreenDistance / MaxScreenDistance ) : 0.0f;
            }
        }
    }
*/
}

double
UArianeEditorPathPushTool::ParseInfluencedPoints( FSceneView* View
                                            , const FVector2D& MouseScreenPosition
                                            , const FPlane& DrawingPlane
                                            , const FVector& RayOrigin
                                            , const FVector& RayDirection
                                            , const TArray<FInfluencedPoint>& InfluencedPoints )
{
    InfluencedPointDisplacements.Reserve( InfluencedPoints.Num() );

    for( const FInfluencedPoint& InfluencedPoint : InfluencedPoints )
    {
        FPointDisplacement* PointDisplacement = CreateOrGetPointDisplacement( View
                                                                            , MouseScreenPosition
                                                                            , InfluencedPointDisplacements
                                                                            , DrawingPlane
                                                                            , InfluencedPoint.Owner->GetTransform()
                                                                            , RayOrigin
                                                                            , RayDirection
                                                                            , InfluencedPoint.Point );
    }

    return 0.0f;
}

bool
UArianeEditorPathPushTool::OnMouseDown( FEditorViewportClient* ViewportClient
                                      , FSceneView* View
                                      , const FKey& Key
                                      , const FArianePointerState& PointerState
                                      , bool iRepeat )
{
    FVector RayOrigin, RayDirection;
    double ScreenMaxDistance = 0.0f;
    TArray<FInfluencedPoint> InfluencedPoints;

    ScreenMousePositionAtDown = ScreenMousePositionAtDrag = FVector2D( PointerState.ViewportX, PointerState.ViewportY );

    bMouseButtonDown = true;

    PickedSegments.Empty();
    InfluencedPointDisplacements.Empty();

    View->DeprojectFVector2D( FVector2D( PointerState.ViewportX, PointerState.ViewportY ), RayOrigin, RayDirection );

    if( Key == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        if( Painting3DComponent )
        {
            UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
            UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(LayerStack->GetCurrentLayer());

            if( VectorLayer )
            {
                FPlane DrawingPlane = GetDrawingPlane( ViewportClient, VectorLayer );

                VectorLayer->GetDrawing()->Modify();

                SegmentQuadTree->PickSegments( ViewportClient, View, ScreenMousePositionAtDown, PickingRadius, PickedSegments );

                InfluencedPoints.Reserve( PickedSegments.Num() * 4 );

                for( FArianeSegment* PickedSegment : PickedSegments )
                {
                    FArianeObject* Owner = PickedSegment->GetOwner();
                    const FTransform& OwnerTransform = Owner->GetTransform();
                    FArianeVertex* Vertex0 = PickedSegment->GetVertex(0);
                    FArianeVertex* Vertex1 = PickedSegment->GetVertex(1);

                    InfluencedPoints.Emplace( FInfluencedPoint{ .Point = Vertex0, .Owner = Owner } );
                    InfluencedPoints.Emplace( FInfluencedPoint{ .Point = Vertex1, .Owner = Owner } );

                    if( PickedSegment->GetClass() == FArianeSegmentCubic::StaticClass() )
                    {
                        FArianeSegmentCubic* CubicSegment = static_cast<FArianeSegmentCubic*>(PickedSegment);
                        FArianeHandleSegment* Handle0 = CubicSegment->GetHandle((uint32)0);
                        FArianeHandleSegment* Handle1 = CubicSegment->GetHandle((uint32)1);

                        InfluencedPoints.Emplace( FInfluencedPoint{ .Point = Handle0, .Owner = Owner } );
                        InfluencedPoints.Emplace( FInfluencedPoint{ .Point = Handle1, .Owner = Owner } );
                    }
                }
/*
                MaxDistance = ParseInfluencedPoints( View
                                               , ScreenMousePositionAtDown
                                               , DrawingPlane
                                               , RayOrigin
                                               , RayDirection
                                               , InfluencedPoints );
*/
                for( FPointDisplacement& PointDisplacement : InfluencedPointDisplacements )
                {
                }
            }
        }

        return true;
    }

    return false;
}

void
UArianeEditorPathPushTool::OnMouseHover( FEditorViewportClient* ViewportClient
                                       , FSceneView* View
                                       , const FArianePointerState& State )
{
}

void
UArianeEditorPathPushTool::DisplacePoint( FArianePoint* Point
                                        , const FPointDisplacement& PointDisplacment
                                        , const FTransform& Transform
                                        , const FVector& RayOrigin
                                        , const FVector& RayDirection )
{
    FVector IntersectAt;

    if( FArianeCore::IntersectPlane( PointDisplacment.WorldPlane, RayOrigin, RayDirection, IntersectAt ) > 0.0f )
    {
        FVector WorldDelta = IntersectAt - PointDisplacment.WorldRayPositionAtDown;

        Point->SetPosition( PointDisplacment.LocalPosition + Transform.InverseTransformVector( WorldDelta ) );
    }
}

bool
UArianeEditorPathPushTool::IsMouseButtonDown()
{
    return bMouseButtonDown;
}

bool
UArianeEditorPathPushTool::OnMouseDrag( FEditorViewportClient* ViewportClient
                                      , FSceneView* View
                                      , const FKey& iKey
                                      , const FArianePointerState& PointerState )
{
    FVector RayOrigin;
    FVector RayDirection;

    ScreenMousePositionAtDrag = FVector2D( PointerState.ViewportX, PointerState.ViewportY );

    View->DeprojectFVector2D( ScreenMousePositionAtDrag
                            , RayOrigin
                            , RayDirection );

    if( iKey == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
/*
        if( Painting3DComponent )
        {
            if( EditionMode == EArianePathPushToolEditionMode::Vertex )
            {
                for( int i = 0; i < PickedVertices.Num(); i++ )
                {
                    FArianeVertex *Vertex = PickedVertices[i];
                    FPointDisplacement& PointDisplacement = PickedVertexDisplacements[i];

                    DisplacePoint( Vertex
                                 , PointDisplacement
                                 , Vertex->GetOwner()->GetTransform()
                                 , RayOrigin
                                 , RayDirection );
                }
            }

            // Update. Note: as the seen is invalidated, it will request a redraw
            Painting3DComponent->Update( true );
        }
*/
    }

    return false;
}

void
UArianeEditorPathPushTool::RebuildQuadTree( FEditorViewportClient* ViewportClient
                                          , FSceneView* View )
{
    UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(GetCurrentLayer());

    if( VectorLayer )
    {
        MakeSegmentQuadTree( ViewportClient, View, SelectedTrees, true );
    }
}

bool
UArianeEditorPathPushTool::OnMouseUp( FEditorViewportClient* ViewportClient
                                    , FSceneView* View
                                    , const FKey& iKey
                                    , const FArianePointerState& PointerState )
{
    bMouseButtonDown = false;

    return false;
}

void
UArianeEditorPathPushTool::Render(IToolsContextRenderAPI* RenderAPI)
{
    if  ( Editor->GetShowGrid() )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        if( Painting3DComponent )
        {
            UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
            UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>( LayerStack->GetCurrentLayer() );

            if( VectorLayer )
            {
                DrawLayerOrientationGrid( RenderAPI, VectorLayer );
            }
        }
    }
}

void
UArianeEditorPathPushTool::DrawHUD ( FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();
    FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( ViewportClient->Viewport
                                                                            , ViewportClient->GetScene()
                                                                            , ViewportClient->EngineShowFlags ) );
    // Note: View is not allocated, it will be destroyed by Unreal at the end of the scope
    FSceneView* View = ViewportClient->CalcSceneView( &ViewFamily );
    FLinearColor FgColor = GetForegroundColor();
    FLinearColor BgColor = GetBackgroundColor();
    FLinearColor HcColor = GetHighlightColor();
    FArianeEditorHUD::FDrawingFlags HUDDrawingFlags = Editor->GetHUDDrawingFlags();
    FVector2D MousePosition = FVector2D( ViewportClient->GetCachedMouseX()
                                       , ViewportClient->GetCachedMouseY() );
    FVector2D HUDPosition = ScreenToHUD( ViewportClient, MousePosition );


    if( Painting3DComponent )
    {
        UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( VectorLayer )
        {
            for( FArianeObject* SelectedTree : SelectedTrees )
            {
                FArianeObject::Traverse( SelectedTree
                                      , [ this
                                        , Canvas
                                        , ViewportClient
                                        , View
                                        , FgColor
                                        , BgColor
                                        , HcColor
                                        , HUDDrawingFlags ]( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
                {
                    if( Object->GetClass() == FArianePath::StaticClass() )
                    {
                        FArianePath* Path = static_cast<FArianePath*>( Object );

                        DrawPathHUD( Canvas
                                   , ViewportClient
                                   , View
                                   , Path
                                   , Path->GetHUDForegroundColor() //FgColor
                                   , BgColor
                                   , HcColor
                                   , HUDDrawingFlags );
                    }

                    return FArianeObject::ETraversalReturnValue::Continue;
                } );
            }
        }
    }
}

bool
UArianeEditorPathPushTool::SupportsColorType( EOdysseyPainterEditorColorType ColorType )
{
    return true;
}

void
UArianeEditorPathPushTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);

    FToolMenuSection& section = iToolMenu->AddSection(NAME_None);
    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UArianeEditorPathPushTool, PickingRadius ), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );
}

void
UArianeEditorPathPushTool::ExtendContextMenu( FMenuBuilder& menu )
{
/*
    UEditorActorSubsystem* editorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();

    menu.AddMenuEntry(
          LOCTEXT("ariane-path-drawing-tool.context-menu.add-painting3D-component.name", "Add Painting3D component")
        , LOCTEXT("ariane-path-drawing-tool.context-menu.add-painting3D-component.tooltip", "Add Painting3D component")
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateRaw( Editor
                                              , &FArianeEditor::AddPainting3DComponent
                                              , editorActorSubsystem->GetSelectedLevelActors() ) ) );
*/
}

#undef LOCTEXT_NAMESPACE
