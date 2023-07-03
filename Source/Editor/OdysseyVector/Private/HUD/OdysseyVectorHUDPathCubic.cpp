#include "HUD/OdysseyVectorHUDPathCubic.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorHUDPathCubic::~FOdysseyVectorHUDPathCubic()
{
}

FOdysseyVectorHUDPathCubic::FOdysseyVectorHUDPathCubic()
    : mDisplayMode(0)
{
}

FOdysseyVectorHUDPathCubic::FOdysseyVectorHUDPathCubic( uint32 iDisplayMode )
{
    SetDisplayMode( iDisplayMode );
}

void
FOdysseyVectorHUDPathCubic::SetDisplayMode( uint32 iDisplayMode )
{
    mDisplayMode = iDisplayMode;
}

uint32
FOdysseyVectorHUDPathCubic::GetDisplayMode()
{
    return mDisplayMode;
}

void
FOdysseyVectorHUDPathCubic::DrawLine( BLContext* iBLCtx
                                    , double iWorldx0
                                    , double iWorldY0
                                    , double iWorldx1
                                    , double iWorldY1
                                    , double iOuterWidth
                                    , double iInnerWidth
                                    , const BLRgba32& iOuterColor
                                    , const BLRgba32& iInnerColor )
{
    // Draw 2 lines. We draw a contour and the inside with different colors,
    // that way the background color does not interfere as it can be of only one color.
    // outer
    iBLCtx->setStrokeWidth( iOuterWidth );
    iBLCtx->setStrokeStyle( iOuterColor );
    iBLCtx->strokeLine( iWorldx0, iWorldY0, iWorldx1, iWorldY1 );
    // inner
    iBLCtx->setStrokeWidth( iInnerWidth );
    iBLCtx->setStrokeStyle( iInnerColor );
    iBLCtx->strokeLine( iWorldx0, iWorldY0, iWorldx1, iWorldY1 );
}

void
FOdysseyVectorHUDPathCubic::DrawCircle( BLContext* iBLCtx
                                      , double iWorldx
                                      , double iWorldY
                                      , double iOuterRadius
                                      , double iInnerRadius
                                      , const BLRgba32& iOuterColor
                                      , const BLRgba32& iInnerColor )
{
    // Draw 2 circles. We draw a contour and the inside with different colors,
    // that way the background color does not interfere as it can be of only one color.
    // Here we could also use stroke methods but I believe this is faster. Just a belief, I haven't benchmarked it.
    // outer
    iBLCtx->setFillStyle( iOuterColor );
    iBLCtx->fillCircle( iWorldx, iWorldY, iOuterRadius );
    // inner
    iBLCtx->setFillStyle( iInnerColor );
    iBLCtx->fillCircle( iWorldx, iWorldY, iInnerRadius );
}

void
FOdysseyVectorHUDPathCubic::DrawVertex( FOdysseyVectorPathCubic* iPath
                                      , FOdysseyVectorVertex* iCubicVertex
                                      , uint64 iFlags )
{
    BLContext* blctx = iPath->GetScene()->GetEngine()->GetBLContext();
    ::ULIS::FVec2D perpendicular = iPath->GetPerpendicularVector( iCubicVertex, true );
    double pointRadius = iCubicVertex->GetRadius();
    double ctrlX = ( perpendicular.x * pointRadius );
    double ctrlY = ( perpendicular.y * pointRadius );
    // TODO: compute that once and pass it as parameter for all vertices
    BLPoint worldPoint = iPath->GetWorldMatrix().mapPoint( iCubicVertex->GetX(), iCubicVertex->GetY() );
    BLPoint worldRadius = iPath->GetWorldMatrix().mapVector( ctrlX, ctrlY );
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );

    if ( mDisplayMode & VIEW_POINT )
    {
        DrawCircle( blctx
                  , worldPoint.x
                  , worldPoint.y
                  , VERTEXRADIUSOUTER
                  , VERTEXRADIUSINNER
                  , bgColor
                  , fgColor );
    }

    if ( mDisplayMode & VIEW_HANDLE_POINT )
    {
        DrawCircle( blctx
                  , worldPoint.x + worldRadius.x
                  , worldPoint.y + worldRadius.y
                  , HANDLERADIUSOUTER
                  , HANDLERADIUSINNER
                  , bgColor
                  , BLRgba32( 0x80, 0x80, 0x80, 0xFF ) );

        DrawCircle( blctx
                  , worldPoint.x - worldRadius.x
                  , worldPoint.y - worldRadius.y
                  , HANDLERADIUSOUTER
                  , HANDLERADIUSINNER
                  , bgColor
                  , BLRgba32( 0x80, 0x80, 0x80, 0xFF ) );
    }
}

void
FOdysseyVectorHUDPathCubic::DrawSegment( FOdysseyVectorPathCubic* iPath
                                       , FOdysseyVectorSegmentCubic* iCubicSegment
                                       , uint64 iFlags )
{
    BLContext* blctx = iPath->GetScene()->GetEngine()->GetBLContext();
    BLMatrix2D& worldMatrix = iPath->GetWorldMatrix();
    FOdysseyVectorVertex* vertex0 = iCubicSegment->GetVertex(0);
    FOdysseyVectorVertex* vertex1 = iCubicSegment->GetVertex(1);
    FOdysseyVectorHandleSegment* handle0 = iCubicSegment->GetHandle(0);
    FOdysseyVectorHandleSegment* handle1 = iCubicSegment->GetHandle(1);
    BLPoint point0 = worldMatrix.mapPoint( vertex0->GetX(), vertex0->GetY() );
    BLPoint point1 = worldMatrix.mapPoint( vertex1->GetX(), vertex1->GetY() );
    BLPoint handlePoint0 = worldMatrix.mapPoint( handle0->GetX(), handle0->GetY() );
    BLPoint handlePoint1 = worldMatrix.mapPoint( handle1->GetX(), handle1->GetY() );
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );

    if ( mDisplayMode & VIEW_PATH )
    {
        BLPath path;

        blctx->setStrokeWidth( 1.0f );
        blctx->setStrokeStyle( fgColor );

        path.moveTo( point0 );
        path.cubicTo( handlePoint0
                    , handlePoint1
                    , point1 );

       blctx->strokePath( path );
    }

    if ( mDisplayMode & VIEW_HANDLE_SEGMENT )
    {
        // line to control handle 0
        DrawLine( blctx
                , point0.x
                , point0.y
                , handlePoint0.x
                , handlePoint0.y
                , 2.0f
                , 1.0f
                , BLRgba32( 0x00, 0x00, 0x00, 0xFF )
                , BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF ) );
        // line to control handle 1
        DrawLine( blctx
                , point1.x
                , point1.y
                , handlePoint1.x
                , handlePoint1.y
                , 2.0f
                , 1.0f
                , BLRgba32( 0x00, 0x00, 0x00, 0xFF )
                , BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF ) );
        // control handle 0
        DrawCircle( blctx
                  , handlePoint1.x
                  , handlePoint1.y
                  , HANDLERADIUSOUTER
                  , HANDLERADIUSINNER
                  , BLRgba32( 0x00, 0x00, 0x00, 0xFF )
                  , BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF ) );
        // control handle 1
        DrawCircle( blctx
                  , handlePoint0.x
                  , handlePoint0.y
                  , HANDLERADIUSOUTER
                  , HANDLERADIUSINNER
                  , BLRgba32( 0x00, 0x00, 0x00, 0xFF )
                  , BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF ) );
    }
}

void
FOdysseyVectorHUDPathCubic::Reset(FOdysseyVectorScene* iScene)
{
}

void
FOdysseyVectorHUDPathCubic::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();

    for( std::list<FOdysseyVectorObject*>::iterator oit = selectedObjectList.begin(); oit != selectedObjectList.end(); ++oit )
    {
        FOdysseyVectorObject* selectedObject = *oit;

        if( selectedObject->GetClass() == FOdysseyVectorPathCubic::StaticClass() )
        {
            FOdysseyVectorPathCubic* cubicPath = static_cast<FOdysseyVectorPathCubic*>(selectedObject);
            std::list<FOdysseyVectorSegment*>& segmentList = cubicPath->GetSegmentList();
            std::list<FOdysseyVectorVertex*>& vertexList = cubicPath->GetVertexList();

            // drawn in World coordinates to get sure the size of HUD items is always the same
            blctx->resetMatrix();

            for( std::list<FOdysseyVectorSegment*>::iterator sit = segmentList.begin(); sit != segmentList.end(); ++sit )
            {
                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*sit);

                DrawSegment( cubicPath, cubicSegment, iFlags );
            }

            // Points and Point size handles
            for( std::list<FOdysseyVectorVertex*>::iterator vit = vertexList.begin(); vit != vertexList.end(); ++vit )
            {
                FOdysseyVectorVertex *cubicVertex = static_cast<FOdysseyVectorVertex*>(*vit);

                DrawVertex( cubicPath, cubicVertex, iFlags );
            }
        }
    }

    blctx->restore();
}
