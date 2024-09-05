#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartToolHUD.h"
#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartTool.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorGroupPaint.h"

FOdysseyPainterEditorVectorChartToolHUD::~FOdysseyPainterEditorVectorChartToolHUD()
{
}

FOdysseyPainterEditorVectorChartToolHUD::FOdysseyPainterEditorVectorChartToolHUD( UOdysseyPainterEditorVectorChartTool* iChartTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iChartTool )
    , mChartRect( 200.0f, 40, 400.0f, 40 )
{
    mChartTool = iChartTool;
}

void
FOdysseyPainterEditorVectorChartToolHUD::Reset( FOdysseyVectorGroupPaint* iScene )
{

}

void
FOdysseyPainterEditorVectorChartToolHUD::DrawChart( BLContext* iBLContext
                                                  , FOdysseyVectorTagInbetweener* iInbetweenerTag )
{
    FInbetweenerChart& chart = iInbetweenerTag->GetChart();
    double cursorRadius = mChartRect.h *.5f;
    double cursorY = mChartRect.y + cursorRadius;

    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setStrokeStyle( BLRgba32( 255, 0, 0, 255 ) );
    iBLContext->setStrokeWidth( 1.0f );

    // horizontal line
    iBLContext->strokeLine( mChartRect.x               , cursorY
                          , mChartRect.x + mChartRect.w, cursorY );

    for( FInbetweenerBreakdown* breakdown : iInbetweenerTag->GetBreakdownList() )
    {
        uint32 sourceDrawingIndex = breakdown->GetSourceDrawingIndex();
        uint32 targetDrawingIndex = breakdown->GetTargetDrawingIndex();
        FInbetweenerDrawing* sourceDrawing = iInbetweenerTag->GetDrawing( sourceDrawingIndex );
        FInbetweenerDrawing* targetDrawing = iInbetweenerTag->GetDrawing( targetDrawingIndex );

        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->setStrokeStyle( BLRgba32( 255, 0, 0, 255 ) );

        for( uint32 i = sourceDrawingIndex + 1; i < targetDrawingIndex; i++ )
        {
            FInbetweenerDrawing* drawing = iInbetweenerTag->GetDrawing( i );
            float cursorX = mChartRect.x + ( drawing->spacing * mChartRect.w );

            iBLContext->strokeLine( cursorX, cursorY - cursorRadius
                                  , cursorX, cursorY + cursorRadius );
        }


        iBLContext->setStrokeWidth( 3.0f );
        iBLContext->setStrokeStyle( BLRgba32( 255, 127, 127, 255 ) );

        // initial keypose. Vertical line
        iBLContext->strokeLine( mChartRect.x + ( sourceDrawing->spacing * mChartRect.w ), cursorY - cursorRadius
                              , mChartRect.x + ( sourceDrawing->spacing * mChartRect.w ), cursorY + cursorRadius );

        // final keypose. Vertical line
        iBLContext->strokeLine( mChartRect.x + ( targetDrawing->spacing * mChartRect.w ), cursorY - cursorRadius
                              , mChartRect.x + ( targetDrawing->spacing * mChartRect.w ), cursorY + cursorRadius );
    }

    iBLContext->restore();
}

void
FOdysseyPainterEditorVectorChartToolHUD::Draw( BLContext* iBLContext
                                                , FOdysseyVectorGroupPaint* iScene )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint64 hudFlags = mChartTool->GetEditor()->GetVectorHUDFlags();

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

                DrawChart( iBLContext, inbetweenerTag );
            }
        }
    }

    iBLContext->save();
    iBLContext->resetMatrix();
}

FInbetweenerDrawing*
FOdysseyPainterEditorVectorChartToolHUD::PickInbetween( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                      , double iWorldX
                                                      , double iWorldY
                                                      , double iRadius )
{
    FInbetweenerChart& chart = iInbetweenerTag->GetChart();
    double cursorRadius = mChartRect.h *.5f;
    double cursorY = mChartRect.y + cursorRadius;

    if( mChartRect.HitTest( ::ULIS::FVec2D( iWorldX, iWorldY ) ) )
    {
        for( FInbetweenerBreakdown* breakdown : iInbetweenerTag->GetBreakdownList() )
        {
            uint32 sourceDrawingIndex = breakdown->GetSourceDrawingIndex();
            uint32 targetDrawingIndex = breakdown->GetTargetDrawingIndex();
            FInbetweenerDrawing* sourceDrawing = iInbetweenerTag->GetDrawing( sourceDrawingIndex );
            FInbetweenerDrawing* targetDrawing = iInbetweenerTag->GetDrawing( targetDrawingIndex );

            for( uint32 i = sourceDrawingIndex + 1; i < targetDrawingIndex; i++ )
            {
                FInbetweenerDrawing* drawing = iInbetweenerTag->GetDrawing( i );
                float cursorX = mChartRect.x + ( drawing->spacing * mChartRect.w );

                if( ( iWorldX >= ( cursorX - iRadius ) )
                 && ( iWorldX <= ( cursorX + iRadius ) ) )
                {
                    return drawing;
                }
            }
        }
    }

    return nullptr;
}

void
FOdysseyPainterEditorVectorChartToolHUD::MoveInbetween( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                      , FInbetweenerDrawing* iDrawing
                                                      , double iWorldX
                                                      , double iWorldY
                                                      , bool iRelative )
{
    double newSpacing = ( iWorldX - mChartRect.x ) / mChartRect.w;

    iInbetweenerTag->MoveInbetween( iDrawing, newSpacing, iRelative );
}
