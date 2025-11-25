// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/RasterLiquifyTool/OdysseyPainterEditorRasterLiquifyToolHUD.h"
#include "Tools/RasterLiquifyTool/OdysseyPainterEditorRasterLiquifyTool.h"
#include "OdysseyPainterEditor.h"
#include "Interfaces/IPluginManager.h"
#include "OdysseyVector.h"
// for 3D HUDs
#include "CanvasTypes.h"
#include "CanvasItem.h"

#include "Fonts/FontMeasure.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

FOdysseyPainterEditorRasterLiquifyToolHUD::~FOdysseyPainterEditorRasterLiquifyToolHUD()
{
}

FOdysseyPainterEditorRasterLiquifyToolHUD::FOdysseyPainterEditorRasterLiquifyToolHUD( UOdysseyPainterEditorRasterLiquifyTool* iLiquifyTool )
    : mLiquifyTool ( iLiquifyTool )
{
}

void
FOdysseyPainterEditorRasterLiquifyToolHUD::Load()
{
}

void
FOdysseyPainterEditorRasterLiquifyToolHUD::Reset()
{
    mCursorEllipsePoints.Clear();

    ::ULIS::GenerateEllipsePoints( 0.0f
                                 , mLiquifyTool->GetRadius()
                                 , mLiquifyTool->GetRadius()
                                 , mCursorEllipsePoints );
}

void
FOdysseyPainterEditorRasterLiquifyToolHUD::SetCursorPositionInTexture( const FVector2D& iCursorPositionInTexture )
{
    mCursorPositionInTexture = iCursorPositionInTexture;
}

void
FOdysseyPainterEditorRasterLiquifyToolHUD::DrawCursorCircle( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                           , const FLinearColor& iColor )
{
    FVector2D cursorPosition = iParams.mTextureToHUD.Execute( mCursorPositionInTexture );

    for( uint32 i = 0; i < mCursorEllipsePoints.Size(); i++)
    {
        uint32 n = ( i + 1 ) % mCursorEllipsePoints.Size();
        FVector2D hudLineP0 = cursorPosition + FVector2D( mCursorEllipsePoints[i].x, mCursorEllipsePoints[i].y );;
        FVector2D hudLineP1 = cursorPosition + FVector2D( mCursorEllipsePoints[n].x, mCursorEllipsePoints[n].y );
        FCanvasLineItem hudLine = FCanvasLineItem( hudLineP0, hudLineP1 );
        FCanvasLineItem line = FCanvasLineItem( hudLineP1, hudLineP1 );

        line.LineThickness = 1.0f;
        line.SetColor( iColor );
        iParams.mCanvas->DrawItem( line );
    }
}

void
FOdysseyPainterEditorRasterLiquifyToolHUD::DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams )
{
    DrawCursorCircle( iParams, FLinearColor( 1.0f, 0.0f, 0.0f, 1.0f ) );
}

#undef LOCTEXT_NAMESPACE
