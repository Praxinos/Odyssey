// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanToolHUD.h"
#include "OdysseyPainterEditor.h"
// Vector engine
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"
//
#include "SOdysseyViewport.h"
#include "FOdysseySceneViewport.h"
// for 3D HUDs
#include "CanvasTypes.h"
#include "CanvasItem.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

FOdysseyPainterEditorVectorScenePanToolHUD::~FOdysseyPainterEditorVectorScenePanToolHUD()
{
}

FOdysseyPainterEditorVectorScenePanToolHUD::FOdysseyPainterEditorVectorScenePanToolHUD( UOdysseyPainterEditorVectorScenePanTool* iScenePanTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iScenePanTool )
    , mScenePanTool( iScenePanTool )
{

}

void
FOdysseyPainterEditorVectorScenePanToolHUD::Reset()
{
    //UpdateSelectionBox( false, mScenePanTool->GetEditor()->GetVectorHUDFlags() );
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::Load()
{
    FOdysseyPainterEditorVectorBaseToolHUD::Load();
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::Unload()
{
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::DrawFrame( const FOdysseyHUD::FDrawHUDParams& iParams
                                                     , ::ULIS::FRectI& iFrame
                                                     , ::ULIS::FVec2I& iFrameLength
                                                     , const FLinearColor& iFgColor
                                                     , const FLinearColor& iBgColor )
{
    ::ULIS::FVec2D texCoords[4] = { ::ULIS::FVec2D( iFrame.x           , iFrame.y            )
                                  , ::ULIS::FVec2D( iFrame.x + iFrame.w, iFrame.y            )
                                  , ::ULIS::FVec2D( iFrame.x + iFrame.w, iFrame.y + iFrame.h )
                                  , ::ULIS::FVec2D( iFrame.x           , iFrame.y + iFrame.h ) };

    FVector2D hudCoords[12] = { iParams.mTextureToHUD.Execute( FVector2D( texCoords[0].x, texCoords[0].y ) )
                              , iParams.mTextureToHUD.Execute( FVector2D( texCoords[1].x, texCoords[1].y ) )
                              , iParams.mTextureToHUD.Execute( FVector2D( texCoords[2].x, texCoords[2].y ) )
                              , iParams.mTextureToHUD.Execute( FVector2D( texCoords[3].x, texCoords[3].y ) )

                              , iParams.mTextureToHUD.Execute( FVector2D( texCoords[0].x + iFrameLength.x, texCoords[0].y                  ) )
                              , iParams.mTextureToHUD.Execute( FVector2D( texCoords[0].x                 , texCoords[0].y + iFrameLength.y ) )

                              , iParams.mTextureToHUD.Execute( FVector2D( texCoords[1].x - iFrameLength.x, texCoords[1].y                  ) )
                              , iParams.mTextureToHUD.Execute( FVector2D( texCoords[1].x                 , texCoords[1].y + iFrameLength.y ) )

                              , iParams.mTextureToHUD.Execute( FVector2D( texCoords[2].x - iFrameLength.x, texCoords[2].y                  ) )
                              , iParams.mTextureToHUD.Execute( FVector2D( texCoords[2].x                 , texCoords[2].y - iFrameLength.y ) )

                              , iParams.mTextureToHUD.Execute( FVector2D( texCoords[3].x + iFrameLength.x, texCoords[3].y                  ) )
                              , iParams.mTextureToHUD.Execute( FVector2D( texCoords[3].x                 , texCoords[3].y - iFrameLength.y ) ) };

    DrawPrimitiveLine( iParams, hudCoords[0], hudCoords[4] , iFgColor, 1.0f );
    DrawPrimitiveLine( iParams, hudCoords[0], hudCoords[5] , iFgColor, 1.0f );

    DrawPrimitiveLine( iParams, hudCoords[1], hudCoords[6] , iFgColor, 1.0f );
    DrawPrimitiveLine( iParams, hudCoords[1], hudCoords[7] , iFgColor, 1.0f );

    DrawPrimitiveLine( iParams, hudCoords[2], hudCoords[8] , iFgColor, 1.0f );
    DrawPrimitiveLine( iParams, hudCoords[2], hudCoords[9] , iFgColor, 1.0f );

    DrawPrimitiveLine( iParams, hudCoords[3], hudCoords[10], iFgColor, 1.0f );
    DrawPrimitiveLine( iParams, hudCoords[3], hudCoords[11], iFgColor, 1.0f );
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::DrawHUD( const FOdysseyHUD::FDrawHUDParams& iParams )
{
    mCurrentHUDParams = iParams;

    FLinearColor fgColor = FLinearColor( FOdysseyVectorHUD::GetForegroundColor() );
    FLinearColor bgColor = FLinearColor( FOdysseyVectorHUD::GetBackgroundColor() );
    FLinearColor hcColor = FLinearColor( FOdysseyVectorHUD::GetHighlightColor() );
    ::ULIS::FVec2I frameLength;
    ::ULIS::FRectI frame;
    uint64 hudFlags = mScenePanTool->GetEditor()->GetVectorHUDFlags();
    //char panText[255];
    //char zoomText[255];
    FText infoText = FText::Format( LOCTEXT("vector-scene-pan-tool-hud-info"
                                          , "Pan[x:{0} y:{1}]     Zoom[x:{2} y:{3}]" )
                                  , mScene->GetTranslationX()
                                  , mScene->GetTranslationY()
                                  , mScene->GetScalingX()
                                  , mScene->GetScalingY() );

/*
    const UFont* font = Cast<UFont>(mFontInfo.FontObject);
    FVector2D infoAt = FVector2D( iParams.mCanvas->GetViewRect().Width() * 0.5f
                                , iParams.mCanvas->GetViewRect().Height() - 20 );
*/
    frame.x = iParams.mTextureWidth  * 0.05f;
    frame.y = iParams.mTextureHeight * 0.05f;
    frame.w = iParams.mTextureWidth  * 0.90f;
    frame.h = iParams.mTextureHeight * 0.90f;

    frameLength.x = iParams.mTextureWidth  * 0.125f;
    frameLength.y = iParams.mTextureHeight * 0.125f;

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    FOdysseyPainterEditorVectorBaseToolHUD::DrawHUD( iParams );

    DrawFrame( iParams, frame, frameLength, fgColor, bgColor );

    // invisible plane will get mouse events
    DrawDummyPlane( iParams );

    DrawInfo( iParams, infoText, fgColor );
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::Draw( BLContext* iBLContext )
{
}

#undef LOCTEXT_NAMESPACE
