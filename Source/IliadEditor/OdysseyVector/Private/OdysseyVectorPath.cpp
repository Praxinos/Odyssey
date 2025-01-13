// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyVectorPath.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVector.h"
#include <execution> // for_each

FOdysseyVectorPath::~FOdysseyVectorPath()
{
   // Free vertices and clear list in one go.
    mVertexList.remove_if( []( FOdysseyVectorVertex* iVertex )
                           {
                               delete iVertex;
                               return true;
                           });

   // Free segments and clear list in one go.
    mSegmentList.remove_if( []( FOdysseyVectorSegment* iSegment )
                           {
                               delete iSegment;
                               return true;
                           });
}

FOdysseyVectorPath::FOdysseyVectorPath( const FString& iName )
    : FOdysseyVectorObject( iName )
    , mPaintingCode( 0 )
    , mBrush( this )
{
    SetJointType( eJointType::Miter, false );

    bFilled = false;
    SetMiterLimit( 4.0f, false );

    mChainArray.reserve( 10 );

    //Invalidate();
/*
    mBrush = new BLImage();
    if( mBrush )
    {
        mBrush->readFromFile("C:\\Users\\Eric\\Desktop\\brush_test.png");
    }
*/
}

bool
FOdysseyVectorPath::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorObject::HasBaseClass( iBaseClassID );
}

void
FOdysseyVectorPath::ToVertexIndexAndSectionArray( std::vector<uint32>& oVertexIndexArray
                                                , std::vector<FOdysseyVectorSection*>& oSectionArray )
{
    if( mSegmentList.size() )
    {
        FOdysseyVectorSegment *segment = GetFirstSegment();
        FOdysseyVectorVertex* firstVertex = segment->GetVertex(0);
        FOdysseyVectorVertex* vertex = firstVertex;

        do
        {
            FOdysseyVectorVertex* nextVertex = segment->GetOtherVertex( vertex );
            FOdysseyVectorSection* section = vertex->GetSection( segment );
            FOdysseyVectorSegment *nextSegment = nextVertex->GetOtherSegment( segment );

            oVertexIndexArray.push_back( vertex->GetIndex( segment ) );
            oSectionArray.push_back( section );

            vertex = nextVertex;
            segment = nextSegment;
        }
        while( segment && ( vertex != firstVertex ) );
    }
}

bool
FOdysseyVectorPath::HasIntersections()
{
    for( FOdysseyVectorSegment* segment : mSegmentList )
    {
        if( segment->GetIntersectionList().size() )
        {
            return true;
        }
    }

    return false;
}

void
FOdysseyVectorPath::SelectVertex( FOdysseyVectorVertex* iVertex )
{
    iVertex->SetSelected( true );

    mSelectedVertexList.push_back( iVertex );
}

void
FOdysseyVectorPath::SelectAllVertices()
{
    for( FOdysseyVectorVertex* vertex : mVertexList )
    {
        if( vertex->IsSelected() == false )
        {
            SelectVertex( vertex );
        }
    }
}

bool
FOdysseyVectorPath::IsLoop()
{
    return ( mSegmentList.size() == mVertexList.size() ) ? true : false;
}

bool
FOdysseyVectorPath::IsFilled()
{
    return bFilled;
}

FOdysseyVectorBrush&
FOdysseyVectorPath::GetBrush()
{
    return mBrush;
}

void
FOdysseyVectorPath::SetBrush( const FOdysseyVectorBrush& iBrush )
{
    FOdysseyVectorEngine* engine = GetEngine();

    mBrush = iBrush;

    Invalidate( INVALIDATE_COLOR );
}

void
FOdysseyVectorPath::SetFilled( bool iIsFilled )
{
    bFilled = iIsFilled;

    Invalidate( INVALIDATE_COLOR );
}

eJointType
FOdysseyVectorPath::GetJointType()
{
    return mJointType;
}

void
FOdysseyVectorPath::SetJointType( eJointType iJointType, bool iInvalidate )
{
    mJointType = iJointType;

    if( iInvalidate )
    {
        InvalidateAllVertices();
        Invalidate( INVALIDATE_SHAPE );
    }
}

double
FOdysseyVectorPath::GetMiterLimit()
{
    return mMiterLimit;
}

void
FOdysseyVectorPath::SetMiterLimit( double iMiterLimit, bool iInvalidate )
{
    mMiterLimit = iMiterLimit;

    if( iInvalidate )
    {
        InvalidateAllVertices();
        Invalidate( INVALIDATE_SHAPE );
    }
}

void
FOdysseyVectorPath::ExportParam( FOdysseyVectorObject* iDestinationObject, bool iInvalidate )
{
    FOdysseyVectorObject::ExportParam( iDestinationObject, false );

    if( iDestinationObject->GetClass() == FOdysseyVectorPath::StaticClass() )
    {
        FOdysseyVectorPath* destinationPath = static_cast<FOdysseyVectorPath*>(iDestinationObject);

        destinationPath->SetBrush( mBrush );
        destinationPath->SetFilled( bFilled );
        destinationPath->SetJointType( mJointType, false );
        destinationPath->SetMiterLimit( mMiterLimit, false );
    }

    if( iInvalidate )
    {
        iDestinationObject->Invalidate( INVALIDATE_SHAPE | INVALIDATE_COLOR | INVALIDATE_TOPOLOGY );
    }
}

std::vector<FOdysseyVectorChain>&
FOdysseyVectorPath::GetChainArray()
{
    return mChainArray;
}

void
FOdysseyVectorPath::UnselectAllVertices()
{
     mSelectedVertexList.remove_if( []( FOdysseyVectorVertex* iVertex )
                                    {
                                        iVertex->SetSelected( false );

                                        return true;
                                    } );
}

std::list<FOdysseyVectorVertex*>&
FOdysseyVectorPath::GetSelectedVertexList()
{
    return mSelectedVertexList;
}

void
FOdysseyVectorPath::SetPaintingCode( uint32 iPaintingCode )
{
    mPaintingCode = iPaintingCode;
}

uint32
FOdysseyVectorPath::GetPaintingCode()
{
    return mPaintingCode;
}

void
FOdysseyVectorPath::UpdateBBox()
{
    double xmin = DBL_MAX, ymin = DBL_MAX, xmax = -DBL_MAX, ymax = -DBL_MAX;
    bool hasBBox = false;

    for( FOdysseyVectorChain& chain : mChainArray )
    {
        double rx1 = chain.mBBox.x
             , ry1 = chain.mBBox.y
             , rx2 = chain.mBBox.x + chain.mBBox.w
             , ry2 = chain.mBBox.y + chain.mBBox.h;

        hasBBox = true;

        if ( rx1 < xmin ) xmin = rx1;
        if ( ry1 < ymin ) ymin = ry1;
        if ( rx2 > xmax ) xmax = rx2;
        if ( ry2 > ymax ) ymax = ry2;
    }

    for( FOdysseyVectorVertex* vertex : mVertexList )
    {
        if( vertex->GetSegmentCount() )
        {
            ::ULIS::FRectD jointBBox = vertex->GetJoint().GetBBox( false );
            double rx1 = jointBBox.x
                 , ry1 = jointBBox.y
                 , rx2 = jointBBox.x + jointBBox.w
                 , ry2 = jointBBox.y + jointBBox.h;

            hasBBox = true;

            if ( rx1 < xmin ) xmin = rx1;
            if ( ry1 < ymin ) ymin = ry1;
            if ( rx2 > xmax ) xmax = rx2;
            if ( ry2 > ymax ) ymax = ry2;
        }
    }

    mBBox = ( hasBBox ) ? ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax ) : ::ULIS::FRectD( 0.0f, 0.0f, 0.0f, 0.0f );
}

bool
FOdysseyVectorPath::HasVertex( FOdysseyVectorVertex* iVertex )
{
    if( std::find( mVertexList.begin(), mVertexList.end(), iVertex ) == mVertexList.end() )
    {
        return false;
    }

    return true;
}

bool
FOdysseyVectorPath::HasSegment( FOdysseyVectorSegment* iSegment )
{
    if( std::find( mSegmentList.begin(), mSegmentList.end(), iSegment ) == mSegmentList.end() )
    {
        return false;
    }

    return true;
}

/*
BLPath&
FOdysseyVectorPath::GetBLPath()
{
    return mBLPath;
}
*/

void
FOdysseyVectorPath::UpdateShape( uint32 iUpdateFlags )
{
    mBLPath.clear();

    // update invalidated segments only
    /*std::for_each( std::execution::par_unseq
                 , mInvalidatedSegmentList.begin()
                 , mInvalidatedSegmentList.end()
                 , [ this ]( FOdysseyVectorSegment *segment )*/

    if( iUpdateFlags & UPDATE_FORCE )
    {
        for ( FOdysseyVectorSegment* segment : mSegmentList )
        {
            segment->Update( iUpdateFlags );
        }
    }
    else
    {
        for ( FOdysseyVectorSegment* segment : mInvalidatedSegmentList )
        {
            segment->Update( iUpdateFlags );
        }

        // update vertices joints AFTER all segments are up to date
        for ( FOdysseyVectorSegment* segment : mInvalidatedSegmentList )
        {
            FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);
            FOdysseyVectorVertex* vertex1 = segment->GetVertex(1);

            if( vertex0->IsInvalidated() )
            {
                vertex0->Update( vertex0->GetOtherSegment( segment ), segment, iUpdateFlags );
            }

            if( vertex1->IsInvalidated() )
            {
                vertex1->Update( segment, vertex1->GetOtherSegment( segment ), iUpdateFlags );
            }
        }
    }

    // clear this anyway
    mInvalidatedSegmentList.clear();

    /*);*/

    if( mInvalidationFlags & INVALIDATE_TOPOLOGY )
    {
        FindChains();
    }

    // Updates vertex chains' length and bounding box
    for( FOdysseyVectorChain& chain : mChainArray )
    {
        UpdateChain( &chain );
    }

    // TODO::Optimize this can be merged with the for loop above
    UpdateBBox();

    // cache BL Path (for drawing structure for example)
    for ( FOdysseyVectorSegment* segment : mSegmentList )
    {
        ::ULIS::FVec2D& point0 = segment->GetVertex(0)->GetCoords();
        ::ULIS::FVec2D& point1 = segment->GetVertex(1)->GetCoords();

        if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
            ::ULIS::FVec2D& ctrlPoint0 = cubicSegment->GetHandle(0)->GetCoords();
            ::ULIS::FVec2D& ctrlPoint1 = cubicSegment->GetHandle(1)->GetCoords();

            mBLPath.moveTo( point0.x, point0.y );
            mBLPath.cubicTo( ctrlPoint0.x
                           , ctrlPoint0.y
                           , ctrlPoint1.x
                           , ctrlPoint1.y
                           , point1.x
                           , point1.y );
        }
    }
}

bool
FOdysseyVectorPath::GetBBoxFromSelectedVertices( ::ULIS::FRectD& oBBox
                                                , bool iWithHandles
                                                , bool iWorld )
{
    double xmin, ymin, xmax, ymax;

    if( FOdysseyVectorVertex::GetMinMaxFromList( mSelectedVertexList
                                               , iWithHandles
                                               , xmin
                                               , ymin
                                               , xmax
                                               , ymax ) )
    {
        if( iWorld )
        {
            BLPoint p[4] = { mWorldMatrix.mapPoint( xmin, ymin )
                           , mWorldMatrix.mapPoint( xmax, ymin )
                           , mWorldMatrix.mapPoint( xmax, ymax )
                           , mWorldMatrix.mapPoint( xmin, ymax ) };

            oBBox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p[0].x, p[1].x, p[2].x, p[3].x )
                                              , ::ULIS::FMath::Min4( p[0].y, p[1].y, p[2].y, p[3].y )
                                              , ::ULIS::FMath::Max4( p[0].x, p[1].x, p[2].x, p[3].x )
                                              , ::ULIS::FMath::Max4( p[0].y, p[1].y, p[2].y, p[3].y ) );
        }
        else
        {
            oBBox = ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );
        }

        return true;
    }

    return false;
}


static void
GetSelectedCubicSegmentHandles( FOdysseyVectorSegmentCubic* iCubicSegment
                              , std::vector<FOdysseyVectorPoint*>& oPointArray
                              , ePointSelectionFlags iPointSelectionFlags )
{
    FOdysseyVectorVertex* vertex0 = iCubicSegment->GetVertex(0);
    FOdysseyVectorVertex* vertex1 = iCubicSegment->GetVertex(1);
    bool isSelected0 = vertex0->IsSelected();
    bool isSelected1 = vertex1->IsSelected();

    if( isSelected0 || isSelected1 )
    {
        FOdysseyVectorHandleSegment* segmentHandle0 = iCubicSegment->GetHandle(0);
        FOdysseyVectorHandleSegment* segmentHandle1 = iCubicSegment->GetHandle(1);

        if( isSelected0 )
        {
            if( iPointSelectionFlags & ePointSelectionFlags::Strict )
            {
                if( isSelected1 )
                {
                    oPointArray.push_back( segmentHandle0 );
                }
            }
            else
            {
                oPointArray.push_back( segmentHandle0 );
            }
        }

        if( isSelected1 )
        {
            if( iPointSelectionFlags & ePointSelectionFlags::Strict )
            {
                if( isSelected0 )
                {
                    oPointArray.push_back( segmentHandle1 );
                }
            }
            else
            {
                oPointArray.push_back( segmentHandle1 );
            }
        }
    }
}

void
FOdysseyVectorPath::GetSelectedVertices( std::vector<FOdysseyVectorVertex*>& oVertexArray )
{
    oVertexArray.reserve( oVertexArray.size() + mSelectedVertexList.size() );

    for( FOdysseyVectorVertex* vertex : mSelectedVertexList )
    {
        oVertexArray.push_back( vertex );
    }
}

void
FOdysseyVectorPath::InvalidateSegment( FOdysseyVectorSegment* iSegment )
{
    if( iSegment->IsInvalidated() == false )
    {
        mInvalidatedSegmentList.push_back( iSegment );
    }

    Invalidate( INVALIDATE_SHAPE );
}

void
FOdysseyVectorPath::AddVertex( FOdysseyVectorVertex* iVertex )
{
    mVertexList.push_back( iVertex );

    iVertex->SetOwner( this );

    Invalidate( INVALIDATE_SHAPE );
}

void
FOdysseyVectorPath::UnselectVertex( FOdysseyVectorVertex* iVertex )
{
    mSelectedVertexList.remove( iVertex );

    iVertex->SetSelected( false );
}

void
FOdysseyVectorPath::RemoveVertex( FOdysseyVectorVertex* iVertex )
{
    mVertexList.remove( iVertex );

    if( iVertex->IsSelected() )
    {
        UnselectVertex( iVertex );
    }

    Invalidate( INVALIDATE_SHAPE );
    //iVertex->SetPath( nullptr );
}

void
FOdysseyVectorPath::RemoveAllVertices()
{
    mVertexList.remove_if( [this]( FOdysseyVectorVertex* vertex )
                           {
                               if( vertex->IsSelected() )
                               {
                                   UnselectVertex( vertex );
                               }

                               //vertex->SetPath( nullptr );

                               return true;
                           } );
}

void
FOdysseyVectorPath::AddSegment( FOdysseyVectorSegment* iSegment )
{
    mSegmentList.push_back( iSegment );

    iSegment->SetOwner( this );

    iSegment->GetVertex(0)->AddSegment( iSegment );
    iSegment->GetVertex(1)->AddSegment( iSegment );

    InvalidateSegment( iSegment );

    // Invalidate vertices as well (for handle alignement e.g
    iSegment->GetVertex(0)->Invalidate();
    iSegment->GetVertex(1)->Invalidate();

    Invalidate( INVALIDATE_TOPOLOGY );
}

void
FOdysseyVectorPath::RemoveAllSegments()
{
    mSegmentList.remove_if( []( FOdysseyVectorSegment* segment )
                            {
                                segment->GetVertex(0)->RemoveSegment( segment );
                                segment->GetVertex(1)->RemoveSegment( segment );

                                // segment->SetPath( nullptr );

                                return true;
                            } );

    // DO NOT invalidate the segments here, only the path. Otherwise the segment
    // would be added to the list of segments to invalidate BUT the segment does
    // not belong to the path anymore, leading to issues if it has been freed.
    Invalidate( INVALIDATE_TOPOLOGY );
}

void
FOdysseyVectorPath::RemoveSegment( FOdysseyVectorSegment* iSegment )
{
    mSegmentList.remove( iSegment );

    iSegment->GetVertex(0)->RemoveSegment( iSegment );
    iSegment->GetVertex(1)->RemoveSegment( iSegment );

    //iSegment->SetPath( nullptr );

    // DO NOT invalidate the segment here, only the path. Otherwise the segment
    // would be added to the list of segments to invalidate BUT the segment does
    // not belong to the path anymore, leading to issues if it has been freed.
    Invalidate( INVALIDATE_TOPOLOGY );
}

void
FOdysseyVectorPath::Invalidate( uint64 iInvalidationFlags )
{
    //if( iInvalidationFlags & INVALIDATE_MATRIX )
    //{
        // Mark all segment as NOT painting ready to force recalculation of cached subsegments
        // use by the parent paint group
        // commented-out: causes issues when the path dores not directly belong to a paintgroup (belongs to a subgroup e.g)
        //for( FOdysseyVectorSegment* segment : mSegmentList )
        //{
        //    segment->SetPaintingReady( false );
        //}
    //}

    FOdysseyVectorObject::Invalidate( iInvalidationFlags );
}

std::list<FOdysseyVectorSegment*>&
FOdysseyVectorPath::GetSegmentList()
{
    return mSegmentList;
}

std::list<FOdysseyVectorVertex*>&
FOdysseyVectorPath::GetVertexList()
{
    return mVertexList;
}

FOdysseyVectorSegment*
FOdysseyVectorPath::GetLastSegment()
{
    if( mSegmentList.size() == 0 ) return nullptr;

    return mSegmentList.back();
}

FOdysseyVectorSegment*
FOdysseyVectorPath::GetFirstSegment()
{
    if( mSegmentList.size() == 0 ) return nullptr;

    return mSegmentList.front();
}

FOdysseyVectorVertex*
FOdysseyVectorPath::GetLastVertex()
{
    if( mVertexList.size() == 0 ) return nullptr;

    return mVertexList.back();
}

FOdysseyVectorVertex*
FOdysseyVectorPath::GetFirstVertex()
{
    if( mVertexList.size() == 0 ) return nullptr;

    return mVertexList.front();
}

void
FOdysseyVectorPath::InvalidateAllVertices()
{
    for( FOdysseyVectorVertex* vertex : mVertexList )
    {
        vertex->Invalidate();
    }
}

void
FOdysseyVectorPath::InvalidateAllSegments()
{
    for( FOdysseyVectorSegment* segment : mSegmentList )
    {
        segment->Invalidate();
    }
}

// static
::ULIS::FVec2D
FOdysseyVectorPath::GetPerpendicularVector( FOdysseyVectorVertex* iVertex, bool iNormalize )
{
    std::list<FOdysseyVectorSegment*>& segmentList = iVertex->GetSegmentList();
    ::ULIS::FVec2D parallel = { 0.0f, 0.0f };
    ::ULIS::FVec2D perpendicular = { 0.0f, 0.0f };

    if ( segmentList.size() )
    {
        uint32 count = 0;

        for( FOdysseyVectorSegment *segment : segmentList )
        {
            if( segment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
            {
                FOdysseyVectorSegmentCubic *cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
                FOdysseyVectorVertex* vertex0 = cubicSegment->GetVertex(0);
                FOdysseyVectorVertex* vertex1 = cubicSegment->GetVertex(1);
                ::ULIS::FVec2D& point0 = vertex0->GetCoords();
                ::ULIS::FVec2D& point1 = vertex1->GetCoords();
                ::ULIS::FVec2D& ctrlPoint0 = cubicSegment->GetHandle(0)->GetCoords();
                ::ULIS::FVec2D& ctrlPoint1 = cubicSegment->GetHandle(1)->GetCoords();

                if( iVertex == vertex0 )
                {
                    // this is less computation-heavy
                    ::ULIS::FVec2D vec = { ctrlPoint0 - point0 };
                    /*::ULIS::FVec2D vec =  { CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0.GetCoords()
                                                                         , ctrlPoint0.GetCoords()
                                                                         , ctrlPoint1.GetCoords()
                                                                         , point1.GetCoords()
                                                                         , 0.0f ) };*/

                    if ( vec.DistanceSquared() )
                    {
                        vec.Normalize();

                        parallel.x += vec.x;
                        parallel.y += vec.y;
                    }
                }

                if( iVertex == vertex1 )
                {
                    ::ULIS::FVec2D vec = { point1 - ctrlPoint1 };
                    /*::ULIS::FVec2D vec =  { CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0.GetCoords()
                                                                         , ctrlPoint0.GetCoords()
                                                                         , ctrlPoint1.GetCoords()
                                                                         , point1.GetCoords()
                                                                         , 1.0f ) };*/

                    if ( vec.DistanceSquared() )
                    {
                        vec.Normalize();

                        parallel.x += vec.x;
                        parallel.y += vec.y;
                    }
                }
            }
        }

        if ( iNormalize == true )
        {
            if ( parallel.DistanceSquared() )
            {
                parallel.Normalize();
            }

            perpendicular.x =   parallel.y;
            perpendicular.y = - parallel.x;
        }
    }

    return perpendicular;
}

///////////////////////////////////////////

// Note: callbak returns true to end immediately, false to keep tracing
static bool
TraceLine( int32 iX0, int32 iY0, double iT0
         , int32 iX1, int32 iY1, double iT1
         , std::function<bool(int32 iX, int32 iY, double iT)> iCallback )
{
    int32  dx  = ( iX1 - iX0 );
    uint32 ddx = abs ( dx );
    int32  dy  = ( iY1 - iY0 );
    uint32 ddy = abs ( dy );
    double dt  = ( iT1 - iT0 );
    int32  dd  = ( ddx > ddy ) ? ddx : ddy;
    int32  px  = ( dx > 0 ) ? 1 : -1;
    int32  py  = ( dy > 0 ) ? 1 : -1;
    double pt  = ( dd ) ? dt / dd : 0.0f;
    int32  x   = iX0;
    int32  y   = iY0;
    double t   = iT0;
    uint32 cumul = 0;

    if ( ddx > ddy )
    {
        for ( uint32 i = 0; i <= ddx; i++ )
        {
            if( i == ddx ) t = iT1; // to address imprecision, we set the exact value on the last loop

            // return as soon as a point is detected inside the mask
            if ( iCallback( x, y, t ) == true ) {
                return true;
            }

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
            if( i == ddy ) t = iT1; // to address imprecision, we set the exact value on the last loop

            // return as soon as a point is detected inside the mask
            if ( iCallback( x, y, t ) == true ) {
                return true;
            }

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

    return false;
}

// Pick from mask image
void
FOdysseyVectorPath::PickVertex( std::vector<FOdysseyVectorVertex*>& oPickedVertexArray )
{
    BLImage* maskImage = GetEngine()->GetBLMask();
    BLImageData imageData;

    maskImage->getData( &imageData );

    for( FOdysseyVectorVertex* vertex : mVertexList )
    {
        ::ULIS::FVec2D& localCoords = vertex->GetCoords();
        // convert vertex coordinates to world coordinates. Easier to detect collision inside the picking circle.
        BLPoint worldCoords = mWorldMatrix.mapPoint( localCoords.x, localCoords.y );
        int32 x = (int32) worldCoords.x;
        int32 y = (int32) worldCoords.y;

        if( ( x >= 0 ) && ( x < imageData.size.w )
         && ( y >= 0 ) && ( y < imageData.size.h ) )
        {
            uint8 *pixel = static_cast<uint8*>( imageData.pixelData );
            uint32 offset = ( y * imageData.size.w ) + x;
            uint8 pixelValue = pixel[offset];

            if( pixelValue == 255 )
            {
                oPickedVertexArray.push_back( vertex );
            }
        }
    }
}

eSegmentAdditionFlags
FOdysseyVectorPath::SegmentAdditionPolicy( FWayPoint* iWayPoint0
                                         , FWayPoint* iWayPoint1
                                         , bool iSplit )
{
    eSegmentAdditionFlags retFlags = eSegmentAdditionFlags::None;

    if( iSplit )
    {
        retFlags |= eSegmentAdditionFlags::RemoveOriginalSegment;

        if( ( iWayPoint0->flags & FWayPoint::OutsideErasureArea )
         && ( iWayPoint1->flags & FWayPoint::OutsideErasureArea ) )
        {
            retFlags |= eSegmentAdditionFlags::CreateDerivedSegment;
        }

        if( iWayPoint0->flags & FWayPoint::LeavesErasureArea )
        {
            retFlags |= eSegmentAdditionFlags::CreateNewPath;
        }
    }
    else
    {
        if( ( iWayPoint0->flags & FWayPoint::OutsideErasureArea )
         && ( iWayPoint1->flags & FWayPoint::OutsideErasureArea ) )
        {
            retFlags |= eSegmentAdditionFlags::KeepOriginalSegment;
        }
    }

    if( ( iWayPoint0->flags & FWayPoint::OutsideErasureArea )
     && ( iWayPoint1->flags & FWayPoint::EntersErasureArea  ) )
    {
        retFlags |= ( eSegmentAdditionFlags::RemoveOriginalSegment
                    | eSegmentAdditionFlags::CreateDerivedSegment );
    }

    if( ( iWayPoint0->flags & FWayPoint::LeavesErasureArea  )
     && ( iWayPoint1->flags & FWayPoint::OutsideErasureArea ) )
    {
        retFlags |= ( eSegmentAdditionFlags::RemoveOriginalSegment
                    | eSegmentAdditionFlags::CreateDerivedSegment );
    }

    if( ( iWayPoint0->flags & FWayPoint::LeavesErasureArea )
     && ( iWayPoint1->flags & FWayPoint::EntersErasureArea ) )
    {
        retFlags |= ( eSegmentAdditionFlags::RemoveOriginalSegment
                    | eSegmentAdditionFlags::CreateDerivedSegment );
    }

    return retFlags;
}

eVertexAdditionFlags
FOdysseyVectorPath::VertexAdditionPolicy( FWayPoint* iWayPoint, bool iSplit )
{
    eVertexAdditionFlags retFlags = eVertexAdditionFlags::None;

    if( iSplit )
    {
        if( iWayPoint->flags & FWayPoint::Original )
        {
            retFlags |= ( eVertexAdditionFlags::RemoveOriginalVertex );

            if( ( iWayPoint->flags & FWayPoint::LeavesErasureArea )
             || ( iWayPoint->flags & FWayPoint::EntersErasureArea ) )
            {
                if( iWayPoint->vertex->GetSegmentCount() == 2 )
                {
                    retFlags |= ( eVertexAdditionFlags::CreateDerivedVertex );
                }
            }

            if( iWayPoint->flags & FWayPoint::OutsideErasureArea )
            {
                retFlags |= ( eVertexAdditionFlags::CreateDerivedVertex );
            }
        }
    }

    if( iWayPoint->flags & FWayPoint::InsideErasureArea )
    {
        if( iWayPoint->flags & FWayPoint::Original )
        {
            retFlags |= eVertexAdditionFlags::RemoveOriginalVertex;
        }
    }

    if( iWayPoint->flags & FWayPoint::EntersErasureArea )
    {
        if( ( iWayPoint->flags & FWayPoint::Original ) == 0 )
        {
            retFlags |= eVertexAdditionFlags::CreateBoundaryVertex;
        }
    }

    if( iWayPoint->flags & FWayPoint::LeavesErasureArea )
    {
        if( ( iWayPoint->flags & FWayPoint::Original ) == 0 )
        {
            retFlags |= eVertexAdditionFlags::CreateBoundaryVertex;
        }
    }

    return retFlags;
}

void
FOdysseyVectorPath::ParseWayPoints( std::vector<FWayPoint>& iWayPointArray
                                  , std::vector<FWayFragment>& iWayFragmentArray
                                  , std::vector<FOdysseyVectorObject*>& oAddedPathArray
                                  , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                                  , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                                  , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                                  , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                                  , bool iSplit )
{
    FOdysseyVectorPath* currentPath = iSplit ? nullptr : this;
    uint32 addedSegmentCountBeforeAlter = oAddedSegmentArray.size();

    if( iWayPointArray.size() )
    {
        FWayPoint& firstWayPoint = iWayPointArray[0];

        // step 1 : create needed vertices
        for( FWayPoint& wayPoint : iWayPointArray )
        {
            eVertexAdditionFlags vertexAdditionFlags = VertexAdditionPolicy( &wayPoint, iSplit );

            if( ( vertexAdditionFlags & eVertexAdditionFlags::RemoveOriginalVertex ) == eVertexAdditionFlags::RemoveOriginalVertex )
            {
                // mark original vertex for deletion. No duplicates (duplicates happen in case of loops)
                if( std::find( oRemovedVertexArray.begin(), oRemovedVertexArray.end(), wayPoint.vertex ) == oRemovedVertexArray.end() )
                {
                    oRemovedVertexArray.push_back( wayPoint.vertex );
                }
            }

            // boundary vertices are guaranteed unique per nature, no need to check uniqueness
            if( ( vertexAdditionFlags & eVertexAdditionFlags::CreateBoundaryVertex ) == eVertexAdditionFlags::CreateBoundaryVertex )
            {
                oAddedVertexArray.push_back( wayPoint.vertex );
            }

            if( ( vertexAdditionFlags & eVertexAdditionFlags::CreateDerivedVertex ) == eVertexAdditionFlags::CreateDerivedVertex )
            {
                ::ULIS::FVec2D& coords = wayPoint.vertex->GetCoords();
                double radius = wayPoint.vertex->GetRadius();
                bool handleAligned = wayPoint.vertex->IsHandleAligned();
                uint32 vertexID = wayPoint.vertex->GetID();

                // replaces the current original vertex. The latter is already saved in oRemovedVertexArray
                wayPoint.vertex = new FOdysseyVectorVertex( coords.x, coords.y, radius );
                wayPoint.vertex->SetHandleAligned( handleAligned );
                wayPoint.vertex->SetID( vertexID );

                // a new vertex for each former vertex
                oAddedVertexArray.push_back( wayPoint.vertex );
            }
        }

        for( FWayFragment& wayFragment : iWayFragmentArray )
        {
            FWayPoint* wayPoint0 = &iWayPointArray[wayFragment.indexWayPoint0];
            FWayPoint* wayPoint1 = &iWayPointArray[wayFragment.indexWayPoint1];
            eSegmentAdditionFlags segmentAdditionFlags = SegmentAdditionPolicy( wayPoint0, wayPoint1, iSplit );

            //UE_LOG(LogTemp, Warning, TEXT("fragment : %d:%x %d:%x"), wayPoint0->vertex->GetID(), wayPoint0->flags
            //                                                       , wayPoint1->vertex->GetID(), wayPoint1->flags );


            if( ( segmentAdditionFlags & eSegmentAdditionFlags::RemoveOriginalSegment ) == eSegmentAdditionFlags::RemoveOriginalSegment )
            {
                // mark original segment for deletion. No duplicates
                if( std::find( oRemovedSegmentArray.begin(), oRemovedSegmentArray.end(), wayFragment.segment ) == oRemovedSegmentArray.end() )
                {
                    oRemovedSegmentArray.push_back( wayFragment.segment );
                }
            }

            if( ( segmentAdditionFlags & eSegmentAdditionFlags::CreateDerivedSegment ) == eSegmentAdditionFlags::CreateDerivedSegment )
            {
                if( ( segmentAdditionFlags & eSegmentAdditionFlags::CreateNewPath ) == eSegmentAdditionFlags::CreateNewPath )
                {
                    currentPath = new FOdysseyVectorPath( GetName() );
                    // for postprocessing. the path is not added to the parent yet
                    currentPath->SetParent( mParent );
                    ExportParam( currentPath, true );
                    CopyTransformation( *currentPath );

                    oAddedPathArray.push_back( currentPath );
                }

                FOdysseyVectorVertex* destVertex0 = wayPoint0->vertex;
                FOdysseyVectorVertex* destVertex1 = wayPoint1->vertex;
                // currentPath can be null at first for some segments. This will be handled
                // in a second pass this is needed for segment chains that loop.
                FOdysseyVectorSegmentCubic* newSegment = new FOdysseyVectorSegmentCubic( currentPath
                                                                                       , destVertex0
                                                                                       , wayFragment.bezier[1].x
                                                                                       , wayFragment.bezier[1].y
                                                                                       , wayFragment.bezier[2].x
                                                                                       , wayFragment.bezier[2].y
                                                                                       , destVertex1
                                                                                       , true );

                // store path here even if technically the vertex does not belong to the path yet.
                // it will once we call path->AddVertex() in the FOdysseyVectorPath::Erase() func.
                wayPoint0->vertex->SetOwner( currentPath );
                wayPoint1->vertex->SetOwner( currentPath );

                // mark new segment for addition
                oAddedSegmentArray.push_back( newSegment );
            }
        }

        // second pass to assign segment that belong to no path yet.
        // This is needed to handle chains that loop. This is necessary only in split mode.
        // This works because the segments are ordered.
        if( iSplit )
        {
            for( int i = addedSegmentCountBeforeAlter; i < oAddedSegmentArray.size(); i++ )
            {
                FOdysseyVectorSegment* segment = oAddedSegmentArray[i];
                FOdysseyVectorPath* segmentPath = segment->GetOwnerAsPath();

                if( segmentPath == nullptr )
                {
                    FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);
                    FOdysseyVectorVertex* vertex1 = segment->GetVertex(1);

                    if( vertex0->GetOwner() )
                    {
                        segmentPath = vertex0->GetOwnerAsPath();
                    }

                    if( vertex1->GetOwner() )
                    {
                        segmentPath = vertex1->GetOwnerAsPath();
                    }

                    // if segmentPath is still null, it means segment vertices were
                    // not assigned a path via another neighbour segment. In that
                    // case we create a new path and assign them to it.
                    if( segmentPath == nullptr )
                    {
                        segmentPath = new FOdysseyVectorPath( GetName() );
                        // for postprocessing. the path is not added to the parent yet
                        segmentPath->SetParent( mParent );
                        ExportParam( segmentPath, true );
                        CopyTransformation( *segmentPath );

                        oAddedPathArray.push_back( segmentPath );
                    }

                    // store ptr now for later use with path->AddVertex() / path->AddSegment()
                    vertex0->SetOwner( segmentPath );
                    vertex1->SetOwner( segmentPath );
                    segment->SetOwner( segmentPath );
                }
            }
        }
    }
}

void
FOdysseyVectorPath::Subdivide( std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                             , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                             , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray )
{
    uint32 addedVertexArrayCount = oAddedVertexArray.size();
    uint32 addedSegmentArrayCount = oAddedSegmentArray.size();

    oRemovedSegmentArray.reserve( oRemovedSegmentArray.capacity() + mSegmentList.size() );

    for( FOdysseyVectorSegment* segment : mSegmentList )
    {
        ::ULIS::FVec2D point = segment->GetPointAt( 0.5f );

        segment->Split( point, 0.5f, oAddedVertexArray, oAddedSegmentArray );

        oRemovedSegmentArray.push_back( segment );
    }

    RemoveAllSegments();

    for( uint32 i = addedVertexArrayCount; i < oAddedVertexArray.size(); i++ )
    {
        AddVertex( oAddedVertexArray[i] );
    }

    for( uint32 i = addedSegmentArrayCount; i < oAddedSegmentArray.size(); i++  )
    {
        AddSegment( oAddedSegmentArray[i] );
    }
}

bool
FOdysseyVectorPath::Erase( std::vector<FOdysseyVectorObject*>& oAddedPathArray
                         , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                         , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                         , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                         , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                         , bool iWholeSection
                         , bool iSplit )
{
    BLImage* blimg = GetEngine()->GetBLMask(); // the mask image must be selected by the vector engine at this point


    if( blimg )
    {
        uint32 removedSegmentCountBeforeAlter = oRemovedSegmentArray.size();
        uint32 removedVertexCountBeforeAlter = oRemovedVertexArray.size();
        uint32 addedSegmentCountBeforeAlter = oAddedSegmentArray.size();
        uint32 addedVertexCountBeforeAlter = oAddedVertexArray.size();
        BLImageData imageData;

        blimg->getData( &imageData );

        for( FOdysseyVectorChain& chain : mChainArray )
        {
            std::vector<FWayFragment> wayFragmentArray;
            std::vector<FWayPoint> wayPointArray;

            wayPointArray.reserve( 10 );
            wayFragmentArray.reserve( 10 );

            // this will return an array of "way points". they are the original
            // points + the points at a contrast zone (erasure boundary).
            // it will fill a "way segment" array, which are the segments that
            // should be created between those way points.
            // no vertex or segment is allocated in chain.HitMask(). the allocation differs
            // depending on the spliting mode, so it is the responsibility of EraseNoSplit() and
            // EraseNoSplit() to allocate new vertices / segments or paths
            bool hit = iWholeSection ? chain.EraseSections( &imageData
                                                          , wayPointArray
                                                          , wayFragmentArray )
                                     : chain.EraseSegments( &imageData
                                                          , wayPointArray
                                                          , wayFragmentArray );

            if( hit )
            {
                // proceed now we know we have hit anything
                // determine which vertices / segments will be deleted and which will be kept
                // it differs in SPLIT and NOSPLIT modes. SPLIT modes removes only those erased,
                // split removes all
                ParseWayPoints( wayPointArray
                              , wayFragmentArray
                              , oAddedPathArray
                              , oAddedVertexArray
                              , oAddedSegmentArray
                              , oRemovedVertexArray
                              , oRemovedSegmentArray
                              , iSplit );
            }
        }

        for( int i = removedSegmentCountBeforeAlter; i < oRemovedSegmentArray.size(); i++ )
        {
            oRemovedSegmentArray[i]->GetOwnerAsPath()->RemoveSegment( oRemovedSegmentArray[i] );
        }

        for( int i = removedVertexCountBeforeAlter; i < oRemovedVertexArray.size(); i++ )
        {
            oRemovedVertexArray[i]->GetOwnerAsPath()->RemoveVertex( oRemovedVertexArray[i] );
        }

        for( int i = addedVertexCountBeforeAlter; i < oAddedVertexArray.size(); i++ )
        {
            oAddedVertexArray[i]->GetOwnerAsPath()->AddVertex( oAddedVertexArray[i] );
        }

        for( int i = addedSegmentCountBeforeAlter; i < oAddedSegmentArray.size(); i++ )
        {
            oAddedSegmentArray[i]->GetOwnerAsPath()->AddSegment( oAddedSegmentArray[i] );
        }
    }

    //UE_LOG(LogTemp, Warning, TEXT("Removed Segments: %d"), oRemovedSegmentArray.size() );
    //UE_LOG(LogTemp, Warning, TEXT("Removed Vertices: %d"), oRemovedVertexArray.size() );
    //UE_LOG(LogTemp, Warning, TEXT("Added Segments: %d"), oAddedVertexArray.size() );
    //UE_LOG(LogTemp, Warning, TEXT("Added Vertices: %d"), oAddedSegmentArray.size() );

    Invalidate( INVALIDATE_SHAPE | INVALIDATE_COLOR | INVALIDATE_TOPOLOGY );

    // return true if path is empty
    return ( mVertexList.size() == 0 ) && ( mSegmentList.size() == 0 );
}

bool
FOdysseyVectorPath::PickShape( const ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
    if( iSelectionFlags & PICK_MATH_BASED )
    {
        BLPoint pt = mInverseWorldMatrix.mapPoint( iRoi.x, iRoi.y );

        if( mBBox.HitTest( ::ULIS::FVec2D( pt.x, pt.y ) ) )
        {
            for( FOdysseyVectorSegment* segment : mSegmentList )
            {
                if( segment->Pick( pt.x, pt.y, 0.0f ) )
                {
                    return true;
                }
            }
        }
    }

    if ( iSelectionFlags & PICK_MASK_BASED )
    {
        BLImage* blimg = GetEngine()->GetBLMask(); // the mask image must be selected by the vector engine at this point
        BLImageData imageData;

        if( blimg )
        {
            blimg->getData( &imageData );

            for( FOdysseyVectorSegment* segment : mSegmentList )
            {
                std::vector<FOdysseyVectorFraction>& fractionCache = segment->GetFractionCache();

                for( uint32 i = 0; i < fractionCache.size(); i++ )
                {
                    ::ULIS::FVec2D& p0Coords = fractionCache[i].point[0]->GetCoords();
                    ::ULIS::FVec2D& p1Coords = fractionCache[i].point[1]->GetCoords();
                    BLPoint p0 = mWorldMatrix.mapPoint( p0Coords.x, p0Coords.y );
                    BLPoint p1 = mWorldMatrix.mapPoint( p1Coords.x, p1Coords.y  );
                    bool pointHitMask = TraceLine( p0.x, p0.y, 0.0f
                                                 , p1.x, p1.y, 0.0f
                                                 , [&imageData]( int32 iX, int32 iY, double iT)
                                                   {
                                                        if( ( iX >= 0 && iX < imageData.size.w )
                                                         && ( iY >= 0 && iY < imageData.size.h ) )
                                                        {
                                                            uint8 *pixel = static_cast<uint8*>(imageData.pixelData);
                                                            uint32 offset = ( iY * imageData.size.w ) + iX;

                                                            return ( pixel[offset] ) ? true : false;
                                                        }

                                                        return false;
                                                   });

                    if( pointHitMask )
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool
FOdysseyVectorPath::PickPoint( double iWorldX
                             , double iWorldY
                             , double iSelectionRadius
                             , std::vector<FOdysseyVectorVertex*>& oPickedVertexArray
                             , std::vector<FOdysseyVectorHandleSegment*>& oPickedHandleArray
                             , uint64 iSelectionFlags )
{
    bool anythingPicked = false;

    for( FOdysseyVectorVertex* vertex : mVertexList )
    {
        ::ULIS::FVec2D perpendicularVector = FOdysseyVectorPath::GetPerpendicularVector( vertex, true );
        BLPoint worldPerpendicularVector = mWorldMatrix.mapVector( perpendicularVector.x * vertex->GetRadius()
                                                                 , perpendicularVector.y * vertex->GetRadius() );

        // Pick vertex
        if ( iSelectionFlags & PICK_VERTEX )
        {
            ::ULIS::FVec2D& localCoords = vertex->GetCoords();
            // convert vertex coordinates to world coordinates. Easier to detect collision inside the picking circle.
            BLPoint worldCoords = mWorldMatrix.mapPoint( localCoords.x, localCoords.y );
            ::ULIS::FVec2D dif = ::ULIS::FVec2D( worldCoords.x - iWorldX, worldCoords.y - iWorldY );

            if( dif.Distance() <= iSelectionRadius )
            {
                oPickedVertexArray.push_back( vertex );

                anythingPicked = true;
            }
        }

        // Pick vertex handle
        if( iSelectionFlags & PICK_HANDLE_VERTEX )
        {
            ::ULIS::FVec2D localCoords[2];

            vertex->GetHandlePosition( localCoords );

            for( int i = 0; i < 2; i++ )
            {
                BLPoint worldCoords = mWorldMatrix.mapPoint( localCoords[i].x, localCoords[i].y );
                ::ULIS::FVec2D dif = ::ULIS::FVec2D( worldCoords.x - iWorldX
                                                   , worldCoords.y - iWorldY );

                if( dif.Distance() <= iSelectionRadius )
                {
                    oPickedVertexArray.push_back( vertex );

                    anythingPicked = true;

                    break;
                }
            }
        }
    }

    // Pick segment handles
    if( iSelectionFlags & PICK_HANDLE_SEGMENT )
    {
        for( FOdysseyVectorSegment* segment : mSegmentList )
        {
            // TODO: hit-test with segment's bounding box.
            FOdysseyVectorHandleSegment* handle0 = segment->GetHandle(0);
            FOdysseyVectorHandleSegment* handle1 = segment->GetHandle(1);
            ::ULIS::FVec2D& handle0LocalCoords = handle0->GetCoords();
            ::ULIS::FVec2D& handle1LocalCoords = handle1->GetCoords();
            // convert handles coordinates to world coordinates. Easier to detect collision inside the picking circle.
            BLPoint handle0WorldCoords = mWorldMatrix.mapPoint( handle0LocalCoords.x, handle0LocalCoords.y );
            BLPoint handle1WorldCoords = mWorldMatrix.mapPoint( handle1LocalCoords.x, handle1LocalCoords.y );
            ::ULIS::FVec2D dif0 = ::ULIS::FVec2D( handle0WorldCoords.x - iWorldX, handle0WorldCoords.y - iWorldY );
            ::ULIS::FVec2D dif1 = ::ULIS::FVec2D( handle1WorldCoords.x - iWorldX, handle1WorldCoords.y - iWorldY );

            if( dif0.Distance() <= iSelectionRadius )
            {
                oPickedHandleArray.push_back( handle0 );

                anythingPicked = true;
            }

            if( dif1.Distance() <= iSelectionRadius )
            {
                oPickedHandleArray.push_back( handle1 );

                anythingPicked = true;
            }
        }
    }

    return anythingPicked;
}

void
FOdysseyVectorPath::Cut( const ::ULIS::FVec2D& iLinePoint0
                       , const ::ULIS::FVec2D& iLinePoint1
                       , std::vector<FOdysseyVectorVertex*>& oNewVertexArray
                       , std::vector<FOdysseyVectorSegment*>& oNewSegmentArray
                       , std::vector<FOdysseyVectorSegment*>& oOldSegmentArray )
{
    // let's work on a copy as we are going to remove items in the original list
    std::list<FOdysseyVectorSegment*> tmpSegmentList = mSegmentList;
    uint32 vertexCount = oNewVertexArray.size();
    uint32 segmentCount = oNewSegmentArray.size();

    while( tmpSegmentList.size () )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(tmpSegmentList.back());

        if ( cubicSegment->Cut ( iLinePoint0, iLinePoint1, oNewVertexArray, oNewSegmentArray ) )
        {
            RemoveSegment ( cubicSegment );

            //cubicSegment->Invalidate();

            oOldSegmentArray.push_back( cubicSegment );
        }

        tmpSegmentList.pop_back ();
    }

    for( int i = vertexCount; i < oNewVertexArray.size(); i++ )
    {
        oNewVertexArray[i]->SetHandleAligned( true );

        AddVertex( oNewVertexArray[i] );
    }

    for( int i = segmentCount; i < oNewSegmentArray.size(); i++ )
    {
        AddSegment( oNewSegmentArray[i] ); // invalidates the segment as well

        //oNewSegmentArray[i]->Invalidate();
    }
}

// only used internally by FOdysseyVectorPath::DeletePoint(). Declare in CPP file
static FOdysseyVectorVertex*
GetStitchingVertex( FOdysseyVectorVertex* iVertex
                  , FOdysseyVectorSegment* iSegment
                  , std::vector<FOdysseyVectorVertex*>& iPickedVertexArray
                  , ::ULIS::FVec2D& oHandle )
{
    FOdysseyVectorVertex *currentVertex = iVertex;
    FOdysseyVectorSegment *currentSegment = iSegment;

    do
    {
        // return the nextVertex if it is not marked for deletion. Then it will be stitched with its counterpart, if any.
        if( std::find( iPickedVertexArray.begin(), iPickedVertexArray.end(), currentVertex ) == iPickedVertexArray.end() )
        {
            oHandle = currentSegment->GetHandle( currentVertex )->GetCoords();

            return currentVertex;
        }

        // move to the next segment
        currentSegment = currentVertex->GetOtherSegment( currentSegment );
        currentVertex = currentSegment ? currentSegment->GetOtherVertex( currentVertex ) : nullptr;

    } while( ( currentVertex ) && ( currentVertex != iVertex ) );
                                  // loop detection

    return nullptr;
}

// only used internally by FOdysseyVectorPath::DeletePoint(). Declare in CPP file
typedef struct FStitchingPair
{
    FOdysseyVectorVertex* vertex[2];
    ::ULIS::FVec2D handle[2];

    FStitchingPair( FOdysseyVectorVertex* iVertex0, ::ULIS::FVec2D& iHandle0
                  , FOdysseyVectorVertex* iVertex1, ::ULIS::FVec2D& iHandle1 )
    {
        // Note: ordering will ease comparisons between stitching pairs.
        vertex[0] = iVertex0 < iVertex1 ? iVertex0 : iVertex1;
        handle[0] = iVertex0 < iVertex1 ? iHandle0 : iHandle1;

        vertex[1] = iVertex0 < iVertex1 ? iVertex1 : iVertex0;
        handle[1] = iVertex0 < iVertex1 ? iHandle1 : iHandle0;
    }

    bool operator==(const FStitchingPair& rhs)
    {
        return ( ( vertex[0] == rhs.vertex[0] ) && ( vertex[1] == rhs.vertex[1] ) );
    }
} FStitchingPair;

FOdysseyVectorSegment*
FOdysseyVectorPath::GetSegmentByID( uint32 iID )
{
    for( FOdysseyVectorSegment* segment : mSegmentList )
    {
        if( segment->GetID() == iID )
        {
            return segment;
        }
    }

    return nullptr;
}

FOdysseyVectorVertex*
FOdysseyVectorPath::GetVertexByID( uint32 iID )
{
    for( FOdysseyVectorVertex* vertex : mVertexList )
    {
        if( vertex->GetID() == iID )
        {
            return vertex;
        }
    }

    return nullptr;
}

//static
void
FOdysseyVectorPath::DeleteVertex( FOdysseyVectorPath* iPath
                                , std::vector<FOdysseyVectorVertex*>& iPickedVertexArray
                                , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                                , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                                , std::vector<FOdysseyVectorPath*>& iRemovedPathArray
                                , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray )
{
    std::vector<FOdysseyVectorVertex*> extendedVertexArray;
    std::vector<FStitchingPair> stitchingPairArray;

    stitchingPairArray.reserve( 10 );
    extendedVertexArray.reserve( 10 );

    extendedVertexArray = iPickedVertexArray;

    // first step
    // Build stitching pairs by finding a vertex that is not doomed for deletion on both sides.
    for( int i = 0; i < iPickedVertexArray.size(); i++ )
    {
        FOdysseyVectorVertex* vertex = iPickedVertexArray[i];
        std::list<FOdysseyVectorSegment*>& segmentList = vertex->GetSegmentList();
        uint32 segmentCount = segmentList.size();

        if( segmentCount )
        {
            ::ULIS::FVec2D handle[2];
            FOdysseyVectorSegment* segment0 = segmentList.front();
            FOdysseyVectorSegment* segment1 = ( segmentCount > 1  ) ? segmentList.back() : nullptr;
            FOdysseyVectorVertex* stitchingVertex[2] = { segment0 ? GetStitchingVertex( segment0->GetOtherVertex(vertex)
                                                                                      , segment0
                                                                                      , iPickedVertexArray
                                                                                      , handle[0] ) : nullptr
                                                       , segment1 ? GetStitchingVertex( segment1->GetOtherVertex(vertex)
                                                                                      , segment1
                                                                                      , iPickedVertexArray
                                                                                      , handle[1] ) : nullptr };

            if( stitchingVertex[0] == stitchingVertex[1] ) // e.g loops
            {
                if( stitchingVertex[0] )
                {
                    // for vertices that were not picked but that cannot be stitched, delete them as well.
                    // they could aready be part of the picked vertices. We have to check if that's not already the case.
                    if( std::find( extendedVertexArray.begin(), extendedVertexArray.end(), stitchingVertex[0] ) == extendedVertexArray.end() )
                    {
                        extendedVertexArray.push_back( stitchingVertex[0] );
                    }
                }
            }
            else // stitchingVertex[0] != stitchingVertex[1]
            {
                if ( stitchingVertex[0] && stitchingVertex[1] )
                {
                    FStitchingPair stitchingPair = FStitchingPair( stitchingVertex[0], handle[0]
                                                                 , stitchingVertex[1], handle[1] );

                    if( std::find( stitchingPairArray.begin(), stitchingPairArray.end(), stitchingPair ) == stitchingPairArray.end() )
                    {
                        stitchingPairArray.push_back( stitchingPair );
                    }
                }
                else
                {
                    if( stitchingVertex[0] && ( stitchingVertex[0]->GetSegmentCount() == 1 ) )
                    {
                        // for vertices that were not picked but that cannot be stitched, delete them as well.
                        // they could aready be part of the picked vertices. We have to check if that's not already the case.
                        if( std::find( extendedVertexArray.begin(), extendedVertexArray.end(), stitchingVertex[0] ) == extendedVertexArray.end() )
                        {
                            extendedVertexArray.push_back( stitchingVertex[0] );
                        }
                    }

                    if( stitchingVertex[1] && ( stitchingVertex[1]->GetSegmentCount() == 1 ) )
                    {
                        // for vertices that were not picked but that cannot be stitched, delete them as well.
                        // they could aready be part of the picked vertices. We have to check if that's not already the case.
                        if( std::find( extendedVertexArray.begin(), extendedVertexArray.end(), stitchingVertex[1] ) == extendedVertexArray.end() )
                        {
                            extendedVertexArray.push_back( stitchingVertex[1] );
                        }
                    }
                }
            }
        }
    }

    // second step
    // the actual deletion
    for( int i = 0; i < extendedVertexArray.size(); i++ )
    {
        FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>( extendedVertexArray[i] );
        // Note: work on a copy of the list, because deletion will alter the segment list
        std::list<FOdysseyVectorSegment*> segmentList = vertex->GetSegmentList();

        for( FOdysseyVectorSegment* segment : segmentList )
        {
            iPath->RemoveSegment( segment );
            // for undoing
            iRemovedSegmentArray.push_back( segment );
        }

        iPath->RemoveVertex( vertex );
        // for undoing
        iRemovedVertexArray.push_back( vertex );
    }

    // third step
    // stitch
    for( int i = 0; i < stitchingPairArray.size(); i++ )
    {
        FOdysseyVectorVertex* stitchingVertex0 = stitchingPairArray[i].vertex[0];
        FOdysseyVectorVertex* stitchingVertex1 = stitchingPairArray[i].vertex[1];
        ::ULIS::FVec2D handle0 = stitchingPairArray[i].handle[0];
        ::ULIS::FVec2D handle1 = stitchingPairArray[i].handle[1];
        FOdysseyVectorSegmentCubic* stitchedSegment = new FOdysseyVectorSegmentCubic( iPath
                                                                                    , stitchingVertex0
                                                                                    , handle0.x
                                                                                    , handle0.y
                                                                                    , handle1.x
                                                                                    , handle1.y
                                                                                    , stitchingVertex1
                                                                                    , true );


        iPath->AddSegment( stitchedSegment );
        // for undoing
        iAddedSegmentArray.push_back( stitchedSegment );
    }

    iPath->InvalidateAllSegments();

    // remove path if empty
    if( iPath->GetSegmentList().size() == 0 )
    {
        // for undoing
        iRemovedPathArray.push_back( iPath );
    }
}

void
FOdysseyVectorPath::AlterRadius( double iRatioRadius )
{
    for( FOdysseyVectorVertex* vertex : mVertexList )
    {
        vertex->SetRadius( vertex->GetRadius() * iRatioRadius );
    }
}

void
FOdysseyVectorPath::DrawSegment( BLContext* iBLContext
                                // the engine that draws may not be the parent engine
                                // so we use the on epassed as parameter
                               , FOdysseyVectorEngine* iVectorEngine
                               , FOdysseyVectorSegment* iSegment
                               , double iStartU
                               , double iEndU
                               , double iCombinedOpacity
                               , uint64 iDrawingFlags )
{
    // don't draw if segment is ridiculously small, it possess artefacts
    // in the fractionCache because the polygon calculations where wrong
    if( iSegment->GetLength() > 0.000001f )
    {
        // if the segment is textured, use our own routines.
        if( mBrush.pixels )
        {
            std::vector<FOdysseyVectorFraction>& fractionCache = iSegment->GetFractionCache();
            FColor foregroundColor = GetForegroundColor();
            double difU = iEndU - iStartU;

            // for testing
            //iBLContext->setStrokeWidth( 1.0f );
            //iBLContext->setStrokeStyle( BLRgba32( 0, 0, 0, 255 ) );

            for( int i = 0; i < fractionCache.size(); i++ )
            {
            // for testing
    /*
                BLPoint pt[6] = { { fractionCache[i].polygon.point[0].x, fractionCache[i].polygon.point[0].y }
                                , { fractionCache[i].polygon.point[1].x, fractionCache[i].polygon.point[1].y }
                                , { fractionCache[i].polygon.point[2].x, fractionCache[i].polygon.point[2].y }
                                , { fractionCache[i].polygon.point[3].x, fractionCache[i].polygon.point[3].y }
                                , { fractionCache[i].polygon.point[4].x, fractionCache[i].polygon.point[4].y }
                                , { fractionCache[i].polygon.point[5].x, fractionCache[i].polygon.point[5].y } };
    */
                FOdysseyVectorFraction* fraction = &fractionCache[i];
                // deprecated
    /*
                double hexaU[6] = { iStartU + ( fraction->polygon.U[0] * difU )
                                  , iStartU + ( fraction->polygon.U[1] * difU )
                                  , iStartU + ( fraction->polygon.U[2] * difU )
                                  , iStartU + ( fraction->polygon.U[3] * difU )
                                  , iStartU + ( fraction->polygon.U[4] * difU )
                                  , iStartU + ( fraction->polygon.U[5] * difU ) };

                double quadU[6] = { iStartU + ( fraction->polygon.U[0] * difU )
                                  , iStartU + ( fraction->polygon.U[1] * difU )
                                  , iStartU + ( fraction->polygon.U[2] * difU )
                                  , iStartU + ( fraction->polygon.U[3] * difU )
                                  , iStartU + ( fraction->polygon.U[4] * difU )
                                  , iStartU + ( fraction->polygon.U[5] * difU ) };
    */
                // We have to divide the hexagon into 2 quads or else it can creates artefacts
                // due to UV Mapping when the hexagon is not "a square".
                ::ULIS::FVec2D quad0P[4] = { fraction->polygon.point[0]
                                           , fraction->polygon.point[1]
                                           , fraction->polygon.point[2]
                                           , fraction->polygon.point[3] };
                double quad0U[4] = { iStartU + ( fraction->polygon.U[0] * difU )
                                   , iStartU + ( fraction->polygon.U[1] * difU )
                                   , iStartU + ( fraction->polygon.U[2] * difU )
                                   , iStartU + ( fraction->polygon.U[3] * difU ) };
                double quad0V[4] = { fraction->polygon.V[0]
                                   , fraction->polygon.V[1]
                                   , fraction->polygon.V[2]
                                   , fraction->polygon.V[3] };
                ::ULIS::FVec2D quad1P[4] = { fraction->polygon.point[3]
                                           , fraction->polygon.point[4]
                                           , fraction->polygon.point[5]
                                           , fraction->polygon.point[0] };
                double quad1U[4] = { iStartU + ( fraction->polygon.U[3] * difU )
                                   , iStartU + ( fraction->polygon.U[4] * difU )
                                   , iStartU + ( fraction->polygon.U[5] * difU )
                                   , iStartU + ( fraction->polygon.U[0] * difU ) };
                double quad1V[4] = { fraction->polygon.V[3]
                                   , fraction->polygon.V[4]
                                   , fraction->polygon.V[5]
                                   , fraction->polygon.V[0] };

                uint64 polygonDrawingFlags = 0;

                polygonDrawingFlags |= mBrush.ColorFromBrush    ? 0 : FPolygonDrawingFlags::BRUSHALPHAONLY;
                polygonDrawingFlags |= mBrush.BilinearFiltering ? FPolygonDrawingFlags::BILINEARFILTERING : 0;

                // should be a static function
                iVectorEngine->FillQuad( iBLContext
                                       , quad0P
                                       , quad0U
                                       , quad0V
                                       , iCombinedOpacity
                                       , foregroundColor
                                       , (int8*) mBrush.pixels // will be nullptr if no texture is loaded
                                       , mBrush.width
                                       , mBrush.height
                                       , mBrush.bitsPerPixel
                                       , polygonDrawingFlags );

                iVectorEngine->FillQuad( iBLContext
                                       , quad1P
                                       , quad1U
                                       , quad1V
                                       , iCombinedOpacity
                                       , foregroundColor
                                       , (int8*) mBrush.pixels // will be nullptr if no texture is loaded
                                       , mBrush.width
                                       , mBrush.height
                                       , mBrush.bitsPerPixel
                                       , polygonDrawingFlags );
                // for testing
                // iBLContext->strokePolygon( pt, 6 );
            }
        }
        else // otherwise use Blend2D's
        {
            iSegment->Draw( iBLContext );
        }
    }
}

void
FOdysseyVectorPath::DrawShape( BLContext* iBLContext
                             , const ::ULIS::FRectD& iInvalidationArea
                             , double iCombinedOpacity
                             , uint64 iDrawingFlags )
{
    FOdysseyVectorEngine* vectorEngine = GetEngine();
    ::ULIS::FRectD worldBBox = GetBBox( true );
    ::ULIS::FVec2D worldBBoxMin;
    ::ULIS::FVec2D worldBBoxMax;
    ::ULIS::FVec2D invalidationAreaMin;
    ::ULIS::FVec2D invalidationAreaMax;

    FOdysseyVector::GetRectMinMax<double>( worldBBox, worldBBoxMin, worldBBoxMax );
    FOdysseyVector::GetRectMinMax<double>( iInvalidationArea
                                         , invalidationAreaMin
                                         , invalidationAreaMax );

    if( worldBBox.Area() > 1.0f ) // do not draw if < 1 pixel
    {
        // do not draw if outside screen
        if( ( ( worldBBoxMin.x ) < invalidationAreaMax.x )
         && ( ( worldBBoxMax.x ) > invalidationAreaMin.x )
         && ( ( worldBBoxMin.y ) < invalidationAreaMax.y )
         && ( ( worldBBoxMax.y ) > invalidationAreaMin.y ) )
        {
            // for testing purpose (vector brush)
            if( FOdysseyVectorBrush::GetDemoBrush() )
            {
                for( FOdysseyVectorChain& chain : mChainArray )
                {
                    FOdysseyVectorBrush::GetDemoBrush()->Draw( iBLContext
                                                             , iInvalidationArea
                                                             , iCombinedOpacity
                                                             , &chain
                                                             , iDrawingFlags );
                }
            }
            else
            {
                for( FOdysseyVectorChain& chain : mChainArray )
                {
                    DrawChain( iBLContext, vectorEngine, iCombinedOpacity, chain, iDrawingFlags );
                }
            }
        }
    }
}

void
FOdysseyVectorPath::DrawChain( BLContext* iBLContext
                             , FOdysseyVectorEngine* iVectorEngine
                             , double iCombinedOpacity
                             , FOdysseyVectorChain& iChain
                             , uint64 iDrawingFlags )
{
    FColor color = mForegroundBucket.GetColor();
    BLRgba32 strokeColor = ( iDrawingFlags & FOdysseyVectorEngine::DRAWING_IGNORECOLOR ) ? BLRgba32( 0, 0, 0, 255 )
                                                                                         : BLRgba32( color.R, color.G, color.B, 255 * iCombinedOpacity /*color.A * iCombinedOpacity*/ );

    BLImage* image = iBLContext->targetImage();
    BLImageData& imageData = iVectorEngine->GetRenderData();
    ::ULIS::FRectD screen;

    screen.x = 0;
    screen.y = 0;
    screen.w = imageData.size.w;
    screen.h = imageData.size.h;

    if( iDrawingFlags & FOdysseyVectorEngine::DRAWING_WIREFRAME )
    {
        DrawStructure( iBLContext, BLRgba32( 255, 255, 255, 255 ), 2.0f, true );
    }
    else
    {
        UTexture2D* texture = mBrush.GetTexture();

        if( texture )
        {
            //double startU = mBrush.Revert ? 1.0f : 0.0f;
            double remainingSegmentLength = 0.0f;

            mBrush.Lock();

            iChain.IterateSegments( [ this
                                    , texture
                                    , iBLContext
                                    , iVectorEngine
                                    , &iCombinedOpacity
                                    , &iDrawingFlags
                                    //, &startU
                                    , &remainingSegmentLength
                                    , &screen
                                    , &iChain ]( FOdysseyVectorVertex* vertex, FOdysseyVectorSegment* segment ) -> bool
            {
                FOdysseyVectorVertex* otherVertex = segment->GetOtherVertex( vertex );
                ::ULIS::FRectD segmentBBox = segment->GetBoundingBox( true );
                ::ULIS::FRectD jointBBox = vertex->GetJoint().GetBBox( true );
                FOdysseyVectorJoint& joint = vertex->GetJoint();
                double segmentLength = segment->GetLength();
                double jointLength = joint.GetLength();
                double segmentAndJointLength = segmentLength + jointLength;
                //double endU = 0.0f;

                if( segmentLength )
                {
                    // WORKAROUND: in some cases U is < 0.0f, I dont know why yet.
                    //if ( segmentStartU < 0.0f ) segmentStartU = 0.0f;
                    //if ( segmentEndU   < 0.0f ) segmentEndU   = 0.0f;

                    // Textured joints are drawn only in texture mode (obviously) and if the texture
                    // goes all over the path.
                    if( ( vertex->IsHandleAligned() == false )
                    && ( mBrush.ExtensionMode != eBrushExtensionMode::Segment ) )
                    {
                        // WORKAROUND: in some cases U is < 0.0f, I dont know why yet.
                        //if ( jointStartU < 0.0f ) jointStartU = 0.0f;
                        //if ( jointEndU   < 0.0f ) jointEndU   = 0.0f;

                        // don't draw if joint is outside the screen
                        if( ( ( jointBBox.x               ) < screen.w )
                         && ( ( jointBBox.x + jointBBox.w ) > 0        )
                         && ( ( jointBBox.y               ) < screen.h )
                         && ( ( jointBBox.y + jointBBox.h ) > 0        ) )
                        {
                            vertex->DrawJoint( iBLContext
                                             , iCombinedOpacity
                                             , iDrawingFlags );
                        }
                    }

                    // don't draw if segment is outside the screen
                    if( ( ( segmentBBox.x                 ) < screen.w )
                     && ( ( segmentBBox.x + segmentBBox.w ) > 0        )
                     && ( ( segmentBBox.y                 ) < screen.h )
                     && ( ( segmentBBox.y + segmentBBox.h ) > 0        ) )
                    {
                        DrawSegment( iBLContext
                                   , iVectorEngine
                                   , segment
                                   , segment->GetTextureStartU()
                                   , segment->GetTextureEndU()
                                   , iCombinedOpacity
                                   , iDrawingFlags );
                    }
                }

                // only when mBrush.ExtendOverPath == true
                //startU = endU;

                return false; // keep iterating
            } );

            mBrush.Unlock();
        }
        else
        {
            // We fill with stroke color because our curve is made of filled shapes.
            //iBLContext->setFillRule( BL_FILL_RULE_NON_ZERO );
            iBLContext->setFillRule( BL_FILL_RULE_EVEN_ODD );
            iBLContext->setFillStyle( strokeColor );
            iBLContext->setStrokeStyle( strokeColor );

            iChain.IterateSegments( [ this
                                    , iBLContext
                                    , iVectorEngine
                                    , &iCombinedOpacity
                                    , &iDrawingFlags
                                    , &screen ]( FOdysseyVectorVertex* vertex, FOdysseyVectorSegment* segment ) -> bool
            {
                ::ULIS::FRectD segmentBBox = segment->GetBoundingBox( true );
                ::ULIS::FRectD jointBBox = vertex->GetJoint().GetBBox( true );

                // don't draw if segment is outside the screen
                if( ( ( segmentBBox.x                 ) < screen.w )
                 && ( ( segmentBBox.x + segmentBBox.w ) > 0        )
                 && ( ( segmentBBox.y                 ) < screen.h )
                 && ( ( segmentBBox.y + segmentBBox.h ) > 0        ) )
                {
                    DrawSegment( iBLContext
                               , iVectorEngine
                               , segment
                               , 0.0f
                               , 0.0f
                               , iCombinedOpacity
                               , iDrawingFlags );
                }

                // don't draw if joint is outside the screen
                if( ( ( jointBBox.x               ) < screen.w )
                 && ( ( jointBBox.x + jointBBox.w ) > 0        )
                 && ( ( jointBBox.y               ) < screen.h )
                 && ( ( jointBBox.y + jointBBox.h ) > 0        ) )
                {
                    vertex->DrawJoint( iBLContext, iCombinedOpacity, iDrawingFlags );
                }

                return false; // keep iterating
            } );
        }
    }
}

void
FOdysseyVectorPath::DrawStructure( BLContext* iBLContext
                                 , const BLRgba32& iStrokeColor
                                 , double iStrokeWidth
                                 , bool iWorld )
{
    static BLRgba32 blackColor = BLRgba32( 0, 0, 0, 255 );

    iBLContext->save();

    if( iWorld )
    {
        iBLContext->resetMatrix();
    }

    iBLContext->setStrokeWidth( iStrokeWidth );
    iBLContext->setStrokeStyle( iStrokeColor );

    for( FOdysseyVectorSegment* segment : mSegmentList )
    {
        //iBLContext->setStrokeWidth( iStrokeWidth + 1.0f );
        //iBLContext->setStrokeStyle( blackColor );
        //segment->DrawStructure( iBLContext, this, iWorld );
        //iBLContext->setStrokeWidth( iStrokeWidth );
        //iBLContext->setStrokeStyle( iStrokeColor );
        segment->DrawStructure( iBLContext, this, iWorld );
    }

    iBLContext->restore();
}

std::list<FOdysseyVectorSegment*>&
FOdysseyVectorPath::GetInvalidatedSegmentList()
{
    return mInvalidatedSegmentList;
}

FOdysseyVectorObject*
FOdysseyVectorPath::CopyShape( uint64 iCopyFlags )
{
    FOdysseyVectorPath* cubicPathCopy = new FOdysseyVectorPath( FString("Cubic Path") );
    std::map<FOdysseyVectorVertex*, FOdysseyVectorVertex*> lookupTable;

    cubicPathCopy->SetBrush( mBrush );
    cubicPathCopy->SetJointType( mJointType, false );
    cubicPathCopy->SetMiterLimit( mMiterLimit, false );
    cubicPathCopy->SetFilled( bFilled );

    for( FOdysseyVectorVertex* originalVertex : mVertexList )
    {
        ::ULIS::FVec2D originalVertexCoords = originalVertex->GetCoords();
        double originalVertexRadius = originalVertex->GetRadius();
        FOdysseyVectorVertex* newVertex;

        newVertex = new FOdysseyVectorVertex( originalVertexCoords.x
                                            , originalVertexCoords.y
                                            , originalVertexRadius );

        newVertex->SetHandleAligned( originalVertex->IsHandleAligned() );

        lookupTable.insert( std::make_pair( originalVertex, newVertex ) );

        cubicPathCopy->AddVertex( newVertex );
    }

    for( FOdysseyVectorSegment* originalSegment : mSegmentList )
    {
        if( originalSegment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            if( iCopyFlags & COPY_RETOPOLOGY )
            {
                std::vector<FOdysseyVectorFraction>& fractionCache = originalSegment->GetFractionCache();
                uint32 fractionCount = originalSegment->GetFractionCount();
                std::vector<::ULIS::FVec2D> pointBuffer;
                FOdysseyVectorVertex* vertex0 = originalSegment->GetVertex(0);
                FOdysseyVectorVertex* vertex1 = nullptr;

                pointBuffer.reserve( originalSegment->GetFractionPointBuffer().size() + 2 );
                pointBuffer.push_back( originalSegment->GetVertex(0)->GetCoords() );

                for( uint32 i = 0; i < fractionCount - 1; i++ )
                {
                    FOdysseyVectorFraction& fraction = fractionCache[i];
                    ::ULIS::FVec2D fractionPointCoords = fraction.point[1]->GetCoords();

                    pointBuffer.push_back( fractionPointCoords );
                }

                pointBuffer.push_back( originalSegment->GetVertex(1)->GetCoords() );

                FOdysseyVector::FitCurve( pointBuffer
                                        , 4.0f
                                        , [ &lookupTable
                                          , cubicPathCopy
                                          , originalSegment
                                          , &vertex0
                                          , &vertex1 ]( const std::vector<::ULIS::FVec2D>& bezierCurve
                                                      , double firstT
                                                      , double lastRecordT )
                {
                    if( lastRecordT == 1.0f )
                    {
                        vertex1 = originalSegment->GetVertex(1);
                    }
                    else
                    {
                        double radius = ( lookupTable[originalSegment->GetVertex(0)]->GetRadius() * ( 1.0f - lastRecordT ) )
                                      + ( lookupTable[originalSegment->GetVertex(1)]->GetRadius() * ( lastRecordT        ) );

                        vertex1 = new FOdysseyVectorVertex( bezierCurve[3].x
                                                          , bezierCurve[3].y
                                                          , radius );

                        cubicPathCopy->AddVertex( vertex1 );
                    }

                    if( lookupTable[vertex0] ) vertex0 = lookupTable[vertex0];
                    if( lookupTable[vertex1] ) vertex1 = lookupTable[vertex1];

                    FOdysseyVectorSegmentCubic* newCubicSegment = new FOdysseyVectorSegmentCubic( cubicPathCopy
                                                                                                , vertex0
                                                                                                , bezierCurve[1].x
                                                                                                , bezierCurve[1].y
                                                                                                , bezierCurve[2].x
                                                                                                , bezierCurve[2].y
                                                                                                , vertex1
                                                                                                , true );

                    cubicPathCopy->AddSegment( newCubicSegment );

                    vertex0 = vertex1;
                } );
            }
            else
            {
                FOdysseyVectorSegmentCubic* originalCubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(originalSegment);
                FOdysseyVectorVertex* vertex0 = static_cast<FOdysseyVectorVertex*>( originalCubicSegment->GetPoint(0) );
                FOdysseyVectorVertex* vertex1 = static_cast<FOdysseyVectorVertex*>( originalCubicSegment->GetPoint(1) );
                FOdysseyVectorHandleSegment* originalHandle0 = originalSegment->GetHandle(0);
                FOdysseyVectorHandleSegment* originalHandle1 = originalSegment->GetHandle(1);
                ::ULIS::FVec2D originalHandle0Coords = originalHandle0->GetCoords();
                ::ULIS::FVec2D originalHandle1Coords = originalHandle1->GetCoords();

                FOdysseyVectorSegmentCubic* newCubicSegment = new FOdysseyVectorSegmentCubic( cubicPathCopy
                                                                                            , lookupTable[vertex0]
                                                                                            , originalHandle0Coords.x
                                                                                            , originalHandle0Coords.y
                                                                                            , originalHandle1Coords.x
                                                                                            , originalHandle1Coords.y
                                                                                            , lookupTable[vertex1]
                                                                                            , true );

                cubicPathCopy->AddSegment( newCubicSegment );
            }
            //newSegment->BuildVariable();
        }
    }

    //cubicPathCopy->Update( 0 );

    return static_cast<FOdysseyVectorObject*>( cubicPathCopy );
}

void
FOdysseyVectorPath::Merge( FOdysseyVectorPath* iMergedPath
                         , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                         , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray )
{
    std::list<FOdysseyVectorSegment*>& segmentList = iMergedPath->GetSegmentList();
    std::list<FOdysseyVectorVertex*>& vertexList = iMergedPath->GetVertexList();
    BLMatrix2D& mergedPathWorldMatrix = iMergedPath->GetWorldMatrix();
    BLMatrix2D conversionMatrix;

    oAddedVertexArray.clear();
    oAddedVertexArray.reserve( vertexList.size() );

    oAddedSegmentArray.clear();
    oAddedSegmentArray.reserve( segmentList.size() );

    if( vertexList.size() )
    {
        uint32 i = 0;

        FOdysseyVector::MatrixMultiply( mInverseWorldMatrix, mergedPathWorldMatrix, conversionMatrix );

        for( FOdysseyVectorVertex* vertex : vertexList )
        {
            BLPoint pt = conversionMatrix.mapPoint( vertex->GetX(), vertex->GetY() );
            BLPoint rd = conversionMatrix.mapVector( 0.7071 * vertex->GetRadius(), 0.7071 * vertex->GetRadius() );
            ::ULIS::FVec2D radius = ::ULIS::FVec2D( rd.x, rd.y );
            FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( pt.x, pt.y, radius.Distance() );

            oAddedVertexArray.push_back( newVertex );

            vertex->SetID( i++ );

            AddVertex( newVertex );
        }

        for( FOdysseyVectorSegment* segment : segmentList )
        {
            if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
                FOdysseyVectorVertex* vertex0 = cubicSegment->GetVertex(0);
                FOdysseyVectorVertex* vertex1 = cubicSegment->GetVertex(1);
                FOdysseyVectorHandleSegment* handle0 = cubicSegment->GetHandle(0);
                FOdysseyVectorHandleSegment* handle1 = cubicSegment->GetHandle(1);
                BLPoint pt[2] = { conversionMatrix.mapPoint( handle0->GetX(), handle0->GetY() )
                                , conversionMatrix.mapPoint( handle1->GetX(), handle1->GetY() ) };
                FOdysseyVectorVertex* newVertex0 = oAddedVertexArray[vertex0->GetID()];
                FOdysseyVectorVertex* newVertex1 = oAddedVertexArray[vertex1->GetID()];
                FOdysseyVectorSegmentCubic* newCubicSegment = new FOdysseyVectorSegmentCubic( this
                                                                                            , newVertex0
                                                                                            , pt[0].x
                                                                                            , pt[0].y
                                                                                            , pt[1].x
                                                                                            , pt[1].y
                                                                                            , newVertex1
                                                                                            , true );
                oAddedSegmentArray.push_back( newCubicSegment );

                AddSegment( newCubicSegment ); // this also invalidates the segment
            }
        }
    }

    Update( 0 );
}

void
FOdysseyVectorPath::ApplyMatrix( BLMatrix2D& iMatrix )
{
    for( FOdysseyVectorVertex* vertex : mVertexList )
    {
        ::ULIS::FVec2D& point = vertex->GetCoords();
        BLPoint localPt = iMatrix.mapPoint( point.x, point.y );
        BLPoint localVec = iMatrix.mapVector( 0.70710678118f * vertex->GetRadius()
                                            , 0.70710678118f * vertex->GetRadius() );
        ::ULIS::FVec2D vec = { localVec.x, localVec.y };

        vertex->Set( localPt.x, localPt.y );
        vertex->SetRadius( vec.Distance() );
    }

    for( FOdysseyVectorSegment* segment : mSegmentList )
    {
        if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
            ::ULIS::FVec2D& ctrlPoint0 = cubicSegment->GetHandle(0)->GetCoords();
            ::ULIS::FVec2D& ctrlPoint1 = cubicSegment->GetHandle(1)->GetCoords();
            BLPoint localPt0 = iMatrix.mapPoint( ctrlPoint0.x, ctrlPoint0.y );
            BLPoint localPt1 = iMatrix.mapPoint( ctrlPoint1.x, ctrlPoint1.y );

            cubicSegment->GetHandle(0)->Set( localPt0.x, localPt0.y );
            cubicSegment->GetHandle(1)->Set( localPt1.x, localPt1.y );
        }

        InvalidateSegment( segment );
    }
}

void
FOdysseyVectorPath::ApplyTransformations()
{
    BLMatrix2D& parentInverseWorldMatrix = mParent->GetInverseWorldMatrix();
    BLMatrix2D conversionMatrix = mLocalMatrix;

    FOdysseyVector::MatrixMultiply( parentInverseWorldMatrix, mWorldMatrix, conversionMatrix );

    ApplyMatrix( conversionMatrix );

    // inherited
    FOdysseyVectorObject::ApplyTransformations();
}

/*
void
FOdysseyVectorPath::SwitchSpace( FOdysseyVectorObject& iNewSpace )
{
    BLMatrix2D& newSpaceInverseWorldMatrix = iNewSpace.GetInverseWorldMatrix();
    BLMatrix2D& newSpaceWorldMatrix = iNewSpace.GetWorldMatrix();

    for( FOdysseyVectorVertex* cubicVertex : mVertexList )
    {
        ::ULIS::FVec2D& point = cubicVertex->GetCoords();
        BLPoint worldPt = mWorldMatrix.mapPoint( point.x, point.y );
        BLPoint wordlVec = mWorldMatrix.mapVector( 0.70710678118f * cubicVertex->GetRadius()
                                                 , 0.70710678118f * cubicVertex->GetRadius() );
        BLPoint localPt = newSpaceInverseWorldMatrix.mapPoint( worldPt );
        BLPoint localVec = newSpaceInverseWorldMatrix.mapVector( wordlVec );
        ::ULIS::FVec2D vec = { localVec.x, localVec.y };

        point.x = localPt.x;
        point.y = localPt.y;

        cubicVertex->SetRadius( vec.Distance() );
    }

    for( FOdysseyVectorSegment* segment : mSegmentList )
    {
        if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
            ::ULIS::FVec2D& ctrlPoint0 = cubicSegment->GetHandle(0)->GetCoords();
            ::ULIS::FVec2D& ctrlPoint1 = cubicSegment->GetHandle(1)->GetCoords();
            BLPoint pt;

            pt = newSpaceInverseWorldMatrix.mapPoint( mWorldMatrix.mapPoint( ctrlPoint0.x, ctrlPoint0.y ) );

            ctrlPoint0.x = pt.x;
            ctrlPoint0.y = pt.y;

            pt = newSpaceInverseWorldMatrix.mapPoint( mWorldMatrix.mapPoint( ctrlPoint1.x, ctrlPoint1.y ) );

            ctrlPoint1.x = pt.x;
            ctrlPoint1.y = pt.y;
        }

        InvalidateSegment( segment );
    }
}
*/

// static
void
FOdysseyVectorPath::SharpSegments( FOdysseyVectorVertex* iVertex, bool iPreserveHandleLength )
{
    for( FOdysseyVectorSegment*segment : iVertex->GetSegmentList() )
    {
        if( segment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);

            cubicSegment->GetHandle( iVertex )->Set( iVertex->GetX(), iVertex->GetY() );

            cubicSegment->Invalidate();
        }
    }
}

// static
void
FOdysseyVectorPath::SmoothSegments( FOdysseyVectorVertex* iVertex, bool iPreserveHandleLength )
{
    ::ULIS::FVec2D perpendicularVector = iVertex->GetAverageStraightVectorOnSegment( true );

    // if the perpendicular vector is 0, use one of the segment's vector as a reference.
    if( perpendicularVector.DistanceSquared() == 0.0f && iVertex->GetFirstSegment() )
    {
        perpendicularVector = iVertex->GetFirstSegment()->GetHandleVector( iVertex, true );
        perpendicularVector = ::ULIS::FVec2D( perpendicularVector.y, -perpendicularVector.x );
    }

    SmoothSegments( iVertex, perpendicularVector, iPreserveHandleLength );
}

// static
void
FOdysseyVectorPath::SmoothSegments( FOdysseyVectorVertex* iVertex, ::ULIS::FVec2D iPerpendicularVector, bool iPreserveHandleLength )
{
    std::list<FOdysseyVectorSegment*>& segmentList = iVertex->GetSegmentList();

    if ( iVertex->GetSegmentCount() > 1 )
    {
        for( FOdysseyVectorSegment* segment : segmentList )
        {
            if( segment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
            {
                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);

                if ( iVertex == cubicSegment->GetPoint(0) )
                {
                    //::ULIS::FVec2D segmentVector = iVertex->GetVectorOnSegment( cubicSegment, true );
                    ::ULIS::FVec2D segmentVector = cubicSegment->GetVector( iVertex, true );
                    //double dot = iPerpendicularVector.DotProduct( segmentVector );
                    ::ULIS::FVec2D tangentVector = ::ULIS::FVec2D( -iPerpendicularVector.y, iPerpendicularVector.x );
                    double distance;

                    // if perpendicular vector equals 0 or is orthogonal to the segment vector
                    if ( tangentVector.DotProduct( segmentVector )  < 0.0f )
                    {
                        tangentVector = -tangentVector;
                    }

                    if( iPreserveHandleLength )
                    {
                        ::ULIS::FVec2D& handlePos = cubicSegment->GetHandle(0)->GetCoords();
                        ::ULIS::FVec2D handleVec = handlePos - iVertex->GetCoords();

                        distance = handleVec.Distance();
                    }
                    else
                    {
                        distance = cubicSegment->GetStraightDistance() * 0.35f;
                    }

                    cubicSegment->GetHandle(0)->Set( iVertex->GetX() + ( tangentVector.x * distance ),
                                                     iVertex->GetY() + ( tangentVector.y * distance ) );
                }

                if ( iVertex == cubicSegment->GetPoint(1) )
                {
                    //::ULIS::FVec2D segmentVector = iVertex->GetVectorOnSegment( cubicSegment, true );
                    ::ULIS::FVec2D segmentVector = cubicSegment->GetVector( iVertex, true );
                    double dot = iPerpendicularVector.DotProduct( segmentVector );
                    ::ULIS::FVec2D tangentVector = ::ULIS::FVec2D( iPerpendicularVector.y, -iPerpendicularVector.x );
                    double distance;

                    // if perpendicular vector equals 0 or is orthogonal to the segment vector
                    if ( tangentVector.DotProduct( segmentVector ) < 0.0f )
                    {
                        tangentVector = -tangentVector;
                    }

                    if( iPreserveHandleLength )
                    {
                        ::ULIS::FVec2D& handlePos = cubicSegment->GetHandle(1)->GetCoords();
                        ::ULIS::FVec2D handleVec = handlePos - iVertex->GetCoords();

                        distance = handleVec.Distance();
                    }
                    else
                    {
                        distance = cubicSegment->GetStraightDistance() * 0.35f;
                    }

                    cubicSegment->GetHandle(1)->Set( iVertex->GetX() + ( tangentVector.x * distance ),
                                                     iVertex->GetY() + ( tangentVector.y * distance ) );
                }

                cubicSegment->Invalidate();
            }
        }
    }
}

// Math based version
void
FOdysseyVectorPath::PickSegments( double iWorldX
                                , double iWorldY
                                , double iWorldRadius
                                , std::vector<FOdysseyVectorSegment*>& oPickedSegmentArray
                                , std::vector<double>* oDistanceArray )
{
    BLPoint localVector = mInverseWorldMatrix.mapVector( 0.7071f, 0.7071f );
    ::ULIS::FVec2D factor = { localVector.x * iWorldRadius, localVector.y * iWorldRadius };
    double localRadius = factor.Distance();
    BLPoint localPoint = mInverseWorldMatrix.mapPoint( iWorldX, iWorldY );
    ::ULIS::FRectD pathBBox = mBBox;

    // get sure we hit the box be enlarging it with the picking circle radius value.
    // otherwise we might not be able to pick points located at the box's boundaries.
    pathBBox.x -=   localRadius;
    pathBBox.y -=   localRadius;
    pathBBox.w += ( localRadius * 2 );
    pathBBox.h += ( localRadius * 2 );

    if( pathBBox.HitTest( ::ULIS::FVec2D( localPoint.x, localPoint.y ) ) == true )
    {
        for( FOdysseyVectorSegment* segment : mSegmentList )
        {
            ::ULIS::FRectD segmentBBox = segment->GetBoundingBox( false );

            segmentBBox.x -=   localRadius;
            segmentBBox.y -=   localRadius;
            segmentBBox.w += ( localRadius * 2 );
            segmentBBox.h += ( localRadius * 2 );

            if( segmentBBox.HitTest( ::ULIS::FVec2D( localPoint.x, localPoint.y ) ) == true )
            {
                double smallestDistance;

                if( segment->ProximityTest( localPoint.x, localPoint.y, localRadius, smallestDistance ) )
                {
                    oPickedSegmentArray.push_back( segment );

                    if( oDistanceArray )
                    {
                        oDistanceArray->push_back( smallestDistance );
                    }
                }
            }
        }
    }
}

// Mask based version
void
FOdysseyVectorPath::PickSegments( std::vector<FOdysseyVectorSegment*>& oPickedSegmentArray )
{
    BLImage* maskImage = GetEngine()->GetBLMask();
    BLImageData maskData;
    ::ULIS::FRectD maskRect;
    bool picked = false;

    maskImage->getData( &maskData );

    maskRect = ::ULIS::FRectD( 0, 0, maskData.size.w, maskData.size.h );

    for( FOdysseyVectorSegment* segment : mSegmentList )
    {
        if( segment->Pick( maskRect, (uint8*)maskData.pixelData ) )
        {
            oPickedSegmentArray.push_back( segment );
        }
    }
}

// Updates length and bounding box
void
FOdysseyVectorPath::UpdateChain( FOdysseyVectorChain* iChain )
{
    double xmin = DBL_MAX, ymin = DBL_MAX, xmax = -DBL_MAX, ymax = -DBL_MAX;
    bool hasBBox = false;

    iChain->mLength = 0.0f;

    iChain->IterateSegments( [ iChain
                             , &xmin
                             , &ymin
                             , &xmax
                             , &ymax
                             , &hasBBox ] ( FOdysseyVectorVertex* vertex, FOdysseyVectorSegment* segment ) -> bool
    {
        FOdysseyVectorSegment* prevSegment = vertex->GetOtherSegment( segment );
        FOdysseyVectorVertex* nextVertex = segment->GetOtherVertex( vertex );
        ::ULIS::FRectD segmentBBox = segment->GetBoundingBox( false );
        double rx1 = segmentBBox.x
             , ry1 = segmentBBox.y
             , rx2 = segmentBBox.x + segmentBBox.w
             , ry2 = segmentBBox.y + segmentBBox.h;

        hasBBox = true;

        segment->SetTextureU( 0.0f, 0.0f );
        vertex->GetJoint().SetTextureU( 0.0f, 0.0f );

        if ( rx1 < xmin ) xmin = rx1;
        if ( ry1 < ymin ) ymin = ry1;
        if ( rx2 > xmax ) xmax = rx2;
        if ( ry2 > ymax ) ymax = ry2;

        iChain->mLength += segment->GetLength() + vertex->GetJointLength();

        return false; // keep iterating
    } );

    // Update texture coords
    if( mBrush.GetTexture() )
    {
        double startU = mBrush.Revert ? 1.0f : 0.0f;
        double remainingSegmentLength = 0.0f;

        mBrush.Lock(); // needed to retrieve texture width and height

        iChain->IterateSegments( [ this
                                 , &startU
                                 , &remainingSegmentLength
                                 , &iChain ]( FOdysseyVectorVertex* vertex, FOdysseyVectorSegment* segment ) -> bool
        {
            FOdysseyVectorVertex* otherVertex = segment->GetOtherVertex( vertex );
            ::ULIS::FRectD segmentBBox = segment->GetBoundingBox( true );
            ::ULIS::FRectD jointBBox = vertex->GetJoint().GetBBox( true );
            FOdysseyVectorJoint& joint = vertex->GetJoint();
            double segmentLength = segment->GetLength();
            double jointLength = joint.GetLength();
            double segmentAndJointLength = segmentLength + jointLength;
            double endU = 0.0f;

            if( mBrush.ExtensionMode == eBrushExtensionMode::Path )
            {
                if( mBrush.Revert )
                {
                    endU = startU - ( iChain->mLength ? ( segmentAndJointLength / iChain->mLength ) : 0.0f );
                }
                else
                {
                    endU = startU + ( iChain->mLength ? ( segmentAndJointLength / iChain->mLength ) : 0.0f );
                }
            }

            if( mBrush.ExtensionMode == eBrushExtensionMode::Segment )
            {
                if( mBrush.Revert )
                {
                    startU = 1.0f;
                    endU   = 0.0f;
                }
                else
                {
                    startU = 0.0f;
                    endU   = 1.0f;
                }
            }

            if( mBrush.ExtensionMode == eBrushExtensionMode::Adapt )
            {
                double brushRatio = mBrush.height ? (double) mBrush.width  / mBrush.height : 0.0f;
                double averageSegmentRadius = ( vertex->GetRadius() + otherVertex->GetRadius() ) * 0.5f;
                double adaptedSegmentLength = averageSegmentRadius * brushRatio;

                if( mBrush.Revert )
                {
                    endU   = startU - ( adaptedSegmentLength ? ( segmentAndJointLength / adaptedSegmentLength ) : 0.0f );
                }
                else
                {
                    endU   = startU + ( adaptedSegmentLength ? ( segmentAndJointLength / adaptedSegmentLength ) : 0.0f );
                }
            }

            if( segmentLength )
            {
                double segmentStartU = ( vertex == segment->GetVertex(0) ) ? startU : endU;
                double segmentEndU   = ( vertex == segment->GetVertex(0) ) ? endU : startU;

                // WORKAROUND: in some cases U is < 0.0f, I dont know why yet.
                //if ( segmentStartU < 0.0f ) segmentStartU = 0.0f;
                //if ( segmentEndU   < 0.0f ) segmentEndU   = 0.0f;

                // Textured joints are drawn only in texture mode (obviously) and if the texture
                // goes all over the path.
                if( ( vertex->IsHandleAligned() == false )
                && ( mBrush.ExtensionMode != eBrushExtensionMode::Segment ) )
                {
                    double segmentJointRatio = jointLength / ( segmentAndJointLength );
                    double jointStartU = segmentStartU;
                    double jointEndU = segmentStartU + ( ( segmentEndU - segmentStartU ) * segmentJointRatio );

                    segmentStartU = jointEndU;

                    vertex->GetJoint().SetTextureU( jointStartU, jointEndU );
                }

                segment->SetTextureU( segmentStartU, segmentEndU );
            }

            // only when mBrush.ExtendOverPath == true
            startU = endU;

            return false; // keep iterating
        } );

        mBrush.Unlock();
    }

    iChain->mBBox = ( hasBBox ) ? ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax )
                                : ::ULIS::FRectD( 0.0f, 0.0f, 0.0f, 0.0f );
}

void
FOdysseyVectorPath::FindChains()
{
    mChainArray.clear();

    // reset status as FOdysseyVectorChain::Ctor is going to set it
    for( FOdysseyVectorVertex* vertex : mVertexList )
    {
        vertex->SetChained( false );
    }

    // first step. Brush vertices at segment end points
    for( FOdysseyVectorVertex* vertex : mVertexList )
    {
        if( vertex->IsChained() == false )
        {
            if( vertex->GetSegmentCount() == 1 )
            {
                mChainArray.emplace_back( this, vertex );
            }
        }
    }

    // second step. Brush remaining vertices (belonging to loops for example)
    for( FOdysseyVectorVertex* vertex : mVertexList )
    {
        if( vertex->IsChained() == false )
        {
            if( vertex->GetSegmentCount() == 2 )
            {
                mChainArray.emplace_back( this, vertex );
            }
        }
    }
}
