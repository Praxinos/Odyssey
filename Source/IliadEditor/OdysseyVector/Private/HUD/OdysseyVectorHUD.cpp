// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "HUD/OdysseyVectorHUD.h"
#include "OdysseyVector.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorLayer.h"

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
                              , std::vector<FPointQuadTreeEntry>& iPointQuadTreeEntryArray
                              , uint32 iDepth
                              , uint32 iMaxDepth )
    : mChildren { nullptr, nullptr, nullptr, nullptr }
    , mRect( iRect )
{
    Build( iMaxPointsPerQuad, iPointQuadTreeEntryArray, iDepth, iMaxDepth );
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
FPointQuadTree::Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene, uint64 iFlags )
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
FPointQuadTree::Build( uint32 iMaxPointsPerQuad
                     , std::vector<FPointQuadTreeEntry>& iParentPointQuadTreeEntryArray
                     , uint32 iDepth
                     , uint32 iMaxDepth )
{
    mPointQuadTreeEntryArray.reserve( iParentPointQuadTreeEntryArray.size() );

    for( int i = 0; i < iParentPointQuadTreeEntryArray.size(); i++ )
    {
        if( mRect.HitTest( iParentPointQuadTreeEntryArray[i].worldCoords ) )
        {
            mPointQuadTreeEntryArray.push_back( iParentPointQuadTreeEntryArray[i] );
        }
    }

    if( ( mPointQuadTreeEntryArray.size() > iMaxPointsPerQuad ) && ( iDepth < iMaxDepth ) )
    {
        uint32 minX =   mRect.x;
        uint32 minY =   mRect.y;
        uint32 maxX = ( mRect.x +   mRect.w );
        uint32 maxY = ( mRect.y +   mRect.h );
        uint32 avgX =   mRect.x + ( mRect.w * 0.5f );
        uint32 avgY =   mRect.y + ( mRect.h * 0.5f );

        mChildren[0] = new FPointQuadTree( ::ULIS::FRectD::FromMinMax( minX, minY, avgX, avgY ), iMaxPointsPerQuad, mPointQuadTreeEntryArray, iDepth + 1, iMaxDepth );
        mChildren[1] = new FPointQuadTree( ::ULIS::FRectD::FromMinMax( avgX, minY, maxX, avgY ), iMaxPointsPerQuad, mPointQuadTreeEntryArray, iDepth + 1, iMaxDepth );
        mChildren[2] = new FPointQuadTree( ::ULIS::FRectD::FromMinMax( avgX, avgY, maxX, maxY ), iMaxPointsPerQuad, mPointQuadTreeEntryArray, iDepth + 1, iMaxDepth );
        mChildren[3] = new FPointQuadTree( ::ULIS::FRectD::FromMinMax( minX, avgY, avgX, maxY ), iMaxPointsPerQuad, mPointQuadTreeEntryArray, iDepth + 1, iMaxDepth );

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
    for( FOdysseyVectorVertex* vertex : iPath->GetVertexList() )
    {
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
    if( iObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>( iObject );

        MapPath( path, iRect, oPointQuadTreeEntryArray );
    }
}

void
FOdysseyVectorHUD::MakePointQuadTree( FOdysseyVectorGroupPaint *iScene
                                    , bool iFocusedObjectsOnly
                                    , uint64 iHUDFlags )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FPointQuadTreeEntry> pointQuadTreeEntryArray;
    uint32 width = vectorEngine->GetLayer()->GetWidth();
    uint32 height = vectorEngine->GetLayer()->GetHeight();
    ::ULIS::FRectD screenRect;

    screenRect = ::ULIS::FRectD::FromXYWH( 0, 0, width, height );

    pointQuadTreeEntryArray.reserve( 200 );

    vectorEngine->Traverse( iScene
                          , iHUDFlags
                          , [ vectorEngine
                          ,   iScene
                          ,   iFocusedObjectsOnly
                          ,   &screenRect
                          ,   &pointQuadTreeEntryArray ]( FOdysseyVectorObject* object, uint64 traverseFlags ) -> uint64
                            {
                                if( ( iFocusedObjectsOnly == false ) || vectorEngine->ObjectHasFocus( iScene, object, traverseFlags ) )
                                {
                                    MapPoints( object, screenRect, pointQuadTreeEntryArray );

                                    return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
                                }

                                return 0;
                            } );

    if( mPointQuadTree )
    {
        delete mPointQuadTree;
    }

    mPointQuadTree = new FPointQuadTree( screenRect, 20, pointQuadTreeEntryArray, 0, 8 );
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
    //static FColor fg = FColor( 0x40, 0xE0, 0xD0, 0xFF ); // teal
    static FColor fg = FColor( 0, 169, 157, 255 ); // Odyssey's teal

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
FOdysseyVectorHUD::DrawCenteredSquare( BLContext* iBLContext
                                     , double iWorldx
                                     , double iWorldY
                                     , double iRadius
                                     , const BLRgba32& fgColor
                                     , const BLRgba32& bgColor )
{
    double width = iRadius * 2;
    BLRect rect( iWorldx - iRadius, iWorldY - iRadius, width, width );

    // inner
    iBLContext->setFillStyle( fgColor );
    iBLContext->fillRect( rect );
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->setStrokeStyle( bgColor );
    iBLContext->strokeRect( rect );
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
    FOdysseyVectorPath* path = iVertex->GetOwnerAsPath();
    BLMatrix2D& HUDMatrix = iWorld ? path->GetWorldMatrix() : path->GetLocalMatrix();
    // TODO: compute that once and pass it as parameter for all vertices
    BLPoint point = HUDMatrix.mapPoint( iVertex->GetX(), iVertex->GetY() );
    static BLRgba32 greenColor  = BLRgba32(   0, 255,   0, 255 );
    static BLRgba32 ltgrayColor = BLRgba32( 128, 128, 128, 255 );
    static BLRgba32 dkgrayColor = BLRgba32(  64,  64,  64, 255 );
    static BLRgba32 redColor    = BLRgba32( 255,   0,   0, 255 );
    double vertexRadius = ( iHUDFlags & HUD_SIZE_SMALL ) ? VERTEXRADIUS_SMALL : VERTEXRADIUS;
    double handleRadius = ( iHUDFlags & HUD_SIZE_SMALL ) ? HANDLERADIUS_SMALL : HANDLERADIUS;

    if ( iHUDFlags & HUD_PATH_VERTEX_HANDLE )
    {
        static BLRgba32 whiteColor = BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF );
        static BLRgba32 blackColor = BLRgba32( 0x00, 0x00, 0x00, 0xFF );
        ::ULIS::FVec2D localHandleCoords[2];
        BLPoint worldHandleCoords[2];

        iVertex->GetHandlePosition( localHandleCoords );

        worldHandleCoords[0] = HUDMatrix.mapPoint( localHandleCoords[0].x, localHandleCoords[0].y );
        worldHandleCoords[1] = HUDMatrix.mapPoint( localHandleCoords[1].x, localHandleCoords[1].y );

        // Line to handle
        iBLContext->setStrokeWidth( 2.0f );
        iBLContext->setStrokeStyle( bgColor );
        iBLContext->strokeLine( point.x, point.y, worldHandleCoords[0].x, worldHandleCoords[0].y );

        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->setStrokeStyle( whiteColor );
        iBLContext->strokeLine( point.x, point.y, worldHandleCoords[0].x, worldHandleCoords[0].y );

        // handle
        FOdysseyVectorHUD::DrawCircle( iBLContext
                                     , worldHandleCoords[0].x
                                     , worldHandleCoords[0].y
                                     , handleRadius
                                     , whiteColor
                                     , blackColor );

        // Line to handle
        iBLContext->setStrokeWidth( 2.0f );
        iBLContext->setStrokeStyle( bgColor );
        iBLContext->strokeLine( point.x, point.y, worldHandleCoords[1].x, worldHandleCoords[1].y );

        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->setStrokeStyle( whiteColor );
        iBLContext->strokeLine( point.x, point.y, worldHandleCoords[1].x, worldHandleCoords[1].y );

        // handle
        FOdysseyVectorHUD::DrawCircle( iBLContext
                                     , worldHandleCoords[1].x
                                     , worldHandleCoords[1].y
                                     , handleRadius
                                     , whiteColor
                                     , blackColor );
    }

    if( iVertex->IsLocked() == false )
    {
        FOdysseyVectorHUD::DrawCircle( iBLContext
                                     , point.x
                                     , point.y
                                     , vertexRadius
                                     , iVertex->IsSelected() && ( iHUDFlags & HUD_MODE_VERTEX ) ? hcColor  : fgColor
                                     , bgColor );
    }
    else
    {
        FOdysseyVectorHUD::DrawCircle( iBLContext
                                     , point.x
                                     , point.y
                                     , vertexRadius
                                     , iVertex->IsSelected() && ( iHUDFlags & HUD_MODE_VERTEX ) ? hcColor  : ltgrayColor
                                     , dkgrayColor );
    }
/*
    if ( iHUDFlags & HUD_PATH_VERTEX_ALIGNMENT )
    {
        if( iVertex->IsHandleAligned() )
        {
            iBLContext->setStrokeWidth( 1.0f );
            iBLContext->setStrokeStyle( greenColor );
            iBLContext->strokeCircle( point.x, point.y, vertexRadius + 2 );
        }
    }
*/
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
    FOdysseyVectorObject* owner = iCubicSegment->GetOwner();
    BLMatrix2D& HUDMatrix = iWorld ? owner->GetWorldMatrix() : owner->GetLocalMatrix();
    FOdysseyVectorVertex* vertex[2] = { iCubicSegment->GetVertex(0)
                                      , iCubicSegment->GetVertex(1) };
    FOdysseyVectorHandleSegment* handle[2] = { iCubicSegment->GetHandle(0)
                                             , iCubicSegment->GetHandle(1) };
    BLPoint point[2] = { HUDMatrix.mapPoint( vertex[0]->GetX(), vertex[0]->GetY() )
                       , HUDMatrix.mapPoint( vertex[1]->GetX(), vertex[1]->GetY() ) };
    BLPoint handlePoint[2] = { HUDMatrix.mapPoint( handle[0]->GetX(), handle[0]->GetY() )
                             , HUDMatrix.mapPoint( handle[1]->GetX(), handle[1]->GetY() ) };
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

    if ( iHUDFlags & HUD_PATH_SEGMENT_HANDLE )
    {
        static BLRgba32 whiteColor = BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF );
        static BLRgba32 blackColor = BLRgba32( 0x00, 0x00, 0x00, 0xFF );
        static BLRgba32 greenColor = BLRgba32( 0x00, 0xFF, 0x00, 0xFF );

        for( int i = 0; i < 2; i++ )
        {
            FOdysseyVectorHUD::DrawLine( iBLContext
                                       , point[i].x
                                       , point[i].y
                                       , handlePoint[i].x
                                       , handlePoint[i].y
                                       , vertex[i]->IsHandleAligned() ? greenColor : whiteColor
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

/*
void
FOdysseyVectorHUD::DrawTargetGrid( BLContext* iBLContext
                                 , FInbetweenerGrid* iGrid )
{
    FOdysseyVectorTagInbetweener* inbetweenerTag = iGrid->GetBreakdown()->GetInbetweenerTag();

    DrawGrid( iBLContext
            , eInbetweenerPointPositionType::TargetPosition
            , inbetweenerTag->GetGridColor() );
}

void
FOdysseyVectorHUD::DrawSourceGrid( BLContext* iBLContext )
{
    DrawGrid( iBLContext
            , eInbetweenerPointPositionType::SourcePosition
            , FColor( 127, 127, 127, 127 ) );
}
*/

void
FOdysseyVectorHUD::DrawGrid( BLContext* iBLContext
                           , FInbetweenerGrid* iGrid
                           , eInbetweenerPointPositionType iPositionType
                           , const FColor& iColor
                           , uint64 iHUDFlags )
{
    FOdysseyVectorTagInbetweener* inbetweenerTag = iGrid->GetBreakdown()->GetInbetweenerTag();
    BLMatrix2D worldMatrix = inbetweenerTag->GetOwner()->GetWorldMatrix();
    BLRgba32 gridColor = BLRgba32( iColor.R
                                 , iColor.G
                                 , iColor.B
                                 , iColor.A );
    std::vector<FInbetweenerPoint>& pointBuffer = iGrid->GetPointBuffer();
    std::vector<FInbetweenerQuad>& quadBuffer = iGrid->GetQuadBuffer();

    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setFillStyle( gridColor );
    iBLContext->setStrokeStyle( gridColor );
    iBLContext->setStrokeWidth( 1.0f );

    if( iPositionType == eInbetweenerPointPositionType::TargetPosition )
    {
        worldMatrix.transform( iGrid->GetBreakdown()->GetTargetLocalMatrix() );
    }

    if( iHUDFlags & HUD_BREAKDOWN_GRID_DOTTED )
    {
        for( uint32 pointIndex : inbetweenerTag->GetUsedPointIndexBuffer() )
        {
            FInbetweenerPoint& point = pointBuffer[pointIndex];
            ::ULIS::FVec2D position = point.GetPosition( iPositionType );
            BLPoint pt = worldMatrix.mapPoint( position.x, position.y );

            iBLContext->fillCircle( pt.x, pt.y, 2 );
        }
    }
    else
    {
        for( uint32 quadIndex : inbetweenerTag->GetUsedQuadIndexBuffer() )
        {
            FInbetweenerQuad& quad = quadBuffer[quadIndex];
            ::ULIS::FVec2D position[4] = { quad.GetPoints()[0]->GetPosition( iPositionType )
                                         , quad.GetPoints()[1]->GetPosition( iPositionType )
                                         , quad.GetPoints()[2]->GetPosition( iPositionType )
                                         , quad.GetPoints()[3]->GetPosition( iPositionType ) };
            BLPoint pt[4] = { worldMatrix.mapPoint( position[0].x, position[0].y )
                            , worldMatrix.mapPoint( position[1].x, position[1].y )
                            , worldMatrix.mapPoint( position[2].x, position[2].y )
                            , worldMatrix.mapPoint( position[3].x, position[3].y ) };

            iBLContext->strokeLine( pt[0].x, pt[0].y, pt[1].x, pt[1].y );
            iBLContext->strokeLine( pt[1].x, pt[1].y, pt[2].x, pt[2].y );
            iBLContext->strokeLine( pt[2].x, pt[2].y, pt[3].x, pt[3].y );
            iBLContext->strokeLine( pt[3].x, pt[3].y, pt[0].x, pt[0].y );
        }
    }

    iBLContext->restore();
}

// static
void
FOdysseyVectorHUD::DrawBreakdown( FOdysseyVectorGroupPaint* iDisplayedScene
                                , BLContext* iBLContext
                                , FInbetweenerBreakdown* iBreakdown
                                , const BLRgba32& iSourceDrawingColor
                                , const BLRgba32& iTargetDrawingColor
                                , uint64 iHUDFlags )
{
    FOdysseyVectorTagInbetweener* inbetweenerTag = iBreakdown->GetInbetweenerTag();
    BLMatrix2D worldMatrix = inbetweenerTag->GetOwner()->GetWorldMatrix();
    FColor inbetweenColor = inbetweenerTag->GetInbetweenColor();

    iBLContext->save();
    iBLContext->resetMatrix();

    if( iHUDFlags & HUD_BREAKDOWN_INBETWEEN )
    {
        iBLContext->setStrokeWidth( 2.0f );
        int32 sourceDrawingIndex = iBreakdown->GetSourceDrawingIndex();
        int32 targetDrawingIndex = iBreakdown->GetTargetDrawingIndex();
        double sourceDrawingSpacing = (double) sourceDrawingIndex / inbetweenerTag->GetDrawingBuffer().size();
        double targetDrawingSpacing = (double) targetDrawingIndex / inbetweenerTag->GetDrawingBuffer().size();

        for( uint32 i = 1; i < iBreakdown->GetDrawingCount() - 1; i++ )
        {
            FInbetweenerChart::Inbetween* inbetween = &iBreakdown->GetChart()->GetInbetweenBuffer()[i];
            uint32 drawingIndex = inbetween->GetDrawing()->GetIndex();
            double opacity = ( double ) inbetweenColor.A / 255;
            double spacing = ( iHUDFlags & HUD_INBETWEEN_FADERELATIVE ) ? inbetween->GetSpacing()
                                                                        : sourceDrawingSpacing + ( ( targetDrawingSpacing - sourceDrawingSpacing ) * inbetween->GetSpacing() );
            uint8 alpha = 255;

            //if( iHUDFlags & HUD_INBETWEEN_FADEFROMTARGET )
                alpha = (       ( spacing * 255 ) ) * opacity;

            //if( iHUDFlags & HUD_INBETWEEN_FADEFROMSOURCE )
            //    alpha = ( 255 - ( spacing * 255 ) ) * opacity;

            iBLContext->setStrokeWidth( 4.0f );
            iBLContext->setStrokeStyle( BLRgba32( inbetweenColor.R
                                                , inbetweenColor.G
                                                , inbetweenColor.B
                                                , 0.25f + ( 0.75f * alpha ) ) ); // minimum alpha is 0.25f

            for( FInterpolatedPath& interpolatedPath : inbetweenerTag->GetInterpolatedPathBuffer() )
            {
                DrawInbetweenerInterpolatedPathAt( iDisplayedScene
                                                  , iBLContext
                                                  , inbetweenerTag
                                                  , &interpolatedPath
                                                  , inbetween );
            }
        }
    }

    if( iHUDFlags & HUD_BREAKDOWN_SOURCE )
    {
        iBLContext->setStrokeWidth( 4.0f );
        iBLContext->setStrokeStyle( iSourceDrawingColor );

        for( FInterpolatedPath& interpolatedPath : inbetweenerTag->GetInterpolatedPathBuffer() )
        {
            DrawInbetweenerInterpolatedPathAt( iDisplayedScene
                                             , iBLContext
                                             , inbetweenerTag
                                             , &interpolatedPath
                                             , &iBreakdown->GetChart()->GetInbetweenBuffer().front() );
        }
    }

    if( iHUDFlags & HUD_BREAKDOWN_TARGET )
    {
        iBLContext->setStrokeWidth( 4.0f );
        iBLContext->setStrokeStyle( iTargetDrawingColor );

        for( FInterpolatedPath& interpolatedPath : inbetweenerTag->GetInterpolatedPathBuffer() )
        {
            DrawInbetweenerInterpolatedPathAt( iDisplayedScene
                                             , iBLContext
                                             , inbetweenerTag
                                             , &interpolatedPath
                                             , &iBreakdown->GetChart()->GetInbetweenBuffer().back() );
        }
    }

    iBLContext->restore();

    if( iHUDFlags & HUD_BREAKDOWN_SOURCE_GRID )
    {
        DrawGrid( iBLContext
                , iBreakdown->GetGrid()
                , eInbetweenerPointPositionType::SourcePosition
                , inbetweenerTag->GetGridColor()
                , iHUDFlags );
    }

    if( iHUDFlags & HUD_BREAKDOWN_TARGET_GRID )
    {
        DrawGrid( iBLContext
                , iBreakdown->GetGrid()
                , eInbetweenerPointPositionType::TargetPosition
                , inbetweenerTag->GetGridColor()
                , iHUDFlags );
    }
}

void
FOdysseyVectorHUD::DrawInbetweenerInterpolatedPathAt( FOdysseyVectorGroupPaint* iDisplayedScene
                                                    , BLContext* iBLContext
                                                    , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                    , FInterpolatedPath* iInterpolatedPath
                                                    , FInbetweenerChart::Inbetween* iInbetween )
{
    uint32 pathPointCount = iInterpolatedPath->GetInterpolatedPointBuffer().size();
    uint32 inbetweenAbsoluteIndex = iInbetween->GetIndexInInbetweener();
    FInterpolatedPath::PointGeometry* interpolatedPointGeometryBuffer = &iInterpolatedPath->GetInterpolatedPointGeometryBuffer()[pathPointCount * inbetweenAbsoluteIndex];
    BLMatrix2D worldMatrix = iInbetweenerTag->GetOwner()->GetWorldMatrix();
    bool mapAsPolyline = iInbetweenerTag->GetMapAsPolyline();
    FOdysseyVectorPath* originalPath = iInterpolatedPath->GetOriginalPath();

    // passed to DrawPathAt()
    worldMatrix.transform( iInbetween->GetDrawing()->localMatrix );
    worldMatrix.transform( iInterpolatedPath->GetRelativeMatrix() );
/*
    iBLContext->setStrokeWidth( 4.0f );
    iBLContext->setStrokeStyle( BLRgba32( inbetweeneColor.R
                                        , inbetweeneColor.G
                                        , inbetweeneColor.B
                                        , inbetweeneColor.A ) );
*/
    for( FInterpolatedSegment& interpolatedSegment : iInterpolatedPath->GetInterpolatedSegmentBuffer() )
    {
        std::vector<FInterpolatedPoint*>& interpolatedPointArray = interpolatedSegment.GetInterpolatedPointArray();
        FOdysseyVectorSegment* segment = interpolatedSegment.GetOriginalSegment();

        if( mapAsPolyline )
        {
            std::vector<FOdysseyVectorFraction>& fractionCache = segment->GetFractionCache();

            uint32 segmentPointCount = interpolatedPointArray.size();
            ::ULIS::FVec2D perpi;
            ::ULIS::FVec2D perpn;

            for( uint32 i = 0; i < segmentPointCount - 1; i++ )
            {
                uint32 n = i + 1;
                FInterpolatedPoint* pointi = interpolatedPointArray[i];
                FInterpolatedPoint* pointn = interpolatedPointArray[n];
                ::ULIS::FVec2D* localPointPositioni = &interpolatedPointGeometryBuffer[pointi->GetIndex()].position;
                ::ULIS::FVec2D* localPointPositionn = &interpolatedPointGeometryBuffer[pointn->GetIndex()].position;

                BLPoint pt[2] = { worldMatrix.mapPoint( localPointPositioni->x
                                                      , localPointPositioni->y )
                                , worldMatrix.mapPoint( localPointPositionn->x
                                                      , localPointPositionn->y ) };

                iBLContext->strokeLine( pt[0].x, pt[0].y, pt[1].x, pt[1].y );
            }
        }
        else
        {
            if( interpolatedSegment.GetOriginalSegment()->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FInterpolatedPoint* interpolatedPoint[4] = { interpolatedPointArray[0]
                                                           , interpolatedPointArray[1]
                                                           , interpolatedPointArray[2]
                                                           , interpolatedPointArray[3] };
                ::ULIS::FVec2D pt[4] = { FOdysseyVector::MapPoint( worldMatrix, interpolatedPointGeometryBuffer[interpolatedPoint[0]->GetIndex()].position )
                                       , FOdysseyVector::MapPoint( worldMatrix, interpolatedPointGeometryBuffer[interpolatedPoint[1]->GetIndex()].position )
                                       , FOdysseyVector::MapPoint( worldMatrix, interpolatedPointGeometryBuffer[interpolatedPoint[2]->GetIndex()].position )
                                       , FOdysseyVector::MapPoint( worldMatrix, interpolatedPointGeometryBuffer[interpolatedPoint[3]->GetIndex()].position ) };
                BLPath path;

                path.moveTo ( pt[0].x, pt[0].y );
                path.cubicTo( pt[1].x, pt[1].y
                            , pt[2].x, pt[2].y
                            , pt[3].x, pt[3].y );

                iBLContext->strokePath( path );
            }
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

    if( iHUDFlags & HUD_PATH_SEGMENT )
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

    if( iHUDFlags & HUD_PATH_VERTEX )
    {
        // Points and Point size handles
        for( FOdysseyVectorVertex* vertex : vertexList )
        {
            uint32 valence = vertex->GetSegmentCount();

            if( ( valence == 0 ) && ( iHUDFlags & HUD_PATH_VERTEX_VALENCE0 ) )
            {
                DrawVertex( iBLContext, vertex, fgColor, bgColor, hcColor, iWorld, iHUDFlags );
            }
            else
            if( ( valence == 1 ) && ( iHUDFlags & HUD_PATH_VERTEX_VALENCE1 ) )
            {
                DrawVertex( iBLContext, vertex, fgColor, bgColor, hcColor, iWorld, iHUDFlags );
            }
            else
            if( ( valence == 2 ) && ( iHUDFlags & HUD_PATH_VERTEX_VALENCE2 ) )
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
    BLRgba32 propColor = iBucket->IsPropagated() ? BLRgba32( 0x00, 0xFF, 0x00, 0xFF ) : fgColor;
    static BLRgba32 blackColor = BLRgba32( 0x00, 0x00, 0x00, 0xFF );
    static BLRgba32 whiteColor = BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF );

    if( iHUDFlags & HUD_GROUPPAINT_BUCKET_HANDLE )
    {
        if( iBucket->GetColorMode() == eBucketColorMode::LinearGradient )
        {
            ::ULIS::FVec2D handleWorldCoords = bucketWorldCoords + ( GetBucketHandleVector( iBucket, true ) * HANDLE_DISTANCE );

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
    iBLContext->setStrokeStyle( blackColor );
    iBLContext->strokeCircle( bucketWorldCoords.x, bucketWorldCoords.y, PELLET_RADIUS );

    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->setStrokeStyle( propColor ); // green if propagated, white otherwise
    iBLContext->strokeCircle( bucketWorldCoords.x, bucketWorldCoords.y, PELLET_RADIUS );

    if( iBucket->IsPropagated() )
    {
        iBLContext->setStrokeWidth( 3.0f );
        iBLContext->setStrokeStyle( blackColor );
        iBLContext->strokeArc( bucketWorldCoords.x
                             , bucketWorldCoords.y
                             , PELLET_RADIUS + 3.0f
                             , 0.0f
                             , 1.0472f );
        iBLContext->strokeArc( bucketWorldCoords.x
                             , bucketWorldCoords.y
                             , PELLET_RADIUS + 3.0f
                             , 2.0944f
                             , 1.0472f );
        iBLContext->strokeArc( bucketWorldCoords.x
                             , bucketWorldCoords.y
                             , PELLET_RADIUS + 3.0f
                             , 4.1888f
                             , 1.0472f );

        iBLContext->setStrokeWidth( 2.0f );
        iBLContext->setStrokeStyle( propColor ); // green if propagated, fg otherwise
        iBLContext->strokeArc( bucketWorldCoords.x
                             , bucketWorldCoords.y
                             , PELLET_RADIUS + 3.0f
                             , 0.0f
                             , 1.0472f );
        iBLContext->strokeArc( bucketWorldCoords.x
                             , bucketWorldCoords.y
                             , PELLET_RADIUS + 3.0f
                             , 2.0944f
                             , 1.0472f );
        iBLContext->strokeArc( bucketWorldCoords.x
                             , bucketWorldCoords.y
                             , PELLET_RADIUS + 3.0f
                             , 4.1888f
                             , 1.0472f );
    }
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

    if( iHUDFlags & HUD_GROUPPAINT_BUCKET )
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
bool
FOdysseyVectorHUD::IsPaintedPath( FOdysseyVectorObject* iObject, bool iHasParentSelected )
{
    if( iObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iObject);
        FOdysseyVectorObject* parent = path->GetParent();

        if( parent->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(parent);

            if( iHasParentSelected )
            {
                return paintGroup->IsSelected() ? true : false;
            }

            return true;
        }
    }

    return false;
}
