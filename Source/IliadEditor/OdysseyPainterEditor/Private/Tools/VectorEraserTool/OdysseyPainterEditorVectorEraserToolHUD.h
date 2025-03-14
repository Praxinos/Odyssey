// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorEraserToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        virtual ~FOdysseyPainterEditorVectorEraserToolHUD();
        FOdysseyPainterEditorVectorEraserToolHUD(  UOdysseyPainterEditorVectorEraserTool* iEraserTool );

        void DrawHUD( const FOdysseyHUD::FDrawHUDParams& iParams );
        virtual void Draw( BLContext* iBLContext ) override;
        virtual void Reset() override;
        virtual void Load() override;
        virtual void Unload() override;
        void Init( uint32 iWidth, uint32 iHeight );

        void SetPosition( double iX, double iY );
        void BlendMask( bool iBlending );

        /**
         * @brief Clear the mask image
         */
        void ClearMask();

        void StrokeLine( const ::ULIS::FVec2D& iP0, const ::ULIS::FVec2D& iP1 );
        void FillCircle( double iX, double iY );
        BLImage* GetMask();

    protected:
        UOdysseyPainterEditorVectorEraserTool* mEraserTool;
        BLContext mBLEraserContext;
        BLImage mBLEraserMask;
        bool mBlending;
        double mX, mY;
};
