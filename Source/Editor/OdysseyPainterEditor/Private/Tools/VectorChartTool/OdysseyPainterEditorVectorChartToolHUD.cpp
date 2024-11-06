#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartToolHUD.h"
#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartTool.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorSharedEnv.h"
#include "Interfaces/IPluginManager.h"

#define INDICATOR_RADIUS 20.0f
#define FONT_SIZE        28.0f
#define DEFAULT_SURFACE  (1920*1080)

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
    //FString fontPath = FPaths::ProjectDir() + FString("/Resources/Font/Jrhand.ttf");
    //BLResult err = face.createFromFile("C:/Windows/Fonts/lucon.ttf"); // Lucida console
    FString fontPath = IPluginManager::Get().FindPlugin( "Iliad" )->GetBaseDir() / TEXT( "Resources/OdysseyAssetResources/Font/LoveStruck.ttf" );
    BLResult err = face.createFromFile( TCHAR_TO_ANSI( *fontPath ) ); // JR!Hand

    mFont.createFromFace( face, FONT_SIZE );
    // reserve enough bounding box for at least 20 numbers. That way we don't recompute them each time.
    mGlyphBuffer.reserve( 20 );
}

const FOdysseyPainterEditorVectorChartToolHUD::FGlyph*
FOdysseyPainterEditorVectorChartToolHUD::GetGlyph( uint32 iNum )
{
    FGlyph* glyph;

    if( iNum >= mGlyphBuffer.size() )
    {
        mGlyphBuffer.resize( iNum + 1 );
    }

    glyph = &mGlyphBuffer[iNum];

    if( ( glyph->bbox.w == 0 ) && ( glyph->bbox.h == 0 ) )
    {
        BLGlyphBuffer blGlyph;
        BLTextMetrics blGlyphMetrics;

        snprintf( glyph->str, 6, "%d", iNum );
        blGlyph.setUtf8Text( mGlyphBuffer[iNum].str, strlen( glyph->str ) );
        mFont.getTextMetrics( blGlyph, blGlyphMetrics );

        glyph->bbox = ::ULIS::FRectI ( 0
                                     , 0
                                     , ( blGlyphMetrics.boundingBox.x1 - blGlyphMetrics.boundingBox.x0 ) + 1
                                     , FONT_SIZE * 0.5f
                                     // commented out. for some reason its value is 0 ??
                                     /*, ( blGlyphMetrics.boundingBox.y1 - blGlyphMetrics.boundingBox.y0 ) + 1*/ );
    }

    return glyph;
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
    uint32 cellIndex = iScene->GetEngine()->GetCell()->GetIndex();

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
                    uint32 sourceCellIndex = inbetweenerTag->GetSourceCellIndex();
                    uint32 sourceDrawingIndex = breakdown->GetSourceDrawingIndex();
                    uint32 targetDrawingIndex = breakdown->GetTargetDrawingIndex();
                    int32 relativeDrawingIndex = ( inbetweenerTag->GetInterpolationDirection() == eInbetweenerInterpolationDirection::Forward ) ? ( cellIndex - sourceCellIndex )
                                                                                                                                                : ( sourceCellIndex - cellIndex );

                    if( ( relativeDrawingIndex >= (int32)sourceDrawingIndex )
                     && ( relativeDrawingIndex <= (int32)targetDrawingIndex ) )
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
    BLRgba32 chartColor = BLRgba32( inbetweenerTag->GetChartColor().R
                                  , inbetweenerTag->GetChartColor().G
                                  , inbetweenerTag->GetChartColor().B
                                  , inbetweenerTag->GetChartColor().A );
    BLRgba32 inbetweenColor = BLRgba32( inbetweenerTag->GetInbetweenColor().R
                                      , inbetweenerTag->GetInbetweenColor().G
                                      , inbetweenerTag->GetInbetweenColor().B
                                      , 255 );
    BLRgba32 blackColor = BLRgba32(   0,   0,   0, 255 );
    BLRgba32 greyColor  = BLRgba32( 127, 127, 127, 255 );
    BLRgba32 whiteColor = BLRgba32( 255, 255, 255, 255 );
    uint32 sourceDrawingIndex = iBreakdown->GetSourceDrawingIndex();
    uint32 targetDrawingIndex = iBreakdown->GetTargetDrawingIndex();
    FInbetweenerDrawing* sourceDrawing = inbetweenerTag->GetDrawing( sourceDrawingIndex );
    FInbetweenerDrawing* targetDrawing = inbetweenerTag->GetDrawing( targetDrawingIndex );
    ::ULIS::FVec2D* HUDBezier = chart->GetHUDBezier();
    float displayRatio = ( float ) ( iBLContext->targetWidth() * iBLContext->targetHeight() ) / DEFAULT_SURFACE;

     // On my colleague's request, the width of the stroke varies relative to the size of the image
    displayRatio = std::max( 1.0f, 1.0f + ( ( displayRatio - 1.0f ) * 0.25f ) );

    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setCompOp( BL_COMP_OP_SRC_OVER  );

    iBLContext->setStrokeStyle( chartColor );
    iBLContext->setStrokeWidth( 2.0f * displayRatio );

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
        FChartDivision* inbetween = &iBreakdown->GetChart()->GetDivisionBuffer()[i];
        float indicatorX = mChartRect.x + ( inbetween->spacing * mChartRect.w );
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
        bool current = ( inbetween->GetCellIndex() == iRenderedCellIndex );

        if( indicatorPerpendicular.DistanceSquared() )
        {
            float lengthFactor = ( ( inbetween->spacing == 0.0f ) || ( inbetween->spacing == 1.0f ) ) ? 1.0f : 0.6f;
            ::ULIS::FVec2D normalizedPerpendicular = indicatorPerpendicular.Normalize() * lengthFactor;
             // Note: "* 0.1f" helps positionning the circle surrounding the numbers a bit away from the indicator
            ::ULIS::FVec2D frameInfoPosition = ::ULIS::FVec2D( indicatorPosition.x + ( normalizedPerpendicular.x * 1.1f * ( FONT_SIZE + ( INDICATOR_RADIUS ) ) )
                                                             , indicatorPosition.y + ( normalizedPerpendicular.y * 1.1f * ( FONT_SIZE + ( INDICATOR_RADIUS ) ) ) );
            // calculate inbetween number position
            const FGlyph* glyph = GetGlyph( iBreakdown->GetSourceDrawingIndex() + i + 1 );
            ::ULIS::FVec2D frameNumberPosition = ::ULIS::FVec2D( frameInfoPosition.x - ( glyph->bbox.w * 0.5f )
                                                               , frameInfoPosition.y + ( glyph->bbox.h * 0.5f ) );

            if( ( inbetween->spacing == 0.0f ) || ( inbetween->spacing == 1.0f ) )
            {
                iBLContext->setStrokeWidth( 2.0f * displayRatio );
                iBLContext->setStrokeStyle( chartColor );
                iBLContext->strokeCircle( frameInfoPosition.x, frameInfoPosition.y, fontSize );
                // draw indicator
                iBLContext->strokeLine( indicatorPosition.x + ( normalizedPerpendicular.x * INDICATOR_RADIUS )
                                      , indicatorPosition.y + ( normalizedPerpendicular.y * INDICATOR_RADIUS )
                                      , indicatorPosition.x - ( normalizedPerpendicular.x * INDICATOR_RADIUS )
                                      , indicatorPosition.y - ( normalizedPerpendicular.y * INDICATOR_RADIUS ) );

                iBLContext->strokeUtf8Text( BLPoint( frameNumberPosition.x
                                                   , frameNumberPosition.y ), mFont, glyph->str );

                iBLContext->setFillStyle( chartColor );
                iBLContext->fillUtf8Text( BLPoint( frameNumberPosition.x
                                                 , frameNumberPosition.y ), mFont, glyph->str );
            }
            else
            {
                iBLContext->setStrokeWidth( hovered ? 3.0f * displayRatio: 2.0f * displayRatio );
                iBLContext->setStrokeStyle( hovered ? iHcColor : inbetweenColor );
                // draw indicator
                iBLContext->strokeLine( indicatorPosition.x + ( normalizedPerpendicular.x * INDICATOR_RADIUS )
                                      , indicatorPosition.y + ( normalizedPerpendicular.y * INDICATOR_RADIUS )
                                      , indicatorPosition.x - ( normalizedPerpendicular.x * INDICATOR_RADIUS )
                                      , indicatorPosition.y - ( normalizedPerpendicular.y * INDICATOR_RADIUS ) );

                iBLContext->strokeUtf8Text( BLPoint( frameNumberPosition.x
                                                   , frameNumberPosition.y ), mFont, glyph->str );

                iBLContext->setFillStyle( hovered ? iHcColor : inbetweenColor );
                iBLContext->fillUtf8Text( BLPoint( frameNumberPosition.x
                                                 , frameNumberPosition.y ), mFont, glyph->str );
            }

            if( current )
            {
                iBLContext->setStrokeStyle( blackColor );
                iBLContext->setStrokeWidth( 3.0f * displayRatio );
                iBLContext->strokeUtf8Text( BLPoint( frameNumberPosition.x
                                                   , frameNumberPosition.y ), mFont, glyph->str );
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
    uint32 cellIndex = iScene->GetEngine()->GetCell()->GetIndex();

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
                      , iScene->GetEngine()->GetCell()->GetIndex() );
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
            FChartDivision* inbetween = &mBreakdown->GetChart()->GetDivisionBuffer()[i];
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
