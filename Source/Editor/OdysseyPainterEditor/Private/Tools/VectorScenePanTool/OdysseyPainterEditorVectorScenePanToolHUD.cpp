#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorScenePanToolHUD::~FOdysseyPainterEditorVectorScenePanToolHUD()
{
}

FOdysseyPainterEditorVectorScenePanToolHUD::FOdysseyPainterEditorVectorScenePanToolHUD( UOdysseyPainterEditorVectorScenePanTool* iScenePanTool )
    : mScenePanTool( iScenePanTool )
{
    BLFontFace face;

    BLResult err = face.createFromFile("C:/Windows/Fonts/lucon.ttf"); // Lucida console
 

    mFont.createFromFace( face, 16.0f );
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::Reset(FOdysseyVectorScene* iScene)
{
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::DrawText( FOdysseyVectorScene* iScene
                                                    , ::ULIS::FRectD& iFrame )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    char str[255];

    snprintf( str
            , 255
            , "Zoom[x:%.2f y:%.2f] Pan[x:%.2f y:%.2f]"
            , iScene->GetScalingX()
            , iScene->GetScalingY()
            , iScene->GetTranslationX()
            , iScene->GetTranslationY() );

    blctx->setCompOp( BL_COMP_OP_DIFFERENCE  );
    blctx->setFillStyle( BLRgba32( 0xFFD0E040 ) );
    blctx->fillUtf8Text( BLPoint( iFrame.x + 10, iFrame.y + iFrame.h - 10 ), mFont, str );
//    blctx->setStrokeStyle( BLRgba32( 0xFF000000 ) );
//    blctx->setStrokeWidth( 1.0f );
//    blctx->strokeUtf8Text( BLPoint( iFrame.x + 10, iFrame.y + iFrame.h - 10 ), mFont, str );
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::DrawFrame( FOdysseyVectorScene* iScene
                                                     , ::ULIS::FRectD& iFrame
                                                     , ::ULIS::FVec2D& iFrameLength )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();

    BLPoint pt[4] = { BLPoint( iFrame.x           , iFrame.y            )
                    , BLPoint( iFrame.x + iFrame.w, iFrame.y            )
                    , BLPoint( iFrame.x + iFrame.w, iFrame.y + iFrame.h )
                    , BLPoint( iFrame.x           , iFrame.y + iFrame.h ) };

    blctx->strokeLine( pt[0].x, pt[0].y, pt[0].x + iFrameLength.x, pt[0].y                  );
    blctx->strokeLine( pt[0].x, pt[0].y, pt[0].x                 , pt[0].y + iFrameLength.y );

    blctx->strokeLine( pt[1].x, pt[1].y, pt[1].x - iFrameLength.x, pt[1].y                  );
    blctx->strokeLine( pt[1].x, pt[1].y, pt[1].x                 , pt[1].y + iFrameLength.y );

    blctx->strokeLine( pt[2].x, pt[2].y, pt[2].x - iFrameLength.x, pt[2].y                 );
    blctx->strokeLine( pt[2].x, pt[2].y, pt[2].x                 , pt[2].y - iFrameLength.y );

    blctx->strokeLine( pt[3].x, pt[3].y, pt[3].x + iFrameLength.x, pt[3].y                  );
    blctx->strokeLine( pt[3].x, pt[3].y, pt[3].x                 , pt[3].y - iFrameLength.y );
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    BLImage* image = iScene->GetEngine()->GetBLImage();
    BLImageData imageData;
    ::ULIS::FRectD frame;
    ::ULIS::FVec2D frameLength;

    image->getData( &imageData );

    frame.x = imageData.size.w * 0.05f;
    frame.y = imageData.size.h * 0.05f;
    frame.w = imageData.size.w * 0.90f;
    frame.h = imageData.size.h * 0.90f;

    frameLength.x = frame.w * 0.125f;
    frameLength.y = frame.h * 0.125f;

    blctx->save();
    blctx->resetMatrix();

    blctx->setCompOp( BL_COMP_OP_DIFFERENCE  );
/*
    blctx->setStrokeStyle( BLRgba32( 0xFF000000 ) ); // teal ABGR
    blctx->setStrokeWidth( 2.0f );
    DrawFrame( iScene, frame, frameLength );
*/
    blctx->setStrokeStyle( BLRgba32( 0xFFD0E040 ) ); // teal ABGR
    blctx->setStrokeWidth( 1.0f );
    DrawFrame( iScene, frame, frameLength );

    DrawText( iScene, frame );

    blctx->restore();


}
