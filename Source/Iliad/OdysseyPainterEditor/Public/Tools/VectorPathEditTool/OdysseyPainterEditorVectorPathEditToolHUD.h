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

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Reset( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Load( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Unload( FOdysseyVectorGroupPaint* iScene ) override;
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
        void DrawMinus( BLContext* iBLContext
                      , const BLRgba32& iFgColor
                      , const BLRgba32& iBgColor
                      , const BLRgba32& iHcColor  );
        void DrawPlus( BLContext* iBLContext
                     , const BLRgba32& iFgColor
                     , const BLRgba32& iBgColor
                     , const BLRgba32& iHcColor  );

    private:
        double mX;
        double mY;
        std::vector<FOdysseyVectorPoint*> mHoveredPointArray;
        UOdysseyPainterEditorVectorPathEditTool* mPathEditTool;
        BLContext mBLSelectionContext;
        BLImage mBLSelectionMask;
        ::ULIS::FVec2D mCutLinePoint[2];
};
