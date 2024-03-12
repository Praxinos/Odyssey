#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorPathStitchToolHUD::~FOdysseyPainterEditorVectorPathStitchToolHUD()
{
}

FOdysseyPainterEditorVectorPathStitchToolHUD::FOdysseyPainterEditorVectorPathStitchToolHUD( UOdysseyPainterEditorVectorPathStitchTool* iPathStitchTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iPathStitchTool )
    , mPathStitchTool( iPathStitchTool )
{
}

void
FOdysseyPainterEditorVectorPathStitchToolHUD::Reset( FOdysseyVectorGroupPaint* iScene )
{
    uint64 hudFlags = mPathStitchTool->GetEditor()->GetVectorHUDFlags();

    mPickedPointArray.reserve(10);
    mPickedPointArray.clear();

    mStitchableVertex[0] = nullptr;
    mStitchableVertex[1] = nullptr;

    MakePointQuadTree( iScene, true, hudFlags );

    UpdateSelectionBox( iScene, false, hudFlags );
}

void
FOdysseyPainterEditorVectorPathStitchToolHUD::Load( FOdysseyVectorGroupPaint* iScene )
{
}

void
FOdysseyPainterEditorVectorPathStitchToolHUD::Unload( FOdysseyVectorGroupPaint* iScene )
{
}

FOdysseyVectorVertex**
FOdysseyPainterEditorVectorPathStitchToolHUD::GetStitchableVertices()
{
    return mStitchableVertex;
}

void
FOdysseyPainterEditorVectorPathStitchToolHUD::SetPosition( double iWorldX, double iWorldY )
{
    mX = iWorldX;
    mY = iWorldY;

    mStitchableVertex[0] = nullptr;
    mStitchableVertex[1] = nullptr;
    mPickedPointArray.clear();

    PickPoints( iWorldX, iWorldY, mPathStitchTool->PickingRadius, mPickedPointArray );

    for( int i = 0; i < mPickedPointArray.size(); i++ )
    {
        if( mPickedPointArray[i]->GetClass() == FOdysseyVectorVertex::StaticClass() )
        {
            FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(mPickedPointArray[i]);

            if( vertex->GetSegmentCount() == 1 )
            {
                if( mStitchableVertex[0] == nullptr )
                {
                    mStitchableVertex[0] = vertex;
                }
                else
                {
                    if( vertex->GetFirstSegment() != mStitchableVertex[0]->GetFirstSegment() )
                    {
                        mStitchableVertex[1] = vertex;

                        return;
                    }
                }
            }
        }
    }
}

void
FOdysseyPainterEditorVectorPathStitchToolHUD::Draw( BLContext* iBLContext
                                                  , FOdysseyVectorGroupPaint* iScene )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint64 hudFlags = mPathStitchTool->GetEditor()->GetVectorHUDFlags();

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX )
    {
        DrawObjects( iBLContext
                   , iScene
                   , fgColor
                   , bgColor
                   , hcColor
                   , hudFlags | HUD_PATH_VERTEX | HUD_PATH_SEGMENT );
    }

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT )
    {
        DrawObjects( iBLContext
                   , iScene
                   , fgColor
                   , bgColor
                   , hcColor
                   , hudFlags | HUD_PATH_VERTEX_VALENCE1 | HUD_PATH_SEGMENT );
    }

    // draw selection box only if we restrict erasure to the selection 
/*
    if( mPathStitchTool->RestrictToSelectedObjects )
    {
        DrawSelectionBox( iBLContext, iScene, fgColor, bgColor, hcColor, hudFlags );
    }
*/
    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setCompOp( BL_COMP_OP_SRC_COPY );

    iBLContext->setStrokeStyle( hcColor );
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->strokeCircle( mX, mY, mPathStitchTool->PickingRadius );

    if( mStitchableVertex[0] && mStitchableVertex[1] )
    {
        BLRgba32 orange = BLRgba32( 255, 127, 0, 255 );

        DrawPath( iBLContext, mStitchableVertex[0]->GetOwnerAsPath(), orange, bgColor, hcColor, true, HUD_PATH_SEGMENT );
        DrawPath( iBLContext, mStitchableVertex[1]->GetOwnerAsPath(), orange, bgColor, hcColor, true, HUD_PATH_SEGMENT );

        DrawVertex( iBLContext, mStitchableVertex[0], orange, bgColor, hcColor, true, 0 );
        DrawVertex( iBLContext, mStitchableVertex[1], orange, bgColor, hcColor, true, 0 );
    }

    iBLContext->restore();
}
