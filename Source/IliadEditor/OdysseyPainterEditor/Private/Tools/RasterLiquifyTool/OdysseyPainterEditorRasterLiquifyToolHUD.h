// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <blend2d.h>
#include <ULIS>

#include "OdysseyHUDElement.h"

class UOdysseyPainterEditorRasterLiquifyTool;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorRasterLiquifyToolHUD : public FOdysseyHUDElement
{
    public:
        virtual ~FOdysseyPainterEditorRasterLiquifyToolHUD();
        FOdysseyPainterEditorRasterLiquifyToolHUD(  UOdysseyPainterEditorRasterLiquifyTool* iLiquifyTool );

        virtual void Load();
        virtual void DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams ) override;
        //virtual void Draw( BLContext* iBLContext ) override;
        virtual void Reset();
        void SetCursorPositionInTexture( const FVector2D& iCursorPositionInTexture );

    protected:
        void DrawCursorCircle( const FOdysseyHUDElement::FDrawHUDParams& iParams
                             , const FLinearColor& iColor );

    private:
        UOdysseyPainterEditorRasterLiquifyTool* mLiquifyTool;
        FVector2D mCursorPositionInTexture;
};
