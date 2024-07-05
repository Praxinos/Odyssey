#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingToolHUD.h"
#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingTool.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTagInbetweener.h"
#include "InbetweenerTag/InbetweenerPoint.h"
#include "InbetweenerTag/InbetweenerQuad.h"

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
                                                    , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                    , const BLRgba32& iFgColor
                                                    , const BLRgba32& iBgColor
                                                    , const BLRgba32& iHcColor )
{
    BLMatrix2D worldMatrix = iInbetweenerTag->GetTargetWorldMatrix();

    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setStrokeStyle( iHcColor );
    iBLContext->setStrokeWidth( 1.0f );

    for( FInbetweenerQuad& quad : iInbetweenerTag->GetGridQuadBuffer() )
    {
        if( quad.IsLinked() )
        {
            FInbetweenerPoint** gridPoint = quad.GetPoints();

            BLPoint pt[4] = { worldMatrix.mapPoint( gridPoint[0]->GetTargetPosition().x
                                                  , gridPoint[0]->GetTargetPosition().y )
                            , worldMatrix.mapPoint( gridPoint[1]->GetTargetPosition().x
                                                  , gridPoint[1]->GetTargetPosition().y )
                            , worldMatrix.mapPoint( gridPoint[2]->GetTargetPosition().x
                                                  , gridPoint[2]->GetTargetPosition().y )
                            , worldMatrix.mapPoint( gridPoint[3]->GetTargetPosition().x
                                                  , gridPoint[3]->GetTargetPosition().y ) };

            iBLContext->strokeLine( pt[0], pt[1] );
            iBLContext->strokeLine( pt[1], pt[2] );
            iBLContext->strokeLine( pt[2], pt[3] );
            iBLContext->strokeLine( pt[3], pt[0] );
        }
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
    static BLRgba32 greyColor = BLRgba32( 128, 128, 128, 128 );
    uint64 hudFlags = mMatchingTool->GetEditor()->GetVectorHUDFlags();

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    //FOdysseyPainterEditorVectorBaseToolHUD::Draw( iBLContext, iScene );

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        FOdysseyVectorObject* selectedObject = iScene->GetEngine()->GetLastSelectedObject();

        DrawObjects( iBLContext
                   , iScene
                   , greyColor
                   , bgColor
                   , hcColor
                   , hudFlags | HUD_TAGINBETWEENER_TARGET | HUD_DRAW_ALL );

        DrawObjects( iBLContext
                   , iScene
                   , fgColor
                   , bgColor
                   , hcColor
                   , hudFlags | HUD_TAGINBETWEENER_TARGET );

        if( selectedObject )
        {
            FOdysseyVectorTag* tag = selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

            if( tag )
            {
                FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>( tag );

                DrawGrid ( iBLContext
                         , inbetweenerTag
                         , fgColor
                         , bgColor
                         , hcColor );
            }
        }
    }

    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setStrokeStyle( hcColor );
    iBLContext->setStrokeWidth( 1.0f );

    iBLContext->strokeCircle( mCursorPosition.x
                            , mCursorPosition.y
                            , mMatchingTool->PickingRadius );

    iBLContext->restore();
}

void
FOdysseyPainterEditorVectorMatchingToolHUD::SetCursorPosition( double iX, double iY )
{
    mCursorPosition.x = iX;
    mCursorPosition.y = iY;
}

void
FOdysseyPainterEditorVectorMatchingToolHUD::PickTargetPoints( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                            , double iWorldX
                                                            , double iWorldY
                                                            , double iRadius
                                                            , std::vector<FInbetweenerPoint*>& oPointArray
                                                            , std::vector<double>& oWorldDistanceArray )
{
    BLMatrix2D worldMatrix = iInbetweenerTag->GetTargetWorldMatrix();

    for( FInbetweenerPoint& point : iInbetweenerTag->GetGridPointBuffer() )
    {
        BLPoint pt = worldMatrix.mapPoint( point.GetTargetPosition().x
                                         , point.GetTargetPosition().y );
        ::ULIS::FVec2D vec = ::ULIS::FVec2D( pt.x - iWorldX, pt.y - iWorldY );
        double distance = vec.Distance();

        if( distance <= iRadius )
        {
            oPointArray.push_back( &point );
            oWorldDistanceArray.push_back( distance );
        }
    }
}
