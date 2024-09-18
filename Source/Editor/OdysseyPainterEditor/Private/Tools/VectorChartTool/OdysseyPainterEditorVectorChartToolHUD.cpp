#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartToolHUD.h"
#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartTool.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorAnimationCell.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyPainterEditorVectorChartToolHUD::~FOdysseyPainterEditorVectorChartToolHUD()
{
}

FOdysseyPainterEditorVectorChartToolHUD::FOdysseyPainterEditorVectorChartToolHUD( UOdysseyPainterEditorVectorChartTool* iChartTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iChartTool )
    , mChartRect( 200.0f, 40, 400.0f, 40 )
    , mChartTool( iChartTool )
{
    BLFontFace face;
   // TODO: do something depending on to the O.S
    BLResult err = face.createFromFile("C:/Windows/Fonts/lucon.ttf"); // Lucida console
 
    mFont.createFromFace( face, 16.0f );
}

void
FOdysseyPainterEditorVectorChartToolHUD::Reset( FOdysseyVectorGroupPaint* iScene )
{
    uint64 hudFlags = mChartTool->GetEditor()->GetVectorHUDFlags();

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        UpdateBreakdown( iScene );
    }
}

FInbetweenerBreakdown*
FOdysseyPainterEditorVectorChartToolHUD::GetBreakdown()
{
    return mBreakdown;
}

void
FOdysseyPainterEditorVectorChartToolHUD::UpdateBreakdown( FOdysseyVectorGroupPaint* iScene )
{
    uint32 cellIndex = iScene->GetEngine()->GetAnimationCell()->GetIndex();

    mBreakdown = nullptr;

    for( FOdysseyVectorTag* tag : iScene->GetSharedEnv()->GetSharedTagList() )
    {
        if( tag->GetOwner()->IsSelected() )
        {
            if( tag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass() )
            {
                FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>( tag );

                for( FInbetweenerBreakdown* breakdown : inbetweenerTag->GetBreakdownList() )
                {
                    uint32 sourceCellIndex = breakdown->GetSourceAnimationCellIndex();
                    uint32 targetCellIndex = breakdown->GetTargetAnimationCellIndex();

                    if( ( cellIndex >= sourceCellIndex ) && ( cellIndex <= targetCellIndex ) )
                    {
                        mBreakdown = breakdown;

                        return;
                    }
                }
            }
        }
    }
}

void
FOdysseyPainterEditorVectorChartToolHUD::DrawChart( BLContext* iBLContext
                                                  , BLRgba32& iFgColor
                                                  , BLRgba32& iBgColor
                                                  , BLRgba32& iHcColor
                                                  , FInbetweenerBreakdown* iBreakdown )
{
    FOdysseyVectorTagInbetweener* inbetweenerTag = iBreakdown->GetInbetweenerTag();
    double cursorRadius = mChartRect.h *.5f;
    float indicatorY = mChartRect.y + cursorRadius;
    float fontSize = mFont.size();
    const FColor& GetColor();
    BLRgba32 tagColor = BLRgba32( inbetweenerTag->GetColor().R
                                , inbetweenerTag->GetColor().G
                                , inbetweenerTag->GetColor().B
                                , 255 );
    BLRgba32 blackColor = BLRgba32( 0, 0, 0, 255 );
    uint32 sourceDrawingIndex = iBreakdown->GetSourceDrawingIndex();
    uint32 targetDrawingIndex = iBreakdown->GetTargetDrawingIndex();
    FInbetweenerDrawing* sourceDrawing = inbetweenerTag->GetDrawing( sourceDrawingIndex );
    FInbetweenerDrawing* targetDrawing = inbetweenerTag->GetDrawing( targetDrawingIndex );

    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setCompOp( BL_COMP_OP_SRC_OVER  );
    iBLContext->setStrokeStyle( tagColor );
    iBLContext->setStrokeWidth( 1.0f );

    // horizontal line
    iBLContext->strokeLine( mChartRect.x               , indicatorY
                          , mChartRect.x + mChartRect.w, indicatorY );

    // vertical lines
    for( uint32 i = 1, n = 0; i < iBreakdown->GetDrawingCount() - 1; i++, n++ )
    {
        FChartInbetween* inbetween = &iBreakdown->GetChart()->GetInbetweenArray()[i];
        float indicatorX = mChartRect.x + ( inbetween->spacing * mChartRect.w );
        float frameNumberX = indicatorX - ( fontSize * 0.25f );
        float frameNumberY = indicatorY + cursorRadius + fontSize + 2.0f;
        char frameNumber[6];

        // draw indicator
        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->setStrokeStyle( tagColor );
        iBLContext->strokeLine( indicatorX, indicatorY - cursorRadius
                                , indicatorX, indicatorY + cursorRadius );

        // draw inbetween number
        snprintf( frameNumber, 6, "%d", n + 1 );

        iBLContext->setStrokeStyle( blackColor );
        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->strokeUtf8Text( BLPoint( frameNumberX, frameNumberY ), mFont, frameNumber );

        iBLContext->setFillStyle( tagColor );
        iBLContext->fillUtf8Text( BLPoint( frameNumberX, frameNumberY ), mFont, frameNumber );
    }

    iBLContext->setStrokeWidth( 3.0f );
    iBLContext->setStrokeStyle( BLRgba32( 255, 127, 127, 255 ) );

    // initial keypose. Vertical line
    iBLContext->strokeLine( mChartRect.x, indicatorY - cursorRadius
                          , mChartRect.x, indicatorY + cursorRadius );

    // final keypose. Vertical line
    iBLContext->strokeLine( mChartRect.x + mChartRect.w, indicatorY - cursorRadius
                          , mChartRect.x + mChartRect.w, indicatorY + cursorRadius );

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
    uint32 cellIndex = iScene->GetEngine()->GetAnimationCell()->GetIndex();

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    FOdysseyPainterEditorVectorBaseToolHUD::Draw( iBLContext, iScene );

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        DrawChart( iBLContext
                    , fgColor
                    , bgColor
                    , hcColor
                    , mBreakdown );
    }

    iBLContext->save();
    iBLContext->resetMatrix();
}

FChartInbetween*
FOdysseyPainterEditorVectorChartToolHUD::PickInbetween( double iWorldX
                                                      , double iWorldY
                                                      , double iRadius )
{
    if( mBreakdown )
    {
        FOdysseyVectorTagInbetweener* inbetweenerTag = mBreakdown->GetInbetweenerTag();
        double cursorRadius = mChartRect.h *.5f;
        double cursorY = mChartRect.y + cursorRadius;

        if( mChartRect.HitTest( ::ULIS::FVec2D( iWorldX, iWorldY ) ) )
        {
            uint32 sourceDrawingIndex = mBreakdown->GetSourceDrawingIndex();
            uint32 targetDrawingIndex = mBreakdown->GetTargetDrawingIndex();

            for( uint32 i = 1; i < mBreakdown->GetDrawingCount() - 1; i++ )
            {
                FChartInbetween* inbetween = &mBreakdown->GetChart()->GetInbetweenArray()[i];
                float cursorX = mChartRect.x + ( inbetween->spacing * mChartRect.w );

                if( ( iWorldX >= ( cursorX - iRadius ) )
                 && ( iWorldX <= ( cursorX + iRadius ) ) )
                {
                    return inbetween;
                }
            }
        }
    }

    return nullptr;
}

void
FOdysseyPainterEditorVectorChartToolHUD::MoveInbetween( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                      , FChartInbetween* iInbetween
                                                      , double iWorldX
                                                      , double iWorldY
                                                      , bool iRelative )
{
    double newSpacing = ( iWorldX - mChartRect.x ) / mChartRect.w;

    iInbetweenerTag->MoveInbetween( iInbetween, newSpacing, iRelative );
}
