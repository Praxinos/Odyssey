// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartToolHUD.h"
#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartTool.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"
#include "Interfaces/IPluginManager.h"
#include "OdysseyVector.h"

#define INBETWEENER_INDICATOR_RADIUS 10.0f
#define BREAKDOWN_INDICATOR_RADIUS   20.0f
#define FONT_SIZE                    28.0f
#define DEFAULT_SURFACE              (1920*1080)

FOdysseyPainterEditorVectorChartToolHUD::~FOdysseyPainterEditorVectorChartToolHUD()
{
}

FOdysseyPainterEditorVectorChartToolHUD::FOdysseyPainterEditorVectorChartToolHUD( UOdysseyPainterEditorVectorChartTool* iChartTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iChartTool )
    , mChartTool( iChartTool )
    , mChartRect( 200.0f, 40, 400.0f, 40 )
{
    BLFontFace face;
   // TODO: do something depending on to the O.S
    //FString fontPath = FPaths::ProjectDir() + FString("/Resources/Font/Jrhand.ttf");
    //BLResult err = face.createFromFile("C:/Windows/Fonts/lucon.ttf"); // Lucida console
    FString fontPath = IPluginManager::Get().FindPlugin( "Odyssey" )->GetBaseDir() / TEXT( "Resources/OdysseyAssetResources/Font/LoveStruck.ttf" );
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

std::list<FInbetweenerBreakdown*>&
FOdysseyPainterEditorVectorChartToolHUD::GetBreakdownList()
{
    return mBreakdownList;
}

void
FOdysseyPainterEditorVectorChartToolHUD::UpdateBreakdown( FOdysseyVectorGroupPaint* iScene )
{
    uint32 cellIndex = iScene->GetCell()->GetIndex();

    mBreakdownList.clear();

    for( FOdysseyVectorTag* tag : iScene->GetLayer()->GetSharedTagList() )
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
                    if( mChartTool->ChartType == eChartType::Partial )
                    {
                        if( ( relativeDrawingIndex >= (int32)sourceDrawingIndex )
                         && ( relativeDrawingIndex <= (int32)targetDrawingIndex ) )
                        {
                            mBreakdownList.push_back( breakdown );

                            return; // display chart only for a single breakdown
                        }
                    }

                    if( mChartTool->ChartType == eChartType::Full )
                    {
                        mBreakdownList.push_back( breakdown );
                    }
                }
            }

            return; // display charts only for a single tag
        }
    }
}

void
FOdysseyPainterEditorVectorChartToolHUD::DrawBreakdownChart( BLContext* iBLContext
                                                           , BLRgba32& iFgColor
                                                           , BLRgba32& iBgColor
                                                           , BLRgba32& iHcColor
                                                           , FInbetweenerBreakdown* iBreakdown
                                                           , FInbetweenerChart::HUDBezier* iHUDBezier
                                                           , uint32 iRenderedCellIndex
                                                           , bool iDrawSourceIndicator )
{
    FOdysseyVectorTagInbetweener* inbetweenerTag = iBreakdown->GetInbetweenerTag();
    FInbetweenerBreakdown* prevBreakdown = iBreakdown->GetPrevBreakdown();
    FInbetweenerBreakdown* nextBreakdown = iBreakdown->GetNextBreakdown();
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
    float displayRatio = ( float ) sqrt( ( iBLContext->targetWidth() * iBLContext->targetHeight() ) / DEFAULT_SURFACE );

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

        path.moveTo( iHUDBezier->GetPoints()[0].GetPosition().x, iHUDBezier->GetPoints()[0].GetPosition().y );
        path.quadTo( iHUDBezier->GetPoints()[1].GetPosition().x, iHUDBezier->GetPoints()[1].GetPosition().y
                   , iHUDBezier->GetPoints()[2].GetPosition().x, iHUDBezier->GetPoints()[2].GetPosition().y );

        iBLContext->strokePath( path );
    }
/*
    iBLContext->strokeLine( mChartRect.x               , indicatorY
                          , mChartRect.x + mChartRect.w, indicatorY );
*/
    // vertical lines
    for( uint32 i = 0; i < iBreakdown->GetDrawingCount(); i++ )
    {
        FInbetweenerChart::Inbetween* inbetween = &iBreakdown->GetChart()->GetInbetweenBuffer()[i];
        float indicatorX = mChartRect.x + ( inbetween->GetSpacing() * mChartRect.w );
        double quadraticT = iHUDBezier->GetQuadraticT( inbetween->GetSpacing() );
        ::ULIS::FVec2D indicatorPosition = ::ULIS::QuadraticBezierPointAtParameter( iHUDBezier->GetPoints()[0].GetPosition(),
                                                                                    iHUDBezier->GetPoints()[1].GetPosition(),
                                                                                    iHUDBezier->GetPoints()[2].GetPosition(),
                                                                                    quadraticT );
        ::ULIS::FVec2D indicatorTangent = ::ULIS::QuadraticBezierTangentAtParameter( iHUDBezier->GetPoints()[0].GetPosition(),
                                                                                     iHUDBezier->GetPoints()[1].GetPosition(),
                                                                                     iHUDBezier->GetPoints()[2].GetPosition(),
                                                                                     quadraticT );
        ::ULIS::FVec2D indicatorPerpendicular = ::ULIS::FVec2D( -indicatorTangent.y, indicatorTangent.x );
        bool hovered = ( inbetween == mChartTool->GetHoveredInbetween() );
        bool current = ( inbetween->GetCellIndex() == iRenderedCellIndex );

        if( indicatorPerpendicular.DistanceSquared() )
        {
            float lengthFactor = ( ( inbetween->GetSpacing() == 0.0f ) || ( inbetween->GetSpacing() == 1.0f ) ) ? 1.0f : 0.6f;
            ::ULIS::FVec2D normalizedPerpendicular = indicatorPerpendicular.Normalize() * lengthFactor;
             // Note: "* 0.1f" helps positionning the circle surrounding the numbers a bit away from the indicator
            ::ULIS::FVec2D frameInfoPosition = ::ULIS::FVec2D( indicatorPosition.x + ( normalizedPerpendicular.x * 1.1f * ( FONT_SIZE + ( BREAKDOWN_INDICATOR_RADIUS ) ) )
                                                             , indicatorPosition.y + ( normalizedPerpendicular.y * 1.1f * ( FONT_SIZE + ( BREAKDOWN_INDICATOR_RADIUS ) ) ) );
            // calculate inbetween number position
            const FGlyph* glyph = GetGlyph( iBreakdown->GetSourceDrawingIndex() + i + 1 );
            ::ULIS::FVec2D frameNumberPosition = ::ULIS::FVec2D( frameInfoPosition.x - ( glyph->bbox.w * 0.5f )
                                                               , frameInfoPosition.y + ( glyph->bbox.h * 0.5f ) );

            if( ( ( inbetween->GetSpacing() == 0.0f ) && iDrawSourceIndicator ) || ( inbetween->GetSpacing() == 1.0f ) )
            {
                iBLContext->setStrokeWidth( 2.0f * displayRatio );
                iBLContext->setStrokeStyle( chartColor );

                if( ( ( inbetween->GetSpacing() == 0.0f ) && prevBreakdown )
                 || ( ( inbetween->GetSpacing() == 1.0f ) && nextBreakdown ) )
                {
                    iBLContext->strokeLine( frameNumberPosition.x
                                          , frameNumberPosition.y + glyph->bbox.h
                                          , frameNumberPosition.x + glyph->bbox.w
                                          , frameNumberPosition.y + glyph->bbox.h );
                }
                else
                {
                    iBLContext->strokeCircle( frameInfoPosition.x, frameInfoPosition.y, fontSize );
                }

                // draw indicator
                iBLContext->strokeLine( indicatorPosition.x + ( normalizedPerpendicular.x * BREAKDOWN_INDICATOR_RADIUS )
                                      , indicatorPosition.y + ( normalizedPerpendicular.y * BREAKDOWN_INDICATOR_RADIUS )
                                      , indicatorPosition.x - ( normalizedPerpendicular.x * BREAKDOWN_INDICATOR_RADIUS )
                                      , indicatorPosition.y - ( normalizedPerpendicular.y * BREAKDOWN_INDICATOR_RADIUS ) );

                iBLContext->strokeUtf8Text( BLPoint( frameNumberPosition.x
                                                   , frameNumberPosition.y ), mFont, glyph->str );

                iBLContext->setFillStyle( chartColor );
                iBLContext->fillUtf8Text( BLPoint( frameNumberPosition.x
                                                 , frameNumberPosition.y ), mFont, glyph->str );
            }

            if ( ( inbetween->GetSpacing() > 0.0f ) && ( inbetween->GetSpacing() < 1.0f ) )
            {
                iBLContext->setStrokeWidth( hovered ? 3.0f * displayRatio: 2.0f * displayRatio );
                iBLContext->setStrokeStyle( hovered ? iHcColor : inbetweenColor );
                // draw indicator
                iBLContext->strokeLine( indicatorPosition.x + ( normalizedPerpendicular.x * BREAKDOWN_INDICATOR_RADIUS )
                                      , indicatorPosition.y + ( normalizedPerpendicular.y * BREAKDOWN_INDICATOR_RADIUS )
                                      , indicatorPosition.x - ( normalizedPerpendicular.x * BREAKDOWN_INDICATOR_RADIUS )
                                      , indicatorPosition.y - ( normalizedPerpendicular.y * BREAKDOWN_INDICATOR_RADIUS ) );

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

    if( mChartTool->EditionMode == eChartEditionMode::Reshape )
    {
        DrawLine( iBLContext
                , iHUDBezier->GetPoints()[0].GetPosition().x
                , iHUDBezier->GetPoints()[0].GetPosition().y
                , iHUDBezier->GetPoints()[1].GetPosition().x
                , iHUDBezier->GetPoints()[1].GetPosition().y
                , iFgColor
                , iBgColor );

        DrawLine( iBLContext
                , iHUDBezier->GetPoints()[1].GetPosition().x
                , iHUDBezier->GetPoints()[1].GetPosition().y
                , iHUDBezier->GetPoints()[2].GetPosition().x
                , iHUDBezier->GetPoints()[2].GetPosition().y
                , iFgColor
                , iBgColor );

        for( uint32 i = 0; i < 3; i++ )
        {
            DrawCircle( iBLContext
                      , iHUDBezier->GetPoints()[i].GetPosition().x
                      , iHUDBezier->GetPoints()[i].GetPosition().y
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
    uint32 cellIndex = iScene->GetCell()->GetIndex();

    iBLContext->save();
    // do not add-up colors
    iBLContext->setCompOp( BL_COMP_OP_SRC_COPY );

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    //FOdysseyPainterEditorVectorBaseToolHUD::Draw( iBLContext, iScene );

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        for( FInbetweenerBreakdown* breakdown : mBreakdownList )
        {
            FInbetweenerBreakdown* prevBreakdown = breakdown->GetPrevBreakdown();

            breakdown->GetInbetweenerTag()->LockDrawing();

            DrawBreakdown( iScene
                         , iBLContext
                         , breakdown
                         , BLRgba32( 127, 127, 127, 255 )
                         , BLRgba32( 255, 127, 127, 255 )
                         , HUD_BREAKDOWN_SOURCE
                         | HUD_BREAKDOWN_INBETWEEN
                         //| HUD_INBETWEEN_FADEFROMTARGET
                         | HUD_BREAKDOWN_TARGET );

            if( mChartTool->ChartType == eChartType::Full )
            {
                DrawBreakdownChart( iBLContext
                                  , fgColor
                                  , bgColor
                                  , hcColor
                                  , breakdown
                                  , breakdown->GetChart()->GetHUDBezier()
                                  , iScene->GetCell()->GetIndex()
                                  , prevBreakdown ? false : true );
            }

            if( mChartTool->ChartType == eChartType::Partial )
            {
                DrawBreakdownChart( iBLContext
                                  , fgColor
                                  , bgColor
                                  , hcColor
                                  , breakdown
                                  , breakdown->GetChart()->GetHUDBezier()
                                  , iScene->GetCell()->GetIndex()
                                  , true );
            }

           breakdown->GetInbetweenerTag()->UnlockDrawing();
        }
    }

    iBLContext->restore();
}

FInbetweenerChart::Inbetween*
FOdysseyPainterEditorVectorChartToolHUD::PickInbetween( double iWorldX
                                                      , double iWorldY )
{
    FInbetweenerChart::Inbetween* closestInbetween = nullptr;
    double minDistance = DBL_MAX;

    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        FOdysseyVectorTagInbetweener* inbetweenerTag = breakdown->GetInbetweenerTag();
        FInbetweenerChart* chart = breakdown->GetChart();
        FInbetweenerChart::HUDBezier* HUDBezier = chart->GetHUDBezier();

        for( uint32 i = 1; i < breakdown->GetDrawingCount() - 1; i++ )
        {
            FInbetweenerChart::Inbetween* inbetween = &breakdown->GetChart()->GetInbetweenBuffer()[i];
            ::ULIS::FVec2D indicatorPosition = ::ULIS::QuadraticBezierPointAtParameter( HUDBezier->GetPoints()[0].GetPosition(),
                                                                                        HUDBezier->GetPoints()[1].GetPosition(),
                                                                                        HUDBezier->GetPoints()[2].GetPosition(),
                                                                                        chart->GetHUDBezier()->GetQuadraticT( inbetween->GetSpacing() ) );
            double distance = ::ULIS::FVec2D( iWorldX - indicatorPosition.x
                                            , iWorldY - indicatorPosition.y ).Distance();

            if( distance < ( BREAKDOWN_INDICATOR_RADIUS + ( FONT_SIZE * 0.5f ) ) )
            {
                if( distance < minDistance )
                {
                    closestInbetween = inbetween;

                    minDistance = distance;
                }
            }
        }
    }

    return closestInbetween;
}

FInbetweenerChart::HUDBezier::Point*
FOdysseyPainterEditorVectorChartToolHUD::PickBezierPoint( double iWorldX
                                                        , double iWorldY
                                                        , double iRadius )
{
    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        FOdysseyVectorTagInbetweener* inbetweenerTag = breakdown->GetInbetweenerTag();
        FInbetweenerChart* chart = breakdown->GetChart();
        FInbetweenerChart::HUDBezier* HUDBezier = chart->GetHUDBezier();

        for( uint32 i = 0; i < 3; i++ )
        {
            if( ::ULIS::FVec2D( iWorldX - HUDBezier->GetPoints()[i].GetPosition().x
                              , iWorldY - HUDBezier->GetPoints()[i].GetPosition().y ).Distance() < iRadius )
            {
                return &HUDBezier->GetPoints()[i];
            }
        }
    }

    return nullptr;
}

FInbetweenerBreakdown*
FOdysseyPainterEditorVectorChartToolHUD::PickBreakdown( double iWorldX
                                                      , double iWorldY
                                                      , double iRadius )
{
    for( FInbetweenerBreakdown* breakdown : mBreakdownList )
    {
        FOdysseyVectorTagInbetweener* inbetweenerTag = breakdown->GetInbetweenerTag();
        FInbetweenerChart* chart = breakdown->GetChart();
        FInbetweenerChart::HUDBezier* HUDBezier = chart->GetHUDBezier();

        double t = FOdysseyVector::QuadraticBezierHitTest( ::ULIS::FVec2D ( iWorldX, iWorldY )
                                                         , HUDBezier->GetPoints()[0].GetPosition()
                                                         , HUDBezier->GetPoints()[1].GetPosition()
                                                         , HUDBezier->GetPoints()[2].GetPosition()
                                                         , 24
                                                         , iRadius );

        if( ( t > 0.0f ) && ( t < 1.0f ) )
        {
            return breakdown;
        }
    }

    return nullptr;
}

void
FOdysseyPainterEditorVectorChartToolHUD::MoveInbetween( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                      , FInbetweenerChart::Inbetween* iInbetween
                                                      , double iWorldX
                                                      , double iWorldY
                                                      , bool iRelative )
{
    double newSpacing = ( iWorldX - mChartRect.x ) / mChartRect.w;

    iInbetweenerTag->MoveInbetween( iInbetween, newSpacing, iRelative );
}
