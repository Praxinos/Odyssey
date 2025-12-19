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

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

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
    FVector2D vector = iParams.mTextureToHUD.Execute( FVector2D( 0, mLiquifyTool->GetRadius() ) )
                     - iParams.mTextureToHUD.Execute( FVector2D( 0, 0 ) );
    double radius = vector.Length();
    uint32 segmentCount = 64;
    double step = ( double ) ( M_PI * 2 ) / segmentCount;
    double angle0 = 0.0f;

    for( uint32 i = 0; i < segmentCount; i++)
    {
        double angle1 = angle0 + step;

        FVector2D hudLineP0 = FVector2D( cos( angle0 ) * radius, sin( angle0 ) * radius ) + cursorPosition;
        FVector2D hudLineP1 = FVector2D( cos( angle1 ) * radius, sin( angle1 ) * radius ) + cursorPosition;
        FCanvasLineItem line = FCanvasLineItem( hudLineP0, hudLineP1 );

        line.LineThickness = 1.0f;
        line.SetColor( iColor );
        iParams.mCanvas->DrawItem( line );

        angle0 = angle1;
    }
}

void
FOdysseyPainterEditorRasterLiquifyToolHUD::DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams )
{
    DrawCursorCircle( iParams, FLinearColor( 1.0f, 0.0f, 0.0f, 1.0f ) );
}

#undef LOCTEXT_NAMESPACE
