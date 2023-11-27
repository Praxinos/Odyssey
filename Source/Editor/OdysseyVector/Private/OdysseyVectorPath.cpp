#include "OdysseyVectorPath.h"

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
{
    SetJointType( eJointType::Miter );

    mPathParam.Filled = false;
    SetMiterLimit( 4.0f );

    mChainArray.reserve( 10 );
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
FOdysseyVectorPath::ToVertexAndSectionArray( std::vector<FOdysseyVectorVertex*>& oVertexArray
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

            oVertexArray.push_back( vertex );
            oSectionArray.push_back( section );

            vertex = nextVertex;
            segment = nextSegment;
        }
        while( segment && ( vertex != firstVertex ) );
    }
}

uint32
FOdysseyVectorPath::GetIntersectionCount()
{
    uint32 count = 0;

    for( FOdysseyVectorSegment* segment : mSegmentList )
    {
        count += segment->GetIntersectionVertexList().size();
    }

    return count;
}

bool
FOdysseyVectorPath::HasIntersections()
{
    for( FOdysseyVectorSegment* segment : mSegmentList )
    {
        if( segment->GetIntersectionVertexList().size() )
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
    return mPathParam.Filled;
}

FOdysseyVectorBrush&
FOdysseyVectorPath::GetBrush()
{
    return mBrush;
}

void
FOdysseyVectorPath::SetBrush( const FOdysseyVectorBrush& iBrush )
{
    mBrush = iBrush;
}

void
FOdysseyVectorPath::SetFilled( bool iIsFilled )
{
    mPathParam.Filled = iIsFilled;
}

eJointType
FOdysseyVectorPath::GetJointType()
{
    return mPathParam.JointType;
}

void
FOdysseyVectorPath::SetJointType( eJointType iJointType )
{
    mPathParam.JointType = iJointType;

    Invalidate();
}

void
FOdysseyVectorPath::SetMiterLimit( double iMiterLimit )
{
    mPathParam.MiterLimit = iMiterLimit;

    Invalidate();
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
    for ( FOdysseyVectorSegment* segment : mInvalidatedSegmentList )
    {
        segment->Update();
    }

    mInvalidatedSegmentList.clear();

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

    if( ( iUpdateFlags & FOdysseyVectorObject::KEEPINVALIDATED ) == 0 )
    {
        mInvalidationFlags = 0;
    }
}

bool
FOdysseyVectorPath::GetBBoxFromSelectedVertices( ::ULIS::FRectD& oBBox, bool iWorld )
{
    double xmin, ymin, xmax, ymax;

    if( FOdysseyVectorVertex::GetMinMaxFromList( mSelectedVertexList, xmin, ymin, xmax, ymax ) )
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

        Invalidate();
    }
}

void
FOdysseyVectorPath::AddVertex( FOdysseyVectorVertex* iVertex )
{
    mVertexList.push_back( iVertex );

    iVertex->SetPath( this );
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

    iSegment->SetPath( this );

    iSegment->GetVertex(0)->AddSegment( iSegment );
    iSegment->GetVertex(1)->AddSegment( iSegment );

    InvalidateSegment( iSegment );

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
FOdysseyVectorPath::Invalidate()
{
    Invalidate( INVALIDATE_ALL );
}

void
FOdysseyVectorPath::Invalidate( uint32 iInvalidationFlags )
{
    //if( iInvalidationFlags & INVALIDATE_MATRIX )
    //{
        // Mark all segment as NOT painting ready to force recalculation of cached subsegments
        // use by the parent paint group
        for( FOdysseyVectorSegment* segment : mSegmentList )
        {
            segment->SetPaintingReady( false );
        }
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

        if( ( iWayPoint0->type == eWayPointType::OutsideErasureArea )
         && ( iWayPoint1->type == eWayPointType::OutsideErasureArea ) )
        {
            retFlags |= eSegmentAdditionFlags::CreateDerivedSegment;
        }
    }
    else
    {
        if( ( iWayPoint0->type == eWayPointType::OutsideErasureArea )
         && ( iWayPoint1->type == eWayPointType::OutsideErasureArea ) )
        {
            retFlags |= eSegmentAdditionFlags::KeepOriginalSegment;
        }
    }

    if( ( iWayPoint0->type == eWayPointType::OutsideErasureArea )
     && ( iWayPoint1->type == eWayPointType::EntersErasureArea  ) )
    {
        retFlags |= ( eSegmentAdditionFlags::RemoveOriginalSegment
                    | eSegmentAdditionFlags::CreateDerivedSegment );
    }

    if( ( iWayPoint0->type == eWayPointType::LeavesErasureArea  )
     && ( iWayPoint1->type == eWayPointType::OutsideErasureArea ) )
    {
        retFlags |= ( eSegmentAdditionFlags::RemoveOriginalSegment
                    | eSegmentAdditionFlags::CreateDerivedSegment );
    }

    if( ( iWayPoint0->type == eWayPointType::LeavesErasureArea )
     && ( iWayPoint1->type == eWayPointType::EntersErasureArea ) )
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
        if( iWayPoint->type == eWayPointType::OutsideErasureArea )
        {
            retFlags |= ( eVertexAdditionFlags::RemoveOriginalVertex
                        | eVertexAdditionFlags::CreateDerivedVertex );
        }
    }
    else
    {
        if( iWayPoint->type == eWayPointType::OutsideErasureArea )
        {
            retFlags |= eVertexAdditionFlags::KeepOriginalVertex;
        }
    }

    if( iWayPoint->type == eWayPointType::InsideErasureArea )
    {
        retFlags |= eVertexAdditionFlags::RemoveOriginalVertex;
    }

    if( iWayPoint->type == eWayPointType::EntersErasureArea )
    {
        retFlags |= eVertexAdditionFlags::CreateBoundaryVertex;
    }

    if( iWayPoint->type == eWayPointType::LeavesErasureArea )
    {
        retFlags |= eVertexAdditionFlags::CreateBoundaryVertex;
    }

    return retFlags;
}

void
FOdysseyVectorPath::ParseWayPoints( std::vector<FWayPoint>& iWayPointArray
                                  , std::vector<FWaySegment>& iWaySegmentArray
                                  , std::vector<FOdysseyVectorObject*>& oAddedPathArray
                                  , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                                  , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                                  , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                                  , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                                  , bool iSplit )
{
    std::map<FOdysseyVectorVertex*, FOdysseyVectorVertex*> vertexLookup;
    FOdysseyVectorPath* currentPath = iSplit ? nullptr : this;

    if( iWayPointArray.size() )
    {
        FWayPoint& firstWayPoint = iWayPointArray[0];

        if( currentPath == nullptr )
        {
            if( firstWayPoint.type == eWayPointType::OutsideErasureArea )
            {
                currentPath = new FOdysseyVectorPath( GetName() );
            }
        }

        // step 1 : create needed vertices
        for( FWayPoint& wayPoint : iWayPointArray )
        {
            eVertexAdditionFlags vertexAdditionFlags = VertexAdditionPolicy( &wayPoint, iSplit );

            // in split mode we will have to create a new path each time we leave the erasure area
            if( iSplit )
            {
                // first create a path that will receive the vertices
                if( wayPoint.type == eWayPointType::LeavesErasureArea )
                {
                    currentPath = new FOdysseyVectorPath( GetName() );
                    // for postprocessing. the path is not added to the parent yet
                    currentPath->SetParent( mParent );
                    CopyTransformation( *currentPath );

                    oAddedPathArray.push_back( currentPath );
                }
            }

            if( ( vertexAdditionFlags & eVertexAdditionFlags::RemoveOriginalVertex ) == eVertexAdditionFlags::RemoveOriginalVertex )
            {
                // add vertex to the array of removed vertices, no duplicates
                if( std::find( oRemovedVertexArray.begin(), oRemovedVertexArray.end(), wayPoint.vertex ) == oRemovedVertexArray.end() )
                {
                    oRemovedVertexArray.push_back( wayPoint.vertex );
                }
            }

            // boundary vertices are guaranteed unique per nature, no need to check uniqueness
            if( ( vertexAdditionFlags & eVertexAdditionFlags::CreateBoundaryVertex ) == eVertexAdditionFlags::CreateBoundaryVertex )
            {
                double radius0 = wayPoint.segment->GetVertex(0)->GetRadius();
                double radius1 = wayPoint.segment->GetVertex(1)->GetRadius();
                double radius = ( (          wayPoint.t ) * radius1 )
                                + ( ( 1.0f - wayPoint.t ) * radius0 );

                ::ULIS::FVec2D newVertexAt = wayPoint.segment->GetPointAt( wayPoint.t );

                wayPoint.vertex = new FOdysseyVectorVertex( currentPath
                                                          , newVertexAt.x
                                                          , newVertexAt.y
                                                          , radius );

                oAddedVertexArray.push_back( wayPoint.vertex );

                // insert the new vertex in the lookup table. Yes it has the same value.
                // This is not needed in no-split mode but we keep-it anyways 
                // in order to have a generic algorithm.
                vertexLookup[wayPoint.vertex] = wayPoint.vertex;
            }

            if( ( vertexAdditionFlags & eVertexAdditionFlags::CreateDerivedVertex ) == eVertexAdditionFlags::CreateDerivedVertex )
            {
                // original vertices are not unique (2 consecutive waypoints can store the same vertex), so we have
                // to ensure they haven't already been processed.
                if( vertexLookup.find(wayPoint.vertex) == vertexLookup.end() )
                {
                    ::ULIS::FVec2D& coords = wayPoint.vertex->GetCoords();
                    double radius = wayPoint.vertex->GetRadius();
                    FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( currentPath
                                                                              , coords.x
                                                                              , coords.y
                                                                              , radius );

                    newVertex->SetHandleAligned( wayPoint.vertex->IsHandleAligned() );

                    // insert the new vertex in the lookup table
                    vertexLookup[wayPoint.vertex] = newVertex;
                    // a new vertex for each former vertex
                    oAddedVertexArray.push_back( newVertex );
                }
            }

            if( ( vertexAdditionFlags & eVertexAdditionFlags::KeepOriginalVertex ) == eVertexAdditionFlags::KeepOriginalVertex )
            {
                // insert the current vertex in the lookup table. Yes it has the same value.
                // This is not needed in no-split mode but we keep-it anyways 
                // in order to have a generic algorithm.
                // Note: std::map ensures there won't be duplicates, but this will be called twice
                // because waypoints are not guaranteed unique.
                vertexLookup[wayPoint.vertex] = wayPoint.vertex;
            }
        }

        for( FWaySegment& waySegment : iWaySegmentArray )
        {
            FWayPoint* wayPoint0 = &iWayPointArray[waySegment.indexWayPoint0];
            FWayPoint* wayPoint1 = &iWayPointArray[waySegment.indexWayPoint1];
            eSegmentAdditionFlags segmentAdditionFlags = SegmentAdditionPolicy( wayPoint0, wayPoint1, iSplit );

            if( ( segmentAdditionFlags & eSegmentAdditionFlags::RemoveOriginalSegment ) == eSegmentAdditionFlags::RemoveOriginalSegment )
            {
                // mark original segment for deletion. No duplicates
                if( std::find( oRemovedSegmentArray.begin(), oRemovedSegmentArray.end(), waySegment.segment ) == oRemovedSegmentArray.end() )
                {
                    oRemovedSegmentArray.push_back( waySegment.segment );
                }
            }

            if( ( segmentAdditionFlags & eSegmentAdditionFlags::CreateDerivedSegment ) == eSegmentAdditionFlags::CreateDerivedSegment )
            {
                FOdysseyVectorVertex* destVertex0 = vertexLookup.find( wayPoint0->vertex )->second;
                FOdysseyVectorVertex* destVertex1 = vertexLookup.find( wayPoint1->vertex )->second;
                FOdysseyVectorPath* vertexPath = destVertex0->GetPath();
                FOdysseyVectorSegmentCubic* newSegment = new FOdysseyVectorSegmentCubic( vertexPath
                                                                                       , destVertex0
                                                                                       , waySegment.bezier[1].x
                                                                                       , waySegment.bezier[1].y
                                                                                       , waySegment.bezier[2].x
                                                                                       , waySegment.bezier[2].y
                                                                                       , destVertex1
                                                                                       , true );

                // mark new segment for addition
                oAddedSegmentArray.push_back( newSegment );
            }
        }
    }
}

bool
FOdysseyVectorPath::Erase( const ::ULIS::FRectD &iRoi
                         , std::vector<FOdysseyVectorObject*>& oAddedPathArray
                         , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                         , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                         , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                         , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
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
            std::vector<FWaySegment> waySegmentArray;
            std::vector<FWayPoint> wayPointArray;

            wayPointArray.reserve( 10 );
            waySegmentArray.reserve( 10 );

            // this will return an array of "way points". they are the original 
            // points + the points at a contrast zone (erasure boundary).
            // it will fill a "way segment" array, which are the segments that
            // should be created between those way points.
            // no vertex or segment is allocated in chain.HitMask(). the allocation differs
            // depending on the spliting mode, so it is the responsibility of EraseNoSplit() and
            // EraseNoSplit() to allocate new vertices / segments or paths 
            if( chain.HitMask( &imageData
                             , oRemovedVertexArray
                             , oRemovedSegmentArray
                             , wayPointArray
                             , waySegmentArray ) )
            {
                // proceed now we know we have hit anything
                // determine which vertices / segments will be deleted and which will be kept
                // it differs in SPLIT and NOSPLIT modes. SPLIT modes removes only those erased,
                // split removes all
                ParseWayPoints( wayPointArray
                              , waySegmentArray
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
            oRemovedSegmentArray[i]->GetPath()->RemoveSegment( oRemovedSegmentArray[i] );
        }

        for( int i = removedVertexCountBeforeAlter; i < oRemovedVertexArray.size(); i++ )
        {
            oRemovedVertexArray[i]->GetPath()->RemoveVertex( oRemovedVertexArray[i] );
        }

        for( int i = addedVertexCountBeforeAlter; i < oAddedVertexArray.size(); i++ )
        {
            oAddedVertexArray[i]->GetPath()->AddVertex( oAddedVertexArray[i] );
        }

        for( int i = addedSegmentCountBeforeAlter; i < oAddedSegmentArray.size(); i++ )
        {
            oAddedSegmentArray[i]->GetPath()->AddSegment( oAddedSegmentArray[i] );
        }
    }

    //UE_LOG(LogTemp, Warning, TEXT("Removed Segments: %d"), oRemovedSegmentArray.size() );
    //UE_LOG(LogTemp, Warning, TEXT("Removed Vertices: %d"), oRemovedVertexArray.size() );
    //UE_LOG(LogTemp, Warning, TEXT("Added Segments: %d"), oAddedVertexArray.size() );
    //UE_LOG(LogTemp, Warning, TEXT("Added Vertices: %d"), oAddedSegmentArray.size() );

    Invalidate();

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
                    BLPoint p0 = mWorldMatrix.mapPoint( fractionCache[i].lineVertex[0].x, fractionCache[i].lineVertex[0].y );
                    BLPoint p1 = mWorldMatrix.mapPoint( fractionCache[i].lineVertex[1].x, fractionCache[i].lineVertex[1].y );
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
                    extendedVertexArray.push_back( stitchingVertex[0] );
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
                        extendedVertexArray.push_back( stitchingVertex[0] );
                    }

                    if( stitchingVertex[1] && ( stitchingVertex[1]->GetSegmentCount() == 1 ) )
                    {
                        // for vertices that were not picked but that cannot be stitched, delete them as well.
                        extendedVertexArray.push_back( stitchingVertex[1] );
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
FOdysseyVectorPath::AlterRadius( double iDeltaRadius )
{
    for( FOdysseyVectorVertex* vertex : mVertexList )
    {
        vertex->SetRadius( vertex->GetRadius() + iDeltaRadius );
    }
}

void
FOdysseyVectorPath::DrawTexturedJoint( FOdysseyVectorJoint* iJoint
                                     , int8*  iScreenPixels
                                     , uint32 iScreenWidth
                                     , uint32 iScreenHeight
                                     , uint32 iScreenBitsPerPixel
                                     , int8*  iTexturePixels
                                     , uint32 iTextureWidth
                                     , uint32 iTextureHeight
                                     , uint32 iTextureBitsPerPixel
                                     , double iStartU
                                     , double iEndU
                                     , double iCombinedOpacity
                                     , uint64 iDrawingFlags )
{
    // TODO: transform this to an argument to avoid repetitive calls. I guess.
    FOdysseyVectorEngine* vectorEngine = GetEngine();
    std::vector<FOdysseyVectorPolygon5>& polygonCache = iJoint->GetPolygonCache();
    FColor foregroundColor = GetForegroundColor();
    double difU = iEndU - iStartU;

    for( int i = 0; i < polygonCache.size(); i++ )
    {
        FOdysseyVectorPolygon5* polygon = &polygonCache[i];
        BLPoint worldPoint[5];
        ::ULIS::FVec2I int32Point[5];
        double polyU[5];

        for( uint32 j = 0; j < polygon->pointCount; j++ )
        {
           worldPoint[j] = mWorldMatrix.mapPoint( polygon->point[j].x, polygon->point[j].y );
           int32Point[j].x = (int32) worldPoint[j].x;
           int32Point[j].y = (int32) worldPoint[j].y;
           polyU[j] = iStartU + ( polygon->U[j] * difU );
        }

        // TODO : check for in-screen visibility
            vectorEngine->DrawPolygon( int32Point
                                     , polyU
                                     , polygon->V
                                     , polygon->pointCount
                                     , iCombinedOpacity
                                     , iScreenPixels
                                     , iScreenWidth
                                     , iScreenHeight
                                     , iScreenBitsPerPixel
                                     , foregroundColor
                                     , iTexturePixels
                                     , iTextureWidth
                                     , iTextureHeight
                                     , iTextureBitsPerPixel
                                     , mBrush.ColorFromBrush ? false : true );
    }
}

void
FOdysseyVectorPath::DrawTexturedSegment( FOdysseyVectorSegment* iSegment
                                       , int8*  iScreenPixels
                                       , uint32 iScreenWidth
                                       , uint32 iScreenHeight
                                       , uint32 iScreenBitsPerPixel
                                       , int8*  iTexturePixels
                                       , uint32 iTextureWidth
                                       , uint32 iTextureHeight
                                       , uint32 iTextureBitsPerPixel
                                       , double iStartU
                                       , double iEndU
                                       , double iCombinedOpacity
                                       , uint64 iDrawingFlags )
{
    FOdysseyVectorEngine* vectorEngine = GetEngine();
    std::vector<FOdysseyVectorFraction>& fractionCache = iSegment->GetFractionCache();
    FColor foregroundColor = GetForegroundColor();
    double difU = iEndU - iStartU;

    for( int i = 0; i < fractionCache.size(); i++ )
    {
        FOdysseyVectorFraction* fraction = &fractionCache[i];
        BLPoint worldPoint[4] = { mWorldMatrix.mapPoint( fraction->polygon.point[0].x, fraction->polygon.point[0].y )
                                , mWorldMatrix.mapPoint( fraction->polygon.point[1].x, fraction->polygon.point[1].y ) 
                                , mWorldMatrix.mapPoint( fraction->polygon.point[2].x, fraction->polygon.point[2].y ) 
                                , mWorldMatrix.mapPoint( fraction->polygon.point[3].x, fraction->polygon.point[3].y ) };
        ::ULIS::FVec2I intPt[4] = { { (int32)worldPoint[0].x, (int32)worldPoint[0].y }
                                  , { (int32)worldPoint[1].x, (int32)worldPoint[1].y }
                                  , { (int32)worldPoint[2].x, (int32)worldPoint[2].y }
                                  , { (int32)worldPoint[3].x, (int32)worldPoint[3].y } };
        int32 xmin = ::ULIS::FMath::Min4( intPt[0].x, intPt[1].x, intPt[2].x, intPt[3].x );
        int32 xmax = ::ULIS::FMath::Max4( intPt[0].x, intPt[1].x, intPt[2].x, intPt[3].x );
        int32 ymin = ::ULIS::FMath::Min4( intPt[0].y, intPt[1].y, intPt[2].y, intPt[3].y );
        int32 ymax = ::ULIS::FMath::Max4( intPt[0].y, intPt[1].y, intPt[2].y, intPt[3].y );

        // don't draw if quad is outside the screen
        if( ( ( xmin ) < (int32)iScreenWidth  )
         && ( ( xmax ) > 0                    )
         && ( ( ymin ) < (int32)iScreenHeight )
         && ( ( ymax ) > 0                    ) )
        {
            double quadU[4] = { iStartU + ( fraction->polygon.U[0] * difU )
                              , iStartU + ( fraction->polygon.U[1] * difU )
                              , iStartU + ( fraction->polygon.U[2] * difU )
                              , iStartU + ( fraction->polygon.U[3] * difU ) };

            // should be a static function
            vectorEngine->DrawPolygon( intPt
                                     , quadU
                                     , fraction->polygon.V
                                     , 4
                                     , iCombinedOpacity
                                     , iScreenPixels
                                     , iScreenWidth
                                     , iScreenHeight
                                     , iScreenBitsPerPixel
                                     , foregroundColor
                                     , iTexturePixels
                                     , iTextureWidth
                                     , iTextureHeight
                                     , iTextureBitsPerPixel
                                     , mBrush.ColorFromBrush ? false : true );
        }
    }
}

void
FOdysseyVectorPath::DrawShape( BLContext* iBLContext, double iCombinedOpacity, uint64 iDrawingFlags )
{
    for( FOdysseyVectorChain& chain : mChainArray )
    {
        DrawChain( iBLContext, iCombinedOpacity, chain, iDrawingFlags );
    }
}

void
FOdysseyVectorPath::DrawChain( BLContext* iBLContext
                             , double iCombinedOpacity
                             , FOdysseyVectorChain& iChain
                             , uint64 iDrawingFlags )
{
    FOdysseyVectorEngine* vectorEngine = GetEngine();
    FColor color = mForegroundBucket.GetColor();
    BLRgba32 strokeColor = ( iDrawingFlags & FOdysseyVectorEngine::DRAWING_IGNORECOLOR ) ? BLRgba32( 0, 0, 0, 255 ) 
                                                                                         : BLRgba32( color.R, color.G, color.B, color.A * iCombinedOpacity );

    UTexture2D* texture = mBrush.GetTexture();
    BLImage* image = iBLContext->targetImage();
    BLImageData imageData;
    ::ULIS::FRectD screen;

    image->makeMutable( &imageData );

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
        if( texture )
        {
            const FColor* brushData = static_cast<const FColor*>(texture->PlatformData->Mips[0].BulkData.LockReadOnly());
            double startU = mBrush.Revert ? 1.0f : 0.0f;

            iChain.Iterate( [ this
                            , texture
                            , brushData
                            , &imageData
                            , &iCombinedOpacity
                            , &iDrawingFlags
                            , &startU
                            , &screen
                            , &iChain ]( FOdysseyVectorVertex* vertex, FOdysseyVectorSegment* segment ) -> bool
                            {
                                ::ULIS::FRectD bbox = segment->GetBoundingBox( true );
                                FOdysseyVectorJoint& joint = vertex->GetJoint();
                                double segmentLength = segment->GetLength();
                                double jointLength = joint.GetLength();
                                double segmentAndJointLength = segmentLength + jointLength;
                                double endU = 0.0f;

                                if( mBrush.ExtendOverPath )
                                {
                                    if( mBrush.Revert )
                                    {
                                        endU = iChain.mLength ? startU - ( segmentAndJointLength / iChain.mLength ) : 0.0f;
                                    }
                                    else
                                    {
                                        endU = iChain.mLength ? startU + ( segmentAndJointLength / iChain.mLength ) : 0.0f;
                                    }
                                }
                                else
                                {
                                    if( mBrush.Revert )
                                    {
                                        startU = 0.0f;
                                        endU   = 1.0f;
                                    }
                                    else
                                    {
                                        startU = 1.0f;
                                        endU   = 0.0f;
                                    }
                                }

                                if( segmentLength )
                                {
                                    double segmentStartU = ( vertex == segment->GetVertex(0) ) ? startU : endU;
                                    double segmentEndU   = ( vertex == segment->GetVertex(0) ) ? endU : startU;

                                    // WORKAROUND: in some cases U is < 0.0f, I dont know why yet. 
                                    if ( segmentStartU < 0.0f ) segmentStartU = 0.0f;
                                    if ( segmentEndU   < 0.0f ) segmentEndU   = 0.0f;

                                    // Textured joints are drawn only in texture mode (obviously) and if the texture
                                    // goes all over the path.
                                    if( ( vertex->IsHandleAligned() == false )
                                    && ( mBrush.ExtendOverPath == true ) )
                                    {
                                        double segmentJointRatio = jointLength / ( segmentAndJointLength );
                                        double jointStartU = segmentStartU;
                                        double jointEndU = segmentStartU + ( ( segmentEndU - segmentStartU ) * segmentJointRatio );

                                        segmentStartU = jointEndU;

                                        // WORKAROUND: in some cases U is < 0.0f, I dont know why yet. 
                                        if ( jointStartU < 0.0f ) jointStartU = 0.0f;
                                        if ( jointEndU   < 0.0f ) jointEndU   = 0.0f;

                                        // TODO : check for in-screen visibility
                                        DrawTexturedJoint( &joint
                                                         , (int8*)imageData.pixelData
                                                         , imageData.size.w
                                                         , imageData.size.h
                                                         , ( imageData.format == BL_FORMAT_PRGB32 ) ? 32 : 0
                                                         , (int8*) brushData
                                                         , texture->GetSurfaceWidth()
                                                         , texture->GetSurfaceHeight()
                                                         , 32
                                                         , jointStartU
                                                         , jointEndU
                                                         , iCombinedOpacity
                                                         , iDrawingFlags );
                                    }

                                    // don't draw if segment is outside the screen
                                    if( ( ( bbox.x          ) < screen.w )
                                     && ( ( bbox.x + bbox.w ) > 0        )
                                     && ( ( bbox.y          ) < screen.h )
                                     && ( ( bbox.y + bbox.h ) > 0        ) )
                                    {
                                        DrawTexturedSegment( segment
                                                            , (int8*)imageData.pixelData
                                                            , imageData.size.w
                                                            , imageData.size.h
                                                            , ( imageData.format == BL_FORMAT_PRGB32 ) ? 32 : 0
                                                            , (int8*) brushData
                                                            , texture->GetSurfaceWidth()
                                                            , texture->GetSurfaceHeight()
                                                            , 32
                                                            , segmentStartU
                                                            , segmentEndU
                                                            , iCombinedOpacity
                                                            , iDrawingFlags );
                                    }
                                }

                                // only when mBrush.ExtendOverPath == true 
                                startU = endU;

                                return false; // keep iterating
                            } );

            texture->PlatformData->Mips[0].BulkData.Unlock();
        }
        else
        {
            // We fill with stroke color because our curve is made of filled shapes.
            //iBLContext->setFillRule( BL_FILL_RULE_NON_ZERO );
            iBLContext->setFillRule( BL_FILL_RULE_EVEN_ODD );
            iBLContext->setFillStyle( strokeColor );
            iBLContext->setStrokeStyle( strokeColor );

            iChain.Iterate( [ this
                            , iBLContext
                            , &iDrawingFlags
                            , &screen ]( FOdysseyVectorVertex* vertex, FOdysseyVectorSegment* segment ) -> bool
                            {
                                ::ULIS::FRectD bbox = segment->GetBoundingBox( true );

                                // don't draw if segment is outside the screen
                                if( ( ( bbox.x          ) < screen.w )
                                 && ( ( bbox.x + bbox.w ) > 0        )
                                 && ( ( bbox.y          ) < screen.h )
                                 && ( ( bbox.y + bbox.h ) > 0        ) )
                                {
                                    segment->Draw( iBLContext );
                                }

                            // TODO : check for in-screen visibility
                                /*if( currentVertex->GetSegmentCount() == 2 )
                                {*/
                                    vertex->DrawJoint( iBLContext, iDrawingFlags );
                                /*}*/

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
    iBLContext->save();

    if( iWorld )
    {
        iBLContext->resetMatrix();
    }

    iBLContext->setStrokeWidth( iStrokeWidth );
    iBLContext->setStrokeStyle( iStrokeColor );

    for( FOdysseyVectorSegment* segment : mSegmentList )
    {
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
FOdysseyVectorPath::CopyShape()
{
    FOdysseyVectorPath* cubicPathCopy = new FOdysseyVectorPath( FString("Cubic Path") );
    std::map<FOdysseyVectorVertex*, FOdysseyVectorVertex*> lookupTable;

    for( FOdysseyVectorVertex* originalVertex : mVertexList )
    {
        FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( cubicPathCopy
                                                                  , originalVertex->GetX()
                                                                  , originalVertex->GetY()
                                                                  , originalVertex->GetRadius() );

        newVertex->SetHandleAligned( originalVertex->IsHandleAligned() );

        lookupTable.insert( std::make_pair( originalVertex, newVertex ) );

        cubicPathCopy->AddVertex( newVertex );
    }

    for( FOdysseyVectorSegment* originalSegment : mSegmentList )
    {
        if( originalSegment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            FOdysseyVectorSegmentCubic* originalCubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(originalSegment);
            FOdysseyVectorVertex* vertex0 = static_cast<FOdysseyVectorVertex*>( originalCubicSegment->GetPoint(0) );
            FOdysseyVectorVertex* vertex1 = static_cast<FOdysseyVectorVertex*>( originalCubicSegment->GetPoint(1) );
            FOdysseyVectorSegmentCubic* newCubicSegment = new FOdysseyVectorSegmentCubic( cubicPathCopy
                                                                                        , lookupTable[vertex0]
                                                                                        , originalCubicSegment->GetHandle(0)->GetX()
                                                                                        , originalCubicSegment->GetHandle(0)->GetY()
                                                                                        , originalCubicSegment->GetHandle(1)->GetX()
                                                                                        , originalCubicSegment->GetHandle(1)->GetY()
                                                                                        , lookupTable[vertex1]
                                                                                        , true );

            cubicPathCopy->AddSegment( newCubicSegment );
        }
        //newSegment->BuildVariable();
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
            FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( this, pt.x, pt.y, radius.Distance() );

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

uint32
FOdysseyVectorPath::GetType()
{
    return FOdysseyVectorObject::VECTORPATHTYPE;
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

        vertex->Set( localPt.x, localPt.y, vec.Distance() );
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

//static
::ULIS::FVec2D
FOdysseyVectorPath::GetAverageHandleVector( FOdysseyVectorVertex* iVertex, bool iNormalize )
{
    std::list<FOdysseyVectorSegment*>& segmentList = iVertex->GetSegmentList();
    ::ULIS::FVec2D averageHandleVector = ::ULIS::FVec2D( 0.0f, 0.0f );
    uint32 segmentCount = segmentList.size();

    if( segmentCount )
    {
        for( FOdysseyVectorSegment* segment : segmentList )
        {
            if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                 FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
  
                averageHandleVector += cubicSegment->GetHandleVector( iVertex, true );
            }
        }

        averageHandleVector /= segmentCount;

        if( iNormalize && averageHandleVector.Distance() )
        {
            averageHandleVector.Normalize();
        }
    }

    return averageHandleVector;
}

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

    iChain->Iterate( [ iChain
                     , &xmin
                     , &ymin
                     , &xmax
                     , &ymax
                     , &hasBBox ] ( FOdysseyVectorVertex* vertex, FOdysseyVectorSegment* segment ) -> bool
                     {
                         ::ULIS::FRectD segmentBBox = segment->GetBoundingBox( false );
                         double rx1 = segmentBBox.x
                              , ry1 = segmentBBox.y
                              , rx2 = segmentBBox.x + segmentBBox.w
                              , ry2 = segmentBBox.y + segmentBBox.h;

                         hasBBox = true;

                         if ( rx1 < xmin ) xmin = rx1;
                         if ( ry1 < ymin ) ymin = ry1;
                         if ( rx2 > xmax ) xmax = rx2;
                         if ( ry2 > ymax ) ymax = ry2;

                         // update joint
                         vertex->MakeJoint( segment );

                         iChain->mLength += segment->GetLength() + vertex->GetJointLength();

                         return false; // keep iterating
                     } );

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
