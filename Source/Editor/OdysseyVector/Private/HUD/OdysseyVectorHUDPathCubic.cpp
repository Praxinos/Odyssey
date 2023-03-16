#include "HUD/OdysseyVectorHUDPathCubic.h"
#include "OdysseyVectorEngine.h"

static const ::ULIS::FRectD POINTRECT  = { -4, -4, 8, 8 };
static const ::ULIS::FRectD HANDLERECT = { -4, -4, 8, 8 };

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
FOdysseyVectorHUDPathCubic::DrawVertex( FOdysseyVectorPathCubic* iPath
                                      , FOdysseyVectorVertexCubic* iCubicVertex
                                      , ::ULIS::FRectD& iRoi
                                      , uint64 iFlags )
{
    BLContext* blctx = iPath->GetScene()->GetEngine()->GetBLContext();
    ::ULIS::FVec2D perpendicular = iCubicVertex->GetPerpendicularVector( true );
    double pointRadius = iCubicVertex->GetRadius();
    double ctrlX = ( perpendicular.x * pointRadius );
    double ctrlY = ( perpendicular.y * pointRadius );
    // TODO: compute that once and pass it as parameter for all vertices
    BLPoint worldPoint = iPath->GetWorldMatrix().mapPoint( iCubicVertex->GetX(), iCubicVertex->GetY() );
    BLPoint worldRadius = iPath->GetWorldMatrix().mapVector( ctrlX, ctrlY );

    if ( mDisplayMode & VIEW_POINT )
    {
        blctx->setFillStyle( BLRgba32( 0xFFFF00FF ) );
        blctx->fillRect( worldPoint.x + POINTRECT.x
                       , worldPoint.y + POINTRECT.y
                       , POINTRECT.w
                       , POINTRECT.h );
    }

    if ( mDisplayMode & VIEW_HANDLE_POINT )
    {
        // 2 control points that represent the same handle
        blctx->setFillStyle( BLRgba32( 0xFF808080 ) );
        blctx->fillRect( worldPoint.x + worldRadius.x + HANDLERECT.x
                       , worldPoint.y + worldRadius.y + HANDLERECT.y
                       , HANDLERECT.w
                       , HANDLERECT.h );

        blctx->fillRect( worldPoint.x - worldRadius.x + HANDLERECT.x
                       , worldPoint.y - worldRadius.y + HANDLERECT.y
                       , HANDLERECT.w
                       , HANDLERECT.h );
    }
}

void
FOdysseyVectorHUDPathCubic::DrawSegment( FOdysseyVectorPathCubic* iPath
                                       , FOdysseyVectorSegmentCubic* iCubicSegment
                                       , ::ULIS::FRectD& iRoi
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

    if ( mDisplayMode & VIEW_PATH )
    {
        BLPath path;

        blctx->setStrokeWidth( 1.0f );
        blctx->setStrokeStyle( BLRgba32(0xFF00FF00) );

        path.moveTo( point0 );
        path.cubicTo( handlePoint0
                    , handlePoint1
                    , point1 );

       blctx->strokePath( path );
    }

    if ( mDisplayMode & VIEW_HANDLE_SEGMENT )
    {
        blctx->setStrokeWidth( 1.0f );
        blctx->setStrokeStyle( BLRgba32( 0xFFFF0000 ) );

        // line to control handle 0
        blctx->strokeLine( point0, handlePoint0 );

        // line to control handle 1
        blctx->strokeLine( point1, handlePoint1 );

        // control handles
        blctx->setFillStyle( BLRgba32( 0xFFFF0000 ) );
        blctx->fillRect( handlePoint0.x + HANDLERECT.x, handlePoint0.y + HANDLERECT.y, HANDLERECT.w, HANDLERECT.h );
        blctx->fillRect( handlePoint1.x + HANDLERECT.x, handlePoint1.y + HANDLERECT.y, HANDLERECT.w, HANDLERECT.h );
    }
}

void
FOdysseyVectorHUDPathCubic::Draw( FOdysseyVectorScene& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = iScene.GetEngine()->GetBLContext();
    FOdysseyVectorObject* selectedObject = iScene.GetLastSelected();

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();

    if( selectedObject )
    {
        if( selectedObject->GetClass() == FOdysseyVectorPathCubic::StaticClass() )
        {
            FOdysseyVectorPathCubic* cubicPath = static_cast<FOdysseyVectorPathCubic*>(selectedObject);
            std::list<FOdysseyVectorSegment*>& segmentList = cubicPath->GetSegmentList();
            std::list<FOdysseyVectorVertex*>& vertexList = cubicPath->GetVertexList();

            // drawn in World coordinates to get sure the size of HUD items is always the same
            blctx->resetMatrix();

            for( std::list<FOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
            {
                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);

                DrawSegment( cubicPath, cubicSegment, iRoi, iFlags );
            }

            // Points and Point size handles
            for( std::list<FOdysseyVectorVertex*>::iterator it = vertexList.begin(); it != vertexList.end(); ++it )
            {
                FOdysseyVectorVertexCubic *cubicVertex = static_cast<FOdysseyVectorVertexCubic*>(*it);

                DrawVertex( cubicPath, cubicVertex, iRoi, iFlags );
            }
        }
    }

    blctx->restore();
}
