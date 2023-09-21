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
FPointQuadTree::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();

    blctx->save();
    blctx->resetMatrix();
    blctx->setStrokeStyle( BLRgba32( 0xFF0000FF )  );
    blctx->setStrokeWidth( 1.0f );
    blctx->strokeRect( mRect.x, mRect.y, mRect.w, mRect.h );
    blctx->restore();

    for( int i = 0 ; i < 4; i++ )
    {
        if( mChildren[i] )
        {
            mChildren[i]->Draw( iScene, iFlags );
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
    uint32 width = vectorEngine->GetWidth()
         , height = vectorEngine->GetHeight();
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

//static
void
FOdysseyVectorHUD::DrawObjectRecursive( FOdysseyVectorObject* iObj, BLContext* iBLCtx )
{
    if( iObj->GetClass() == FOdysseyVectorPath::StaticClass() )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iObj);
        std::list<FOdysseyVectorVertex*>& vertexList = path->GetVertexList();

        iBLCtx->setCompOp( BL_COMP_OP_SRC_OVER );
        iBLCtx->setFillStyle( BLRgba32( 0x80FFFFFF ) );

        // Points and Point size handles
        for( std::list<FOdysseyVectorVertex*>::iterator it = vertexList.begin(); it != vertexList.end(); ++it )
        {
            FOdysseyVectorVertex *vertex = static_cast<FOdysseyVectorVertex*>(*it);
            BLMatrix2D& worldMatrix = path->GetWorldMatrix();
            BLPoint worldPoint = worldMatrix.mapPoint( vertex->GetX(), vertex->GetY() );

            iBLCtx->setFillStyle( BLRgba32( 0xFF000000 ) );
            iBLCtx->fillCircle( worldPoint.x, worldPoint.y, 2.0f );
            iBLCtx->setFillStyle( BLRgba32( 0xFFFFFFFF ) );
            iBLCtx->fillCircle( worldPoint.x, worldPoint.y, 1.0f );

            /*iBLCtx->fillRect( worldPoint.x + POINTRECT.x
                            , worldPoint.y + POINTRECT.y
                            , POINTRECT.w
                            , POINTRECT.h );*/
        }
    }

    for( std::list<FOdysseyVectorObject*>::iterator it = iObj->GetChildrenList().begin(); it != iObj->GetChildrenList().end(); ++it )
    {
        FOdysseyVectorObject* child = (*it);

        DrawObjectRecursive( child, iBLCtx );
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

// static
void
FOdysseyVectorHUD::DrawLine( double iWorldx0
                           , double iWorldY0
                           , double iWorldx1
                           , double iWorldY1
                           , BLContext* iBLContext
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
FOdysseyVectorHUD::DrawCircle( double iWorldx
                             , double iWorldY
                             , double iRadius
                             , BLContext* iBLContext
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
FOdysseyVectorHUD::DrawVertex( FOdysseyVectorVertex* iVertex
                             , BLContext* iBLContext
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

    FOdysseyVectorHUD::DrawCircle( point.x
                                 , point.y
                                 , VERTEXRADIUS
                                 , iBLContext
                                 , iVertex->IsSelected() ? hcColor : fgColor
                                 , bgColor );

    if ( iHUDFlags & VIEW_VERTEX_ALIGNMENT )
    {
        if( iVertex->IsHandleAligned() )
        {
            iBLContext->setStrokeWidth( 1.0f );
            iBLContext->setStrokeStyle( greenColor );
            iBLContext->strokeCircle( point.x, point.y, VERTEXRADIUS + 2 );
        }
    }

    if ( iHUDFlags & VIEW_VERTEX_HANDLE )
    {
        static BLRgba32 whiteColor = BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF );
        static BLRgba32 blackColor = BLRgba32( 0x00, 0x00, 0x00, 0xFF );

        FOdysseyVectorHUD::DrawCircle( point.x + handle.x
                                     , point.y + handle.y
                                     , HANDLERADIUS
                                     , iBLContext
                                     , whiteColor
                                     , blackColor );

        FOdysseyVectorHUD::DrawCircle( point.x - handle.x
                                     , point.y - handle.y
                                     , HANDLERADIUS
                                     , iBLContext
                                     , whiteColor
                                     , blackColor );
    }
}

// static
void
FOdysseyVectorHUD::DrawCubicSegment( FOdysseyVectorSegmentCubic* iCubicSegment
                                   , BLContext* iBLContext
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

    if ( iHUDFlags & VIEW_SEGMENT_HANDLE )
    {
        static BLRgba32 whiteColor = BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF );
        static BLRgba32 blackColor = BLRgba32( 0x00, 0x00, 0x00, 0xFF );

        for( int i = 0; i < 2; i++ )
        {
            FOdysseyVectorHUD::DrawLine( point[i].x
                                       , point[i].y
                                       , handlePoint[i].x
                                       , handlePoint[i].y
                                       , iBLContext
                                       , whiteColor
                                       , blackColor );

            // control handle 0
            FOdysseyVectorHUD::DrawCircle( handlePoint[i].x
                                         , handlePoint[i].y
                                         , HANDLERADIUS
                                         , iBLContext
                                         , whiteColor
                                         , blackColor );
        }
    }
}

// static
void
FOdysseyVectorHUD::DrawPath( FOdysseyVectorPath* iPath
                           , const BLRgba32& fgColor
                           , const BLRgba32& bgColor
                           , const BLRgba32& hcColor
                           , bool iWorld
                           , uint64 iHUDFlags )
{
    BLContext* blctx = iPath->GetScene()->GetEngine()->GetBLContext();
    std::list<FOdysseyVectorSegment*>& segmentList = iPath->GetSegmentList();
    std::list<FOdysseyVectorVertex*>& vertexList = iPath->GetVertexList();

    blctx->save();

    if( iWorld )
    {
        blctx->resetMatrix();
    }

    if( iHUDFlags & VIEW_SEGMENT )
    {
        for( std::list<FOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
        {
            FOdysseyVectorSegment* segment = (*it);

            if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);

                DrawCubicSegment( cubicSegment, blctx, fgColor, bgColor, hcColor, iWorld, iHUDFlags );
            }
        }
    }

    if( iHUDFlags & VIEW_VERTEX )
    {
        // Points and Point size handles
        for( std::list<FOdysseyVectorVertex*>::iterator it = vertexList.begin(); it != vertexList.end(); ++it )
        {
            FOdysseyVectorVertex *vertex = (*it);
            uint32 valence = vertex->GetSegmentCount();

            if( ( valence == 0 ) && ( iHUDFlags & VIEW_VERTEX_VALENCE0 ) )
            {
                DrawVertex( vertex, blctx, fgColor, bgColor, hcColor, iWorld, iHUDFlags );
            }
            else
            if( ( valence == 1 ) && ( iHUDFlags & VIEW_VERTEX_VALENCE1 ) )
            {
                DrawVertex( vertex, blctx, fgColor, bgColor, hcColor, iWorld, iHUDFlags );
            }
            else
            if( ( valence == 2 ) && ( iHUDFlags & VIEW_VERTEX_VALENCE2 ) )
            {
                DrawVertex( vertex, blctx, fgColor, bgColor, hcColor, iWorld, iHUDFlags );
            }
        }
    }

    blctx->restore();
}

// static
void
FOdysseyVectorHUD::DrawPaintGroup( FOdysseyVectorGroupPaint* iPaintGroup
                                 , const BLRgba32& fgColor
                                 , const BLRgba32& bgColor
                                 , const BLRgba32& hcColor
                                 , bool iWorld
                                 , uint64 iHUDFlags )
{
    std::list<FOdysseyVectorObject*>& childrenObjectList = iPaintGroup->GetChildrenList();
    std::list<FOdysseyVectorObject*>::iterator it;

    for( it = childrenObjectList.begin(); it != childrenObjectList.end(); ++it )
    {
        FOdysseyVectorObject* child = (*it);

        if( child->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(child);

            DrawPath( path, fgColor, bgColor, hcColor, iWorld, iHUDFlags );
        }
    }
}
