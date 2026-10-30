// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor headers
#include "PathEditTool/ArianeEditorPathEditTool.h"
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

#define SMALL_CIRCLE_RADIUS 10.0f

UArianeEditorPathEditTool::FSegmentAdjustment::FSegmentAdjustment( FArianeSegment* InSegment )
{
    double SegmentLength = InSegment->GetLength();

    Segment = InSegment;
    HandleRatio[0] = HandleRatio[1] = 0.0f;

    if( SegmentLength )
    {
        if( Segment->HasBaseClass( FArianeSegmentCubic::StaticClass() ) )
        {
            FArianeSegmentCubic* CubicSegment = static_cast<FArianeSegmentCubic*>(InSegment);
            FVector HandleVector0 = CubicSegment->GetHandleVector( (uint32)0, false );
            FVector HandleVector1 = CubicSegment->GetHandleVector( (uint32)1, false );

            HandleRatio[0] = HandleVector0.Length() / SegmentLength;
            HandleRatio[1] = HandleVector1.Length() / SegmentLength;
        }
    }
}

void
UArianeEditorPathEditTool::FSegmentAdjustment::Adjust()
{
    if( Segment->HasBaseClass( FArianeSegmentCubic::StaticClass() ) )
    {
        FArianeSegmentCubic* CubicSegment = static_cast<FArianeSegmentCubic*>(Segment);
        FArianeVertex* Vertex0 = CubicSegment->GetVertex(0);
        FArianeVertex* Vertex1 = CubicSegment->GetVertex(1);
        FArianeHandleSegment* Handle0 = CubicSegment->GetHandle((uint32)0);
        FArianeHandleSegment* Handle1 = CubicSegment->GetHandle((uint32)1);
        FVector HandleVector0 = CubicSegment->GetHandleVector( Vertex0, true );
        FVector HandleVector1 = CubicSegment->GetHandleVector( Vertex1, true );
        double CurrentSegmentLength = CubicSegment->GetLength();

        Handle0->SetPosition( Vertex0->GetPosition() + ( HandleVector0 * CurrentSegmentLength * HandleRatio[0] ) );
        Handle1->SetPosition( Vertex1->GetPosition() + ( HandleVector1 * CurrentSegmentLength * HandleRatio[1] ) );
    }
}


UArianeEditorPathEditTool::FPointDisplacement::~FPointDisplacement()
{

}

UArianeEditorPathEditTool::FPointDisplacement::FPointDisplacement( FArianePoint* Point
                                                                 , const FVector& InWorldPlaneNormal
                                                                 , const FTransform& Transform
                                                                 , const FVector& RayOrigin
                                                                 , const FVector& RayDirection )
{
    LocalPosition = Point->GetPosition();
    WorldPosition = Transform.TransformPosition( LocalPosition );

    WorldPlane = FPlane( WorldPosition, InWorldPlaneNormal );

    FArianeCore::IntersectPlane( WorldPlane, RayOrigin, RayDirection, WorldRayPositionAtDown );
}

UArianeEditorPathEditTool::~UArianeEditorPathEditTool()
{

}

UArianeEditorPathEditTool::UArianeEditorPathEditTool()
    : PickingRadius( 25.0f )
    , bWidenAllAlong( true )
    , bMouseButtonDown ( false )
    , EditionMode  ( EArianePathEditToolEditionMode::Vertex )
{
    Icon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsTab.PathEdit64");

    bHasContextMenu = true;
}

void
UArianeEditorPathEditTool::OnPostUpdate( bool bInteractive, UArianePainting3DComponent* Painting3DComponent )
{
    if( bInteractive == false )
    {
        Reset();
    }
}

void
UArianeEditorPathEditTool::ResetQuadTree()
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
UArianeEditorPathEditTool::Reset()
{
    UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(GetCurrentLayer());

    ResetQuadTree();

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
}

void
UArianeEditorPathEditTool::OnPostDrawingChanged( UArianeLayerVector* VectorLayer )
{
    Reset();
}

void
UArianeEditorPathEditTool::OnPreLayerStackSelectionChanged( UArianeLayerStack* LayerStack )
{
    UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(LayerStack->GetCurrentLayer());

    if( VectorLayer )
    {
        UnbindVectorLayerDelegates( VectorLayer );
    }
}

void
UArianeEditorPathEditTool::OnPostLayerStackSelectionChanged( UArianeLayerStack* LayerStack )
{
    UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(LayerStack->GetCurrentLayer());

    if( VectorLayer )
    {
        BindVectorLayerDelegates( VectorLayer );
    }
}

void
UArianeEditorPathEditTool::BindVectorLayerDelegates( UArianeLayerVector* VectorLayer )
{
    // commented-out: nothing to do on Pre. Left there for consistency
    //VectorLayer->OnPreImageChangedDelegate().AddSP( this, &UArianeEditorPathEditTool::OnPreImageChanged );
    VectorLayer->OnPostDrawingChangedDelegate().AddUObject( this, &UArianeEditorPathEditTool::OnPostDrawingChanged, VectorLayer );
}

void
UArianeEditorPathEditTool::UnbindVectorLayerDelegates( UArianeLayerVector* VectorLayer )
{
    // commented-out: nothing to do on Pre. Left there for consistency
    //VectorLayer->OnPreImageChangedDelegate().RemoveAll( this );
    VectorLayer->OnPostDrawingChangedDelegate().RemoveAll( this );
}

void
UArianeEditorPathEditTool::BindLayerStackDelegates( UArianeLayerStack* LayerStack )
{
    UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(LayerStack->GetCurrentLayer());

    LayerStack->OnPreSelectionChangedDelegate().AddUObject( this, &UArianeEditorPathEditTool::OnPreLayerStackSelectionChanged, LayerStack );
    LayerStack->OnPostSelectionChangedDelegate().AddUObject( this, &UArianeEditorPathEditTool::OnPostLayerStackSelectionChanged, LayerStack );

    if( VectorLayer )
    {
        BindVectorLayerDelegates ( VectorLayer );
    }
}

void
UArianeEditorPathEditTool::UnbindLayerStackDelegates( UArianeLayerStack* LayerStack )
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
UArianeEditorPathEditTool::BindComponentDelegates( UArianePainting3DComponent* Painting3DComponent )
{
    Painting3DComponent->OnPostUpdateDelegate().AddUObject( this, &UArianeEditorPathEditTool::OnPostUpdate, Painting3DComponent );

    BindLayerStackDelegates ( Painting3DComponent->GetLayerStack() );
}

void
UArianeEditorPathEditTool::UnbindComponentDelegates( UArianePainting3DComponent* Painting3DComponent )
{
    Painting3DComponent->OnPostUpdateDelegate().RemoveAll( this );

    UnbindLayerStackDelegates ( Painting3DComponent->GetLayerStack() );
}

void
UArianeEditorPathEditTool::BindEditorDelegates()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    Editor->OnPost3DPaintingComponentSelectionChangedDelegate().AddUObject( this, &UArianeEditorPathEditTool::Reset );

    if( Painting3DComponent )
    {
        BindComponentDelegates ( Painting3DComponent );
    }
}

void
UArianeEditorPathEditTool::UnbindEditorDelegates()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    Editor->OnPost3DPaintingComponentSelectionChangedDelegate().RemoveAll( this );

    if( Painting3DComponent )
    {
        UnbindComponentDelegates( Painting3DComponent );
    }
}

void
UArianeEditorPathEditTool::Activate()
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
UArianeEditorPathEditTool::Inactivate()
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
UArianeEditorPathEditTool::OnKeyDownGlobal( const FKeyEvent& InKeyEvent )
{
    Super::OnKeyDownGlobal( InKeyEvent );

    EditionMode = EArianePathEditToolEditionMode::Vertex;

    if( AreAllGlobalPressedKeysModifierKeys() )
    {
        if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() && ( FSlateApplication::Get().GetModifierKeys().IsControlDown()
                                                                        || FSlateApplication::Get().GetModifierKeys().IsCommandDown() ) )
        {
            FEditorViewportClient* ViewportClient = GetActiveViewportClient();
            UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(GetCurrentLayer());

            EditionMode  = EArianePathEditToolEditionMode::Alter;

            if( ViewportClient && VectorLayer )
            {
                ResetQuadTree();
            }

            return true;
        }

        if ( FSlateApplication::Get().GetModifierKeys().IsControlDown()
          || FSlateApplication::Get().GetModifierKeys().IsCommandDown() )
        {
            EditionMode   = EArianePathEditToolEditionMode::SegmentHandle;

            return true;
        }

        if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
        {
            EditionMode  = EArianePathEditToolEditionMode::VertexHandle;

            return true;
        }
    }

    return false;
}

bool
UArianeEditorPathEditTool::OnKeyUpGlobal( const FKeyEvent& InKeyEvent )
{
    FKey key = InKeyEvent.GetKey();

    Super::OnKeyUpGlobal( InKeyEvent );

    // note, we cannot use FSlateApplication::Get().GetModifierKeys()
    // because the keys are already released. For consistency we do
    // the same in the KeyDown event even though we could use
    // FSlateApplication::Get().GetModifierKeys()
    //if ( ( key == EKeys::LeftControl ) || ( key == EKeys::RightControl )
    //  || ( key == EKeys::LeftCommand ) || ( key == EKeys::RightCommand )
    //  || ( key == EKeys::LeftShift   ) || ( key == EKeys::RightShift   )
    //  || ( key == EKeys::LeftAlt     ) || ( key == EKeys::RightAlt     ) )
    //{
    //}

    // first reset display mode
    EditionMode = EArianePathEditToolEditionMode::Vertex;

    return false;
}

// static
void
UArianeEditorPathEditTool::BuildSegmentAdjustments( UArianeLayerVector* VectorLayer
                                                  , const TArray<FArianeSegment*>& Segments
                                                  , TArray<FSegmentAdjustment>& OutSegmentAdjustments )
{
    UArianeDrawing* Image = VectorLayer->GetDrawing();

    OutSegmentAdjustments.Reserve( Segments.Num() );

    for( const FArianeSegmentID& SegmentID : Segments )
    {
        FArianeSegment* Segment = const_cast<FArianeSegmentID&>(SegmentID).GetSegment(Image);

        OutSegmentAdjustments.Emplace( Segment );
    }
}

void
UArianeEditorPathEditTool::OnMouseDownPickPoint( FEditorViewportClient* ViewportClient
                                               , FSceneView* View
                                               , const FKey& iKey
                                               , const FArianePointerState& PointerState
                                               , bool iRepeat )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
    FVector RayOrigin;
    FVector RayDirection;

    View->DeprojectFVector2D( FVector2D( PointerState.ViewportX, PointerState.ViewportY ), RayOrigin, RayDirection );

    SelectedPaths.Empty();
    PickedVertices.Empty();
    PickedVertexDisplacements.Empty();
    PickedHandles.Empty();
    PickedHandleDisplacements.Empty();
    SegmentAdjustments.Empty();

    SelectedPaths.Reserve( 200 );

    if( Painting3DComponent )
    {
        UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( VectorLayer )
        {
            FPlane DrawingPlane = GetDrawingPlane( ViewportClient, VectorLayer );
            UArianeDrawing* Image = VectorLayer->GetDrawing();
            FVector2D MousePosition = FVector2D( ViewportClient->GetCachedMouseX()
                                               , ViewportClient->GetCachedMouseY() );
            FVector2D HUDPosition = ScreenToHUD( ViewportClient, MousePosition );

            for( FArianeObject* SelectedTree : SelectedTrees )
            {
                FArianeObject::Traverse ( SelectedTree
                                        , [ this
                                          , ViewportClient
                                          , View
                                          , HUDPosition ]( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
                    {
                        if( /*iScene->GetCell()->ObjectHasFocus( object, traversalFlags )*/1 )
                        {
                            if( Object->GetClass() == FArianePath::StaticClass() )
                            {
                                FArianePath* Path = static_cast<FArianePath*>(Object);

                                // for widening all paths
                                SelectedPaths.Add( Path );

                                PickPathPoints( ViewportClient
                                            , View
                                            , Path
                                            , HUDPosition
                                            , PickingRadius
                                            , PickedVertices
                                            , PickedHandles
                                            , EditonModeToPickingFlags() );
                            }
                        }

                        return FArianeObject::ETraversalReturnValue::Continue;
                    } );
            }

            switch( EditionMode )
            {
                case EArianePathEditToolEditionMode::VertexHandle :
                    GetToolManager()->BeginUndoTransaction(LOCTEXT("ariane-path-edit-tool.edit-vertex-handles","Edit Vertex Handles"));
                    Image->Modify();
                break;

                case EArianePathEditToolEditionMode::Vertex :
                {
                    GetToolManager()->BeginUndoTransaction(LOCTEXT("ariane-path-edit-tool.edit-vertices","Edit Vertices"));

                    Image->Modify();

                    TArray<FArianeSegment*> AlteredSegments;

                    // static call
                    FArianeVertex::ArrayToSegmentArray( PickedVertices, AlteredSegments, false );
                    // static call
                    BuildSegmentAdjustments( VectorLayer, AlteredSegments, SegmentAdjustments );

                    // Control points must move with the point. Store them in the iPickedHandleArray
                    for( FArianeVertex* Vertex : PickedVertices )
                    {
                        for( const FArianeSegmentID& SegmentID : Vertex->GetSegments() )
                        {
                            FArianeSegment* Segment = const_cast<FArianeSegmentID&>(SegmentID).GetSegment( Image );

                            if( Segment->GetClass() == FArianeSegmentCubic::StaticClass() )
                            {
                                FArianeSegmentCubic* CubicSegment = static_cast<FArianeSegmentCubic*>(Segment);
                                FArianeHandleSegment* Handle = CubicSegment->GetHandle( Vertex );

                                PickedHandles.Add( Handle );
                            }
                        }
                    }
                }
                break;

                case EArianePathEditToolEditionMode::SegmentHandle :
                    GetToolManager()->BeginUndoTransaction(LOCTEXT("ariane-path-edit-tool.edit-handles","Edit Handles"));
                    Image->Modify();
                break;

                default:
                break;
            }

            // remember vertex position at mouse down
            PickedVertexDisplacements.Reserve( PickedVertices.Num() );
            for( FArianeVertex* Vertex : PickedVertices )
            {
                const FTransform& OwnerTransform = Vertex->GetOwner()->GetTransform();

                PickedVertexDisplacements.Emplace( Vertex
                                                 , DrawingPlane.GetNormal()
                                                 , OwnerTransform
                                                 , RayOrigin
                                                 , RayDirection );
            }

            // remember handle position at mouse down
            PickedHandleDisplacements.Reserve( PickedHandles.Num() );
            for( FArianeHandleSegment* Handle : PickedHandles )
            {
                const FTransform& OwnerTransform = Handle->GetOwnerSegment()->GetOwner()->GetTransform();

                PickedHandleDisplacements.Emplace( Handle
                                                 , DrawingPlane.GetNormal()
                                                 , OwnerTransform
                                                 , RayOrigin
                                                 , RayDirection );
            }

            VectorLayer->Update( true );
        }
    }
}

bool
UArianeEditorPathEditTool::OnMouseDown( FEditorViewportClient* ViewportClient
                                      , FSceneView* View
                                      , const FKey& Key
                                      , const FArianePointerState& PointerState
                                      , bool iRepeat )
{
    ScreenMousePositionAtDown = ScreenMousePositionAtDrag = FVector2D( PointerState.ViewportX, PointerState.ViewportY );

    bMouseButtonDown = true;

    if( Key == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        if( Painting3DComponent )
        {
            //painting3DComponent->PrintPointers();
            UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
            UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(LayerStack->GetCurrentLayer());

            if( VectorLayer )
            {
                //mPointInTextureAtDown = ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y );

                //mPathEditHUD->SetCutLineP0( iPointInTexture.x, iPointInTexture.y );
                //mPathEditHUD->SetCutLineP1( iPointInTexture.x, iPointInTexture.y );

                VectorLayer->GetDrawing()->Modify();

                switch( EditionMode )
                {
                    case EArianePathEditToolEditionMode::Alter :
                    // dealt with in OnMouseUpVector
                    break;

                    default:
                        OnMouseDownPickPoint( ViewportClient, View, Key, PointerState, iRepeat );
                    break;
                }
            }
        }

        return true;
    }

    return false;
}

void
UArianeEditorPathEditTool::OnMouseHover( FEditorViewportClient* ViewportClient
                                       , FSceneView* View
                                       , const FArianePointerState& State )
{
    if( PointQuadTree )
    {
        FVector2D MousePosition = FVector2D( ViewportClient->GetCachedMouseX()
                                           , ViewportClient->GetCachedMouseY() );
        FVector2D HUDPosition = ScreenToHUD( ViewportClient, MousePosition );

        HoveredPoints.Empty();
        HoveredSegments.Empty();

        PointQuadTree->PickPoints( HUDPosition
                                 , PickingRadius
                                 , HoveredPoints );

        SegmentQuadTree->PickSegments( ViewportClient
                                     , View
                                     , HUDPosition
                                     , SMALL_CIRCLE_RADIUS // PickingRadius does not make sense when deleting a vertex, soi we use a fixed-size circle instead
                                     , HoveredSegments );
    }
}

void
UArianeEditorPathEditTool::DragVertexHandle( FArianeVertex* Vertex
                                           , const TArray<FArianePath*>& SelectedPaths
                                           , const FPointDisplacement& PointDisplacment
                                           , const FTransform& Transform
                                           , const FVector& RayOrigin
                                           , const FVector& RayDirection
                                           , bool bInWidenAllAlong )
{
    FVector IntersectAt;

    if( FArianeCore::IntersectPlane( PointDisplacment.WorldPlane, RayOrigin, RayDirection, IntersectAt ) > 0.0f )
    {
        FVector WorldDelta = IntersectAt - PointDisplacment.WorldRayPositionAtDown;
        FVector LocalMouse = Transform.InverseTransformPosition( IntersectAt );

        if( Vertex->GetRadius() )
        {
            double Ratio = ( FVector( PointDisplacment.LocalPosition - LocalMouse ).Length() ) / Vertex->GetRadius();

            if( bInWidenAllAlong )
            {
                for( FArianePath* Path : SelectedPaths )
                {
                    Path->AlterRadius( Ratio  );
                }
            }
            else
            {
                Vertex->SetRadius( Vertex->GetRadius() * Ratio );
            }
        }
    }
}

void
UArianeEditorPathEditTool::DisplacePoint( FArianePoint* Point
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
UArianeEditorPathEditTool::IsMouseButtonDown()
{
    return bMouseButtonDown;
}

bool
UArianeEditorPathEditTool::OnMouseDrag( FEditorViewportClient* ViewportClient
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

        if( Painting3DComponent )
        {
            if( EditionMode == EArianePathEditToolEditionMode::VertexHandle  )
            {
                for( int i = 0; i < PickedVertices.Num(); i++ )
                {
                    FArianeVertex *Vertex = PickedVertices[i];
                    FPointDisplacement& PointDisplacement = PickedVertexDisplacements[i];

                    DragVertexHandle( Vertex
                                    , SelectedPaths
                                    , PointDisplacement
                                    , Vertex->GetOwner()->GetTransform()
                                    , RayOrigin
                                    , RayDirection
                                    , bWidenAllAlong /*&& ( PickedVertices.Num() == 1 )*/ );
                }
            }

            if( EditionMode == EArianePathEditToolEditionMode::Vertex )
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

            if( ( EditionMode == EArianePathEditToolEditionMode::SegmentHandle )
            ||  ( EditionMode == EArianePathEditToolEditionMode::Vertex        ) )
            {
                for( int i = 0; i < PickedHandles.Num(); i++ )
                {
                    FArianeHandleSegment *Handle = PickedHandles[i];
                    FArianeSegment* Segment = Handle->GetOwnerSegment();
                    FPointDisplacement& PointDisplacement = PickedHandleDisplacements[i];
                    TArray<FArianeVertex*> AttachedVertices;

                    Handle->GetAttachedVertices( AttachedVertices );

                    DisplacePoint( Handle
                                 , PointDisplacement
                                 , Handle->GetOwnerSegment()->GetOwner()->GetTransform()
                                 , RayOrigin
                                 , RayDirection );


                    // we will have to invalidate any segment connected to the segment that is edited
                    // in case the tangent at T = 0 or T = 1 is FVector::Zero. For example, if in a cubic
                    // bezier P0 = P1, then the tangent is 0. We than use [P2 - P0] as a tangent. This means
                    // the average tangent at point P0 between both segments will change.
                    for( FArianeVertex* AttachedVertex : AttachedVertices )
                    {
                        for( FArianeSegment* AttachedSegment : AttachedVertex->GetSegments() )
                        {
                            AttachedSegment->GetVertex(0)->Invalidate();
                            AttachedSegment->GetVertex(1)->Invalidate();
                        }
                    }
                }
            }

            // adjust handle length to keep the same ratio as before the editing
            if( EditionMode == EArianePathEditToolEditionMode::Vertex )
            {
                for( FSegmentAdjustment& SegmentAdjustment : SegmentAdjustments )
                {
                    SegmentAdjustment.Adjust();
                }
            }

            // Update. Note: as the seen is invalidated, it will request a redraw
            Painting3DComponent->Update( true );
        }
    }

    return false;
}

void
UArianeEditorPathEditTool::OnMouseUpSliceSegments( FEditorViewportClient* ViewportClient
                                                 , FSceneView* View
                                                 , const FVector2D& ScreenMousePositionP0
                                                 , const FVector2D& ScreenMousePositionP1
                                                 , const TArray<FArianeSegment*>& Segments )
{
    FVector2D HUDMousePositionP0 = ScreenToHUD( ViewportClient, ScreenMousePositionP0 );
    FVector2D HUDMousePositionP1 = ScreenToHUD( ViewportClient, ScreenMousePositionP1 );

    auto FractionFilterFunction = [ ViewportClient
                                  , View
                                  , HUDMousePositionP0
                                  , HUDMousePositionP1 ]( FArianeSegment* Segment
                                                        , const FArianeSegment::FFraction& Fraction ) -> bool
    {
        FArianeObject* Owner = Segment->GetOwner();
        const FTransform& OwnerTransform = Owner->GetTransform();
        const FVector& LocalP0 = Fraction.Steps[0]->Point->GetPosition();
        const FVector& LocalP1 = Fraction.Steps[1]->Point->GetPosition();
        FVector WorldP0 = OwnerTransform.TransformPosition( LocalP0 );
        FVector WorldP1 = OwnerTransform.TransformPosition( LocalP1 );
        FVector2D HUDP0, HUDP1;

        bool bProjected =  WorldToHUD( ViewportClient, View, WorldP0, HUDP0 )
                        && WorldToHUD( ViewportClient, View, WorldP1, HUDP1 );

        if( bProjected && FArianeCore::IntersectSegment( HUDP0, HUDP1, HUDMousePositionP0, HUDMousePositionP1, nullptr, nullptr ) )
        {
            return true;
        }

        return false;
    };

    FVector RayOriginP0, RayDirectionP0;
    FVector RayOriginP1, RayDirectionP1;

    View->DeprojectFVector2D( ScreenMousePositionP0, RayOriginP0, RayDirectionP0 );
    View->DeprojectFVector2D( ScreenMousePositionP1, RayOriginP1, RayDirectionP1 );

    // We create a plane to cut the segment, this is necessary because we are in 3D. doing a 2D interplation would result in a loss
    // of precision for param T. For "point adding", the plane is computed from the fraction itself and leaving-ray at mouse click
    FVector SliceVector = ( RayOriginP1 - RayOriginP0 );
    FPlane CuttingPlane = FPlane( RayOriginP0, FVector::CrossProduct( RayDirectionP0, SliceVector ).GetSafeNormal() );

    auto GetCuttingPlaneFunction = [ CuttingPlane ]( FArianeSegment* Segment
                                                   , const FArianeSegment::FFraction& Fraction ) -> FPlane
    {
        return CuttingPlane;
    };

    OnMouseUpAddPoint( ViewportClient
                     , View
                     , Segments
                     , FractionFilterFunction
                     , GetCuttingPlaneFunction );
}

void
UArianeEditorPathEditTool::OnMouseUpAddPoint( FEditorViewportClient* ViewportClient
                                            , FSceneView* View
                                            , const FVector2D& ScreenMousePosition
                                            , const TArray<FArianeSegment*>& Segments )
{
    FVector2D HUDMousePosition = ScreenToHUD( ViewportClient, ScreenMousePosition );
    FVector CamUp = View->GetViewUp();
    FVector RayOrigin, RayDirection;

    View->DeprojectFVector2D( ScreenMousePosition, RayOrigin, RayDirection );

    auto FractionFilterFunction = [ ViewportClient
                                  , View
                                  , HUDMousePosition]( FArianeSegment* Segment
                                                     , const FArianeSegment::FFraction& Fraction ) -> bool
    {
        FArianeObject* Owner = Segment->GetOwner();
        const FTransform& OwnerTransform = Owner->GetTransform();
        const FVector& LocalP0 = Fraction.Steps[0]->Point->GetPosition();
        const FVector& LocalP1 = Fraction.Steps[1]->Point->GetPosition();
        FVector WorldP0 = OwnerTransform.TransformPosition( LocalP0 );
        FVector WorldP1 = OwnerTransform.TransformPosition( LocalP1 );
        FVector2D HUDP0, HUDP1;

        bool bProjected =  WorldToHUD( ViewportClient, View, WorldP0, HUDP0 )
                        && WorldToHUD( ViewportClient, View, WorldP1, HUDP1 );
        double HUDDistance;

        FArianeCore::DistanceToSegmentConstrained( HUDMousePosition, HUDP0, HUDP1, HUDDistance );

        if( bProjected && ( HUDDistance < SMALL_CIRCLE_RADIUS ) )
        {
            return true;
        }

        return false;
    };

    // We create a plane to cut the segment, this is necessary because we are in 3D. doing a 2D interplation would result in a loss
    // of precision for param T. For "point adding", the plane is computed from the fraction itself and leaving-ray at mouse click
    auto GetCuttingPlaneFunction = [ RayOrigin
                                   , RayDirection]( FArianeSegment* Segment
                                                  , const FArianeSegment::FFraction& Fraction ) -> FPlane
    {
        FArianeObject* Owner = Segment->GetOwner();
        const FTransform& OwnerTransform = Owner->GetTransform();
        FVector LocalDirection = Fraction.Direction;
        FVector WorldDirection = OwnerTransform.TransformVector( LocalDirection ).GetSafeNormal();
        FVector Perpendicular = FVector::CrossProduct( RayDirection, WorldDirection ).GetSafeNormal();
        FPlane CuttingPlane = FPlane( RayOrigin
                                    , FVector::CrossProduct( RayDirection, Perpendicular ).GetSafeNormal() );

        return CuttingPlane;
    };

    OnMouseUpAddPoint( ViewportClient
                     , View
                     , Segments
                     , FractionFilterFunction
                     , GetCuttingPlaneFunction );
}

void
UArianeEditorPathEditTool::OnMouseUpAddPoint( FEditorViewportClient* ViewportClient
                                            , FSceneView* View
                                            , const TArray<FArianeSegment*>& Segments
                                            , TFunction<bool( FArianeSegment* Segment
                                                            , const FArianeSegment::FFraction& Fraction )> FractionFilterFunction
                                            , TFunction<FPlane( FArianeSegment* Segment
                                                              , const FArianeSegment::FFraction& Fraction )> GetCuttingPlaneFunction )
{
    struct FSegmentCutRecord
    {
        double T;
        FArianeVertex* Vertex;
    };

    TMap<FArianeSegment*,TArray<FSegmentCutRecord>> Map;

    Map.Reserve ( 10 );

    GetToolManager()->BeginUndoTransaction(LOCTEXT("ariane-path-edit-tool.add-vertex","Add Vertex"));

    for( FArianeSegment* Segment : Segments )
    {
        FArianeObject* Owner = Segment->GetOwner();
        const FTransform& OwnerTransform = Owner->GetTransform();
        FArianeVertex* Vertex0 = Segment->GetVertex(0);
        FArianeVertex* Vertex1 = Segment->GetVertex(1);
        FVector Vertex0Normal = Vertex0->GetNormal();
        FVector Vertex1Normal = Vertex1->GetNormal();
        double Vertex0Radius = Vertex0->GetRadius();
        double Vertex1Radius = Vertex1->GetRadius();

        Owner->GetDrawing()->Modify();

        for( const FArianeSegment::FFraction& Fraction : Segment->GetFractions() )
        {
            const FVector& LocalP0 = Fraction.Steps[0]->Point->GetPosition();
            const FVector& LocalP1 = Fraction.Steps[1]->Point->GetPosition();
            FVector WorldP0 = OwnerTransform.TransformPosition( LocalP0 );
            FVector WorldP1 = OwnerTransform.TransformPosition( LocalP1 );
            FVector LocalDirection = Fraction.Direction;
            FVector WorldDirection = OwnerTransform.TransformVector( LocalDirection ).GetSafeNormal();

            // 2D Filtering (fraction is under the mouse or crosses the line drawn by the mouse
            if( FractionFilterFunction( Segment, Fraction ) )
            {
                FPlane CuttingPlane = GetCuttingPlaneFunction( Segment, Fraction );

                // Make sure the fraction crosses the plane (not colinear to the plane, i.e perpendicular to the normal component of the plane)
                if( fabs( FVector::DotProduct( WorldDirection, CuttingPlane.GetNormal() ) ) > UE_SMALL_NUMBER )
                {
                    FVector WorldIntersection = FMath::LinePlaneIntersection ( WorldP0, WorldP1, CuttingPlane );
                    FVector LocalIntersection = OwnerTransform.InverseTransformPosition( WorldIntersection );
                    // T must be signed, hence the use of the dot product
                    const double T = FVector::DotProduct( LocalIntersection - LocalP0, LocalP1 - LocalP0 ) / ( Fraction.Length * Fraction.Length );

                    if( FMath::IsWithin(T, 0.0, 1.0) && ( FVector::DotProduct( ( LocalIntersection - LocalP0 )
                                                                             , ( LocalP1           - LocalP0 ) ) > 0.0f ) )
                    {
                        double SegmentT = Fraction.Steps[0]->T + ( T * ( Fraction.Steps[1]->T - Fraction.Steps[0]->T ) );

                        if( Owner->GetClass() == FArianePath::StaticClass() )
                        {
                            FArianePath* Path = static_cast<FArianePath*>(Owner);

                            FArianeVertex* NewVertex = Path->AllocVertex( LocalIntersection
                                                                        , Vertex0Normal + ( SegmentT * ( Vertex1Normal - Vertex0Normal ) )
                                                                        , Vertex0Radius + ( SegmentT * ( Vertex1Radius - Vertex0Radius ) )
                                                                        , EArianeAllocationModel::InstancedStruct );

                            Path->AddVertex( NewVertex );

                            // we store the Cut records in an array because there can be multiple cuts per segment in case of non-linear segments
                            TArray<FSegmentCutRecord>& SegmentCutRecord = Map.FindOrAdd(Segment);
                            // Note: the records will be ordered by design
                            SegmentCutRecord.Emplace( FSegmentCutRecord{ .T = SegmentT, .Vertex = NewVertex } );
                        }
                    }
                }
            }
        }
    }

    for( TPair<FArianeSegment*,TArray<FSegmentCutRecord>>& Pair : Map )
    {
        FArianeSegment* Segment = Pair.Key;
        TArray<FSegmentCutRecord>* SegmentCutRecords = &Pair.Value;
        FArianeObject* SegmentOwner = Segment->GetOwner();

        if( SegmentOwner->GetClass() == FArianePath::StaticClass() )
        {
            FArianePath* Path = static_cast<FArianePath*>(SegmentOwner);
            FArianeVertex* PrevVertex = Segment->GetVertex(0);
            double PrevVertexT = 0.0f;

            if( SegmentCutRecords->Num() )
            {
                // add a final record to "close the gap"
                SegmentCutRecords->Emplace( FSegmentCutRecord{ .T = 1.0f, .Vertex = Segment->GetVertex(1) } );

                for( FSegmentCutRecord& SegmentCutRecord : *SegmentCutRecords )
                {
                    FArianeVertex* NextVertex = SegmentCutRecord.Vertex;
                    double NextVertexT = SegmentCutRecord.T;

                    FArianeSegment* NewSegment = Segment->Extract( SegmentOwner
                                                                 , PrevVertex
                                                                 , PrevVertexT
                                                                 , NextVertex
                                                                 , NextVertexT
                                                                 , EArianeAllocationModel::InstancedStruct );


                    Path->AddSegment( NewSegment );

                    PrevVertex = NextVertex;
                    PrevVertexT = NextVertexT;
                }

                Path->RemoveSegment( Segment, true );
            }
        }
    }

    GetToolManager()->EndUndoTransaction();
}

void
UArianeEditorPathEditTool::OnMouseUpDeletePoint( const TArray<FArianePoint*>& PickedPoints )
{
    // temporary structure to store the path that will have their vertices removed.
    // the path are retrived via the quadtree that is built by the HUD. Each quad
    // stores points (vertices), and we retrieve the path from those vertices.
    // then we can pass the data to FOdysseyVectorPath::DeleteVertex()
    struct FAlteredPathRecord
    {
        FArianePath* Path;
        TArray<FArianeVertex*> VertexArray;

        FAlteredPathRecord( FArianePath* InPath )
        {
            Path = InPath;
            VertexArray.Reserve( 10 );
        }
    };

    TArray<UArianeLayerVector*> VectorLayers;
    TArray<FArianePath*> RemovedPaths;
    TArray<FArianeVertex*> RemovedVertices;
    TArray<FArianeSegment*> RemovedSegments;
    TArray<FArianePath*> AddedPaths;
    TArray<FArianeVertex*> AddedVertices; // not filled, here just for the undo record
    TArray<FArianeSegment*> AddedSegments;
    TArray<FAlteredPathRecord> AlteredPathRecords;
    bool hasHit = false;

    AlteredPathRecords.Reserve( 10 );
    RemovedPaths.Reserve( 10 );
    RemovedVertices.Reserve( 10 );
    RemovedSegments.Reserve( 10 );
    AddedSegments.Reserve( 10 );

    for( FArianePoint* Point : PickedPoints )
    {
        if( Point->GetClass() == FArianeVertex::StaticClass() )
        {
            FArianeVertex* Vertex = static_cast<FArianeVertex*>(Point);
            FArianeObject* Owner = Vertex->GetOwner();

            if( Owner->GetClass() == FArianePath::StaticClass() )
            {
                FArianePath* OwnerPath = static_cast<FArianePath*>(Owner);

                FAlteredPathRecord* AlteredPathRecord = AlteredPathRecords.FindByPredicate(
                                                           [OwnerPath]( const FAlteredPathRecord& AlteredPathRecord ) -> bool
                                                           {
                                                               return ( AlteredPathRecord.Path == OwnerPath );
                                                           } );

                if( AlteredPathRecord == nullptr )
                {
                    AlteredPathRecord = &AlteredPathRecords.Emplace_GetRef( OwnerPath );
                }

                AlteredPathRecord->VertexArray.Add( Vertex );
            }

            if( VectorLayers.Find( Owner->GetDrawing()->GetVectorLayer().Get() ) == INDEX_NONE )
            {
                VectorLayers.Add( Owner->GetDrawing()->GetVectorLayer().Get() );
            }
        }
    }

    GetToolManager()->BeginUndoTransaction(LOCTEXT("ariane-path-edit-tool.delete-vertex","Delete Vertex"));

    // Snapshot for undos
    for( UArianeLayerVector* VectorLayer : VectorLayers )
    {
        VectorLayer->GetDrawing()->Modify();
    }

    for( FAlteredPathRecord& AlteredPathRecord : AlteredPathRecords )
    {
        if( AlteredPathRecord.Path->DeleteVertex( AlteredPathRecord.VertexArray
                                                , nullptr/*RemovedVertices*/
                                                , nullptr/*RemovedSegments*/
                                                , nullptr/*AddedSegments*/ ) )
        {
            RemovedPaths.Add( AlteredPathRecord.Path );
        }
    }

    for( FArianePath* RemovedPath : RemovedPaths )
    {
        RemovedPath->GetParent()->RemoveChild( RemovedPath, true );
    }

    GetToolManager()->EndUndoTransaction();
}

void
UArianeEditorPathEditTool::RebuildQuadTree( FEditorViewportClient* ViewportClient
                                          , FSceneView* View )
{
    UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(GetCurrentLayer());

    if( VectorLayer )
    {
        MakePointQuadTree( ViewportClient, View, {VectorLayer}, true );
        MakeSegmentQuadTree( ViewportClient, View, {VectorLayer}, true );
    }
}

bool
UArianeEditorPathEditTool::OnMouseUp( FEditorViewportClient* ViewportClient
                                    , FSceneView* View
                                    , const FKey& iKey
                                    , const FArianePointerState& PointerState )
{
    bMouseButtonDown = false;

    if( iKey == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        if( Painting3DComponent )
        {
            switch( EditionMode )
            {
                case EArianePathEditToolEditionMode::Vertex :
                case EArianePathEditToolEditionMode::VertexHandle :
                case EArianePathEditToolEditionMode::SegmentHandle :
                    // Link or Unlink segment handles
                    if( ( PickedVertices.Num() == 1 ) && ( PickedHandles.Num() == 0 ) )
                    {
                        FArianeVertex* Vertex = PickedVertices[0];

                        Vertex->SetHandleAligned( Vertex->IsHandleAligned() ? false : true );
                    }

                    GetToolManager()->EndUndoTransaction();
                break;

                case EArianePathEditToolEditionMode::Alter :
                {
                    if( HoveredPoints.Num() )
                    {
                        OnMouseUpDeletePoint( HoveredPoints );
                    }
                    else
                    {
                        if( HoveredSegments.Num() )
                        {
                            FVector2D ScreenMousePosition = FVector2D( PointerState.ViewportX, PointerState.ViewportY );

                            OnMouseUpAddPoint( ViewportClient, View, ScreenMousePosition, HoveredSegments );
                        }
                        else
                        {
                            FVector2D ScreenMin = FVector2D( FMath::Min( ScreenMousePositionAtDown.X, ScreenMousePositionAtDrag.X )
                                                           , FMath::Min( ScreenMousePositionAtDown.Y, ScreenMousePositionAtDrag.Y ) );
                            FVector2D ScreenMax = FVector2D( FMath::Max( ScreenMousePositionAtDown.X, ScreenMousePositionAtDrag.X )
                                                           , FMath::Max( ScreenMousePositionAtDown.Y, ScreenMousePositionAtDrag.Y ) );
                            TArray<FArianeSegment*> SegmentsInRect;
                            FBox2D AreaRect = FBox2D( ScreenMin, ScreenMax );

                            SegmentQuadTree->PickSegmentsInRect( ViewportClient, View, AreaRect, SegmentsInRect );

                            OnMouseUpSliceSegments( ViewportClient
                                                  , View
                                                  , ScreenMousePositionAtDown
                                                  , ScreenMousePositionAtDrag
                                                  , SegmentsInRect );
                        }
                    }
                }
                break;

                default:
                {
                    // If nothing was selected, we pick an object
                    if( ( PickedVertices.Num() == 0 ) && ( PickedHandles.Num() == 0 ) )
                    {
                        ////PickObjects( iScene, iPointInTexture.x, iPointInTexture.y );
                    }
                }
                break;
            }

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

UArianeEditorTool::FPickingFlags
UArianeEditorPathEditTool::EditonModeToPickingFlags()
{
    FPickingFlags PickingFlags;

    switch( EditionMode )
    {
        case EArianePathEditToolEditionMode::Alter :
        case EArianePathEditToolEditionMode::Vertex :
        {
            PickingFlags.SetPathVertex();
        }
        break;

        case EArianePathEditToolEditionMode::SegmentHandle :
        {
            PickingFlags.SetPathSegmentHandle()
                        .SetPathVertex(); // for vertex handle alignment
        }
        break;

        case EArianePathEditToolEditionMode::VertexHandle :
        {
            PickingFlags.SetPathVertexHandle();
        }
        break;

        default :
        {
        }
        break;
    }

    return PickingFlags;
}

FArianeEditorHUD::FDrawingFlags
UArianeEditorPathEditTool::EditonModeToHUDDrawingFlags()
{
    FArianeEditorHUD::FDrawingFlags HUDDrawingFlags = Editor->GetHUDDrawingFlags();

    switch( EditionMode )
    {
        case EArianePathEditToolEditionMode::Alter :
        case EArianePathEditToolEditionMode::Vertex :
        {
            HUDDrawingFlags.SetPathVertex()
                           .SetPathSegment();
        }
        break;

        case EArianePathEditToolEditionMode::SegmentHandle :
        {
            HUDDrawingFlags.SetPathVertex()
                           .SetPathSegment()
                           .SetPathSegmentHandle();
        }
        break;

        case EArianePathEditToolEditionMode::VertexHandle :
        {
            HUDDrawingFlags.SetPathVertex()
                           .SetPathVertexHandle()
                           .SetPathSegment();
        }
        break;

        default :
        {
        }
        break;
    }

    return HUDDrawingFlags;
}

void
UArianeEditorPathEditTool::Render(IToolsContextRenderAPI* RenderAPI)
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
UArianeEditorPathEditTool::DrawHUD ( FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI )
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
    FArianeEditorHUD::FDrawingFlags HUDDrawingFlags = EditonModeToHUDDrawingFlags();
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

    if( EditionMode == EArianePathEditToolEditionMode::Alter )
    {
        // Draw a minus sign
        if( HoveredPoints.Num() )
        {
            FVector2D MinusP0 = FVector2D( HUDPosition.X + PickingRadius
                                         , HUDPosition.Y - PickingRadius );
            FVector2D MinusP1 = FVector2D( HUDPosition.X + PickingRadius + 8
                                         , HUDPosition.Y - PickingRadius );

            DrawLineHUD( Canvas, ViewportClient, View, MinusP0, MinusP1, HcColor, 1.0f );
        }
        // Draw a plus sign
        else
        {
            //if( HoveredSegments.Num() )
            {
                FVector2D PlusLine0P0 = FVector2D( HUDPosition.X + PickingRadius
                                                 , HUDPosition.Y - PickingRadius );
                FVector2D PlusLine0P1 = FVector2D( HUDPosition.X + PickingRadius + 8
                                                 , HUDPosition.Y - PickingRadius );
                FVector2D PlusLine1P0 = FVector2D( HUDPosition.X + PickingRadius + 4
                                                 , HUDPosition.Y - PickingRadius - 4 );
                FVector2D PlusLine1P1 = FVector2D( HUDPosition.X + PickingRadius + 4
                                                 , HUDPosition.Y - PickingRadius + 4 );

                DrawLineHUD( Canvas, ViewportClient, View, PlusLine0P0, PlusLine0P1, HcColor, 1.0f );
                DrawLineHUD( Canvas, ViewportClient, View, PlusLine1P0, PlusLine1P1, HcColor, 1.0f );
            }
            //else
            {
                FVector2D LineP0 = ScreenToHUD ( ViewportClient, ScreenMousePositionAtDown );
                FVector2D LineP1 = ScreenToHUD ( ViewportClient, ScreenMousePositionAtDrag );

                // cutting Line
                if( IsMouseButtonDown() == true )
                {
                    DrawLineHUD( Canvas, ViewportClient, View, LineP0, LineP1, HcColor, 1.0f );
                }
            }
        }

        if( IsMouseButtonDown() == false )
        {
             // small fixed-size picking circle
            DrawCircleHUD( Canvas
                         , ViewportClient
                         , View
                         , HUDPosition
                         , SMALL_CIRCLE_RADIUS
                         , HcColor
                         , 1.0f );
         }
    }
    else
    {
         // picking circle
        DrawCircleHUD( Canvas
                     , ViewportClient
                     , View
                     , HUDPosition
                     , PickingRadius
                     , HcColor
                     , 1.0f );
    }
}

bool
UArianeEditorPathEditTool::SupportsColorType( EOdysseyPainterEditorColorType ColorType )
{
    return true;
}

EArianePathEditToolEditionMode
UArianeEditorPathEditTool::GetEditionMode()
{
    return EditionMode;
}

void
UArianeEditorPathEditTool::SetEditionMode( EArianePathEditToolEditionMode iMode )
{
    EditionMode = iMode;
}

const FSlateBrush*
UArianeEditorPathEditTool::GetBackgroundBrush( EArianePathEditToolEditionMode iMode ) const
{
    static FSlateColorBrush selected = FSlateColorBrush( FStyleColors::Select );

    return ( iMode == EditionMode ) ? &selected : nullptr;
}

TSharedRef<SWidget>
UArianeEditorPathEditTool::CreateModifierSegmentControl()
{
    return SNew(SSegmentedControl<EArianePathEditToolEditionMode>)
           .Value_Lambda( [this]{ return EditionMode; } )
           .SupportsEmptySelection( false )
           .SupportsMultiSelection( false )
           .IsEnabled( false ) // currently not clickable - Info only
           .UniformPadding( FMargin( 2, 0, 2, 0 ) )
           .OnValueChanged( SSegmentedControl<EArianePathEditToolEditionMode>::FOnValueChanged::CreateUObject( this, &UArianeEditorPathEditTool::SetEditionMode ) )
           // DEFAULT
           + SSegmentedControl<EArianePathEditToolEditionMode>::Slot( EArianePathEditToolEditionMode::Vertex )
           .ToolTip( LOCTEXT("ariane-path-edit-tool.edition-mode.default.name", "Default") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UArianeEditorPathEditTool::GetBackgroundBrush, EArianePathEditToolEditionMode::Vertex  )
               [
                   SNew(SImage)
                   .Image( FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsShortcuts.PathEditMoveVertex20") )
               ]
           ]
           // CTRL
           + SSegmentedControl<EArianePathEditToolEditionMode>::Slot( EArianePathEditToolEditionMode::SegmentHandle )
#if PLATFORM_WINDOWS
           .ToolTip( LOCTEXT("ariane-path-edit-tool.edition-mode.ctrl.name", "Deform Segment (CTRL)") )
#endif
#if PLATFORM_MAC
           .ToolTip( LOCTEXT("ariane-path-edit-tool.edition-mode.cmd.name", "Deform Segment (CMD)") )
#endif
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UArianeEditorPathEditTool::GetBackgroundBrush, EArianePathEditToolEditionMode::SegmentHandle  )
               [
                   SNew(SImage)
                   .Image( FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsShortcuts.PathEditDeformSegment20") )
               ]
           ]
           // SHIFT
           + SSegmentedControl<EArianePathEditToolEditionMode>::Slot( EArianePathEditToolEditionMode::VertexHandle )
           .ToolTip( LOCTEXT("ariane-path-edit-tool.edition-mode.shift.name", "Widen Vertex (SHIFT)") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UArianeEditorPathEditTool::GetBackgroundBrush, EArianePathEditToolEditionMode::VertexHandle  )
               [
                   SNew(SImage)
                   .Image( FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsShortcuts.PathEditWidenVertex20") )
               ]
           ]
           // ALT
           + SSegmentedControl<EArianePathEditToolEditionMode>::Slot( EArianePathEditToolEditionMode::Alter )
           .ToolTip( LOCTEXT("ariane-path-edit-tool.edition-mode.alt.name", "Add/Remove Vertex (ALT)") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UArianeEditorPathEditTool::GetBackgroundBrush, EArianePathEditToolEditionMode::Alter  )
               [
                   SNew(SImage)
                   .Image( FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsShortcuts.PathEditAddRemoveVertex20") )
               ]
           ];
}

void
UArianeEditorPathEditTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);

    FToolMenuSection& section = iToolMenu->AddSection(NAME_None);
    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            CreateModifierSegmentControl(),
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UArianeEditorPathEditTool, PickingRadius ), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UArianeEditorPathEditTool, bWidenAllAlong ), FSinglePropertyParams())
                .InnerPadding(10.f)
            ],
            FText()
        )
    );
}

void
UArianeEditorPathEditTool::ExtendContextMenu( FMenuBuilder& menu )
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
