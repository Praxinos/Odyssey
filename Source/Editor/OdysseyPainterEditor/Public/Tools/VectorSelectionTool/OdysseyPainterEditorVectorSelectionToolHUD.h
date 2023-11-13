#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorSelectionToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        virtual ~FOdysseyPainterEditorVectorSelectionToolHUD();
        FOdysseyPainterEditorVectorSelectionToolHUD(  UOdysseyPainterEditorVectorSelectionTool* iSelectionTool );

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorScene* iScene ) override;
        virtual void Load( FOdysseyVectorScene* iScene ) override;
        virtual void Unload( FOdysseyVectorScene* iScene ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        void ShowSelectionBox( bool iShowSelectionBox );
        void Init( uint32 iWidth, uint32 iHeight );
        void GetSelectedVertices( FOdysseyVectorScene* iScene, std::vector<FOdysseyVectorPoint*>& oPointArray );

        void ClearMask();
        BLImage* GetMask();

        /**
         * @brief Fill the mask image with a shape defined by a array of points.
         * @param iPointArray a reference to the array of points.
         */
        ::ULIS::FRectD GenerateFreehandMask( std::vector<::ULIS::FVec2D>& iPointArray );

        /**
         * @brief Fill the mask image with an alpha Circle.
         * @param iX the circle's center on X axis.
         * @param iY the circle's center on Y axis.
         * @param iRadius the circle's radius.
         * @return the bounding box including the circle.
         */
        ::ULIS::FRectD GenerateCircleMask( double iX, double iY, double iRadius );

        /**
         * @brief Fill the mask image with an alpha Rectangle.
         * @param iRect the rectangle
         * @return the bounding box including the rectangle.
         */
        ::ULIS::FRectD GenerateRectangleMask( const ::ULIS::FRectD& iRect );


    protected:
        void DrawPickingArea( BLContext* iBLContext
                            , BLRgba32 fgColor
                            , BLRgba32 bgColor
                            , BLRgba32 hcColor );
        void DrawVertexSelection( BLContext* iBLContext, FOdysseyVectorScene* iScene, uint64 iFlags );
        void DrawObjectSelection( BLContext* iBLContext, FOdysseyVectorScene* iScene, uint64 iFlags );
        void DrawSelectionSpace( BLContext* iBLContext, FOdysseyVectorScene* iScene,uint64 iFlags);

    protected:
        UOdysseyPainterEditorVectorSelectionTool* mSelectionTool;
        std::vector<::ULIS::FVec2D>* mPointArray;
        //bool mSelecting;
        BLContext mBLSelectionContext;
        BLImage mBLSelectionMask;

        bool mShowSelectionBox;
        bool mShowSelectionIfEmpty;
};
