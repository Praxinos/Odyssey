// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingToolHUD.h"
#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingTool.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorCell.h"
#include "InbetweenerTag/InbetweenerPoint.h"
#include "InbetweenerTag/InbetweenerQuad.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

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
FOdysseyPainterEditorVectorMatchingToolHUD::Reset()
{
    uint64 hudFlags = mMatchingTool->GetEditor()->GetVectorHUDFlags();

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        UpdateSelectionInbetweenMode( true );
    }
}

void
FOdysseyPainterEditorVectorMatchingToolHUD::DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams )
{
    mCurrentHUDParams = iParams;

    if (!mBaseTool->GetWorkingCell() || !mBaseTool->GetWorkingCell()->GetScene())
        return;

    FOdysseyVectorGroupPaint* scene = mBaseTool->GetWorkingCell()->GetScene();
    FLinearColor fgColor = FLinearColor( FOdysseyVectorHUD::GetForegroundColor() );
    FLinearColor bgColor = FLinearColor( FOdysseyVectorHUD::GetBackgroundColor() );
    FLinearColor hcColor = FLinearColor( FOdysseyVectorHUD::GetHighlightColor() );
    static FLinearColor greyColor = FLinearColor( 0.5f, 0.5f, 0.5f, 0.5f );
    uint64 hudFlags = mMatchingTool->GetEditor()->GetVectorHUDFlags();
    FOdysseyVectorLayer *sharedEnv = scene->GetLayer();
    uint64 gridDotted = mMatchingTool->GridDisplayMode == eMatchingGridDisplayMode::AsPoints ?  FOdysseyVectorHUD::HUD_BREAKDOWN_GRID_DOTTED : 0;
    FVector2D hudCursor = iParams.mTextureToHUD.Execute( FVector2D( mX, mY ) );

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    //FOdysseyPainterEditorVectorBaseToolHUD::Draw( iBLContext, iScene );

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        for( FInbetweenerBreakdown* breakdown : mSelectedBreakdownList )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = breakdown->GetInbetweenerTag();
            // Caution: even though here we pick a tag that is displayed in the scene,
            // it does not mean it belongs to an object that belongs to the scene.
            FOdysseyVectorGroupPaint* inbetweenerTagScene = inbetweenerTag->GetOwner()->GetScene();
            uint32 currentCellIndex = scene->GetCell()->GetIndex();

            if( breakdown->GetTargetCellIndex() == currentCellIndex )
            {
                DrawInbetweens( iParams
                              , breakdown
                              , mMatchingTool->ShowInbetweens
                              , 0
                              , FOdysseyVectorHUD::HUD_BREAKDOWN_TARGET_GRID
                              | gridDotted );
            }
        }
    }

    DrawPrimitiveCircle( iParams
                       , hudCursor
                       , WorldVectorToHUD( ::ULIS::FVec2D( mX, mY )
                                         , ::ULIS::FVec2D( mMatchingTool->PickingRadius, 0 ) ).Distance()
                       , hcColor
                       , 1.0f );

    // invisible plane will get mouse events
    DrawDummyPlane( iParams );
}

void
FOdysseyPainterEditorVectorMatchingToolHUD::Draw( BLContext* iBLContext )
{
}

void
FOdysseyPainterEditorVectorMatchingToolHUD::PickTargetPoints( FInbetweenerBreakdown* iBreakdown
                                                            , double iWorldX
                                                            , double iWorldY
                                                            , double iRadius
                                                            , std::vector<FInbetweenerPoint*>& oPointArray
                                                            , std::vector<float>& oWorldDistanceArray
                                                            , std::vector<FInbetweenerGrid*>& oGridArray )
{
    BLMatrix2D worldMatrix = iBreakdown->GetInbetweenerTag()->GetOwner()->GetWorldMatrix();
    BLMatrix2D& localMatrix = iBreakdown->GetTargetLocalMatrix();
    bool anyPointPicked = false;

    worldMatrix.transform( localMatrix );

    for( FInbetweenerPoint& point : iBreakdown->GetGrid()->GetPointBuffer() )
    {
        BLPoint pt = worldMatrix.map_point( point.GetTargetPosition().x
                                         , point.GetTargetPosition().y );
        ::ULIS::FVec2D vec = ::ULIS::FVec2D( pt.x - iWorldX, pt.y - iWorldY );
        double distance = vec.Distance();

        if( distance <= iRadius )
        {
            oPointArray.push_back( &point );
            oWorldDistanceArray.push_back( distance );

            anyPointPicked = true;
        }
    }

    if( anyPointPicked )
    {
        oGridArray.push_back( iBreakdown->GetGrid() );
    }
}

#undef LOCTEXT_NAMESPACE
