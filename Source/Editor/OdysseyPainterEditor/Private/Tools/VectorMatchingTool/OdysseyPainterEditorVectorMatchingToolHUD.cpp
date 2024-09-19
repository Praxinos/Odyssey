#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingToolHUD.h"
#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingTool.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorAnimationCell.h"
#include "InbetweenerTag/InbetweenerPoint.h"
#include "InbetweenerTag/InbetweenerQuad.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"

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
    uint64 hudFlags = mMatchingTool->GetEditor()->GetVectorHUDFlags();

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        UpdateSelectionInbetweenMode( iScene );
    }
}

void
FOdysseyPainterEditorVectorMatchingToolHUD::DrawTargetGrid( BLContext* iBLContext
                                                          , FInbetweenerBreakdown* iBreakdown
                                                          , const BLRgba32& iFgColor
                                                          , const BLRgba32& iBgColor
                                                          , const BLRgba32& iHcColor )
{
    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setStrokeStyle( iHcColor );
    iBLContext->setStrokeWidth( 1.0f );

    BLMatrix2D worldMatrix = iBreakdown->GetInbetweenerTag()->GetOwner()->GetWorldMatrix();
    BLMatrix2D& localMatrix = iBreakdown->GetTargetLocalMatrix();

    worldMatrix.transform( localMatrix );

    for( FInbetweenerQuad& quad : iBreakdown->GetGrid()->GetQuadBuffer() )
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
    FOdysseyVectorEngine *engine = iScene->GetEngine();
    FOdysseyVectorSharedEnv *sharedEnv = iScene->GetSharedEnv();

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    //FOdysseyPainterEditorVectorBaseToolHUD::Draw( iBLContext, iScene );

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        // Caution: even though here we pick a tag that is displayed in the scene,
        // it does not mean it belongs to an object that belongs to the scene.
        for( FOdysseyVectorTag* tag : sharedEnv->GetSharedTagList() )
        {
            if( ( tag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass() )
             && ( tag->GetOwner()->IsSelected() ) )
            {
                FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
                FOdysseyVectorGroupPaint* inbetweenerTagScene = inbetweenerTag->GetOwner()->GetScene();
                uint32 currentCellIndex = iScene->GetEngine()->GetAnimationCell()->GetIndex();

                for( FInbetweenerBreakdown* breakdown : inbetweenerTag->GetBreakdownList() )
                {
                    if( breakdown->GetTargetAnimationCellIndex() == currentCellIndex )
                    {
                        DrawBreakdown( iBLContext
                                     , breakdown
                                     , BLRgba32( 127, 127, 127, 255 )
                                     , BLRgba32( 255, 127, 127, 255 )
                                     , HUD_BREAKDOWN_SOURCE | HUD_BREAKDOWN_TARGET | HUD_BREAKDOWN_INBETWEEN );

                        DrawTargetGrid ( iBLContext
                                       , breakdown
                                       , fgColor
                                       , bgColor
                                       , hcColor );
                    }
                }
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
FOdysseyPainterEditorVectorMatchingToolHUD::PickTargetPoints( FInbetweenerBreakdown* iBreakdown
                                                            , double iWorldX
                                                            , double iWorldY
                                                            , double iRadius
                                                            , std::vector<FInbetweenerPoint*>& oPointArray
                                                            , std::vector<FInbetweenerGrid*>& oGridArray )
{
    BLMatrix2D worldMatrix = iBreakdown->GetInbetweenerTag()->GetOwner()->GetWorldMatrix();
    BLMatrix2D& localMatrix = iBreakdown->GetTargetLocalMatrix();
    bool anyPointPicked = false;

    worldMatrix.transform( localMatrix );

    for( FInbetweenerPoint& point : iBreakdown->GetGrid()->GetPointBuffer() )
    {
        BLPoint pt = worldMatrix.mapPoint( point.GetTargetPosition().x
                                            , point.GetTargetPosition().y );
        ::ULIS::FVec2D vec = ::ULIS::FVec2D( pt.x - iWorldX, pt.y - iWorldY );
        double distance = vec.Distance();

        if( distance <= iRadius )
        {
            oPointArray.push_back( &point );

            anyPointPicked = true;
        }
    }

    if( anyPointPicked )
    {
        oGridArray.push_back( iBreakdown->GetGrid() );
    }
}
