#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorScenePanToolHUD::~FOdysseyPainterEditorVectorScenePanToolHUD()
{
}

FOdysseyPainterEditorVectorScenePanToolHUD::FOdysseyPainterEditorVectorScenePanToolHUD( UOdysseyPainterEditorVectorScenePanTool* iScenePanTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iScenePanTool )
    , mScenePanTool( iScenePanTool )
{
    BLFontFace face;
   // TODO: do something depending on to the O.S
    BLResult err = face.createFromFile("C:/Windows/Fonts/lucon.ttf"); // Lucida console
 
    mFont.createFromFace( face, 16.0f );
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::Reset(FOdysseyVectorScene* iScene)
{
    // Updates the selection box
    FOdysseyPainterEditorVectorBaseToolHUD::Reset( iScene );
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::Load(FOdysseyVectorScene* iScene)
{
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::Unload( FOdysseyVectorScene* iScene )
{
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::DrawText( BLContext* iBLContext
                                                    , FOdysseyVectorScene* iScene
                                                    , ::ULIS::FRectD& iFrame )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );

    char str[255];

    iBLContext->setCompOp( BL_COMP_OP_SRC_OVER  );
    iBLContext->setFillStyle( fgColor );

    // Zoom
    snprintf( str
            , 255
            , "Zoom[x:%.2f y:%.2f]"
            , iScene->GetScalingX()
            , iScene->GetScalingY() );

    iBLContext->fillUtf8Text( BLPoint( iFrame.x + 10, iFrame.y + iFrame.h - 10 ), mFont, str );

    // Pan
    snprintf( str
            , 255
            , "Pan[x:%.2f y:%.2f]"
            , iScene->GetTranslationX()
            , iScene->GetTranslationY() );

    iBLContext->fillUtf8Text( BLPoint( iFrame.x + 10, iFrame.y + iFrame.h - 28 ), mFont, str );

//    blctx->setStrokeStyle( BLRgba32( 0xFF000000 ) );
//    blctx->setStrokeWidth( 1.0f );
//    blctx->strokeUtf8Text( BLPoint( iFrame.x + 10, iFrame.y + iFrame.h - 10 ), mFont, str );
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::DrawFrame( BLContext* iBLContext
                                                     , FOdysseyVectorScene* iScene
                                                     , ::ULIS::FRectD& iFrame
                                                     , ::ULIS::FVec2D& iFrameLength )
{
    BLPoint pt[4] = { BLPoint( iFrame.x           , iFrame.y            )
                    , BLPoint( iFrame.x + iFrame.w, iFrame.y            )
                    , BLPoint( iFrame.x + iFrame.w, iFrame.y + iFrame.h )
                    , BLPoint( iFrame.x           , iFrame.y + iFrame.h ) };

    iBLContext->strokeLine( pt[0].x, pt[0].y, pt[0].x + iFrameLength.x, pt[0].y                  );
    iBLContext->strokeLine( pt[0].x, pt[0].y, pt[0].x                 , pt[0].y + iFrameLength.y );

    iBLContext->strokeLine( pt[1].x, pt[1].y, pt[1].x - iFrameLength.x, pt[1].y                  );
    iBLContext->strokeLine( pt[1].x, pt[1].y, pt[1].x                 , pt[1].y + iFrameLength.y );

    iBLContext->strokeLine( pt[2].x, pt[2].y, pt[2].x - iFrameLength.x, pt[2].y                 );
    iBLContext->strokeLine( pt[2].x, pt[2].y, pt[2].x                 , pt[2].y - iFrameLength.y );

    iBLContext->strokeLine( pt[3].x, pt[3].y, pt[3].x + iFrameLength.x, pt[3].y                  );
    iBLContext->strokeLine( pt[3].x, pt[3].y, pt[3].x                 , pt[3].y - iFrameLength.y );
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::Draw( BLContext* iBLContext
                                                , FOdysseyVectorScene* iScene
                                                , uint64 iDrawingFlags )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLImage* image = iBLContext->targetImage();
    ::ULIS::FVec2D frameLength;
    ::ULIS::FRectD frame;

    frame.x = image->width()  * 0.05f;
    frame.y = image->height() * 0.05f;
    frame.w = image->width()  * 0.90f;
    frame.h = image->height() * 0.90f;

    frameLength.x = frame.w * 0.125f;
    frameLength.y = frame.h * 0.125f;

    // Draw scene in object or vertex mode
    FOdysseyPainterEditorVectorBaseToolHUD::Draw( iBLContext, iScene, iDrawingFlags );


    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setCompOp( BL_COMP_OP_SRC_OVER );
    iBLContext->setStrokeWidth( 2.0f );
    iBLContext->setStrokeStyle( bgColor );
    DrawFrame( iBLContext, iScene, frame, frameLength );

    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->setStrokeStyle( fgColor );
    DrawFrame( iBLContext, iScene, frame, frameLength );

    DrawText( iBLContext, iScene, frame );

    iBLContext->restore();
}
