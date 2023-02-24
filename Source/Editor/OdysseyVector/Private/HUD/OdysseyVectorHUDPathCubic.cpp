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
FOdysseyVectorHUDPathCubic::DrawVertex( UOdysseyVectorPathCubic* iPath
                                      , UOdysseyVectorVertexCubic* iCubicVertex
                                      , ::ULIS::FVec2D& iFactor
                                      , ::ULIS::FRectD& iRoi
                                      , uint64 iFlags )
{
    BLContext* blctx = iPath->GetRoot()->GetEngine()->GetBLContext();
    ::ULIS::FVec2D perpendicular = iCubicVertex->GetPerpendicularVector( true );
    double pointRadius = iCubicVertex->GetRadius();
    double ctrlX = ( perpendicular.x * pointRadius );
    double ctrlY = ( perpendicular.y * pointRadius );
    // TODO: compute that once and pass it as parameter for all vertices
    double handleRadiusX = 4.0f * iFactor.x;
    double handleRadiusY = 4.0f * iFactor.y;
    double handleWidth = handleRadiusX * 2.0f;
    double handleHeight = handleRadiusY * 2.0f;

    if ( mDisplayMode & VIEW_POINT )
    {
        blctx->setFillStyle( BLRgba32( 0xFFFF00FF ) );
        blctx->fillRect( iCubicVertex->GetX() - handleRadiusX
                       , iCubicVertex->GetY() - handleRadiusY
                       , handleWidth
                       , handleHeight );
    }

    if ( mDisplayMode & VIEW_HANDLE_POINT )
    {
        // control points
        blctx->setFillStyle( BLRgba32( 0xFF808080 ) );
        blctx->fillRect( iCubicVertex->GetX() + ctrlX - handleRadiusX
                       , iCubicVertex->GetY() + ctrlY - handleRadiusY
                       , handleWidth
                       , handleHeight );

        blctx->fillRect( iCubicVertex->GetX() - ctrlX - handleRadiusX
                       , iCubicVertex->GetY() - ctrlY - handleRadiusY
                       , handleWidth
                       , handleHeight );
    }
}

void
FOdysseyVectorHUDPathCubic::DrawSegment( UOdysseyVectorPathCubic* iPath
                                       , UOdysseyVectorSegmentCubic* iCubicSegment
                                       , ::ULIS::FVec2D& iFactor
                                       , ::ULIS::FRectD& iRoi
                                       , uint64 iFlags )
{
    BLContext* blctx = iPath->GetRoot()->GetEngine()->GetBLContext();
    ::ULIS::FVec2D& point0 = iCubicSegment->GetPoint(0)->GetCoords();
    ::ULIS::FVec2D& point1 = iCubicSegment->GetPoint(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = iCubicSegment->GetControlPoint(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = iCubicSegment->GetControlPoint(1)->GetCoords();
    // TODO: compute that once and pass it as parameter for all segments
    double handleRadiusX = 4.0f * iFactor.x;
    double handleRadiusY = 4.0f * iFactor.y;
    double handleWidth = handleRadiusX * 2.0f;
    double handleHeight = handleRadiusY * 2.0f;

    if ( mDisplayMode & VIEW_PATH )
    {
        BLPath path;

        blctx->setStrokeWidth( iFactor.Distance() );
        blctx->setStrokeStyle( BLRgba32(0xFF00FF00) );

        path.moveTo( point0.x, point0.y );
        path.cubicTo( ctrlPoint0.x
                    , ctrlPoint0.y
                    , ctrlPoint1.x
                    , ctrlPoint1.y
                    , point1.x
                    , point1.y );

       blctx->strokePath( path );
    }

    if ( mDisplayMode & VIEW_HANDLE_SEGMENT )
    {
        BLPath ctrlPath0;
        BLPath ctrlPath1;

        blctx->setStrokeWidth( iFactor.Distance() );
        blctx->setStrokeStyle( BLRgba32(0xFFFF0000) );

        // line to control handle 0
        ctrlPath0.moveTo( point0.x, point0.y );
        ctrlPath0.lineTo( ctrlPoint0.x, ctrlPoint0.y );
        blctx->strokePath( ctrlPath0 );

        // line to control handle 1
        ctrlPath1.moveTo( point1.x, point1.y );
        ctrlPath1.lineTo( ctrlPoint1.x, ctrlPoint1.y );
        blctx->strokePath( ctrlPath1 );

        // control handles
        blctx->setFillStyle( BLRgba32( 0xFFFF0000 ) );
        blctx->fillRect( ctrlPoint0.x - handleRadiusX, ctrlPoint0.y - handleRadiusY, handleWidth, handleHeight );
        blctx->fillRect( ctrlPoint1.x - handleRadiusX, ctrlPoint1.y - handleRadiusY, handleWidth, handleHeight );
    }
}

void
FOdysseyVectorHUDPathCubic::Draw( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = iScene.GetEngine()->GetBLContext();
    UOdysseyVectorObject* selectedObject = iScene.GetLastSelected();

    if( selectedObject )
    {
        if( selectedObject->GetClass() == UOdysseyVectorPathCubic::StaticClass() )
        {
            UOdysseyVectorPathCubic* cubicPath = Cast<UOdysseyVectorPathCubic>(selectedObject);
            BLPoint localVector = cubicPath->GetInverseWorldMatrix().mapVector ( 0.7071f, 0.7071f );
            std::list<UOdysseyVectorSegment*>& segmentList = cubicPath->GetSegmentList();
            std::list<UOdysseyVectorVertex*>& vertexList = cubicPath->GetVertexList();
            ::ULIS::FVec2D factor = { localVector.x, localVector.y };
            /*double handleRadiusX = 6.0f * factor.x;
            double handleRadiusY = 6.0f * factor.y;
            double handleWidth = handleRadiusX * 2.0f;
            double handleHeight = handleRadiusY * 2.0f;*/

            blctx->setMatrix( cubicPath->GetWorldMatrix() );

            for( std::list<UOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
            {
                UOdysseyVectorSegmentCubic* cubicSegment = Cast<UOdysseyVectorSegmentCubic>(*it);

                DrawSegment( cubicPath, cubicSegment, factor, iRoi, iFlags );
            }

            // Points and Point size handles
            for( std::list<UOdysseyVectorVertex*>::iterator it = vertexList.begin(); it != vertexList.end(); ++it )
            {
                UOdysseyVectorVertexCubic *cubicVertex = static_cast<UOdysseyVectorVertexCubic*>(*it);

                DrawVertex( cubicPath, cubicVertex, factor, iRoi, iFlags );
            }
        }
    }
}
