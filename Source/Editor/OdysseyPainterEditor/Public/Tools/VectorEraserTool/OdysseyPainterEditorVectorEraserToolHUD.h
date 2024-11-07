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

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Reset( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Load( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Unload( FOdysseyVectorGroupPaint* iScene ) override;
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
