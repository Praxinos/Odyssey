// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditToolHUD.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
// for 3D HUDs
#include "CanvasTypes.h"
#include "CanvasItem.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

FOdysseyPainterEditorVectorPathEditToolHUD::~FOdysseyPainterEditorVectorPathEditToolHUD()
{
}

FOdysseyPainterEditorVectorPathEditToolHUD::FOdysseyPainterEditorVectorPathEditToolHUD( UOdysseyPainterEditorVectorPathEditTool* iPathEditTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iPathEditTool )
    , mPathEditTool( iPathEditTool )
{
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Reset()
{
    uint64 hudFlags = mPathEditTool->GetEditor()->GetVectorHUDFlags();

    // reset cut line by setting both end points at the same location
    mCutLinePoint[0] = mCutLinePoint[1] = ::ULIS::FVec2D( 0.0f, 0.0f );

    MakePointQuadTree( true, hudFlags );

    UpdateSelectionBox( false, hudFlags );
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Load()
{
    FText ctrlInfoText = LOCTEXT("vector-path-edit-tool-hud-info-ctrl", "deform segment" );
    FText shiftInfoText = LOCTEXT("vector-path-edit-tool-hud-info-shift", "widen vertex" );
    FText altInfoText = LOCTEXT("vector-path-edit-tool-hud-info-alt", "add/remove vertex" );
    uint32 width = mPathEditTool->GetViewportWidth();
    uint32 height = mPathEditTool->GetViewportHeight();

    FormatModifierInfo( &ctrlInfoText, &shiftInfoText, &altInfoText );

    mBLSelectionMask.create( width, height, BL_FORMAT_A8 );

    mBLSelectionContext.begin( mBLSelectionMask );

    FOdysseyPainterEditorVectorBaseToolHUD::Load();
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Unload()
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

    mBLSelectionContext.set_comp_op( BL_COMP_OP_SRC_COPY );
    mBLSelectionContext.set_fill_alpha( 0.0f );
    mBLSelectionContext.clear_all();
    mBLSelectionContext.flush( BL_CONTEXT_FLUSH_SYNC );

    mBLSelectionContext.restore();
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::GenerateMask( double iTexX
                                                        , double iTexY
                                                        , double iRadius )
{
    FVector2D hudCoords = mCurrentHUDParams.mTextureToHUD.Execute( FVector2D( iTexX, iTexY ) );

    ClearMask();

    mBLSelectionContext.save();

    mBLSelectionContext.set_comp_op( BL_COMP_OP_SRC_COPY );
    mBLSelectionContext.set_fill_alpha( 1.0f );
    mBLSelectionContext.fill_circle( hudCoords.X, hudCoords.Y, iRadius );
    mBLSelectionContext.flush( BL_CONTEXT_FLUSH_SYNC );

    mBLSelectionContext.restore();

    mROI = ::ULIS::FRectD::FromMinMax( hudCoords.X - iRadius, hudCoords.Y - iRadius
                                     , hudCoords.X + iRadius, hudCoords.Y + iRadius );
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::SelectObject( FOdysseyVectorGroupPaint* iScene
                                                        , std::vector<FOdysseyVectorObject*>& oPickedObjectArray )
{
    Pick( iScene, mBLSelectionMask, mROI, oPickedObjectArray );
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams )
{
    mCurrentHUDParams = iParams;

    if (!mBaseTool->GetWorkingCell() || !mBaseTool->GetWorkingCell()->GetScene())
        return;

    FOdysseyVectorGroupPaint* scene = mBaseTool->GetWorkingCell()->GetScene();
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    FLinearColor fgColor = FLinearColor( fg );
    FLinearColor bgColor = FLinearColor( bg );
    FLinearColor hcColor = FLinearColor( hc );
    eVectorPathEditEditionMode pickingMode = mPathEditTool->GetEditionMode();
    uint64 vertexHandleFlag  = ( pickingMode == eVectorPathEditEditionMode::VertexHandle  ) ? FOdysseyVectorHUD::HUD_PATH_VERTEX_HANDLE  : 0;
    uint64 segmentHandleFlag = ( pickingMode == eVectorPathEditEditionMode::SegmentHandle ) ? FOdysseyVectorHUD::HUD_PATH_SEGMENT_HANDLE
                                                                                  | FOdysseyVectorHUD::HUD_PATH_VERTEX_ALIGNMENT : 0;
    uint64 hudFlags = mPathEditTool->GetEditor()->GetVectorHUDFlags();
    FVector2D hudCursor = iParams.mTextureToHUD.Execute( FVector2D( mX, mY ) );

    //char infoText[255] = "CTRL: deform segment      SHIFT: widen vertex        ALT: add/remove vertex";

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    //FOdysseyPainterEditorVectorBaseToolHUD::DrawHUD( iParams );

    // draw object details in any mode (if statement is useles per-se but here for clarity)
    if( ( hudFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT    )
     || ( hudFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX    )
     || ( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN ) )
    {
        DrawHierarchy( iParams
                     , scene
                     , fgColor
                     , bgColor
                     , hcColor
                     , hudFlags
                     | FOdysseyVectorHUD::HUD_PATH_VERTEX
                     | FOdysseyVectorHUD::HUD_PATH_SEGMENT
                     | vertexHandleFlag
                     | segmentHandleFlag );
    }

    if( mPathEditTool->GetEditionMode() == eVectorPathEditEditionMode::Alter )
    {
        if(  mHoveredPointArray.size() )
        {
            FVector2D minusP0 = FVector2D( hudCursor.X + mPathEditTool->PickingRadius
                                         , hudCursor.Y - mPathEditTool->PickingRadius );
            FVector2D minusP1 = FVector2D( hudCursor.X + mPathEditTool->PickingRadius + 8
                                         , hudCursor.Y - mPathEditTool->PickingRadius );

            DrawPrimitiveLine( iParams, minusP0, minusP1, hcColor, 1.0f );
        }
        else
        {
            FVector2D lineP0 = iParams.mTextureToHUD.Execute( FVector2D( mCutLinePoint[0].x, mCutLinePoint[0].y ) );
            FVector2D lineP1 = iParams.mTextureToHUD.Execute( FVector2D( mCutLinePoint[1].x, mCutLinePoint[1].y ) );

            // we are NOT over a vertex, draw a plus sign
            DrawPrimitivePlus( iParams
                             , FVector2D( hudCursor.X + mPathEditTool->PickingRadius
                                        , hudCursor.Y - mPathEditTool->PickingRadius )
                             , 4
                             , hcColor
                             , 1.0f );

            // cutting Line
            DrawPrimitiveLine( iParams, lineP0, lineP1, hcColor, 1.0f );
        }
    }

    if( ( mPathEditTool->GetEditionMode() == eVectorPathEditEditionMode::Alter ) == 0 )
    {
        // cursor
        DrawPrimitiveCircle( iParams
                           , hudCursor
                           , mPathEditTool->PickingRadius
                           , hcColor
                           , 1.0f );
    }

    // invisible plane will get mouse events
    DrawDummyPlane( iParams );

    //DrawModifierInfo( iParams );
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::Draw( BLContext* iBLContext )
{

}

std::vector<FOdysseyVectorPoint*>&
FOdysseyPainterEditorVectorPathEditToolHUD::GetHoveredPointArray()
{
    return mHoveredPointArray;
}

void
FOdysseyPainterEditorVectorPathEditToolHUD::SetCursorPosition( double iWorldX, double iWorldY )
{
    FOdysseyPainterEditorVectorBaseToolHUD::SetCursorPosition( iWorldX, iWorldY );

    if( mPathEditTool->GetEditionMode() == eVectorPathEditEditionMode::Alter )
    {
        mHoveredPointArray.clear();

        PickPoints( iWorldX, iWorldY, mPathEditTool->PickingRadius, mHoveredPointArray );
    }
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

#undef LOCTEXT_NAMESPACE
