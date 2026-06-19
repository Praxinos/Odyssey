// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionTool.h"
#include "OdysseyHUDElement.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorSelectionToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        virtual ~FOdysseyPainterEditorVectorSelectionToolHUD();
        FOdysseyPainterEditorVectorSelectionToolHUD(  UOdysseyPainterEditorVectorSelectionTool* iSelectionTool );

        virtual void Draw( BLContext* iBLContext ) override;
        virtual void Load() override;
        virtual void Unload() override;
        virtual void Reset( ) override;
        void Init( uint32 iWidth, uint32 iHeight );
        void GetSelectedVertices( std::vector<FOdysseyVectorPoint*>& oPointArray );

        void ClearMask();
        BLImage* GetMask();

        /**
         * @brief Fill the mask image with a shape defined by a array of points.
         * @param iPointArray a reference to the array of points.
         */
        void GenerateFreehandMask( const std::vector<::ULIS::FVec2D>& iTexPointArray );

        /**
         * @brief Fill the mask image with an alpha Circle.
         * @param iX the circle's center on X axis.
         * @param iY the circle's center on Y axis.
         * @param iRadius the circle's radius.
         * @return the bounding box including the circle.
         */
        void GenerateCircleMask( double iX, double iY, double iRadius );

        /**
         * @brief Fill the mask image with an alpha Rectangle.
         * @param iRect the rectangle
         * @return the bounding box including the rectangle.
         */
        void GenerateRectangleMask( const std::vector<::ULIS::FVec2D>& iTexPointArray );
        void GenerateMask( const std::vector<::ULIS::FVec2D>& iTexPointArray
                         , EOdysseyShapeType iActiveShapeType );
        virtual void DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams ) override;
        void SelectObject( FOdysseyVectorGroupPaint* iScene
                         , std::vector<FOdysseyVectorObject*>& oPickedObjectArray );

    protected:
        void DrawPickingArea( BLContext* iBLContext
                            , BLRgba32 fgColor
                            , BLRgba32 bgColor
                            , BLRgba32 hcColor );
        void DrawPickingArea( const FOdysseyHUDElement::FDrawHUDParams& iParams
                            , const FLinearColor& fgColor
                            , const FLinearColor& bgColor
                            , const FLinearColor& hcColor );

        void DrawVertexSelection( BLContext* iBLContext, uint64 iFlags );
        void DrawObjectSelection( BLContext* iBLContext, uint64 iFlags );
        void DrawSelectionSpace( BLContext* iBLContext, uint64 iFlags);

    protected:
        UOdysseyPainterEditorVectorSelectionTool* mSelectionTool;
        std::vector<::ULIS::FVec2D>* mPointArray;
        //bool mSelecting;
        BLContext mBLSelectionContext;
        BLImage mBLSelectionMask;
        ::ULIS::FRectD mROI;
};
