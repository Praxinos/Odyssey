// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanToolHUD.h"
#include "OdysseyPainterEditor.h"
// Vector engine
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"

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
FOdysseyPainterEditorVectorScenePanToolHUD::Reset(FOdysseyVectorGroupPaint* iScene)
{
    // it's unused but we could use it at some point so, we init it anyways
    UpdateSelectionBox( iScene, false, mScenePanTool->GetEditor()->GetVectorHUDFlags() );
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::Load(FOdysseyVectorGroupPaint* iScene)
{
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::Unload( FOdysseyVectorGroupPaint* iScene )
{
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::DrawFrame( BLContext* iBLContext
                                                     , FOdysseyVectorGroupPaint* iScene
                                                     , ::ULIS::FRectI& iFrame
                                                     , ::ULIS::FVec2I& iFrameLength )
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
                                                , FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    BLImage* image = iBLContext->targetImage();
    ::ULIS::FVec2I frameLength;
    ::ULIS::FRectI frame;
    uint64 hudFlags = mScenePanTool->GetEditor()->GetVectorHUDFlags();
    char panText[255];
    char zoomText[255];

    frame.x = image->width()  * 0.05f;
    frame.y = image->height() * 0.05f;
    frame.w = image->width()  * 0.90f;
    frame.h = image->height() * 0.90f;

    frameLength.x = frame.w * 0.125f;
    frameLength.y = frame.h * 0.125f;

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    FOdysseyPainterEditorVectorBaseToolHUD::Draw( iBLContext, iScene );

    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setCompOp( BL_COMP_OP_SRC_OVER );
    iBLContext->setStrokeWidth( 2.0f );
    iBLContext->setStrokeStyle( bgColor );
    DrawFrame( iBLContext, iScene, frame, frameLength );

    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->setStrokeStyle( fgColor );
    DrawFrame( iBLContext, iScene, frame, frameLength );

    // Zoom Text
    snprintf( zoomText
            , 255
            , "Zoom[x:%.2f y:%.2f]"
            , iScene->GetScalingX()
            , iScene->GetScalingY() );

    iBLContext->setFillStyle( fgColor );
    iBLContext->fillUtf8Text( BLPoint( frame.x + 10, frame.y + frame.h - 28 ), mFont, zoomText );

    // Pan Text
    snprintf( panText
            , 255
            , "Pan[x:%.2f y:%.2f]"
            , iScene->GetTranslationX()
            , iScene->GetTranslationY() );

    iBLContext->setFillStyle( fgColor );
    iBLContext->fillUtf8Text( BLPoint( frame.x + 10, frame.y + frame.h - 10 ), mFont, panText );

    iBLContext->restore();
}
