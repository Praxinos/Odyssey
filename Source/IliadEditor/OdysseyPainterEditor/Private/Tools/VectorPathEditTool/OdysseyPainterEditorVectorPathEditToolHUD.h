// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPathEditToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        virtual ~FOdysseyPainterEditorVectorPathEditToolHUD(  );
        FOdysseyPainterEditorVectorPathEditToolHUD( UOdysseyPainterEditorVectorPathEditTool* iPathEditTool );

        void DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams );
        virtual void Draw( BLContext* iBLContext ) override;
        virtual void Reset() override;
        virtual void Load() override;
        virtual void Unload() override;
        void ClearMask();
        void GenerateMask( double iTexX
                         , double iTexY
                         , double iRadius );
        BLImage* GetMask();

        virtual void SetCursorPosition( double iWorldX, double iWorldY );

        std::vector<FOdysseyVectorPoint*>& GetHoveredPointArray();
        void SelectObject( FOdysseyVectorGroupPaint* iScene
                         , std::vector<FOdysseyVectorObject*>& oPickedObjectArray );
        void SetCutLineP0( double iX, double iY );
        void SetCutLineP1(  double iX, double iY );
        ::ULIS::FVec2D& GetCutLineP0();
        ::ULIS::FVec2D& GetCutLineP1();

    private:

        void DrawMinus( const FOdysseyHUDElement::FDrawHUDParams& iParams
                      , const FLinearColor& iFgColor
                      , const FLinearColor& iBgColor
                      , float iThickness
                      , bool iContour );

        void DrawPlus( const FOdysseyHUDElement::FDrawHUDParams& iParams
                     , const FLinearColor& iFgColor
                     , const FLinearColor& iBgColor
                     , float iThickness
                     , bool iContour );

    private:
        std::vector<FOdysseyVectorPoint*> mHoveredPointArray;
        UOdysseyPainterEditorVectorPathEditTool* mPathEditTool;
        BLContext mBLSelectionContext;
        BLImage mBLSelectionMask;
        ::ULIS::FVec2D mCutLinePoint[2];
        ::ULIS::FRectD mROI;
};
