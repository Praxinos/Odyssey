#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartToolHUD.h"
#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartTool.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorAnimationCell.h"
#include "OdysseyVectorSharedEnv.h"

#define INDICATOR_RADIUS 20.0f

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
                                                  , FInbetweenerBreakdown* iBreakdown
                                                  , uint32 iRenderedCellIndex )
{
    FOdysseyVectorTagInbetweener* inbetweenerTag = iBreakdown->GetInbetweenerTag();
    FInbetweenerChart* chart = iBreakdown->GetChart();
    double cursorRadius = mChartRect.h *.5f;
    float indicatorY = mChartRect.y + cursorRadius;
    float fontSize = mFont.size();
    const FColor& GetColor();
    BLRgba32 tagColor = BLRgba32( inbetweenerTag->GetColor().R
                                , inbetweenerTag->GetColor().G
                                , inbetweenerTag->GetColor().B
                                , 255 );
    BLRgba32 blackColor = BLRgba32(   0,   0,   0, 255 );
    BLRgba32 greyColor  = BLRgba32( 127, 127, 127, 255 );
    BLRgba32 whiteColor = BLRgba32( 255, 255, 255, 255 );
    uint32 sourceDrawingIndex = iBreakdown->GetSourceDrawingIndex();
    uint32 targetDrawingIndex = iBreakdown->GetTargetDrawingIndex();
    FInbetweenerDrawing* sourceDrawing = inbetweenerTag->GetDrawing( sourceDrawingIndex );
    FInbetweenerDrawing* targetDrawing = inbetweenerTag->GetDrawing( targetDrawingIndex );
    ::ULIS::FVec2D* HUDBezier = chart->GetHUDBezier();

    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setCompOp( BL_COMP_OP_SRC_OVER  );

    iBLContext->setStrokeStyle( iHcColor );
    iBLContext->setStrokeWidth( 1.0f );

    // chart quadratic bezier line
    {
        BLPath path;

        path.moveTo( HUDBezier[0].x, HUDBezier[0].y );
        path.quadTo( HUDBezier[1].x, HUDBezier[1].y
                   , HUDBezier[2].x, HUDBezier[2].y );

        iBLContext->strokePath( path );
    }
/*
    iBLContext->strokeLine( mChartRect.x               , indicatorY
                          , mChartRect.x + mChartRect.w, indicatorY );
*/
    // vertical lines
    for( uint32 i = 0; i < iBreakdown->GetDrawingCount(); i++ )
    {
        FChartDivision* inbetween = &iBreakdown->GetChart()->GetDivisionArray()[i];
        float indicatorX = mChartRect.x + ( inbetween->spacing * mChartRect.w );
        float frameNumberX = indicatorX - ( fontSize * 0.25f );
        float frameNumberY = indicatorY + cursorRadius + fontSize + 2.0f;
        char frameNumber[6];
        ::ULIS::FVec2D indicatorPosition = ::ULIS::QuadraticBezierPointAtParameter( HUDBezier[0],
                                                                                    HUDBezier[1],
                                                                                    HUDBezier[2],
                                                                                    inbetween->spacing );
        ::ULIS::FVec2D indicatorTangent = ::ULIS::QuadraticBezierTangentAtParameter( HUDBezier[0],
                                                                                     HUDBezier[1],
                                                                                     HUDBezier[2],
                                                                                     inbetween->spacing );
        ::ULIS::FVec2D indicatorPerpendicular = ::ULIS::FVec2D( -indicatorTangent.y, indicatorTangent.x );
        bool hovered = ( inbetween == mChartTool->GetHoveredInbetween() );
        bool current = ( inbetween->GetAnimationCellIndex() == iRenderedCellIndex );

        if( indicatorPerpendicular.DistanceSquared() )
        {
            indicatorPerpendicular.Normalize();

            if( ( inbetween->spacing == 0.0f ) || ( inbetween->spacing == 1.0f ) )
            {
                iBLContext->setStrokeWidth( 3.0f );
                iBLContext->setStrokeStyle( BLRgba32( 255, 127, 127, 255 ) );
            }
            else
            {
                iBLContext->setStrokeWidth( hovered ? 3.0f     : 1.0f     );
                iBLContext->setStrokeStyle( hovered ? iHcColor : tagColor );
            }

            // draw indicator
            iBLContext->strokeLine( indicatorPosition.x + ( indicatorPerpendicular.x * INDICATOR_RADIUS )
                                  , indicatorPosition.y + ( indicatorPerpendicular.y * INDICATOR_RADIUS )
                                  , indicatorPosition.x - ( indicatorPerpendicular.x * INDICATOR_RADIUS )
                                  , indicatorPosition.y - ( indicatorPerpendicular.y * INDICATOR_RADIUS ) );

            // draw inbetween number
            {
                ::ULIS::FVec2D frameNumberPosition = ::ULIS::FVec2D( indicatorPosition.x + ( indicatorPerpendicular.x * 2.0f * INDICATOR_RADIUS )
                                                                   , indicatorPosition.y + ( indicatorPerpendicular.y * 2.0f * INDICATOR_RADIUS ) );

                snprintf( frameNumber, 6, "%d", i + 1 );

                iBLContext->setStrokeStyle( blackColor );
                iBLContext->setStrokeWidth( hovered ? 2.0f : 1.0f );
                iBLContext->strokeUtf8Text( BLPoint( frameNumberPosition.x
                                                   , frameNumberPosition.y ), mFont, frameNumber );

                iBLContext->setFillStyle( hovered ? iHcColor : greyColor );
                iBLContext->fillUtf8Text( BLPoint( frameNumberPosition.x
                                                 , frameNumberPosition.y ), mFont, frameNumber );

                // underline the current frame
                if( current )
                {
                    iBLContext->setStrokeWidth( 4.0f );
                    iBLContext->strokeLine( frameNumberPosition.x
                                          , frameNumberPosition.y + 4
                                          , frameNumberPosition.x + 11
                                          , frameNumberPosition.y + 4 );
                }
            }
        }
    }

    if( mChartTool->GetPickingMode() == eChartPickingMode::Control )
    {
        DrawLine( iBLContext
                , HUDBezier[0].x
                , HUDBezier[0].y
                , HUDBezier[1].x
                , HUDBezier[1].y
                , iFgColor
                , iBgColor );

        DrawLine( iBLContext
                , HUDBezier[1].x
                , HUDBezier[1].y
                , HUDBezier[2].x
                , HUDBezier[2].y
                , iFgColor
                , iBgColor );
   
        for( uint32 i = 0; i < 3; i++ )
        {
            DrawCircle( iBLContext
                      , HUDBezier[i].x
                      , HUDBezier[i].y
                      , 3.0f
                      , iFgColor
                      , iBgColor );
        }
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
    uint32 cellIndex = iScene->GetEngine()->GetAnimationCell()->GetIndex();

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    //FOdysseyPainterEditorVectorBaseToolHUD::Draw( iBLContext, iScene );

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        if( mBreakdown )
        {
            DrawBreakdown( iBLContext
                         , mBreakdown
                         , BLRgba32( 127, 127, 127, 255 )
                         , BLRgba32( 255, 127, 127, 255 )
                         , HUD_BREAKDOWN_SOURCE | HUD_BREAKDOWN_TARGET | HUD_BREAKDOWN_INBETWEEN );

            DrawChart( iBLContext
                      , fgColor
                      , bgColor
                      , hcColor
                      , mBreakdown
                      , iScene->GetEngine()->GetAnimationCell()->GetIndex() );
        }
    }

    iBLContext->save();
    iBLContext->resetMatrix();
}

FChartDivision*
FOdysseyPainterEditorVectorChartToolHUD::PickInbetween( double iWorldX
                                                      , double iWorldY )
{
    if( mBreakdown )
    {
        FOdysseyVectorTagInbetweener* inbetweenerTag = mBreakdown->GetInbetweenerTag();
        FInbetweenerChart* chart = mBreakdown->GetChart();
        ::ULIS::FVec2D* HUDBezier = chart->GetHUDBezier();

        for( uint32 i = 1; i < mBreakdown->GetDrawingCount() - 1; i++ )
        {
            FChartDivision* inbetween = &mBreakdown->GetChart()->GetDivisionArray()[i];
            ::ULIS::FVec2D indicatorPosition = ::ULIS::QuadraticBezierPointAtParameter( HUDBezier[0],
                                                                                        HUDBezier[1],
                                                                                        HUDBezier[2],
                                                                                        inbetween->spacing );

            if( ::ULIS::FVec2D( iWorldX - indicatorPosition.x
                              , iWorldY - indicatorPosition.y ).Distance() < INDICATOR_RADIUS )
            {
                return inbetween;
            }
        }
    }

    return nullptr;
}

::ULIS::FVec2D*
FOdysseyPainterEditorVectorChartToolHUD::PickBezierPoint( double iWorldX
                                                        , double iWorldY
                                                        , double iRadius )
{
    if( mBreakdown )
    {
        FOdysseyVectorTagInbetweener* inbetweenerTag = mBreakdown->GetInbetweenerTag();
        FInbetweenerChart* chart = mBreakdown->GetChart();
        ::ULIS::FVec2D* HUDBezier = chart->GetHUDBezier();

        for( uint32 i = 0; i < 3; i++ )
        {
            if( ::ULIS::FVec2D( iWorldX - HUDBezier[i].x, iWorldY - HUDBezier[i].y ).Distance() < iRadius )
            {
                return &HUDBezier[i];
            }
        }
    }

    return nullptr;
}

void
FOdysseyPainterEditorVectorChartToolHUD::MoveInbetween( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                      , FChartDivision* iInbetween
                                                      , double iWorldX
                                                      , double iWorldY
                                                      , bool iRelative )
{
    double newSpacing = ( iWorldX - mChartRect.x ) / mChartRect.w;

    iInbetweenerTag->MoveInbetween( iInbetween, newSpacing, iRelative );
}
