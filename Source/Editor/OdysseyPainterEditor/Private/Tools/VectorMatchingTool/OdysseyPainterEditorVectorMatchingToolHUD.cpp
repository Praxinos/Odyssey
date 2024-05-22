#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingToolHUD.h"
#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingTool.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorMatchingToolHUD::~FOdysseyPainterEditorVectorMatchingToolHUD()
{
}

FOdysseyPainterEditorVectorMatchingToolHUD::FOdysseyPainterEditorVectorMatchingToolHUD( UOdysseyPainterEditorVectorMatchingTool* iMatchingTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iMatchingTool )
    , mChartRect( 200.0f, 40, 400.0f, 40 )
{
    mMatchingTool = iMatchingTool;
}

void
FOdysseyPainterEditorVectorMatchingToolHUD::Reset( FOdysseyVectorGroupPaint* iScene )
{

}

void
FOdysseyPainterEditorVectorMatchingToolHUD::DrawGrid( BLContext* iBLContext
                                                    , FOdysseyVectorTagInbetweener* iInbetweenerTag )
{
    BLMatrix2D worldMatrix = iInbetweenerTag->GetOwner()->GetWorldMatrix();

    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setStrokeStyle( BLRgba32( 255, 0, 255, 255 ) );
    iBLContext->setStrokeWidth( 1.0f );

    for( FInbetweenerCell& cell : iInbetweenerTag->GetGridCellBuffer() )
    {
        BLPoint pt[4] = { worldMatrix.mapPoint( cell.point[0]->targetPosition.x
                                              , cell.point[0]->targetPosition.y )
                        , worldMatrix.mapPoint( cell.point[1]->targetPosition.x
                                              , cell.point[1]->targetPosition.y )
                        , worldMatrix.mapPoint( cell.point[2]->targetPosition.x
                                              , cell.point[2]->targetPosition.y )
                        , worldMatrix.mapPoint( cell.point[3]->targetPosition.x
                                              , cell.point[3]->targetPosition.y ) };

        iBLContext->strokeLine( pt[0], pt[1] );
        iBLContext->strokeLine( pt[1], pt[2] );
        iBLContext->strokeLine( pt[2], pt[3] );
        iBLContext->strokeLine( pt[3], pt[0] );
    }

    iBLContext->restore();
}

void
FOdysseyPainterEditorVectorMatchingToolHUD::Draw( BLContext* iBLContext
                                                , FOdysseyVectorGroupPaint* iScene )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint64 hudFlags = mMatchingTool->GetEditor()->GetVectorHUDFlags();

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    FOdysseyPainterEditorVectorBaseToolHUD::Draw( iBLContext, iScene );

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        FOdysseyVectorObject* selectedObject = iScene->GetEngine()->GetLastSelectedObject();

        if( selectedObject )
        {
            FOdysseyVectorTag* tag = selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

            if( tag )
            {
                FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>( tag );

                DrawGrid ( iBLContext, inbetweenerTag );
            }
        }
    }

    iBLContext->save();
    iBLContext->resetMatrix();
}

void
FOdysseyPainterEditorVectorMatchingToolHUD::PickTargetPoints( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                            , double iWorldX
                                                            , double iWorldY
                                                            , double iRadius
                                                            , std::vector<FInbetweenerPoint*>& oPointArray
                                                            , std::vector<double>& oWorldDistanceArray )
{
    BLMatrix2D worldMatrix = iInbetweenerTag->GetOwner()->GetWorldMatrix();

    for( FInbetweenerPoint& point : iInbetweenerTag->GetGridPointBuffer() )
    {
        BLPoint pt = worldMatrix.mapPoint( point.targetPosition.x, point.targetPosition.y );
        ::ULIS::FVec2D vec = ::ULIS::FVec2D( pt.x - iWorldX, pt.y - iWorldY );
        double distance = vec.Distance();

        if( distance <= iRadius )
        {
            oPointArray.push_back( &point );
            oWorldDistanceArray.push_back( distance );
        }
    }
}
