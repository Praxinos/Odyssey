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
FOdysseyPainterEditorVectorMatchingToolHUD::DrawChart( BLContext* iBLContext
                                                     , FOdysseyVectorTagInbetweener* iInbetweenerTag )
{
    FInbetweenerChart& chart = iInbetweenerTag->GetChart();
    double cursorRadius = mChartRect.h *.5f;
    double cursorY = mChartRect.y + cursorRadius;

    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setStrokeStyle( BLRgba32( 255, 0, 0, 255 ) );
    iBLContext->setStrokeWidth( 1.0f );

    iBLContext->strokeLine( mChartRect.x               , cursorY
                          , mChartRect.x + mChartRect.w, cursorY );

    for( FInbetweenerInbetween& inbetween : chart.inbetweenBuffer )
    {
        float cursorX = mChartRect.x + ( inbetween.spacing * mChartRect.w );

        iBLContext->strokeLine( cursorX, cursorY - cursorRadius
                              , cursorX, cursorY + cursorRadius );
    }

    iBLContext->setStrokeWidth( 3.0f );
    // initial keypose
    iBLContext->strokeLine( mChartRect.x, cursorY - 20
                          , mChartRect.x, cursorY + 20 );
    // final keypose
    iBLContext->strokeLine( mChartRect.x + mChartRect.w, cursorY - cursorRadius
                          , mChartRect.x + mChartRect.w, cursorY + cursorRadius );

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
                DrawChart( iBLContext, inbetweenerTag );
            }
        }
    }

    iBLContext->save();
    iBLContext->resetMatrix();
}

FInbetweenerInbetween*
FOdysseyPainterEditorVectorMatchingToolHUD::PickInbetween( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                         , double iWorldX
                                                         , double iWorldY
                                                         , double iRadius )
{
    FInbetweenerChart& chart = iInbetweenerTag->GetChart();
    double cursorRadius = mChartRect.h *.5f;
    double cursorY = mChartRect.y + cursorRadius;

    if( mChartRect.HitTest( ::ULIS::FVec2D( iWorldX, iWorldY ) ) )
    {
        for( FInbetweenerInbetween& inbetween : chart.inbetweenBuffer )
        {
            float cursorX = mChartRect.x + ( inbetween.spacing * mChartRect.w );

            if( ( iWorldX >= ( cursorX - iRadius ) )
             && ( iWorldX <= ( cursorX + iRadius ) ) )
            {
                return &inbetween;
            }
        }
    }

    return nullptr;
}

void
FOdysseyPainterEditorVectorMatchingToolHUD::MoveInbetween( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                         , FInbetweenerInbetween* iInbetween
                                                         , double iWorldX
                                                         , double iWorldY
                                                         , bool iRelative )
{
    double newSpacing = ( iWorldX - mChartRect.x ) / mChartRect.w;

    iInbetweenerTag->MoveInbetween( iInbetween, newSpacing, iRelative );
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
