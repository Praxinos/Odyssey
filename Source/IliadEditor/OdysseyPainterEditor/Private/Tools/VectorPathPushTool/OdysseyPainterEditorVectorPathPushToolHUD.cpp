// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushToolHUD.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorGroupPaint.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

FOdysseyPainterEditorVectorPathPushToolHUD::~FOdysseyPainterEditorVectorPathPushToolHUD()
{
}

FOdysseyPainterEditorVectorPathPushToolHUD::FOdysseyPainterEditorVectorPathPushToolHUD( UOdysseyPainterEditorVectorPathPushTool* iPathPushTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iPathPushTool )
    , mPathPushTool( iPathPushTool )
    , mX( 0.0f )
    , mY( 0.0f )
{

}

void
FOdysseyPainterEditorVectorPathPushToolHUD::Reset()
{
    // Updates the selection box
    UpdateSelectionBox( false, mPathPushTool->GetEditor()->GetVectorHUDFlags() );
}

void
FOdysseyPainterEditorVectorPathPushToolHUD::Load()
{
    FOdysseyPainterEditorVectorBaseToolHUD::Load();
}

void
FOdysseyPainterEditorVectorPathPushToolHUD::Unload( )
{
}

void
FOdysseyPainterEditorVectorPathPushToolHUD::DrawHUD( const FOdysseyHUD::FDrawHUDParams& iParams )
{
    FLinearColor fgColor = FLinearColor( FOdysseyVectorHUD::GetForegroundColor() );
    FLinearColor bgColor = FLinearColor( FOdysseyVectorHUD::GetBackgroundColor() );
    FLinearColor hcColor = FLinearColor( FOdysseyVectorHUD::GetHighlightColor() );
    uint64 hudFlags = mPathPushTool->GetEditor()->GetVectorHUDFlags();
    FVector2D hudCursor = iParams.mTextureToHUD.Execute( FVector2D( mX, mY ) );

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    FOdysseyPainterEditorVectorBaseToolHUD::DrawHUD( iParams );

    // cursor
    DrawPrimitiveCircle( iParams
                       , hudCursor
                       , WorldVectorToHUD( iParams
                                         , ::ULIS::FVec2D( mX, mY )
                                         , ::ULIS::FVec2D( mPathPushTool->Radius, 0 ) ).Distance()
                       , hcColor
                       , 1.0f );

    // invisible plane will get mouse events
    DrawDummyPlane( iParams );
}

void
FOdysseyPainterEditorVectorPathPushToolHUD::Draw( BLContext* iBLContext )
{
}

void
FOdysseyPainterEditorVectorPathPushToolHUD::SetCursorPosition( double iX, double iY )
{
    mX = iX;
    mY = iY;
}

#undef LOCTEXT_NAMESPACE
