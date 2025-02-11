// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchToolHUD.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyPainterEditor.h"
#include "SOdysseyViewport.h"

FOdysseyPainterEditorVectorPathStitchToolHUD::~FOdysseyPainterEditorVectorPathStitchToolHUD()
{
}

FOdysseyPainterEditorVectorPathStitchToolHUD::FOdysseyPainterEditorVectorPathStitchToolHUD( UOdysseyPainterEditorVectorPathStitchTool* iPathStitchTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iPathStitchTool )
    , mPathStitchTool( iPathStitchTool )
{
}

void
FOdysseyPainterEditorVectorPathStitchToolHUD::Reset()
{
    uint64 hudFlags = mPathStitchTool->GetEditor()->GetVectorHUDFlags();

    mPickedPointArray.reserve(10);
    mPickedPointArray.clear();

    mStitchableVertex[0] = nullptr;
    mStitchableVertex[1] = nullptr;

    MakePointQuadTree( true, hudFlags );

    UpdateSelectionBox( false, hudFlags );
}

void
FOdysseyPainterEditorVectorPathStitchToolHUD::Load()
{
    FOdysseyPainterEditorVectorBaseToolHUD::Load();
}

void
FOdysseyPainterEditorVectorPathStitchToolHUD::Unload()
{
}

FOdysseyVectorVertex**
FOdysseyPainterEditorVectorPathStitchToolHUD::GetStitchableVertices()
{
    return mStitchableVertex;
}

void
FOdysseyPainterEditorVectorPathStitchToolHUD::SetPosition( double iWorldX
                                                         , double iWorldY )
{
    mX = iWorldX;
    mY = iWorldY;

    mStitchableVertex[0] = nullptr;
    mStitchableVertex[1] = nullptr;
    mPickedPointArray.clear();

    PickPoints( iWorldX, iWorldY, mPathStitchTool->PickingRadius / mPathStitchTool->GetViewport()->GetZoom(), mPickedPointArray );

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
FOdysseyPainterEditorVectorPathStitchToolHUD::DrawHUD( const FOdysseyHUDSystem::FDrawHUDParams& iParams )
{
    FLinearColor fgColor = FLinearColor( FOdysseyVectorHUD::GetForegroundColor() );
    FLinearColor bgColor = FLinearColor( FOdysseyVectorHUD::GetBackgroundColor() );
    FLinearColor hcColor = FLinearColor( FOdysseyVectorHUD::GetHighlightColor() );
    FLinearColor noColor = FLinearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    uint64 hudFlags = mBaseTool->GetEditor()->GetVectorHUDFlags();
    FVector2D hudCursor = iParams.mTextureToHUD.Execute( FVector2D( mX, mY ) );

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    FOdysseyPainterEditorVectorBaseToolHUD::DrawHUD( iParams );

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT )
    {
        DrawHierarchy( iParams
                     , mScene
                     , fgColor
                     , bgColor
                     , hcColor
                     , hudFlags
                     | FOdysseyVectorHUD::HUD_PATH_VERTEX_VALENCE1
                     | FOdysseyVectorHUD::HUD_PATH_SEGMENT );
    }

    // draw selection box only if we restrict erasure to the selection
/*
    if( mPathStitchTool->RestrictToSelectedObjects )
    {
        DrawSelectionBox( iBLContext, iScene, fgColor, bgColor, hcColor, hudFlags );
    }
*/
    // cursor
    DrawPrimitiveCircle( iParams, hudCursor, mPathStitchTool->PickingRadius, hcColor, noColor, 1.0f, false );

    if( mStitchableVertex[0] && mStitchableVertex[1] )
    {
        FLinearColor orange = FLinearColor( 1.0f, 0.5f, 0.0f, 1.0f );

        DrawPath( iParams, mStitchableVertex[0]->GetOwnerAsPath(), orange, bgColor, hcColor, FOdysseyVectorHUD::HUD_PATH_SEGMENT );
        DrawPath( iParams, mStitchableVertex[1]->GetOwnerAsPath(), orange, bgColor, hcColor, FOdysseyVectorHUD::HUD_PATH_SEGMENT );

        DrawVertex( iParams, mStitchableVertex[0], orange, bgColor, hcColor, 0 );
        DrawVertex( iParams, mStitchableVertex[1], orange, bgColor, hcColor, 0 );
    }

}

void
FOdysseyPainterEditorVectorPathStitchToolHUD::Draw( BLContext* iBLContext )
{
}
