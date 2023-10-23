#pragma once

#include <blend2d.h>
#include <ULIS>
#include "HUD/OdysseyVectorHUD.h"
#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorEraserToolHUD : public FOdysseyVectorHUD
{
    public:
        virtual ~FOdysseyPainterEditorVectorEraserToolHUD();
        FOdysseyPainterEditorVectorEraserToolHUD(  UOdysseyPainterEditorVectorEraserTool* iEraserTool );

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        virtual void Load( FOdysseyVectorScene* iScene ) override;
        virtual void Unload( FOdysseyVectorScene* iScene ) override;
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
