// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor headers
#include "PathEditTool/ArianeEditorPathEditTool.h"
#include "ArianeEditor.h"
#include "ArianeEditorStyle.h"
// Ariane headers
#include "ArianePainting3DComponent.h"
#include "ArianeImage.h"
#include "ArianeLayerDrawing.h"
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

#define LOCTEXT_NAMESPACE "ArianeEditor"

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
    , EditionMode  ( EArianePathEditToolEditionMode::Vertex )
{
    Icon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsTab.PathEdit64");

    bHasContextMenu = true;
}

void
UArianeEditorPathEditTool::OnPostUpdate( bool bInteractive )
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
    UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(GetCurrentLayer());

    ResetQuadTree();

    SelectedTrees.Empty();

    if( DrawingLayer )
    {
        DrawingLayer->GetImage()->GetSelectedTrees( SelectedTrees );
    }
}

void
UArianeEditorPathEditTool::BindDelegates()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        Painting3DComponent->OnPostUpdateDelegate().AddUObject( this, &UArianeEditorPathEditTool::OnPostUpdate );
    }

    Editor->OnPost3DPaintingComponentSelectionChangedDelegate().AddUObject( this, &UArianeEditorPathEditTool::Reset );
}

void
UArianeEditorPathEditTool::UnbindDelegates()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    Editor->OnPost3DPaintingComponentSelectionChangedDelegate().RemoveAll( this );

    if( Painting3DComponent )
    {
        Painting3DComponent->OnPostUpdateDelegate().RemoveAll( this );
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

    BindDelegates();
}

void
UArianeEditorPathEditTool::Inactivate()
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();

    UnbindDelegates();

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
    if( InKeyEvent.IsRepeat() == false )
    {
        if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() && ( FSlateApplication::Get().GetModifierKeys().IsControlDown()
                                                                        || FSlateApplication::Get().GetModifierKeys().IsCommandDown() ) )
        {
            FEditorViewportClient* ViewportClient = GetActiveViewportClient();
            UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(GetCurrentLayer());

            EditionMode  = EArianePathEditToolEditionMode::Alter;

            if( ViewportClient && DrawingLayer )
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
UArianeEditorPathEditTool::BuildSegmentAdjustments( const TArray<FArianeSegment*>& Segments
                                                  , TArray<FSegmentAdjustment>& OutSegmentAdjustments )
{
    OutSegmentAdjustments.Reserve( Segments.Num() );

    for( const FArianeSegmentID& SegmentID : Segments )
    {
        FArianeSegment* Segment = const_cast<FArianeSegmentID&>(SegmentID).GetSegment();

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
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        for( FArianeObject* SelectedTree : SelectedTrees )
        {
            FArianeObject::Traverse ( SelectedTree
                                    , [ this
                                      , ViewportClient
                                      , View
                                      , &PointerState ]( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
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
                                        , PointerState.ViewportX
                                        , PointerState.ViewportY
                                        , PickingRadius
                                        , PickedVertices
                                        , PickedHandles
                                        , EditonModeToPickingFlags() );
                        }
                    }

                    return FArianeObject::ETraversalReturnValue::Continue;
                } );
        }

        // Link or Unlink segment handles
        if( ( EditionMode == EArianePathEditToolEditionMode::SegmentHandle ) &&  ( PickedVertices.Num() == 1 ) )
        {
            FArianeVertex* Vertex = PickedVertices[0];

            //-------------- undo ---------------//
            GetToolManager()->BeginUndoTransaction(LOCTEXT("ariane-path-edit-tool.transaction.align-point-selection","Align Point Selection"));
            DrawingLayer->GetImage()->Modify();

            Vertex->SetHandleAligned( Vertex->IsHandleAligned() ? false : true );

            GetToolManager()->EndUndoTransaction();
        }
        // Else, save point coordinates before changing them
        else
        {
            switch( EditionMode )
            {
                case EArianePathEditToolEditionMode::VertexHandle :
                    GetToolManager()->BeginUndoTransaction(LOCTEXT("ariane-path-edit-tool.edit-vertex-handles","Edit Vertex Handles"));
                    DrawingLayer->GetImage()->Modify();
                break;

                case EArianePathEditToolEditionMode::Vertex :
                {
                    GetToolManager()->BeginUndoTransaction(LOCTEXT("ariane-path-edit-tool.edit-vertices","Edit Vertices"));
                    DrawingLayer->GetImage()->Modify();

                    TArray<FArianeSegment*> AlteredSegments;

                    // static call
                    FArianeVertex::ArrayToSegmentArray( PickedVertices, AlteredSegments, false );
                    // static call
                    BuildSegmentAdjustments( AlteredSegments, SegmentAdjustments );

                    // Control points must move with the point. Store them in the iPickedHandleArray
                    for( FArianeVertex* Vertex : PickedVertices )
                    {
                        for( const FArianeSegmentID& SegmentID : Vertex->GetSegments() )
                        {
                            FArianeSegment* Segment = const_cast<FArianeSegmentID&>(SegmentID).GetSegment();

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
                    DrawingLayer->GetImage()->Modify();
                break;

                default:
                break;
            }
        }

        // remember vertex position at mouse down
        PickedVertexDisplacements.Reserve( PickedVertices.Num() );
        for( FArianeVertex* Vertex : PickedVertices )
        {
            const FTransform& OwnerTransform = Vertex->GetOwner()->GetTransform();

            PickedVertexDisplacements.Emplace( Vertex
                                             , View->GetViewDirection()
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
                                             , View->GetViewDirection()
                                             , OwnerTransform
                                             , RayOrigin
                                             , RayDirection );
        }

        DrawingLayer->Update( true );
    }
}

bool
UArianeEditorPathEditTool::OnMouseDown( FEditorViewportClient* ViewportClient
                                      , FSceneView* View
                                      , const FKey& Key
                                      , const FArianePointerState& PointerState
                                      , bool iRepeat )
{
    if( Key == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        if( Painting3DComponent )
        {
            //painting3DComponent->PrintPointers();
            UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
            UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(LayerStack->GetCurrentLayer());

            if( DrawingLayer )
            {
                //mPointInTextureAtDown = ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y );

                //mPathEditHUD->SetCutLineP0( iPointInTexture.x, iPointInTexture.y );
                //mPathEditHUD->SetCutLineP1( iPointInTexture.x, iPointInTexture.y );

                DrawingLayer->GetImage()->Modify();

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
        HoveredPoints.Empty();

        PointQuadTree->PickPoints( FVector2D( ViewportClient->Viewport->GetMouseX()
                                            , ViewportClient->Viewport->GetMouseY() )
                                 , PickingRadius
                                 , HoveredPoints );
    }
}

void
UArianeEditorPathEditTool::Render( IToolsContextRenderAPI* RenderAPI )
{

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
UArianeEditorPathEditTool::OnMouseDrag( FEditorViewportClient* ViewportClient
                                      , FSceneView* View
                                      , const FKey& iKey
                                      , const FArianePointerState& PointerState )
{
    FVector RayOrigin;
    FVector RayDirection;

    View->DeprojectFVector2D( FVector2D( PointerState.ViewportX, PointerState.ViewportY )
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
                                    , bWidenAllAlong && ( PickedVertices.Num() == 1 ) );
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
                    FPointDisplacement& PointDisplacement = PickedHandleDisplacements[i];

                    DisplacePoint( Handle
                                 , PointDisplacement
                                 , Handle->GetOwnerSegment()->GetOwner()->GetTransform()
                                 , RayOrigin
                                 , RayDirection );
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

    TArray<UArianeLayerDrawing*> DrawingLayers;
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

            if( DrawingLayers.Find( Owner->GetImage()->GetDrawingLayer().Get() ) == INDEX_NONE )
            {
                DrawingLayers.Add( Owner->GetImage()->GetDrawingLayer().Get() );
            }
        }
    }

    GetToolManager()->BeginUndoTransaction(LOCTEXT("ariane-path-edit-tool.delete-vertex","Delete Vertex"));

    // Snapshot for undos
    for( UArianeLayerDrawing* DrawingLayer : DrawingLayers )
    {
        DrawingLayer->GetImage()->Modify();
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
    UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(GetCurrentLayer());

    if( DrawingLayer )
    {
        MakePointQuadTree( ViewportClient, View, {DrawingLayer}, true );
    }
}

bool
UArianeEditorPathEditTool::OnMouseUp( FEditorViewportClient* ViewportClient
                                    , FSceneView* View
                                    , const FKey& iKey
                                    , const FArianePointerState& PointerState )
{

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
                    GetToolManager()->EndUndoTransaction();
                break;

                case EArianePathEditToolEditionMode::Alter :
                {
                    ////TArray<FArianePoint*>& HoveredPointArray = PathEditHUD->GetHoveredPointArray();

                    // This populates mPathEditHUD::mHoveredPointArray
                    //mPathEditHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

                    if( HoveredPoints.Num() )
                    {
                        OnMouseUpDeletePoint( HoveredPoints );
                    }
                    else
                    {
/*
                        TArray<FArianeSegment*> PickedSegmentArray;

                        // check if we picked any segment
                        PickSegments( iScene
                                    , iPointInTexture.x
                                    , iPointInTexture.y
                                    , PickingRadius
                                    , true
                                    , false
                                    , pickedSegmentArray );

                        if( pickedSegmentArray.size() )
                        {
                            OnMouseUpAddPoint( iScene
                                             , iPointInTexture
                                             , pickedSegmentArray );
                        }
                        else
                        {
                            OnMouseUpCutPaths( iScene, iPointInTexture );
                        }
*/
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

    if( Painting3DComponent )
    {
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( DrawingLayer )
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
        if(  HoveredPoints.Num() )
        {
            FVector2D MinusP0 = FVector2D( MousePosition.X + PickingRadius
                                         , MousePosition.Y - PickingRadius );
            FVector2D MinusP1 = FVector2D( MousePosition.X + PickingRadius + 8
                                         , MousePosition.Y - PickingRadius );

            DrawLineHUD( Canvas, ViewportClient, View, MinusP0, MinusP1, HcColor, 1.0f );
        }
        else
        {
/*
            FVector2D lineP0 = iParams.mTextureToHUD.Execute( FVector2D( mCutLinePoint[0].x, mCutLinePoint[0].y ) );
            FVector2D lineP1 = iParams.mTextureToHUD.Execute( FVector2D( mCutLinePoint[1].x, mCutLinePoint[1].y ) );

            // we are NOT over a vertex, draw a plus sign
            DrawPrimitivePlus( iParams
                             , FVector2D( MousePosition.X + PickingRadius
                                        , MousePosition.Y - PickingRadius )
                             , 4
                             , HcColor
                             , 1.0f );

            // cutting Line
            DrawPrimitiveLine( iParams, lineP0, lineP1, HcColor, 1.0f );
*/
        }
    }

     // picking circle
    DrawCircleHUD( Canvas
                 , ViewportClient
                 , View
                 , FVector2D( ViewportClient->GetCachedMouseX()
                            , ViewportClient->GetCachedMouseY() )
                 , PickingRadius
                 , HcColor
                 , 1.0f );
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
