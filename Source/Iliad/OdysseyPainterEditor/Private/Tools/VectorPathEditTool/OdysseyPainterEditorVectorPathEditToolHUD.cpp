// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditToolHUD.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyPainterEditor.h"

FOdysseyPainterEditorVectorPathEditToolHUD::~FOdysseyPainterEditorVectorPathEditToolHUD()
{
}

FOdysseyPainterEditorVectorPathEditToolHUD::FOdysseyPainterEditorVectorPathEditToolHUD( UOdysseyPainterEditorVectorPathEditTool* iPathEditTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iPathEditTool )
    , mPathEditTool( iPathEditTool )
{
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Reset( FOdysseyVectorGroupPaint* iScene )
{
    uint64 hudFlags = mPathEditTool->GetEditor()->GetVectorHUDFlags();

    // reset cut line by setting both end points at the same location
    mCutLinePoint[0] = mCutLinePoint[1] = ::ULIS::FVec2D( 0.0f, 0.0f );

    MakePointQuadTree( iScene, true, hudFlags );

    // Updates the selection box
    UpdateSelectionBox( iScene, false, hudFlags );
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Load( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    uint32 width = vectorEngine->GetPreferredWidth();
    uint32 height = vectorEngine->GetPreferredHeight();

    mBLSelectionMask.create( width, height, BL_FORMAT_A8 );

    mBLSelectionContext.begin( mBLSelectionMask );
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Unload( FOdysseyVectorGroupPaint* iScene )
{
    mBLSelectionContext.end();
}

BLImage*
FOdysseyPainterEditorVectorPathEditToolHUD::GetMask()
{
    return &mBLSelectionMask;
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::ClearMask()
{
    mBLSelectionContext.save();

    mBLSelectionContext.setCompOp( BL_COMP_OP_SRC_COPY );
    mBLSelectionContext.setFillAlpha( 0.0f );
    mBLSelectionContext.clearAll();
    mBLSelectionContext.flush( BL_CONTEXT_FLUSH_SYNC );

    mBLSelectionContext.restore();
}

::ULIS::FRectD
FOdysseyPainterEditorVectorPathEditToolHUD::GenerateMask( double iX
                                                        , double iY
                                                        , double iRadius )
{
    mBLSelectionContext.save();

    mBLSelectionContext.setCompOp( BL_COMP_OP_SRC_COPY );
    mBLSelectionContext.setFillAlpha( 1.0f );
    mBLSelectionContext.fillCircle( iX, iY, iRadius );
    mBLSelectionContext.flush( BL_CONTEXT_FLUSH_SYNC );

    mBLSelectionContext.restore();

    return ::ULIS::FRectD::FromMinMax( iX - iRadius, iY - iRadius
                                     , iX + iRadius, iY + iRadius );
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::DrawMinus( BLContext* iBLContext
                                                     , const BLRgba32& iFgColor
                                                     , const BLRgba32& iBgColor
                                                     , const BLRgba32& iHcColor  )
{
    BLRgba32 blackColor = BLRgba32( 0, 0, 0, 255 );

    iBLContext->setStrokeStyle( blackColor );
    iBLContext->setStrokeWidth( 2.0f );
    // we are over a vertex, draw a minus sign
    iBLContext->strokeLine( mX + mPathEditTool->PickingRadius
                          , mY - mPathEditTool->PickingRadius
                          , mX + mPathEditTool->PickingRadius + 8
                          , mY - mPathEditTool->PickingRadius );

    iBLContext->setStrokeStyle( iHcColor );
    iBLContext->setStrokeWidth( 1.0f );
    // we are over a vertex, draw a minus sign
    iBLContext->strokeLine( mX + mPathEditTool->PickingRadius
                          , mY - mPathEditTool->PickingRadius
                          , mX + mPathEditTool->PickingRadius + 8
                          , mY - mPathEditTool->PickingRadius );
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::DrawPlus( BLContext* iBLContext
                                                    , const BLRgba32& iFgColor
                                                    , const BLRgba32& iBgColor
                                                    , const BLRgba32& iHcColor  )
{
    BLRgba32 blackColor = BLRgba32( 0, 0, 0, 255 );

    iBLContext->setStrokeStyle( blackColor );
    iBLContext->setStrokeWidth( 2.0f );
    // we are over a vertex, draw a minus sign
    iBLContext->strokeLine( mX + mPathEditTool->PickingRadius
                          , mY - mPathEditTool->PickingRadius
                          , mX + mPathEditTool->PickingRadius + 8
                          , mY - mPathEditTool->PickingRadius );
    iBLContext->strokeLine( mX + mPathEditTool->PickingRadius + 4
                          , mY - mPathEditTool->PickingRadius - 4
                          , mX + mPathEditTool->PickingRadius + 4
                          , mY - mPathEditTool->PickingRadius + 4 );

    iBLContext->setStrokeStyle( iHcColor );
    iBLContext->setStrokeWidth( 1.0f );
    // we are over a vertex, draw a minus sign
    iBLContext->strokeLine( mX + mPathEditTool->PickingRadius
                          , mY - mPathEditTool->PickingRadius
                          , mX + mPathEditTool->PickingRadius + 8
                          , mY - mPathEditTool->PickingRadius );
    iBLContext->strokeLine( mX + mPathEditTool->PickingRadius + 4
                          , mY - mPathEditTool->PickingRadius - 4
                          , mX + mPathEditTool->PickingRadius + 4
                          , mY - mPathEditTool->PickingRadius + 4 );
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Draw( BLContext* iBLContext
                                                , FOdysseyVectorGroupPaint* iScene )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    ePathPickingMode pickingMode = mPathEditTool->GetPickingMode();
    uint64 vertexHandleFlag  = ( pickingMode == ePathPickingMode::VertexHandle  ) ? HUD_PATH_VERTEX_HANDLE  : 0;
    uint64 segmentHandleFlag = ( pickingMode == ePathPickingMode::SegmentHandle ) ? HUD_PATH_SEGMENT_HANDLE
                                                                                  | HUD_PATH_VERTEX_ALIGNMENT : 0;
    uint64 hudFlags = mPathEditTool->GetEditor()->GetVectorHUDFlags();

    // draw object details in any mode (if statement is useles per-se but here for clarity)
    if( ( hudFlags & HUD_MODE_VERTEX ) || ( hudFlags & HUD_MODE_OBJECT ) )
    {
        DrawObjects( iBLContext
                   , iScene
                   , fgColor
                   , bgColor
                   , hcColor
                   , hudFlags
                   | HUD_PATH_VERTEX
                   | HUD_PATH_SEGMENT
                   | vertexHandleFlag
                   | segmentHandleFlag );
    }

    // draw selection box only if we restrict erasure to the selection
    if( iScene->GetEngine()->GetSelectedObjectList().size() )
//    {
//        DrawSelectionBox( iBLContext, iScene, fgColor, bgColor, hcColor, hudFlags );
//    }

    iBLContext->save();
    iBLContext->setCompOp( BL_COMP_OP_SRC_COPY );
    iBLContext->setStrokeStyle( hcColor );
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->strokeCircle( mX, mY, mPathEditTool->PickingRadius );

    if( mPathEditTool->GetPickingMode() == ePathPickingMode::Alter )
    {
        if(  mHoveredPointArray.size() )
        {
            // we are over a vertex, draw a minus sign
            DrawMinus( iBLContext, fgColor, bgColor, hcColor );
        }
        else
        {
            // we are NOT over a vertex, draw a plus sign
            DrawPlus( iBLContext, fgColor, bgColor, hcColor );

            // cutting Line
            iBLContext->setStrokeStyle( hcColor );
            iBLContext->strokeLine( mCutLinePoint[0].x
                                  , mCutLinePoint[0].y
                                  , mCutLinePoint[1].x
                                  , mCutLinePoint[1].y );
        }
    }


    iBLContext->restore();
}

std::vector<FOdysseyVectorPoint*>&
FOdysseyPainterEditorVectorPathEditToolHUD::GetHoveredPointArray()
{
    return mHoveredPointArray;
}

bool
FOdysseyPainterEditorVectorPathEditToolHUD::SetCursorPosition( double iX, double iY )
{
    bool needsFullRedrawing = false;

    mX = iX;
    mY = iY;

    if( mPathEditTool->GetPickingMode() == ePathPickingMode::Alter )
    {
        mHoveredPointArray.clear();

        PickPoints( iX, iY, mPathEditTool->PickingRadius, mHoveredPointArray );
    }

    return needsFullRedrawing;
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::SetCutLineP0( double iX, double iY )
{
    mCutLinePoint[0].x = iX;
    mCutLinePoint[0].y = iY;
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::SetCutLineP1(  double iX, double iY )
{
    mCutLinePoint[1].x = iX;
    mCutLinePoint[1].y = iY;
}

::ULIS::FVec2D&
FOdysseyPainterEditorVectorPathEditToolHUD::GetCutLineP0()
{
    return mCutLinePoint[0];
}

::ULIS::FVec2D&
FOdysseyPainterEditorVectorPathEditToolHUD::GetCutLineP1()
{
    return mCutLinePoint[1];
}
