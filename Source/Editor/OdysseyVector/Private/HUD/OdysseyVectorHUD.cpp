#include "HUD/OdysseyVectorHUD.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorScene.h"
#include "OdysseyVectorEngine.h"

FPointQuadTree::~FPointQuadTree()
{
    for( int i = 0 ; i < 4; i++ )
    {
        if( mChildren[i] )
        {
            delete mChildren[i];
        }
    }
}

FPointQuadTree::FPointQuadTree( const ::ULIS::FRectD& iRect
                              , uint32 iMaxPointsPerQuad
                              , std::vector<FPointQuadTreeEntry>& iPointQuadTreeEntryArray )
    : mChildren { nullptr, nullptr, nullptr, nullptr }
    , mRect( iRect )
{
    Build( iMaxPointsPerQuad, iPointQuadTreeEntryArray );
}

FOdysseyVectorHUD::~FOdysseyVectorHUD()
{
    if( mPointQuadTree )
    {
        delete mPointQuadTree;
    }
}

FOdysseyVectorHUD::FOdysseyVectorHUD()
    : mPointQuadTree( nullptr )
{
}

void
FPointQuadTree::Draw( BLContext* iBLContext, FOdysseyVectorScene* iScene, uint64 iFlags )
{
    iBLContext->save();
    iBLContext->resetMatrix();
    iBLContext->setStrokeStyle( BLRgba32( 0xFF0000FF )  );
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->strokeRect( mRect.x, mRect.y, mRect.w, mRect.h );
    iBLContext->restore();

    for( int i = 0 ; i < 4; i++ )
    {
        if( mChildren[i] )
        {
            mChildren[i]->Draw( iBLContext, iScene, iFlags );
        }
    }
}

void
FPointQuadTree::Build( uint32 iMaxPointsPerQuad, std::vector<FPointQuadTreeEntry>& iParentPointQuadTreeEntryArray )
{
    mPointQuadTreeEntryArray.reserve( iParentPointQuadTreeEntryArray.size() );

    for( int i = 0; i < iParentPointQuadTreeEntryArray.size(); i++ )
    {
        if( mRect.HitTest( iParentPointQuadTreeEntryArray[i].worldCoords ) )
        {
            mPointQuadTreeEntryArray.push_back( iParentPointQuadTreeEntryArray[i] );
        }
    }

    if( mPointQuadTreeEntryArray.size() > iMaxPointsPerQuad )
    {
        uint32 minX =   mRect.x;
        uint32 minY =   mRect.y;
        uint32 maxX = ( mRect.x +   mRect.w );
        uint32 maxY = ( mRect.y +   mRect.h );
        uint32 avgX =   mRect.x + ( mRect.w * 0.5f );
        uint32 avgY =   mRect.y + ( mRect.h * 0.5f );

        mChildren[0] = new FPointQuadTree( ::ULIS::FRectD::FromMinMax( minX, minY, avgX, avgY ), iMaxPointsPerQuad, mPointQuadTreeEntryArray );
        mChildren[1] = new FPointQuadTree( ::ULIS::FRectD::FromMinMax( avgX, minY, maxX, avgY ), iMaxPointsPerQuad, mPointQuadTreeEntryArray );
        mChildren[2] = new FPointQuadTree( ::ULIS::FRectD::FromMinMax( avgX, avgY, maxX, maxY ), iMaxPointsPerQuad, mPointQuadTreeEntryArray );
        mChildren[3] = new FPointQuadTree( ::ULIS::FRectD::FromMinMax( minX, avgY, avgX, maxY ), iMaxPointsPerQuad, mPointQuadTreeEntryArray );

        mPointQuadTreeEntryArray.clear();
    }
}

void
FPointQuadTree::PickPoints( double iWorldX
                          , double iWorldY
                          , double iSelectionRadius
                          , std::vector<FOdysseyVectorPoint*>& oPickedPointArray )
{
   ::ULIS::FRectD rect = ::ULIS::FRectD( mRect.x -   iSelectionRadius,
                                         mRect.y -   iSelectionRadius,
                                         mRect.w + ( iSelectionRadius * 2 ),
                                         mRect.h + ( iSelectionRadius * 2 ) );

    if( rect.HitTest( ::ULIS::FVec2D( iWorldX, iWorldY ) ) )
    {
        if( mChildren[0] == nullptr )
        {
            for( int i = 0; i < mPointQuadTreeEntryArray.size(); i++ )
            {
                ::ULIS::FVec2D dif = ::ULIS::FVec2D( mPointQuadTreeEntryArray[i].worldCoords.x - iWorldX
                                                   , mPointQuadTreeEntryArray[i].worldCoords.y - iWorldY );

                if( dif.Distance() <= iSelectionRadius )
                {
                    oPickedPointArray.push_back( mPointQuadTreeEntryArray[i].point );
                }
            }
        }
        else
        {
            for( int i = 0; i < 4; i++ )
            {
                mChildren[i]->PickPoints( iWorldX, iWorldY, iSelectionRadius, oPickedPointArray );
            }
        }
    }
}

static void
MapPath( FOdysseyVectorPath* iPath
       , const ::ULIS::FRectD& iRect
       , std::vector<FPointQuadTreeEntry>& oPointQuadTreeEntryArray )
{
    std::list<FOdysseyVectorVertex*>& vertexList = iPath->GetVertexList();

    for( std::list<FOdysseyVectorVertex*>::iterator it = vertexList.begin(); it != vertexList.end(); ++it )
    {
        FOdysseyVectorVertex* vertex = (*it);
        ::ULIS::FVec2D& coords = vertex->GetCoords();
        BLPoint worldCoords = iPath->GetWorldMatrix().mapPoint( coords.x, coords.y );
        ::ULIS::FVec2D screenCoords = ::ULIS::FVec2D( worldCoords.x, worldCoords.y );

        if( iRect.HitTest( screenCoords ) )
        {
            oPointQuadTreeEntryArray.push_back( FPointQuadTreeEntry( vertex, screenCoords ) );
        }
    }
}
 
static void
MapPoints( FOdysseyVectorObject* iObject
         , const ::ULIS::FRectD& iRect
         , std::vector<FPointQuadTreeEntry>& oPointQuadTreeEntryArray )
{
    std::list<FOdysseyVectorObject*>& childrenList = iObject->GetChildrenList();

    if( iObject->GetClass() == FOdysseyVectorPath::StaticClass() )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>( iObject );

        MapPath( path, iRect, oPointQuadTreeEntryArray );
    }

    for( std::list<FOdysseyVectorObject*>::iterator it = childrenList.begin(); it != childrenList.end(); ++it )
    {
        FOdysseyVectorObject* child = (*it);

        MapPoints( child, iRect, oPointQuadTreeEntryArray );
    }
}

void
FOdysseyVectorHUD::MakePointQuadTree( FOdysseyVectorScene *iScene, bool iRestrictToSelection )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FPointQuadTreeEntry> pointQuadTreeEntryArray;
    uint32 width = vectorEngine->GetPreferredWidth()
         , height = vectorEngine->GetPreferredHeight();
    ::ULIS::FRectD screenRect;

    screenRect = ::ULIS::FRectD::FromXYWH( 0, 0, width, height );

    pointQuadTreeEntryArray.reserve( 200 );

    if( iRestrictToSelection )
    {
        std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
        std::list<FOdysseyVectorObject*>::iterator it;

        for( it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
        {
            FOdysseyVectorObject* selectedObject = (*it);

            MapPoints( selectedObject, screenRect, pointQuadTreeEntryArray );
        }
    }
    else
    {
        MapPoints( iScene, screenRect, pointQuadTreeEntryArray );
    }

    if( mPointQuadTree )
    {
        delete mPointQuadTree;
    }

    mPointQuadTree = new FPointQuadTree( screenRect, 20, pointQuadTreeEntryArray );
}

void
FOdysseyVectorHUD::PickPoints( double iWorldX
                             , double iWorldY
                             , double iSelectionRadius
                             , std::vector<FOdysseyVectorPoint*>& oPickedPointArray )
{
    if( mPointQuadTree )
    {
        mPointQuadTree->PickPoints( iWorldX, iWorldY, iSelectionRadius, oPickedPointArray );
    }
}

// static
FColor&
FOdysseyVectorHUD::GetForegroundColor()
{
    static FColor fg = FColor( 0x40, 0xE0, 0xD0, 0xFF ); // teal

    return fg;
}

// static
FColor&
FOdysseyVectorHUD::GetBackgroundColor()
{
    static FColor bg = FColor( 0x00, 0x00, 0x00, 0xFF ); // black

    return bg;
}

// static
FColor&
FOdysseyVectorHUD::GetHighlightColor()
{
    static FColor hc = FColor( 0xFF, 0x00, 0x00, 0xFF ); // red

    return hc;
}


FSelectionBox&
FOdysseyVectorHUD::GetSelectionBox()
{
    return mSelectionBox;
}

void
FOdysseyVectorHUD::UpdateSelectionBoxVertexModeRecursive( FOdysseyVectorObject* iObject
                                                        , bool iForceWorld )
{
    if( iObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        FOdysseyVectorPath* selectedPath = static_cast<FOdysseyVectorPath*>(iObject);
        ::ULIS::FRectD selectedPathBBox;

        if( selectedPath->GetBBoxFromSelectedVertices( selectedPathBBox, true ) )
        {
            mSelectionBox.rect = mSelectionBox.inited ? mSelectionBox.rect | selectedPathBBox
                                                      : selectedPathBBox;

            mSelectionBox.inited = true;
        }
    }

    for( FOdysseyVectorObject* childObject : iObject->GetChildrenList() )
    {
        UpdateSelectionBoxVertexModeRecursive( childObject, iForceWorld );
    }
}

void
FOdysseyVectorHUD::UpdateSelectionBoxVertexMode( FOdysseyVectorScene* iScene
                                               , std::list<FOdysseyVectorObject*>& iSelectedObjectList
                                               , bool iForceWorld )
{
    mSelectionBox.inited = false;
    //mSelectionBox.rect = ::ULIS::FRectD( 0, 0, 0, 0 );
    mSelectionBox.worldMatrix.reset();

    for( FOdysseyVectorObject* selectedObject : iSelectedObjectList )
    {
        UpdateSelectionBoxVertexModeRecursive( selectedObject, iForceWorld );
    }

    BLMatrix2D::invert( mSelectionBox.inverseWorldMatrix, mSelectionBox.worldMatrix );
}

void
FOdysseyVectorHUD::UpdateSelectionBoxObjectMode( FOdysseyVectorScene* iScene
                                               , std::list<FOdysseyVectorObject*>& iSelectedObjectList
                                               , bool iForceWorld )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iSelectedObjectList;

    mSelectionBox.inited = false;
    mSelectionBox.rect = ::ULIS::FRectD( 0, 0, 0, 0 );

    if( ( selectedObjectList.size() == 1 ) && ( iForceWorld == false ) )
    {
        FOdysseyVectorObject* selectedObject = selectedObjectList.front();

        mSelectionBox.inited = true;
        mSelectionBox.rect = selectedObject->GetBBox( false );

        mSelectionBox.worldMatrix = selectedObject->GetWorldMatrix();
        mSelectionBox.inverseWorldMatrix = selectedObject->GetInverseWorldMatrix();
    }
    else
    {
        BLPoint p0, p1, p2, p3;
        ::ULIS::FRectD rect = FOdysseyVectorObject::GetBoundingBoxFromList( selectedObjectList );
        ::ULIS::FVec2D origin = ::ULIS::FVec2D( rect.x + (rect.w * 0.5f)
                                              , rect.y + (rect.h * 0.5f) );
        mSelectionBox.worldMatrix.reset();
        mSelectionBox.worldMatrix.translate( origin.x, origin.y );

        BLMatrix2D::invert( mSelectionBox.inverseWorldMatrix, mSelectionBox.worldMatrix );

        p0 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x         , rect.y          );
        p1 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x + rect.w, rect.y          );
        p2 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x + rect.w, rect.y + rect.h );
        p3 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x         , rect.y + rect.h );

        mSelectionBox.inited = true;
        mSelectionBox.rect = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                       , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                       , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                       , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );
    }
}

void
FOdysseyVectorHUD::UpdateSelectionBox( FOdysseyVectorScene* iScene
                                     , std::list<FOdysseyVectorObject*>& iSelectedObjectList
                                     , bool iForceWorld
                                     , uint64 iHUDFlags )
{
    if( iHUDFlags & VIEW_MODE_OBJECT )
    {
        //case eVectorEditionMode::Object :
        UpdateSelectionBoxObjectMode( iScene, iSelectedObjectList, iForceWorld );
    }

    if( iHUDFlags & VIEW_MODE_VERTEX )
    {
        //case eVectorEditionMode::Vertex:
        UpdateSelectionBoxVertexMode( iScene, iSelectedObjectList, iForceWorld );
    }
}

void
FOdysseyVectorHUD::DrawSelectionBox( BLContext* iBLContext
                                   , FOdysseyVectorScene* iScene
                                   , BLRgba32& iForegroundColor
                                   , BLRgba32& iBackgroundColor
                                   , BLRgba32& iHighlightColor
                                   , uint64 iHUDFlags )
{
    BLRgba32 white = BLRgba32( 255, 255, 255, 255 );

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    iBLContext->save();
    iBLContext->resetMatrix();

    if( mSelectionBox.inited )
    {
        BLMatrix2D& worldMatrix = mSelectionBox.worldMatrix;
        BLPoint point[4] = { worldMatrix.mapPoint( mSelectionBox.rect.x                       , mSelectionBox.rect.y                        )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y                        )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y + mSelectionBox.rect.h )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x                       , mSelectionBox.rect.y + mSelectionBox.rect.h ) };
        BLPath path;

        path.moveTo( point[0] );
        path.lineTo( point[1] );
        path.lineTo( point[2] );
        path.lineTo( point[3] );
        path.close();

        iBLContext->setStrokeStyle( iBackgroundColor );
        iBLContext->setStrokeWidth( 2.0f );
        iBLContext->strokePath( path );

        // draw box as white if nothing is selected, colored if something is selected
        iBLContext->setStrokeStyle( ( iScene->GetSelectedObjectList().size() == 0 ) ? white : iForegroundColor );
        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->strokePath( path );
    }

    iBLContext->restore();
}

// static
void
FOdysseyVectorHUD::DrawLine( BLContext* iBLContext
                           , double iWorldx0
                           , double iWorldY0
                           , double iWorldx1
                           , double iWorldY1
                           , const BLRgba32& fgColor
                           , const BLRgba32& bgColor )
{
    // Draw 2 lines. We draw a contour and the inside with different colors,
    // that way the background color does not interfere as it can be of only one color.
    // outer
    iBLContext->setStrokeWidth( 2.0f );
    iBLContext->setStrokeStyle( bgColor );
    iBLContext->strokeLine( iWorldx0, iWorldY0, iWorldx1, iWorldY1 );
    // inner
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->setStrokeStyle( fgColor );
    iBLContext->strokeLine( iWorldx0, iWorldY0, iWorldx1, iWorldY1 );
}

// static
void
FOdysseyVectorHUD::DrawCircle( BLContext* iBLContext
                             , double iWorldx
                             , double iWorldY
                             , double iRadius
                             , const BLRgba32& fgColor
                             , const BLRgba32& bgColor )
{
    // inner
    iBLContext->setFillStyle( fgColor );
    iBLContext->fillCircle( iWorldx, iWorldY, iRadius );
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->setStrokeStyle( bgColor );
    iBLContext->strokeCircle( iWorldx, iWorldY, iRadius );
}

// static
void
FOdysseyVectorHUD::DrawVertex( BLContext* iBLContext
                             , FOdysseyVectorVertex* iVertex
                             , const BLRgba32& fgColor
                             , const BLRgba32& bgColor
                             , const BLRgba32& hcColor
                             , bool iWorld
                             , uint64 iHUDFlags )
{
    FOdysseyVectorPath* path = iVertex->GetPath();
    BLMatrix2D& HUDMatrix = iWorld ? path->GetWorldMatrix() : path->GetLocalMatrix();
    ::ULIS::FVec2D perpendicular = FOdysseyVectorPath::GetPerpendicularVector( iVertex, true );
    ::ULIS::FVec2D ctrl = perpendicular * iVertex->GetRadius();
    // TODO: compute that once and pass it as parameter for all vertices
    BLPoint point = HUDMatrix.mapPoint( iVertex->GetX(), iVertex->GetY() );
    BLPoint handle = HUDMatrix.mapVector( ctrl.x, ctrl.y );
    static BLRgba32 greenColor = BLRgba32( 0, 255, 0, 255 );

    if ( iHUDFlags & VIEW_PATH_VERTEX_HANDLE )
    {
        static BLRgba32 whiteColor = BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF );
        static BLRgba32 blackColor = BLRgba32( 0x00, 0x00, 0x00, 0xFF );

        // Line to handle
        iBLContext->setStrokeWidth( 2.0f );
        iBLContext->setStrokeStyle( bgColor );
        iBLContext->strokeLine( point.x, point.y, point.x + handle.x, point.y + handle.y );

        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->setStrokeStyle( whiteColor );
        iBLContext->strokeLine( point.x, point.y, point.x + handle.x, point.y + handle.y );

        // handle
        FOdysseyVectorHUD::DrawCircle( iBLContext
                                     , point.x + handle.x
                                     , point.y + handle.y
                                     , HANDLERADIUS
                                     , whiteColor
                                     , blackColor );

        // Line to handle
        iBLContext->setStrokeWidth( 2.0f );
        iBLContext->setStrokeStyle( bgColor );
        iBLContext->strokeLine( point.x, point.y, point.x - handle.x, point.y - handle.y );

        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->setStrokeStyle( whiteColor );
        iBLContext->strokeLine( point.x, point.y, point.x - handle.x, point.y - handle.y );

        // handle
        FOdysseyVectorHUD::DrawCircle( iBLContext
                                     , point.x - handle.x
                                     , point.y - handle.y
                                     , HANDLERADIUS
                                     , whiteColor
                                     , blackColor );
    }

    FOdysseyVectorHUD::DrawCircle( iBLContext
                                 , point.x
                                 , point.y
                                 , VERTEXRADIUS
                                 , iVertex->IsSelected() ? hcColor : fgColor
                                 , bgColor );

    if ( iHUDFlags & VIEW_PATH_VERTEX_ALIGNMENT )
    {
        if( iVertex->IsHandleAligned() )
        {
            iBLContext->setStrokeWidth( 1.0f );
            iBLContext->setStrokeStyle( greenColor );
            iBLContext->strokeCircle( point.x, point.y, VERTEXRADIUS + 2 );
        }
    }
}

// static
void
FOdysseyVectorHUD::DrawCubicSegment( BLContext* iBLContext
                                   , FOdysseyVectorSegmentCubic* iCubicSegment
                                   , const BLRgba32& fgColor
                                   , const BLRgba32& bgColor
                                   , const BLRgba32& hcColor
                                   , bool iWorld
                                   , uint64 iHUDFlags )
{
    FOdysseyVectorPath* path = iCubicSegment->GetPath();
    BLMatrix2D& HUDMatrix = iWorld ? path->GetWorldMatrix() : path->GetLocalMatrix();
    FOdysseyVectorVertex* vertex0 = iCubicSegment->GetVertex(0);
    FOdysseyVectorVertex* vertex1 = iCubicSegment->GetVertex(1);
    FOdysseyVectorHandleSegment* handle0 = iCubicSegment->GetHandle(0);
    FOdysseyVectorHandleSegment* handle1 = iCubicSegment->GetHandle(1);
    BLPoint point[2] = { HUDMatrix.mapPoint( vertex0->GetX(), vertex0->GetY() )
                       , HUDMatrix.mapPoint( vertex1->GetX(), vertex1->GetY() ) };
    BLPoint handlePoint[2] = { HUDMatrix.mapPoint( handle0->GetX(), handle0->GetY() )
                             , HUDMatrix.mapPoint( handle1->GetX(), handle1->GetY() ) };
    BLPath segment;

    segment.moveTo( point[0] );
    segment.cubicTo( handlePoint[0]
                   , handlePoint[1]
                   , point[1] );

    iBLContext->setStrokeWidth( 2.0f );
    iBLContext->setStrokeStyle( bgColor );
    iBLContext->strokePath( segment );

    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->setStrokeStyle( fgColor );
    iBLContext->strokePath( segment );

    if ( iHUDFlags & VIEW_PATH_SEGMENT_HANDLE )
    {
        static BLRgba32 whiteColor = BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF );
        static BLRgba32 blackColor = BLRgba32( 0x00, 0x00, 0x00, 0xFF );

        for( int i = 0; i < 2; i++ )
        {
            FOdysseyVectorHUD::DrawLine( iBLContext
                                       , point[i].x
                                       , point[i].y
                                       , handlePoint[i].x
                                       , handlePoint[i].y
                                       , whiteColor
                                       , blackColor );

            // control handle 0
            FOdysseyVectorHUD::DrawCircle( iBLContext
                                         , handlePoint[i].x
                                         , handlePoint[i].y
                                         , HANDLERADIUS
                                         , whiteColor
                                         , blackColor );
        }
    }
}

// static
void
FOdysseyVectorHUD::DrawPath( BLContext* iBLContext
                           , FOdysseyVectorPath* iPath
                           , const BLRgba32& fgColor
                           , const BLRgba32& bgColor
                           , const BLRgba32& hcColor
                           , bool iWorld
                           , uint64 iHUDFlags )
{
    std::list<FOdysseyVectorSegment*>& segmentList = iPath->GetSegmentList();
    std::list<FOdysseyVectorVertex*>& vertexList = iPath->GetVertexList();

    iBLContext->save();

    if( iWorld )
    {
        iBLContext->resetMatrix();
    }

    if( iHUDFlags & VIEW_PATH_SEGMENT )
    {
        for( FOdysseyVectorSegment* segment : segmentList )
        {
            if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);

                DrawCubicSegment( iBLContext, cubicSegment, fgColor, bgColor, hcColor, iWorld, iHUDFlags );
            }
        }
    }

    if( iHUDFlags & VIEW_PATH_VERTEX )
    {
        // Points and Point size handles
        for( FOdysseyVectorVertex* vertex : vertexList )
        {
            uint32 valence = vertex->GetSegmentCount();

            if( ( valence == 0 ) && ( iHUDFlags & VIEW_PATH_VERTEX_VALENCE0 ) )
            {
                DrawVertex( iBLContext, vertex, fgColor, bgColor, hcColor, iWorld, iHUDFlags );
            }
            else
            if( ( valence == 1 ) && ( iHUDFlags & VIEW_PATH_VERTEX_VALENCE1 ) )
            {
                DrawVertex( iBLContext, vertex, fgColor, bgColor, hcColor, iWorld, iHUDFlags );
            }
            else
            if( ( valence == 2 ) && ( iHUDFlags & VIEW_PATH_VERTEX_VALENCE2 ) )
            {
                DrawVertex( iBLContext, vertex, fgColor, bgColor, hcColor, iWorld, iHUDFlags );
            }
        }
    }

    iBLContext->restore();
}

// static
::ULIS::FVec2D
FOdysseyVectorHUD::GetBucketPosition( FOdysseyVectorBucket* iBucket, bool iWorld )
{
    ::ULIS::FVec2D& bucketCoords = iBucket->GetCoords();

    if( iWorld )
    {
        FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
        BLMatrix2D& worldMatrix = ownerObject->GetWorldMatrix();
        BLPoint worldPosition = worldMatrix.mapPoint( bucketCoords.x, bucketCoords.y );

        return ::ULIS::FVec2D( worldPosition.x, worldPosition.y );
    }

    return bucketCoords;
}

// static
::ULIS::FVec2D
FOdysseyVectorHUD::GetBucketRadialHandlePosition( FOdysseyVectorBucket* iBucket, bool iWorld )
{
    ::ULIS::FVec2D radialHandleCoords = iBucket->GetCoords() + iBucket->GetRadialOffset();

    radialHandleCoords.x += iBucket->GetRadialRadius();

    if( iWorld )
    {
        FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
        BLMatrix2D& worldMatrix = ownerObject->GetWorldMatrix();
        BLPoint worldPosition = worldMatrix.mapPoint( radialHandleCoords.x, radialHandleCoords.y );

        return ::ULIS::FVec2D( worldPosition.x, worldPosition.y );
    }

    return radialHandleCoords;
}

// static
::ULIS::FVec2D
FOdysseyVectorHUD::GetBucketRadialPosition( FOdysseyVectorBucket* iBucket, bool iWorld )
{
    ::ULIS::FVec2D radialCoords = iBucket->GetCoords() + iBucket->GetRadialOffset();

    if( iWorld )
    {
        FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
        BLMatrix2D& worldMatrix = ownerObject->GetWorldMatrix();
        BLPoint worldPosition = worldMatrix.mapPoint( radialCoords.x, radialCoords.y );

        return ::ULIS::FVec2D( worldPosition.x, worldPosition.y );
    }

    return radialCoords;
}

// static
::ULIS::FVec2D
FOdysseyVectorHUD::GetBucketHandleVector( FOdysseyVectorBucket* iBucket, bool iWorld )
{
    double a = iBucket->GetRotation();
    ::ULIS::FVec2D handleVector = ::ULIS::FVec2D( cos( a ), sin( a ) );

    if( iWorld )
    {
        FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
        BLMatrix2D& worldMatrix = ownerObject->GetWorldMatrix();
        BLPoint worldVector = worldMatrix.mapVector( handleVector.x, handleVector.y );
        ::ULIS::FVec2D normalizedVector = ::ULIS::FVec2D( worldVector.x, worldVector.y );

        normalizedVector.Normalize();

        return normalizedVector;
    }

    return handleVector;
}

// static
void
FOdysseyVectorHUD::DrawBucket( BLContext* iBLContext
                             , FOdysseyVectorBucket* iBucket
                             , const BLRgba32& fgColor
                             , const BLRgba32& bgColor
                             , const BLRgba32& hcColor
                             , uint64 iHUDFlags )
{
    ::ULIS::FVec2D bucketWorldCoords = GetBucketPosition( iBucket, true );
    FColor bucketColor = iBucket->GetColor();
    BLRgba32 fillColor = BLRgba32( bucketColor.R, bucketColor.G, bucketColor.B, bucketColor.A );
    BLRgba32 propColor = iBucket->IsPropagated() ? BLRgba32( 0x00, 0xFF, 0x00, 0xFF )
                                                 : BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF );

    if( iHUDFlags & VIEW_GROUPPAINT_BUCKET_HANDLE )
    {
        if( iBucket->GetColorMode() == eBucketColorMode::LinearGradient )
        {
            ::ULIS::FVec2D handleWorldCoords = bucketWorldCoords + ( GetBucketHandleVector( iBucket, true ) * HANDLE_DISTANCE );
            BLRgba32 blackColor = BLRgba32( 0x00, 0x00, 0x00, 0xFF );
            BLRgba32 whiteColor = BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF );

            // Bucket-to-handle line
            iBLContext->setStrokeWidth( 2.0f );
            iBLContext->setStrokeStyle( blackColor );
            iBLContext->strokeLine( bucketWorldCoords.x, bucketWorldCoords.y
                                  , handleWorldCoords.x, handleWorldCoords.y );
            iBLContext->setStrokeWidth( 1.0f );
            iBLContext->setStrokeStyle( whiteColor );
            iBLContext->strokeLine( bucketWorldCoords.x, bucketWorldCoords.y
                                  , handleWorldCoords.x, handleWorldCoords.y );

            // Handle
            iBLContext->setFillStyle( whiteColor );
            iBLContext->fillCircle( handleWorldCoords.x, handleWorldCoords.y, HANDLE_RADIUS );
            iBLContext->setStrokeWidth( 1.0f );
            iBLContext->setStrokeStyle( blackColor );
            iBLContext->strokeCircle( handleWorldCoords.x, handleWorldCoords.y, HANDLE_RADIUS );
        }

        if( iBucket->GetColorMode() == eBucketColorMode::RadialGradient )
        {
            ::ULIS::FVec2D radialWorldCoords = GetBucketRadialPosition( iBucket, true );
            ::ULIS::FVec2D radialHandleWorldCoords = GetBucketRadialHandlePosition( iBucket, true );
            double radialRadius = ( radialHandleWorldCoords - radialWorldCoords ).Distance();

            BLRgba32 blackColor = BLRgba32( 0x00, 0x00, 0x00, 0xFF );
            BLRgba32 whiteColor = BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF );

            // Bucket-to-radial line
            iBLContext->setStrokeWidth( 2.0f );
            iBLContext->setStrokeStyle( blackColor );
            iBLContext->strokeLine( bucketWorldCoords.x, bucketWorldCoords.y
                                  , radialWorldCoords.x, radialWorldCoords.y );
            iBLContext->setStrokeWidth( 1.0f );
            iBLContext->setStrokeStyle( whiteColor );
            iBLContext->strokeLine( bucketWorldCoords.x, bucketWorldCoords.y
                                  , radialWorldCoords.x, radialWorldCoords.y );

            // Radial Circle
            iBLContext->setStrokeWidth( 2.0f );
            iBLContext->setStrokeStyle( blackColor );
            iBLContext->strokeCircle( radialWorldCoords.x, radialWorldCoords.y, RADIAL_AREA_RADIUS );
            iBLContext->setStrokeWidth( 1.0f );
            iBLContext->setStrokeStyle( whiteColor );
            iBLContext->strokeCircle( radialWorldCoords.x, radialWorldCoords.y, RADIAL_AREA_RADIUS );

            // Radial-to-RadialHandle line
            iBLContext->setStrokeWidth( 2.0f );
            iBLContext->setStrokeStyle( blackColor );
            iBLContext->strokeLine( radialWorldCoords.x, radialWorldCoords.y
                                  , radialHandleWorldCoords.x, radialHandleWorldCoords.y );
            iBLContext->setStrokeWidth( 1.0f );
            iBLContext->setStrokeStyle( whiteColor );
            iBLContext->strokeLine( radialWorldCoords.x, radialWorldCoords.y
                                  , radialHandleWorldCoords.x, radialHandleWorldCoords.y );

            // RadialHandle Circle
            iBLContext->setStrokeWidth( 2.0f );
            iBLContext->setStrokeStyle( blackColor );
            iBLContext->strokeCircle( radialWorldCoords.x, radialWorldCoords.y, radialRadius );
            iBLContext->setStrokeWidth( 1.0f );
            iBLContext->setStrokeStyle( whiteColor );
            iBLContext->strokeCircle( radialWorldCoords.x, radialWorldCoords.y, radialRadius );

            // RadialHandle
            iBLContext->setFillStyle( whiteColor );
            iBLContext->fillCircle( radialHandleWorldCoords.x, radialHandleWorldCoords.y, HANDLE_RADIUS );
            iBLContext->setStrokeWidth( 1.0f );
            iBLContext->setStrokeStyle( blackColor );
            iBLContext->strokeCircle( radialHandleWorldCoords.x, radialHandleWorldCoords.y, HANDLE_RADIUS );
        }
    }

    // Bucket
    iBLContext->setFillStyle( fillColor );
    iBLContext->fillCircle( bucketWorldCoords.x, bucketWorldCoords.y, PELLET_RADIUS );

    iBLContext->setStrokeWidth( 2.0f );
    iBLContext->setStrokeStyle( BLRgba32( 0x00, 0x00, 0x00, 0xFF ) );
    iBLContext->strokeCircle( bucketWorldCoords.x, bucketWorldCoords.y, PELLET_RADIUS );
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->setStrokeStyle( propColor ); // green if propagated, white otherwise
    iBLContext->strokeCircle( bucketWorldCoords.x, bucketWorldCoords.y, PELLET_RADIUS );
}

// static
void
FOdysseyVectorHUD::DrawGroupPaint( BLContext* iBLContext
                                 , FOdysseyVectorGroupPaint* iPaintGroup
                                 , const BLRgba32& fgColor
                                 , const BLRgba32& bgColor
                                 , const BLRgba32& hcColor
                                 , bool iWorld
                                 , uint64 iHUDFlags )
{
    iBLContext->save();

    if( iWorld )
    {
        iBLContext->resetMatrix();
    }

    if( iHUDFlags & VIEW_GROUPPAINT_BUCKET )
    {
        std::list<FOdysseyVectorBucket*>& bucketList = iPaintGroup->GetBucketList();

        for( FOdysseyVectorBucket *bucket : bucketList )
        {
            DrawBucket( iBLContext, bucket, fgColor, bgColor, hcColor, iHUDFlags );
        }
    }

    iBLContext->restore();
}

// static
void
FOdysseyVectorHUD::DrawObjectRecursive( BLContext* iBLContext
                                      , FOdysseyVectorObject* iObject
                                      , BLRgba32& iForegroundColor
                                      , BLRgba32& iBackgroundColor
                                      , BLRgba32& iHighlightColor
                                      , uint64 iHUDFlags )
{
    if( iHUDFlags & VIEW_PATH_ALL )
    {
        if( iObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iObject);

            FOdysseyVectorHUD::DrawPath( iBLContext
                                       , path
                                       , iForegroundColor
                                       , iBackgroundColor
                                       , iHighlightColor
                                       , true
                                       , iHUDFlags );
        }
    }

    if( iHUDFlags & VIEW_GROUPPAINT_ALL )
    {
        if( iObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(iObject);

            FOdysseyVectorHUD::DrawGroupPaint( iBLContext
                                             , paintGroup
                                             , iForegroundColor
                                             , iBackgroundColor
                                             , iHighlightColor
                                             , true
                                             , iHUDFlags );
        }
    }

    for( FOdysseyVectorObject* child : iObject->GetChildrenList() )
    {
        DrawObjectRecursive( iBLContext
                           , child
                           , iForegroundColor
                           , iBackgroundColor
                           , iHighlightColor
                           , iHUDFlags );
    }
}

// static
void
FOdysseyVectorHUD::DrawObjects( BLContext* iBLContext
                              , std::list<FOdysseyVectorObject*>* iFocusedObjectList
                              , BLRgba32& iForegroundColor
                              , BLRgba32& iBackgroundColor
                              , BLRgba32& iHighlightColor
                              , uint64 iHUDFlags )
{
    iBLContext->save();
    iBLContext->resetMatrix();

    if( iFocusedObjectList )
    {
        for( FOdysseyVectorObject* focusedObject : *iFocusedObjectList )
        {
            DrawObjectRecursive( iBLContext
                               , focusedObject
                               , iForegroundColor
                               , iBackgroundColor
                               , iHighlightColor
                               , iHUDFlags );
        }
    }

    iBLContext->restore();
}

/*
void
FOdysseyVectorHUD::Draw( BLContext* iBLContext, FOdysseyVectorScene* iScene, uint64 iHUDFlags )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    std::list<FOdysseyVectorObject*>* focusedObjectList = GetFocusedObjectList( iScene );

    if( focusedObjectList )
    {
        DrawObjects( iBLContext, focusedObjectList, fgColor, bgColor, hcColor, iHUDFlags );
    }

    if( iHUDFlags & VIEW_SELECTIONBOX )
    {
        DrawSelectionBox( iBLContext, focusedObjectList, fgColor, bgColor, hcColor, iHUDFlags );
    }
}
*/