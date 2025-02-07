// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

        void DrawHUD( const FOdysseyHUDSystem::FDrawHUDParams& iParams );
        virtual void Draw( BLContext* iBLContext ) override;
        virtual void Reset() override;
        virtual void Load() override;
        virtual void Unload() override;
        void ClearMask();
        ::ULIS::FRectD GenerateMask( double iX
                                   , double iY
                                   , double iRadius );
        BLImage* GetMask();

        bool SetCursorPosition( double iX, double iY );
        std::vector<FOdysseyVectorPoint*>& GetHoveredPointArray();

        void SetCutLineP0( double iX, double iY );
        void SetCutLineP1(  double iX, double iY );
        ::ULIS::FVec2D& GetCutLineP0();
        ::ULIS::FVec2D& GetCutLineP1();

    private:

        void DrawMinus( const FOdysseyHUDSystem::FDrawHUDParams& iParams
                      , const FLinearColor& iFgColor
                      , const FLinearColor& iBgColor
                      , float iThickness
                      , bool iContour );

        void DrawPlus( const FOdysseyHUDSystem::FDrawHUDParams& iParams
                     , const FLinearColor& iFgColor
                     , const FLinearColor& iBgColor
                     , float iThickness
                     , bool iContour );

    private:
        double mX;
        double mY;
        std::vector<FOdysseyVectorPoint*> mHoveredPointArray;
        UOdysseyPainterEditorVectorPathEditTool* mPathEditTool;
        BLContext mBLSelectionContext;
        BLImage mBLSelectionMask;
        ::ULIS::FVec2D mCutLinePoint[2];
};
