// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
// for 3D HUDs
#include "CanvasTypes.h"
#include "CanvasItem.h"

#include "Fonts/FontMeasure.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

#define INBETWEENER_INDICATOR_RADIUS 10.0f
#define BREAKDOWN_INDICATOR_RADIUS   20.0f
#define FONT_SIZE                    16.0f
#define DEFAULT_SURFACE              (1920*1080)

FOdysseyPainterEditorVectorChartToolHUD::~FOdysseyPainterEditorVectorChartToolHUD()
{
}

FOdysseyPainterEditorVectorChartToolHUD::FOdysseyPainterEditorVectorChartToolHUD( UOdysseyPainterEditorVectorChartTool* iChartTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iChartTool )
    , mChartTool( iChartTool )
    , mChartRect( 200.0f, 40, 400.0f, 40 )
{
    //FString fontPath = IPluginManager::Get().FindPlugin( "Odyssey" )->GetBaseDir() / TEXT( "Resources/OdysseyAssetResources/Font/LoveStruck.ttf" );

    mChartFontInfo = FSlateFontInfo( LoadObject<UFont>( nullptr, TEXT("/Odyssey/Fonts/LoveStruck_Font") ), FONT_SIZE );
}

void
FOdysseyPainterEditorVectorChartToolHUD::Load()
{
    FText ctrlInfoText = LOCTEXT("vector-chart-tool-hud-info-ctrl", "Relative" );
    FText shiftInfoText = LOCTEXT("vector-chart-tool-hud-info-shift", "Ease in or out" );
    FText altInfoText = LOCTEXT("vector-chart-tool-hud-info-alt", "Magnet" );

    FormatModifierInfo( &ctrlInfoText, &shiftInfoText, &altInfoText );

    FOdysseyPainterEditorVectorBaseToolHUD::Load();
}

void
FOdysseyPainterEditorVectorChartToolHUD::Reset()
{
    uint64 hudFlags = mChartTool->GetEditor()->GetVectorHUDFlags();

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        UpdateBreakdown();
    }
}

std::list<FInbetweenerBreakdown*>&
FOdysseyPainterEditorVectorChartToolHUD::GetBreakdownList()
{
    return mBreakdownList;
}

void
FOdysseyPainterEditorVectorChartToolHUD::UpdateBreakdown()
{
    uint32 cellIndex = mScene->GetCell()->GetIndex();

    mBreakdownList.clear();

    for( FOdysseyVectorTag* tag : mScene->GetLayer()->GetSharedTagList() )
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
FOdysseyPainterEditorVectorChartToolHUD::DrawBreakdownChart( const FOdysseyHUD::FDrawHUDParams& iParams
                                                           , const FLinearColor& iFgColor
                                                           , const FLinearColor& iBgColor
                                                           , const FLinearColor& iHcColor
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
    float fontSize = mChartFontInfo.Size;
    FLinearColor chartColor = FLinearColor( inbetweenerTag->GetChartColor() );
    FLinearColor inbetweenColor = FLinearColor( inbetweenerTag->GetInbetweenColor() );
    FLinearColor blackColor = FLinearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    FLinearColor greyColor  = FLinearColor( 0.5f, 0.5f, 0.5f, 1.0f );
    FLinearColor whiteColor = FLinearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    uint32 sourceDrawingIndex = iBreakdown->GetSourceDrawingIndex();
    uint32 targetDrawingIndex = iBreakdown->GetTargetDrawingIndex();
    FInbetweenerDrawing* sourceDrawing = inbetweenerTag->GetDrawing( sourceDrawingIndex );
    FInbetweenerDrawing* targetDrawing = inbetweenerTag->GetDrawing( targetDrawingIndex );
    ::ULIS::FVec2D texCoords[3] = { iHUDBezier->GetPoints()[0].GetPosition()
                                  , iHUDBezier->GetPoints()[1].GetPosition()
                                  , iHUDBezier->GetPoints()[2].GetPosition() };
    FVector2D hudCoords[3] = { iParams.mTextureToHUD.Execute( FVector2D( texCoords[0].x, texCoords[0].y ) )
                             , iParams.mTextureToHUD.Execute( FVector2D( texCoords[1].x, texCoords[1].y ) )
                             , iParams.mTextureToHUD.Execute( FVector2D( texCoords[2].x, texCoords[2].y ) ) };
    const UFont* font = Cast<UFont>(mChartFontInfo.FontObject);

    // force inbetween opacity
    inbetweenColor.A = 1.0f;

    // chart quadratic bezier line
    {
        DrawPrimitiveBezierQuadratic( iParams
                                    , hudCoords[0]
                                    , hudCoords[1]
                                    , hudCoords[2]
                                    , 24
                                    , chartColor
                                    , 2.0f );
    }

    // vertical lines
    for( uint32 i = 0; i < iBreakdown->GetDrawingCount(); i++ )
    {
        FInbetweenerChart::Inbetween* inbetween = &iBreakdown->GetChart()->GetInbetweenBuffer()[i];
        float indicatorX = mChartRect.x + ( inbetween->GetSpacing() * mChartRect.w );
        double quadraticT = iHUDBezier->GetQuadraticT( inbetween->GetSpacing() );
        ::ULIS::FVec2D texIndicatorPosition = ::ULIS::QuadraticBezierPointAtParameter( ::ULIS::FVec2D( texCoords[0].x, texCoords[0].y ),
                                                                                       ::ULIS::FVec2D( texCoords[1].x, texCoords[1].y ),
                                                                                       ::ULIS::FVec2D( texCoords[2].x, texCoords[2].y ),
                                                                                       quadraticT );
        ::ULIS::FVec2D texIndicatorTangent = ::ULIS::QuadraticBezierTangentAtParameter( ::ULIS::FVec2D( texCoords[0].x, texCoords[0].y ),
                                                                                        ::ULIS::FVec2D( texCoords[1].x, texCoords[1].y ),
                                                                                        ::ULIS::FVec2D( texCoords[2].x, texCoords[2].y ),
                                                                                        quadraticT );
        if( texIndicatorTangent.DistanceSquared() )
        {
            texIndicatorTangent.Normalize();

            ::ULIS::FVec2D texIndicatorPerpendicular = ::ULIS::FVec2D( -texIndicatorTangent.y, texIndicatorTangent.x );

            float lengthFactor = ( ( inbetween->GetSpacing() == 0.0f ) || ( inbetween->GetSpacing() == 1.0f ) ) ? 1.0f : 0.6f;
            ::ULIS::FVec2D hudIndicatorPosition = WorldPointToHUD( iParams, texIndicatorPosition );
            ::ULIS::FVec2D hudIndicatorTangent = WorldVectorToHUD( iParams, texIndicatorPosition, texIndicatorTangent );
            ::ULIS::FVec2D hudIndicatorPerpendicular = ::ULIS::FVec2D( -hudIndicatorTangent.y, hudIndicatorTangent.x );
            bool hovered = ( inbetween == mChartTool->GetHoveredInbetween() );
            bool current = ( inbetween->GetCellIndex() == iRenderedCellIndex );

             // Note: "* 0.1f" helps positionning the circle surrounding the numbers a bit away from the indicator
            ::ULIS::FVec2D texFrameInfoPosition = ::ULIS::FVec2D( texIndicatorPosition.x + ( texIndicatorPerpendicular.x * 1.1f * ( FONT_SIZE + ( BREAKDOWN_INDICATOR_RADIUS ) ) )
                                                                , texIndicatorPosition.y + ( texIndicatorPerpendicular.y * 1.1f * ( FONT_SIZE + ( BREAKDOWN_INDICATOR_RADIUS ) ) ) );
            FText glyphText = FText::AsNumber( iBreakdown->GetSourceDrawingIndex() + i + 1 );
            ::ULIS::FVec2D texFrameNumberPosition;
            UE::Slate::FDeprecateVector2DResult glyphMesure;
            int32 glyphW, glyphH;

            glyphMesure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService()->Measure ( glyphText.ToString()
                                                                                                   , mChartFontInfo
                                                                                                   , 1.0f );
            glyphW = glyphMesure.X;
            glyphH = glyphMesure.Y;

            hudIndicatorPerpendicular.Normalize();

            if( ( ( inbetween->GetSpacing() == 0.0f ) && iDrawSourceIndicator ) || ( inbetween->GetSpacing() == 1.0f ) )
            {
                ::ULIS::FVec2D hudFrameInfoPosition = hudIndicatorPosition + ( 1.1f * hudIndicatorPerpendicular * ( BREAKDOWN_INDICATOR_RADIUS + FONT_SIZE ) );
                ::ULIS::FVec2D hudFrameNumberPosition = ::ULIS::FVec2D( hudFrameInfoPosition.x - ( glyphW    * 0.5f )
                                                                      , hudFrameInfoPosition.y - ( FONT_SIZE * 0.5f ) );
                FCanvasTextItem textItem = FCanvasTextItem( FVector2D( hudFrameNumberPosition.x
                                                                     , hudFrameNumberPosition.y )
                                                          , glyphText
                                                          , mChartFontInfo
                                                          , chartColor );


                if( ( ( inbetween->GetSpacing() == 0.0f ) && prevBreakdown )
                 || ( ( inbetween->GetSpacing() == 1.0f ) && nextBreakdown ) )
                {
                    FVector2D lineP0 = FVector2D ( hudFrameNumberPosition.x         , hudFrameNumberPosition.y + FONT_SIZE + 2 );
                    FVector2D lineP1 = FVector2D ( hudFrameNumberPosition.x + glyphW, hudFrameNumberPosition.y + FONT_SIZE + 2 );

                    // underline
                    DrawPrimitiveLineOutlined( iParams, lineP0, lineP1, chartColor, 1.0f );
                }
                else
                {
                    FVector2D circleCenter = FVector2D ( hudFrameInfoPosition.x, hudFrameInfoPosition.y );

                    DrawPrimitiveCircle( iParams, circleCenter, FONT_SIZE, chartColor, 1.0f );
                }

                // draw indicator
                DrawPrimitiveLineOutlined( iParams
                                         , FVector2D( hudIndicatorPosition.x + ( hudIndicatorPerpendicular.x * BREAKDOWN_INDICATOR_RADIUS )
                                                    , hudIndicatorPosition.y + ( hudIndicatorPerpendicular.y * BREAKDOWN_INDICATOR_RADIUS ) )
                                         , FVector2D( hudIndicatorPosition.x - ( hudIndicatorPerpendicular.x * BREAKDOWN_INDICATOR_RADIUS )
                                                    , hudIndicatorPosition.y - ( hudIndicatorPerpendicular.y * BREAKDOWN_INDICATOR_RADIUS ) )
                                         , chartColor
                                         , 1.0f );

                if( current )
                {
                    textItem.EnableShadow( blackColor );
                }

                iParams.mCanvas->DrawItem( textItem );
            }

            if ( ( inbetween->GetSpacing() > 0.0f ) && ( inbetween->GetSpacing() < 1.0f ) )
            {
                ::ULIS::FVec2D hudFrameInfoPosition = hudIndicatorPosition + ( 1.1f * hudIndicatorPerpendicular * ( INBETWEENER_INDICATOR_RADIUS + FONT_SIZE ) );
                ::ULIS::FVec2D hudFrameNumberPosition = ::ULIS::FVec2D( hudFrameInfoPosition.x - ( glyphW * 0.5f )
                                                                      , hudFrameInfoPosition.y - ( glyphH * 0.5f ) );
                FCanvasTextItem textItem = FCanvasTextItem( FVector2D( hudFrameNumberPosition.x
                                                                     , hudFrameNumberPosition.y )
                                                          , glyphText
                                                          , mChartFontInfo
                                                          , hovered ? iHcColor : inbetweenColor );

                // draw indicator
                DrawPrimitiveLineOutlined( iParams
                                         , FVector2D( hudIndicatorPosition.x + ( hudIndicatorPerpendicular.x * INBETWEENER_INDICATOR_RADIUS )
                                                    , hudIndicatorPosition.y + ( hudIndicatorPerpendicular.y * INBETWEENER_INDICATOR_RADIUS ) )
                                         , FVector2D( hudIndicatorPosition.x - ( hudIndicatorPerpendicular.x * INBETWEENER_INDICATOR_RADIUS )
                                                    , hudIndicatorPosition.y - ( hudIndicatorPerpendicular.y * INBETWEENER_INDICATOR_RADIUS ) )
                                         , hovered ? iHcColor : inbetweenColor
                                         , hovered ? 2.0f: 1.0f );

                if( current )
                {
                    textItem.EnableShadow( blackColor );
                }

                iParams.mCanvas->DrawItem( textItem );

            }
        }
    }

    if( mChartTool->GetEditionMode() == eVectorChartEditionMode::Reshape )
    {
        DrawPrimitiveLineOutlined( iParams, hudCoords[0], hudCoords[1], iFgColor, 1.0f );
        DrawPrimitiveLineOutlined( iParams, hudCoords[1], hudCoords[2], iFgColor, 1.0f );

        for( uint32 i = 0; i < 3; i++ )
        {
            DrawPrimitiveHandle( iParams, hudCoords[i], 3.0f, iFgColor, iBgColor );
        }
    }
}

void
FOdysseyPainterEditorVectorChartToolHUD::DrawHUD( const FOdysseyHUD::FDrawHUDParams& iParams )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    FLinearColor fgColor = FLinearColor( fg );
    FLinearColor bgColor = FLinearColor( bg );
    FLinearColor hcColor = FLinearColor( hc );
    uint64 hudFlags = mChartTool->GetEditor()->GetVectorHUDFlags();
    uint32 cellIndex = mScene->GetCell()->GetIndex();

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

            breakdown->GetInbetweenerTag()->GetOwner()->LockDrawing();

            DrawBreakdown( iParams
                         , breakdown
                         , FLinearColor( 0.5f, 0.5f, 0.5f, 1.0f )
                         , FLinearColor( 1.0f, 0.5f, 0.5f, 1.0f )
                         , FOdysseyVectorHUD::HUD_BREAKDOWN_SOURCE
                         | FOdysseyVectorHUD::HUD_BREAKDOWN_INBETWEEN
                         //| HUD_INBETWEEN_FADEFROMTARGET
                         | FOdysseyVectorHUD::HUD_BREAKDOWN_TARGET );

            if( mChartTool->ChartType == eChartType::Full )
            {
                DrawBreakdownChart( iParams
                                  , fgColor
                                  , bgColor
                                  , hcColor
                                  , breakdown
                                  , breakdown->GetChart()->GetHUDBezier()
                                  , mScene->GetCell()->GetIndex()
                                  , prevBreakdown ? false : true );
            }

            if( mChartTool->ChartType == eChartType::Partial )
            {
                DrawBreakdownChart( iParams
                                  , fgColor
                                  , bgColor
                                  , hcColor
                                  , breakdown
                                  , breakdown->GetChart()->GetHUDBezier()
                                  , mScene->GetCell()->GetIndex()
                                  , true );
            }

           breakdown->GetInbetweenerTag()->GetOwner()->UnlockDrawing();
        }
    }

    // invisible plane will get mouse events
    DrawDummyPlane( iParams );

    //DrawModifierInfo( iParams );
}

void
FOdysseyPainterEditorVectorChartToolHUD::Draw( BLContext* iBLContext )
{
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

#undef LOCTEXT_NAMESPACE
