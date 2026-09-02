// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor headers
#include "EraserTool/ArianeEditorEraserTool.h"
#include "ArianeEditor.h"
#include "ArianeEditorStyle.h"
// Ariane headers
#include "ArianePainting3DComponent.h"
#include "ArianePath.h"
#include "ArianeGroup.h"
#include "ArianeVertex.h"
#include "ArianeLayerStack.h"
#include "ArianeLayerDrawing.h"
#include "ArianeImage.h"
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

UArianeEditorEraserTool::~UArianeEditorEraserTool()
{
}

UArianeEditorEraserTool::UArianeEditorEraserTool()
    : Size( 20.0f )
    , bSplit ( true )
    , CanvasRenderTarget ( nullptr )
    , Brush( nullptr )
{
    Icon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsTab.Eraser64");

    bHasContextMenu = true;
}

void
UArianeEditorEraserTool::Init( FArianeEditor* InEditor )
{
    Super::Init( InEditor );

    CanvasRenderTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D( GetWorld()
                                                                          , UCanvasRenderTarget2D::StaticClass()
                                                                          , 1024
                                                                          , 1024 );

    //CanvasRenderTarget->SetShouldClearRenderTargetOnReceiveUpdate( false );
    //CanvasRenderTarget->OnCanvasRenderTargetUpdate.AddDynamic( this, &UArianeEditorEraserTool::StampBrush );
    //CanvasRenderTarget->UpdateResource();

    ClearCanvas();

    Brush = UTexture2D::CreateTransient( Size, Size, PF_B8G8R8A8 );

    Brush->UpdateResource();

    UpdateBrush();
}

UArianeEditorEraserTool::FWayFragment::FWayFragment( FArianeSegment* InSegment
                                                   , FWayPoint* InWayPoint0
                                                   , FWayPoint* InWayPoint1
                                                   , bool bInErased )
{
    Segment = InSegment;

    WayPoint0 = InWayPoint0;
    WayPoint1 = InWayPoint1;

    WayPoint0->Fragments.Push( this );
    WayPoint1->Fragments.Push( this );

    bErased = bInErased;

    if( WayPoint0->Flags & FWayPoint::Original )
    {
        T0 = WayPoint0->OriginalVertex->GetIndex( Segment );
    }
    else
    {
        T0 = WayPoint0->T;
    }

    if( WayPoint1->Flags & FWayPoint::Original )
    {
        T1 = WayPoint1->OriginalVertex->GetIndex( Segment );
    }
    else
    {
        T1 = WayPoint1->T;
    }
/*
    if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
        ::ULIS::FVec2D* cubicSegmentBezier = cubicSegment->GetBezier();

        if( t0 < t1 )
        {
            FOdysseyVector::BezierExtract( cubicSegmentBezier[0]
                                         , cubicSegmentBezier[1]
                                         , cubicSegmentBezier[2]
                                         , cubicSegmentBezier[3]
                                         , t0
                                         , t1
                                         , bezier[0]
                                         , bezier[1]
                                         , bezier[2]
                                         , bezier[3] );
        }
        else
        {
            FOdysseyVector::BezierExtract( cubicSegmentBezier[0]
                                         , cubicSegmentBezier[1]
                                         , cubicSegmentBezier[2]
                                         , cubicSegmentBezier[3]
                                         , t1
                                         , t0
                                         , bezier[3]
                                         , bezier[2]
                                         , bezier[1]
                                         , bezier[0] );
        }
    }
*/
}

UArianeEditorEraserTool::FWayFragment*
UArianeEditorEraserTool::FWayPoint::GetOtherFragment( FWayFragment* WayFragment )
{
    for( FWayFragment* CandidateFragment : Fragments )
    {
        if( CandidateFragment != WayFragment )
        {
            return CandidateFragment;
        }
    }

    return nullptr;
}

UArianeEditorEraserTool::FWayPoint*
UArianeEditorEraserTool::FWayFragment::GetOtherWayPoint( FWayPoint* WayPoint )
{
    if( WayPoint0 == WayPoint )
    {
        return WayPoint1;
    }

    if( WayPoint1 == WayPoint )
    {
        return WayPoint0;
    }

    return nullptr;
}

FVector2D
UArianeEditorEraserTool::ProjectWorldToHUD( FEditorViewportClient* ViewportClient
                                          , FSceneView* View
                                          , const FVector& WorldPosition )
{
    FVector2D ScreenCoords;
    FIntRect Rect = FIntRect( 0, 0, ViewportClient->Viewport->GetSizeXY().X, ViewportClient->Viewport->GetSizeXY().Y );

    View->ProjectWorldToScreen( WorldPosition
                              , Rect
                              , View->ViewMatrices.GetViewProjectionMatrix()
                              , ScreenCoords
                              , true ); // calc outside view position

    return ScreenToHUD( ViewportClient, ScreenCoords );
}

void
UArianeEditorEraserTool::VertexToWaypoint( FEditorViewportClient* ViewportClient
                                         , FSceneView* View
                                         , const FTransform& WorldTransform
                                         , FArianeVertex* Vertex
                                         , double T
                                         , const TArray<FColor>& Pixels
                                         , TArray<FWayPoint>& OutWayPoints )
{
    FVector2D HUDCoords = ProjectWorldToHUD( ViewportClient
                                           , View
                                           , WorldTransform.TransformPosition( Vertex->GetPosition() ) );

    uint8 AlphaValue = GetAlpha( HUDCoords.X, HUDCoords.Y, Pixels );

    if( AlphaValue == 0 ) // vertex in dark zone, keep it
    {
        HUDCoords = ProjectWorldToHUD( ViewportClient
                                     , View
                                     , WorldTransform.TransformPosition( Vertex->GetPosition() ) );

        OutWayPoints.Emplace( Vertex
                            , T
                            , FWayPoint::OutsideErasureArea
                            | FWayPoint::Original );
    }
    else
    {
        OutWayPoints.Emplace( Vertex
                            , T
                            , FWayPoint::InsideErasureArea
                            | FWayPoint::Original );
    }
}

// returns true if there were any intersection with the erasure zone
bool
UArianeEditorEraserTool::CheckContrast( uint8 iAlphaValue0, uint8 iAlphaValue1 )
{
    if( ( ( iAlphaValue0 == 0 ) && iAlphaValue1 )
     || ( iAlphaValue0 && ( iAlphaValue1 == 0 ) ) )
    {
        return true;
    }

    return false;
}

// returns current alpha value
uint8
UArianeEditorEraserTool::GetAlpha( int32 X, int32 Y, const TArray<FColor>& Pixels )
{
    if( ( X >= 0 ) && ( X < CanvasRenderTarget->SizeX  )
     && ( Y >= 0 ) && ( Y < CanvasRenderTarget->SizeY  ) )
    {
        uint32 Offset = ( Y * CanvasRenderTarget->SizeX ) + X;

        return Pixels[Offset].A;
    }

    return 0;
}

void
UArianeEditorEraserTool::TraceLine( FArianePath* Path
                                  , FArianeSegment* Segment
                                  , const FArianeSegment::FFractionStep* Step0
                                  , int32 ScreenX0
                                  , int32 ScreenY0
                                  , const FArianeSegment::FFractionStep* Step1
                                  , int32 ScreenX1
                                  , int32 ScreenY1
                                  , const TArray<FColor>& Pixels
                                  , TArray<FWayPoint>& OutWayPointBuffer
                                  , TArray<FMetaFragment>& OutMetaFragmentBuffer
                                  , bool iRevert )
{
    int32  dx  = ( ScreenX1 - ScreenX0 );
    uint32 ddx = abs ( dx );
    int32  dy  = ( ScreenY1 - ScreenY0 );
    uint32 ddy = abs ( dy );
    double dt  = ( Step1->T - Step0->T );
    int32  dd  = ( ddx > ddy ) ? ddx : ddy;
    int32  px  = ( dx > 0 ) ? 1 : -1;
    int32  py  = ( dy > 0 ) ? 1 : -1;
    double pt  = ( dd ) ? dt / dd : 0.0f;
    int32  x   = ScreenX0;
    int32  y   = ScreenY0;
    double t   = Step0->T;
    uint32 cumul = 0;
    uint8 LastAlphaValue = GetAlpha( ScreenX0, ScreenY0, Pixels );
    double LastT = Step0->T;

    if ( ddx > ddy )
    {
        for ( uint32 i = 0; i <= ddx; i++ )
        {
            uint8 AlphaValue = GetAlpha( x, y, Pixels );

            if( CheckContrast( AlphaValue, LastAlphaValue ) )
            {
                // coords will always be right outside the erasure area
                double BestT = AlphaValue ? LastT : t;
                uint32 WaypointID = OutWayPointBuffer.Num();

                OutWayPointBuffer.Emplace( BestT, FWayPoint::BordersErasureArea );

                OutMetaFragmentBuffer.Emplace( Segment
                                             , WaypointID - 1
                                             , WaypointID
                                             , LastAlphaValue ? true : false );
            }

            LastAlphaValue = AlphaValue;
            LastT = t;

            cumul += ddy;
            x     += px;
            t     += pt;

            if ( cumul >= ddx )
            {
                cumul -= ddx;
                y     += py;
            }
        }
    }
    else
    {
        for ( uint32 i = 0; i <= ddy; i++ )
        {
            uint8 AlphaValue = GetAlpha( x, y, Pixels );

            if( CheckContrast( AlphaValue, LastAlphaValue ) )
            {
                // coords will always be right outside the erasure area
                double BestT = AlphaValue ? LastT : t;
                uint32 WaypointID = OutWayPointBuffer.Num();

                OutWayPointBuffer.Emplace( BestT, FWayPoint::BordersErasureArea );

                OutMetaFragmentBuffer.Emplace( Segment
                                             , WaypointID - 1
                                             , WaypointID
                                             , LastAlphaValue ? true : false );
            }

            LastAlphaValue = AlphaValue;
            LastT = t;

            cumul += ddx;
            y     += py;
            t     += pt;

            if ( cumul >= ddy )
            {
                cumul -= ddy;
                x     += px;
            }
        }
    }
}

FBox2D
UArianeEditorEraserTool::GetErasureBoundingArea( FEditorViewportClient* ViewportClient, FSceneView* View )
{
    FVector2D HUDMouseAtDown = ScreenToHUD( ViewportClient, MouseAtDown );
    FVector2D HUDMouseAtUp = ScreenToHUD( ViewportClient, MouseAtUp );
    double EraserRadius = Size * 0.5f;
    double XMin = FMath::Min( HUDMouseAtDown.X, HUDMouseAtUp.X ) - EraserRadius;
    double YMin = FMath::Min( HUDMouseAtDown.Y, HUDMouseAtUp.Y ) - EraserRadius;
    double XMax = FMath::Max( HUDMouseAtDown.X, HUDMouseAtUp.X ) + EraserRadius;
    double YMax = FMath::Max( HUDMouseAtDown.Y, HUDMouseAtUp.Y ) + EraserRadius;

    return ( XMin < XMax ) ? FBox2D( FVector2D( XMin, YMin ), FVector2D( XMax, YMax ) ) : FBox2D();
}

FBox2D
UArianeEditorEraserTool::GetPathBoundingArea( FEditorViewportClient* ViewportClient, FSceneView* View, FArianePath* Path )
{
    const FBox& PathBox = Path->GetBoundingBox();
    FVector P[8] = { PathBox.Min
                   , FVector( PathBox.Min.X, PathBox.Min.Y, PathBox.Max.Z )
                   , FVector( PathBox.Min.X, PathBox.Max.Y, PathBox.Min.Z )
                   , FVector( PathBox.Min.X, PathBox.Max.Y, PathBox.Max.Z )
                   , FVector( PathBox.Max.X, PathBox.Min.Y, PathBox.Min.Z )
                   , FVector( PathBox.Max.X, PathBox.Min.Y, PathBox.Max.Z )
                   , FVector( PathBox.Max.X, PathBox.Max.Y, PathBox.Min.Z )
                   , PathBox.Max };
    FTransform PathTransform = Path->GetTransform();
    double XMin = DBL_MAX, YMin = DBL_MAX, XMax = -DBL_MAX, YMax = -DBL_MAX;

    for( uint32 i = 0; i < 8; i++ )
    {
        FVector2D HUDPosition = ProjectWorldToHUD( ViewportClient, View, PathTransform.TransformPosition( P[i] ) );

        if( HUDPosition.X < XMin ) XMin = HUDPosition.X;
        if( HUDPosition.Y < YMin ) YMin = HUDPosition.Y;
        if( HUDPosition.X > XMax ) XMax = HUDPosition.X;
        if( HUDPosition.Y > YMax ) YMax = HUDPosition.Y;
    }

    return ( XMin < XMax ) ? FBox2D( FVector2D( XMin, YMin ), FVector2D( XMax, YMax ) ) : FBox2D();
}

bool
UArianeEditorEraserTool::ErasePaths( FEditorViewportClient* ViewportClient
                                   , FSceneView* View
                                   , UArianePainting3DComponent* Painting3DComponent )
{
    FTextureRenderTargetResource* RTResource = CanvasRenderTarget->GameThread_GetRenderTargetResource();
    TArray<FColor> Pixels;
    uint32 Width = CanvasRenderTarget->SizeX;
    uint32 Height = CanvasRenderTarget->SizeY;
    FBox2D ErasureArea = GetErasureBoundingArea( ViewportClient, View );

    RTResource->ReadPixels( Pixels
                          , FReadSurfaceDataFlags(RCM_UNorm)
                          , FIntRect( 0, 0, Width, Height ) );

    for( UArianeLayer* SelectedLayer : Painting3DComponent->GetLayerStack()->GetSelectedLayers() )
    {
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>( SelectedLayer );

        if( DrawingLayer )
        {
            TArray<FArianePath*> AddedPaths;
            TArray<FArianePath*> RemovedPaths;
            FArianeGroup* RootGroup = DrawingLayer->GetImage()->GetRootGroup();

            DrawingLayer->GetImage()->Modify();

            FArianeObject::Traverse( RootGroup
                                   , [ this
                                     , ViewportClient
                                     , View
                                     , ErasureArea
                                     , DrawingLayer
                                     , Pixels
                                     , Width
                                     , &AddedPaths
                                     , &RemovedPaths ] ( FArianeObject* TravesedObject ) -> FArianeObject::ETraversalReturnValue
            {
                if( TravesedObject->GetClass() == FArianePath::StaticClass() )
                {
                    FArianePath* Path = static_cast<FArianePath*>( TravesedObject );
                    FBox2D PathBox = GetPathBoundingArea( ViewportClient, View, Path );

                    if( ErasureArea.Intersect( PathBox ) )
                    {
                        TArray<ChainProcessor> ChainProcessors;
                        bool bPathHit = false;

                        ChainProcessors.Reserve( Path->GetChains().Num() );

                        for( const FArianePath::Chain& Chain : Path->GetChains() )
                        {
                            ChainProcessor& ChainProcessor = ChainProcessors.Emplace_GetRef();

                            ChainProcessor.WayPoints.Reserve( 100 );
                            ChainProcessor.WayFragments.Reserve( 100 );

                            bPathHit = EraseChainSegments( ViewportClient
                                                         , View
                                                         , DrawingLayer
                                                         , Path
                                                         , Chain
                                                         , Pixels
                                                         , ChainProcessor.WayPoints
                                                         , ChainProcessor.WayFragments ) ? true : bPathHit;
                        }

                        if( bPathHit )
                        {
                            for( ChainProcessor& ChainProcessor : ChainProcessors )
                            {
                                // proceed now we know we have hit anything
                                // determine which vertices / segments will be deleted and which will be kept
                                // it differs in SPLIT and NOSPLIT modes. SPLIT modes removes only those erased,
                                // split removes all
                                ParseChainWayPoints( DrawingLayer
                                                   , Path
                                                   , ChainProcessor.WayPoints
                                                   , ChainProcessor.WayFragments
                                                   , AddedPaths
                                                   , ChainProcessor.AddedVertices
                                                   , ChainProcessor.AddedSegments
                                                   , ChainProcessor.RemovedVertices
                                                   , ChainProcessor.RemovedSegments );

                                for( FArianeSegment* RemovedSegment : ChainProcessor.RemovedSegments )
                                {
                                    // Remove and Free
                                    static_cast<FArianePath*>(RemovedSegment->GetOwner())->RemoveSegment( RemovedSegment, true );
                                }

                                for( FArianeVertex* RemovedVertex : ChainProcessor.RemovedVertices )
                                {
                                    // Remove and Free
                                    static_cast<FArianePath*>(RemovedVertex->GetOwner())->RemoveVertex( RemovedVertex, true );
                                }

                                for( FArianeVertex* AddedVertex : ChainProcessor.AddedVertices )
                                {
                                    // Add
                                    static_cast<FArianePath*>(AddedVertex->GetOwner())->AddVertex( AddedVertex );
                                }

                                for( FArianeSegment* AddedSegment : ChainProcessor.AddedSegments )
                                {
                                    // Add
                                    static_cast<FArianePath*>(AddedSegment->GetOwner())->AddSegment( AddedSegment );
                                }
                            }

                            if( bSplit )
                            {
                                if( bPathHit )
                                {
                                    RemovedPaths.Add( Path );
                                }
                            }
                            else // bSplit == false
                            {
                                if( ( Path->GetSegments().Num() == 0 ) && ( Path->GetVertices().Num() == 0 ) )
                                {
                                    RemovedPaths.Add( Path );
                                }
                            }
                        }
                    }
                }

                return FArianeObject::ETraversalReturnValue::Continue;
            } );

            // Process outside the Traverse function because Traverse will alter the hierarchy
            for( FArianePath* RemovedPath : RemovedPaths )
            {
                RemovedPath->GetParent()->RemoveChild( RemovedPath, true );
            }

            for( FArianePath* AddedPath : AddedPaths )
            {
                AddedPath->GetParent()->AppendChild( AddedPath );
            }
        }
    }

    // TODO: check for empty paths

    Painting3DComponent->Update( false );

    // although the component is not directly modified, we need PostEditUndo to be called after undoing, so we set it as modified.
    //Painting3DComponent->Modify();

    return false;
}


//static
UArianeEditorEraserTool::FStartingPoint
UArianeEditorEraserTool::GetStartingPoint( FWayPoint* WayPoint, FWayFragment* Fragment )
{
    FWayPoint* CurrWayPoint = WayPoint;
    FWayFragment* CurrFragment = Fragment;

    // extend prev
    while( CurrFragment )
    {
        FWayFragment* PrevFragment = CurrWayPoint->GetOtherFragment( Fragment );

        if( ( PrevFragment == nullptr )
         || ( PrevFragment == Fragment ) // loop prevention
         || ( ( PrevFragment->bErased == true ) &&  ( CurrFragment->bErased == false ) ) )
        {
            return FStartingPoint( CurrWayPoint, CurrFragment );
        }

        CurrWayPoint = PrevFragment->GetOtherWayPoint( CurrWayPoint );
        CurrFragment = PrevFragment;
    }

    return FStartingPoint( CurrWayPoint, CurrFragment ); // this case should never be met anyways. theorically
}

// static
UArianeEditorEraserTool::ESegmentAdditionFlags
UArianeEditorEraserTool::SegmentAdditionPolicy( FWayPoint* WayPoint, FWayFragment* Fragment, bool bSplit )
{
    ESegmentAdditionFlags retFlags = ESegmentAdditionFlags::None;
    FWayFragment* PrevFragment = WayPoint->GetOtherFragment( Fragment );

    if( bSplit )
    {
        if( Fragment->bErased == false )
        {
            if( ( PrevFragment == nullptr ) || PrevFragment->bErased )
            {
                retFlags |= ESegmentAdditionFlags::CreateNewPath;
            }

            retFlags |= ESegmentAdditionFlags::CreateDerivedSegment;
        }
    }
    else
    {
        if( Fragment->bErased == true )
        {
            retFlags |= ESegmentAdditionFlags::RemoveOriginalSegment;
        }
        else
        {
            if( ( ( Fragment->WayPoint0->Flags & FWayPoint::Original ) ==  0 )
             || ( ( Fragment->WayPoint1->Flags & FWayPoint::Original ) ==  0 ) )
            {
                retFlags |= ESegmentAdditionFlags::CreateDerivedSegment;
            }
        }
    }

    return retFlags;
}

// static
UArianeEditorEraserTool::EVertexAdditionFlags
UArianeEditorEraserTool::VertexAdditionPolicy( FWayPoint* WayPoint, bool bSplit )
{
    EVertexAdditionFlags retFlags = EVertexAdditionFlags::None;

    if( bSplit )
    {
        if( WayPoint->Flags & FWayPoint::Original )
        {
            if( WayPoint->Flags & FWayPoint::OutsideErasureArea )
            {
                retFlags |= ( EVertexAdditionFlags::CreateDerivedVertex );
            }
        }
    }
    else
    {
        if( WayPoint->Flags & FWayPoint::InsideErasureArea )
        {
            if( WayPoint->Flags & FWayPoint::Original )
            {
                retFlags |= EVertexAdditionFlags::RemoveOriginalVertex;
            }
        }

        if( WayPoint->Flags & FWayPoint::OutsideErasureArea )
        {
            if( WayPoint->Flags & FWayPoint::Original )
            {
                retFlags |= EVertexAdditionFlags::KeepOriginalVertex;
            }
        }
    }

    if( WayPoint->Flags & FWayPoint::BordersErasureArea )
    {
        if( ( WayPoint->Flags & FWayPoint::Original ) == 0 )
        {
            retFlags |= EVertexAdditionFlags::CreateBoundaryVertex;
        }
    }

    return retFlags;
}

FArianeVertex*
UArianeEditorEraserTool::AssignVertex( FArianePath* OwnerPath
                                     , FWayPoint& WayPoint
                                     , TArray<FArianeVertex*>& OutAddedVertices
                                     , TArray<FArianeVertex*>& OutRemovedVertices
                                     , bool bSplit )
{
    if( WayPoint.AssignedVertex == nullptr )
    {
        EVertexAdditionFlags VertexAdditionFlags = VertexAdditionPolicy( &WayPoint, bSplit );

        if( ( VertexAdditionFlags & EVertexAdditionFlags::RemoveOriginalVertex ) == EVertexAdditionFlags::RemoveOriginalVertex )
        {
            // mark original vertex for deletion. No duplicates (duplicates happen in case of loops)
            if( OutRemovedVertices.Find( WayPoint.OriginalVertex ) == INDEX_NONE )
            {
                OutRemovedVertices.Add( WayPoint.OriginalVertex );
            }
        }

        // boundary vertices are guaranteed unique per nature, no need to check uniqueness
        if( ( VertexAdditionFlags & EVertexAdditionFlags::CreateBoundaryVertex ) == EVertexAdditionFlags::CreateBoundaryVertex )
        {
            WayPoint.AssignedVertex = OwnerPath->AllocVertex( FVector::Zero()
                                                            , FVector::Zero()
                                                            , 0.0f
                                                            , EArianeAllocationModel::InstancedStruct );

            OutAddedVertices.Add( WayPoint.AssignedVertex );
        }

        if( ( VertexAdditionFlags & EVertexAdditionFlags::CreateDerivedVertex ) == EVertexAdditionFlags::CreateDerivedVertex )
        {
            // waypoint.AssignedVertex will already be existing in case of a loop
            if( WayPoint.AssignedVertex == nullptr )
            {
                WayPoint.AssignedVertex = OwnerPath->AllocVertex( FVector::Zero()
                                                                , FVector::Zero()
                                                                , 0.0f
                                                                , EArianeAllocationModel::InstancedStruct );

                OutAddedVertices.Add( WayPoint.AssignedVertex );
            }
        }

        if ((VertexAdditionFlags & EVertexAdditionFlags::KeepOriginalVertex ) == EVertexAdditionFlags::KeepOriginalVertex )
        {
            WayPoint.AssignedVertex = WayPoint.OriginalVertex;
        }
    }

    return WayPoint.AssignedVertex;
}

void
UArianeEditorEraserTool::ParseChainWayPoints( UArianeLayerDrawing* DrawingLayer
                                            , FArianePath* ChainPath
                                            , TArray<FWayPoint>& WayPoints
                                            , TArray<FWayFragment>& WayFragments
                                            , TArray<FArianePath*>& OutAddedPaths
                                            , TArray<FArianeVertex*>& OutAddedVertices
                                            , TArray<FArianeSegment*>& OutAddedSegments
                                            , TArray<FArianeVertex*>& OutRemovedVertices
                                            , TArray<FArianeSegment*>& OutRemovedSegments )
{
    uint32 addedSegmentCountBeforeAlter = OutAddedSegments.Num();
    FArianePath* CurrentPath = ChainPath;

    if( WayPoints.Num() )
    {
        FWayPoint* FirstWayPoint = &WayPoints[0];

        FWayFragment *FirstFragment = &WayFragments[0];
        FStartingPoint StartingPoint = GetStartingPoint( FirstWayPoint, FirstFragment );
        FWayFragment *CurrFragment = StartingPoint.WayFragment;
        FWayPoint* CurrWayPoint = StartingPoint.WayPoint;

        while( CurrFragment )
        {
            FWayPoint* WayPoint0 = CurrFragment->WayPoint0;
            FWayPoint* WayPoint1 = CurrFragment->WayPoint1;
            ESegmentAdditionFlags SegmentAdditionFlags = SegmentAdditionPolicy( CurrWayPoint, CurrFragment, bSplit );
            FWayPoint* NextWayPoint = CurrFragment->GetOtherWayPoint( CurrWayPoint );
            FWayFragment* NextFragment = NextWayPoint->GetOtherFragment( CurrFragment );

            if( VertexAdditionPolicy( WayPoint0, bSplit ) == EVertexAdditionFlags::RemoveOriginalVertex )
            {
                // mark original vertex for deletion. No duplicates
                if( OutRemovedVertices.Find( WayPoint0->OriginalVertex ) == INDEX_NONE )
                {
                    OutRemovedVertices.Add( WayPoint0->OriginalVertex );
                }
            }

            if( VertexAdditionPolicy( WayPoint1, bSplit ) == EVertexAdditionFlags::RemoveOriginalVertex )
            {
                // mark original vertex for deletion. No duplicates
                if( OutRemovedVertices.Find( WayPoint1->OriginalVertex ) == INDEX_NONE )
                {
                    OutRemovedVertices.Add( WayPoint1->OriginalVertex );
                }
            }

            if( ( SegmentAdditionFlags & ESegmentAdditionFlags::RemoveOriginalSegment ) == ESegmentAdditionFlags::RemoveOriginalSegment )
            {
                // mark original segment for deletion. No duplicates
                if( OutRemovedSegments.Find( CurrFragment->Segment ) == INDEX_NONE )
                {
                    OutRemovedSegments.Add( CurrFragment->Segment );
                }
            }

            if( ( SegmentAdditionFlags & ESegmentAdditionFlags::CreateNewPath ) == ESegmentAdditionFlags::CreateNewPath )
            {
                CurrentPath = DrawingLayer->GetImage()->AllocPath( ChainPath->GetMaterial()
                                                                 , ChainPath->GetName()
                                                                 , EArianeAllocationModel::InstancedStruct );
                // for postprocessing. the path is not added to the parent yet
                CurrentPath->SetParent( ChainPath->GetParent() );

                ChainPath->ExportProperties( CurrentPath );

                CurrentPath->UpdateTransform();

                OutAddedPaths.Add( CurrentPath );
            }

            if( ( SegmentAdditionFlags & ESegmentAdditionFlags::CreateDerivedSegment ) == ESegmentAdditionFlags::CreateDerivedSegment )
            {
                FArianeVertex* DestVertex0 = AssignVertex( CurrentPath
                                                         , *WayPoint0
                                                         , OutAddedVertices
                                                         , OutRemovedVertices
                                                         , bSplit );
                FArianeVertex* DestVertex1 = AssignVertex( CurrentPath
                                                         , *WayPoint1
                                                         , OutAddedVertices
                                                         , OutRemovedVertices
                                                         , bSplit );

                FArianeSegment* NewSegment = ( CurrFragment->T0 < CurrFragment->T1 ) ? CurrFragment->Segment->Extract( CurrentPath
                                                                                                                     , DestVertex0
                                                                                                                     , CurrFragment->T0
                                                                                                                     , DestVertex1
                                                                                                                     , CurrFragment->T1 )
                                                                                     : CurrFragment->Segment->Extract( CurrentPath
                                                                                                                     , DestVertex1
                                                                                                                     , CurrFragment->T1
                                                                                                                     , DestVertex0
                                                                                                                     , CurrFragment->T0 );

                // mark new segment for addition
                OutAddedSegments.Add( NewSegment );
            }

            CurrWayPoint = NextWayPoint;
            CurrFragment = ( NextFragment == StartingPoint.WayFragment ) ? nullptr : NextFragment;
        }
    }
}


bool
UArianeEditorEraserTool::EraseChainSegments( FEditorViewportClient* ViewportClient
                                           , FSceneView* View
                                           , UArianeLayerDrawing* DrawingLayer
                                           , FArianePath* Path
                                           , const FArianePath::Chain& Chain
                                           , const TArray<FColor>& Pixels
                                           , TArray<FWayPoint>& OutWayPoints
                                           , TArray<FWayFragment>& OutWayFragments )
{
    //const FTransform& WorldTransform = Painting3DComponent->GetComponentTransform();
    UArianePainting3DComponent* Painting3DComponent = DrawingLayer->GetLayerStack()->GetPainting3DComponent();
    const FTransform& WorldTransform = Path->GetTransform();

    FArianeVertex* FirstVertex = Chain.LeadingVertex;
    TArray<FMetaFragment> MetaFragmentBuffer;
    bool bHasHit = false;

    if( Chain.Segments.Num() )
    {
        FArianeSegment* FirstSegment = Chain.Segments[0];

        OutWayPoints.Empty();
        // reserve 4 point per segment to limit reallocations (just for performance)
        OutWayPoints.Reserve( ( Chain.Segments.Num() * 4 ) );

        // reserve 3 meta segment per segment to limit reallocations (just for performance)
        MetaFragmentBuffer.Reserve( Chain.Segments.Num() * 3 );

        // create WayPoints for each Vertex of the chain
        VertexToWaypoint( ViewportClient, View, WorldTransform, Chain.LeadingVertex, FirstSegment->GetVertexT( Chain.LeadingVertex ), Pixels, OutWayPoints );

        // then create WayPoints at each intersection
        Chain.IterateSegments( [ this
                               , Path
                               , Chain
                               , ViewportClient
                               , View
                               , &Pixels
                               , &OutWayPoints
                               , &MetaFragmentBuffer
                               , &bHasHit
                               , &WorldTransform ]( FArianeVertex* Vertex, FArianeSegment* Segment ) -> bool
        {
            // for better precision when the line has only few fractions. This is slow
            // and will be changed.
            //segment->Update( FArianeObject::UPDATE_NEEDPOLYLINE );

            const TArray<FArianeSegment::FFraction>& Fractions = Segment->GetFractions();
            FArianeVertex* OtherVertex = Segment->GetOtherVertex( Vertex );
            FArianeSegment* NextSegment = OtherVertex->GetOtherSegment( Segment );
            bool revert = ( Vertex == Segment->GetVertex(0) ) ? false : true;

            // create waypoints when the alpha value of the mask changes
            if( revert == false )
            {
                for( auto it = Fractions.begin(); it != Fractions.end(); ++it )
                {
                    const FArianeSegment::FFraction& Fraction = *it;
                    const FArianeSegment::FFractionStep* Step0 = Fraction.Steps[0];
                    const FArianeSegment::FFractionStep* Step1 = Fraction.Steps[1];
                    FVector WorlCoords0 = WorldTransform.TransformPosition( Step0->Point->GetPosition() ) ;
                    FVector WorlCoords1 = WorldTransform.TransformPosition( Step1->Point->GetPosition() ) ;
                    FVector2D HUDCoords0 = ProjectWorldToHUD( ViewportClient, View, WorlCoords0 );
                    FVector2D HUDCoords1 = ProjectWorldToHUD( ViewportClient, View, WorlCoords1 );

                    TraceLine( Path
                             , Segment
                             , Step0
                             , HUDCoords0.X
                             , HUDCoords0.Y
                             , Step1
                             , HUDCoords1.X
                             , HUDCoords1.Y
                             , Pixels
                             , OutWayPoints
                             , MetaFragmentBuffer
                             , revert );
                }
            }
            else
            {
                for( auto it = Fractions.rbegin(); it != Fractions.rend(); ++it )
                {
                    const FArianeSegment::FFraction& Fraction = *it;
                    const FArianeSegment::FFractionStep* Step0 = Fraction.Steps[0];
                    const FArianeSegment::FFractionStep* Step1 = Fraction.Steps[1];
                    FVector WorlCoords0 = WorldTransform.TransformPosition( Step0->Point->GetPosition() ) ;
                    FVector WorlCoords1 = WorldTransform.TransformPosition( Step1->Point->GetPosition() ) ;
                    FVector2D HUDCoords0 = ProjectWorldToHUD( ViewportClient, View, WorlCoords0 );
                    FVector2D HUDCoords1 = ProjectWorldToHUD( ViewportClient, View, WorlCoords1 );

                    TraceLine( Path
                             , Segment
                             , Step1
                             , HUDCoords1.X
                             , HUDCoords1.Y
                             , Step0
                             , HUDCoords0.X
                             , HUDCoords0.Y
                             , Pixels
                             , OutWayPoints
                             , MetaFragmentBuffer
                             , revert );
                }
            }

            uint32 LastFragmentWayPoint0Index;
            uint32 LastFragmentWayPoint1Index;

            if( Chain.LeadingVertex != OtherVertex )
            {
                // create the last for the final vertex of the segment
                VertexToWaypoint( ViewportClient, View, WorldTransform, OtherVertex, Segment->GetVertexT( OtherVertex ), Pixels, OutWayPoints );

                LastFragmentWayPoint0Index = OutWayPoints.Num() - 2;
                LastFragmentWayPoint1Index = OutWayPoints.Num() - 1;
            }
            else // loop detected
            {
                LastFragmentWayPoint0Index = OutWayPoints.Num() - 1;
                LastFragmentWayPoint1Index = 0;
            }

            // last fragment
            MetaFragmentBuffer.Emplace( Segment
                                      , LastFragmentWayPoint0Index
                                      , LastFragmentWayPoint1Index
                                      , ( OutWayPoints[LastFragmentWayPoint1Index].Flags & FWayPoint::InsideErasureArea ) ? true : false );

            return false; // keep iterating;
        } );
    }

    OutWayFragments.Empty();
    OutWayFragments.Reserve( MetaFragmentBuffer.Num() );

    // create fragments afterwards so that the waypoint pointers won't change due to array growing with push_backs
    for( FMetaFragment& MetaFragment : MetaFragmentBuffer )
    {
        FWayPoint* WayPoint0 = &OutWayPoints[MetaFragment.WayPoint0Index];
        FWayPoint* WayPoint1 = &OutWayPoints[MetaFragment.WayPoint1Index];

        OutWayFragments.Emplace( MetaFragment.Segment
                               , WayPoint0
                               , WayPoint1
                               , MetaFragment.bErased );

        if( ( WayPoint0->Flags & FWayPoint::OutsideErasureArea ) == 0 )
        {
            bHasHit = true;
        }

        if( ( WayPoint1->Flags & FWayPoint::OutsideErasureArea ) == 0 )
        {
            bHasHit = true;
        }
    }

    return bHasHit;
}

void
UArianeEditorEraserTool::ClearCanvas()
{
    FTextureRenderTargetResource* RTResource = CanvasRenderTarget->GameThread_GetRenderTargetResource();
    FCanvas Canvas( RTResource, nullptr, GetWorld(), GMaxRHIFeatureLevel );

    Canvas.Clear( FLinearColor( 1.0f, 1.0f, 1.0f, 0.0f ) );
    Canvas.Flush_GameThread();
}

void
UArianeEditorEraserTool::ResizeCanvas( uint32 Width, uint32 Height )
{
    CanvasRenderTarget->SizeX = Width;
    CanvasRenderTarget->SizeY = Height;

    CanvasRenderTarget->UpdateResource();
}

bool
UArianeEditorEraserTool::OnMouseDown( FEditorViewportClient* iViewportClient
                                    , FSceneView* View
                                    , const FKey& iKey
                                    , const FArianePointerState& PointerState
                                    , bool iRepeat )
{
    ResizeCanvas( iViewportClient->Viewport->GetSizeXY().X
                , iViewportClient->Viewport->GetSizeXY().Y );
    ClearCanvas();

    if( iKey == EKeys::LeftMouseButton )
    {
        MouseAtDown = FVector2D( PointerState.ViewportX, PointerState.ViewportY );
        MouseRecords[0] = MouseRecords[1] = FIntVector2( PointerState.ViewportX, PointerState.ViewportY );
        // Will call UArianeEditorEraserTool::StampBrush()
        //CanvasRenderTarget->UpdateResource();
        StampBrush( iViewportClient );

        return true;
    }

    return false;
}

void
UArianeEditorEraserTool::OnMouseHover( FEditorViewportClient* iViewportClient
                                     , FSceneView* View
                                     , const FArianePointerState& PointerState )
{
    MousePosition = FVector2D( PointerState.ViewportX, PointerState.ViewportY ) ;
}

bool
UArianeEditorEraserTool::OnMouseDrag( FEditorViewportClient* iViewportClient
                                    , FSceneView* View
                                    , const FKey& iKey
                                    , const FArianePointerState& PointerState )
{
    MousePosition = FVector2D( PointerState.ViewportX, PointerState.ViewportY ) ;

    if( iKey == EKeys::LeftMouseButton )
    {
        MouseRecords[1] = FIntVector2( PointerState.ViewportX, PointerState.ViewportY );
        // Will call UArianeEditorEraserTool::StampBrush()
        //CanvasRenderTarget->UpdateResource();

        StampBrush( iViewportClient );

        MouseRecords[0] = MouseRecords[1];
    }

    return false;
}

bool
UArianeEditorEraserTool::OnMouseUp( FEditorViewportClient* ViewportClient
                                  , FSceneView* View
                                  , const FKey& iKey
                                  , const FArianePointerState& PointerState )
{
    if( iKey == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        MouseAtUp = FVector2D( PointerState.ViewportX, PointerState.ViewportY );
        MouseRecords[1] = FIntVector2( PointerState.ViewportX, PointerState.ViewportY );
        // Will call UArianeEditorEraserTool::StampBrush()
        //CanvasRenderTarget->UpdateResource();
        StampBrush( ViewportClient );

        GetToolManager()->BeginUndoTransaction(FText::FromString("Erase object"));

        if( Painting3DComponent )
        {
            ErasePaths( ViewportClient, View, Painting3DComponent );
        }
        GetToolManager()->EndUndoTransaction();

        ClearCanvas();

        return true;
    }

    if( iKey == EKeys::RightMouseButton )
    {

        return false;
    }

    return false;
}

void
UArianeEditorEraserTool::ExtendContextMenu( FMenuBuilder& menu )
{
}

void
UArianeEditorEraserTool::DrawHUD ( FCanvas* HUDCanvas, IToolsContextRenderAPI* RenderAPI )
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
        FVector2D HUDPosition = ScreenToHUD( ViewportClient, MousePosition );

        HUDCanvas->DrawTile(
            0, 0,
            CanvasRenderTarget->SizeX, CanvasRenderTarget->SizeY,
            0.0f, 0.0f, 1.0f, 1.0f, // UVs
            FLinearColor::White,
            CanvasRenderTarget->GetResource(),
            true
        );

        DrawCircleHUD ( HUDCanvas, ViewportClient, View, HUDPosition, ( double ) Size * 0.5f, HcColor, 1.0f );
    }
}

void
UArianeEditorEraserTool::ResizeBrush()
{
    Brush->GetPlatformData()->SizeX = Size;
    Brush->GetPlatformData()->SizeY = Size;

    FTexture2DMipMap& Mip = Brush->GetPlatformData()->Mips[0];

    Mip.SizeX = Size;
    Mip.SizeY = Size;
    Mip.BulkData.Lock(LOCK_READ_WRITE);
    Mip.BulkData.Realloc(Size * Size * 4); // PF_B8G8R8A8
    Mip.BulkData.Unlock();

    Brush->UpdateResource();
}

void
UArianeEditorEraserTool::StampBrush( FEditorViewportClient* ViewportClient )
{
    FTextureRenderTargetResource* RTResource = CanvasRenderTarget->GameThread_GetRenderTargetResource();
    FCanvas Canvas(RTResource, nullptr, GetWorld(), GMaxRHIFeatureLevel);
    FVector2D HUDMouseRecord0 = ScreenToHUD( ViewportClient, FVector2D( MouseRecords[0] ) );
    FVector2D HUDMouseRecord1 = ScreenToHUD( ViewportClient, FVector2D( MouseRecords[1] ) );
    FVector2D DeltaHUDMouse = HUDMouseRecord1 - HUDMouseRecord0;
    int32 LenSq = ( DeltaHUDMouse.X * DeltaHUDMouse.X ) + ( DeltaHUDMouse.Y * DeltaHUDMouse.Y );
    int32 Len = LenSq ? sqrt( LenSq ) : 0;
    FVector2D StampAt = HUDMouseRecord0;
    FVector2D Step = Len ? FVector2D( ( double ) DeltaHUDMouse.X / Len
                                    , ( double ) DeltaHUDMouse.Y / Len )
                         : FVector2D( 0.0f, 0.0f );

    for( int32 i = 0; i <= Len; i++ )
    {
        FCanvasTileItem Tile = FCanvasTileItem( FVector2D( StampAt.X - ( Size * 0.5f )
                                                         , StampAt.Y - ( Size * 0.5f ) )
                                              , Brush->GetResource()
                                              , FVector2D( Size, Size )
                                              , FLinearColor::White );

        Tile.SetColor( FLinearColor::White );
        Tile.BlendMode = SE_BLEND_AlphaComposite;
        Canvas.DrawItem(Tile);

        StampAt += Step;
    }

    Canvas.Flush_GameThread();
}

/*
void
UArianeEditorEraserTool::StampBrush( UCanvas* Canvas, int32 Width, int32 Height )
{
    FIntVector2 DeltaMouse = MouseRecords[1] - MouseRecords[0];
    int32 LenSq = ( DeltaMouse.X * DeltaMouse.X ) + ( DeltaMouse.Y * DeltaMouse.Y );
    int32 Len = LenSq ? sqrt( LenSq ) : 0;
    FIntVector2 StampAt = MouseRecords[0];

    if( Len )
    {
        FIntVector2 Step = DeltaMouse / Len;

        for( int32 i = 0; i < Len; i++ )
        {
            Canvas->K2_DrawTexture( Brush
                                  , FVector2D( StampAt.X - ( Size * 0.5f )
                                             , StampAt.Y  -( Size * 0.5f ) )
                                  , FVector2D( Size, Size )
                                  , FVector2D( 0.0f ,0.0f )
                                  , FVector2D( 1.0f, 1.0f )
                                  , FLinearColor::White
                                  , EBlendMode::BLEND_Translucent );

            StampAt += Step;
        }
    }
}
*/

void
UArianeEditorEraserTool::FillBrush()
{
    FTexture2DMipMap& Mip = Brush->GetPlatformData()->Mips[0];
    void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
    FColor* Pixels = static_cast<FColor*>(Data);

    float Center = Size / 2.0f;
    float Radius = Size / 2.0f;

    for ( int32 y = 0; y < Size; y++ )
    {
        for ( int32 x = 0; x < Size; x++ )
        {
            float Dist = FVector2D::Distance(FVector2D(x, y), FVector2D(Center, Center));
            float Alpha = FMath::Clamp((Radius - Dist) / 2.0f, 0.0f, 1.0f); // Anti-aliasing 2 pixels
            uint32 Offset = y * Size + x;

            Pixels[Offset].A = Alpha * 255;
            Pixels[Offset].R = Pixels[Offset].G = Pixels[Offset].B = 255;
        }
    }

    Mip.BulkData.Unlock();

    Brush->UpdateResource();
}

void
UArianeEditorEraserTool::UpdateBrush()
{
    ResizeBrush();
    FillBrush();
}

void
UArianeEditorEraserTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    FName PropertyName = PropertyChangedEvent.GetPropertyName();
    FName MemberPropertyName = PropertyChangedEvent.GetMemberPropertyName();

    if( PropertyName == GET_MEMBER_NAME_CHECKED( UArianeEditorEraserTool, Size ) )
    {
        UpdateBrush();
    }
/*
    PropertyChanged( PropertyChangedEvent.GetPropertyName()
                   , PropertyChangedEvent.GetMemberPropertyName()
                   , PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive );

    PostPropertyChanged( PropertyChangedEvent.GetMemberPropertyName()
                       , PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive );
*/
}

#undef LOCTEXT_NAMESPACE
