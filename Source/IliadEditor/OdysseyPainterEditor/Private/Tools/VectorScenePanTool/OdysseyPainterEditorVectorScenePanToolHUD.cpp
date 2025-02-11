// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

FOdysseyPainterEditorVectorScenePanToolHUD::~FOdysseyPainterEditorVectorScenePanToolHUD()
{
}

FOdysseyPainterEditorVectorScenePanToolHUD::FOdysseyPainterEditorVectorScenePanToolHUD( UOdysseyPainterEditorVectorScenePanTool* iScenePanTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iScenePanTool )
    , mScenePanTool( iScenePanTool )
{
    mFontInfo = FSlateFontInfo( LoadObject<UFont>( nullptr, TEXT("/Odyssey/Fonts/Lucida_Console_Font") ), 16 );
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
FOdysseyPainterEditorVectorScenePanToolHUD::DrawFrame( const FOdysseyHUDSystem::FDrawHUDParams& iParams
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

    DrawPrimitiveLine( iParams, hudCoords[0], hudCoords[4], iFgColor, iBgColor, 1.0f, false );
    DrawPrimitiveLine( iParams, hudCoords[0], hudCoords[5], iFgColor, iBgColor, 1.0f, false );

    DrawPrimitiveLine( iParams, hudCoords[1], hudCoords[6], iFgColor, iBgColor, 1.0f, false );
    DrawPrimitiveLine( iParams, hudCoords[1], hudCoords[7], iFgColor, iBgColor, 1.0f, false );

    DrawPrimitiveLine( iParams, hudCoords[2], hudCoords[8], iFgColor, iBgColor, 1.0f, false );
    DrawPrimitiveLine( iParams, hudCoords[2], hudCoords[9], iFgColor, iBgColor, 1.0f, false );

    DrawPrimitiveLine( iParams, hudCoords[3], hudCoords[10], iFgColor, iBgColor, 1.0f, false );
    DrawPrimitiveLine( iParams, hudCoords[3], hudCoords[11], iFgColor, iBgColor, 1.0f, false );
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::DrawHUD( const FOdysseyHUDSystem::FDrawHUDParams& iParams )
{
    FLinearColor fgColor = FLinearColor( FOdysseyVectorHUD::GetForegroundColor() );
    FLinearColor bgColor = FLinearColor( FOdysseyVectorHUD::GetBackgroundColor() );
    FLinearColor hcColor = FLinearColor( FOdysseyVectorHUD::GetHighlightColor() );
    ::ULIS::FVec2I frameLength;
    ::ULIS::FRectI frame;
    uint64 hudFlags = mScenePanTool->GetEditor()->GetVectorHUDFlags();
    //char panText[255];
    //char zoomText[255];
    char infoText[255];
    const UFont* font = Cast<UFont>(mFontInfo.FontObject);
    FVector2D infoAt = FVector2D( iParams.mCanvas->GetViewRect().Width() * 0.5f
                                , iParams.mCanvas->GetViewRect().Height() - 20 );

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

/*
    // Zoom Text
    snprintf( zoomText
            , 255
            , "Zoom[x:%.2f y:%.2f]"
            , mScene->GetScalingX()
            , mScene->GetScalingY() );

    FCanvasTextItem zoomItem = FCanvasTextItem( FVector2D( 0, 48 )
                                              , FText::FromString( zoomText )
                                              , font
                                              , fgColor );

    zoomItem.EnableShadow( FLinearColor( 0, 0, 0, 1 ) );

    iParams.mCanvas->DrawItem( zoomItem );

    // Pan Text
    snprintf( panText
            , 255
            , "Pan[x:%.2f y:%.2f]"
            , mScene->GetTranslationX()
            , mScene->GetTranslationY() );

    FCanvasTextItem panItem = FCanvasTextItem( FVector2D( 0, 68 )
                                             , FText::FromString( panText )
                                             , font
                                             , fgColor );

    panItem.EnableShadow( FLinearColor( 0, 0, 0, 1 ) );

    iParams.mCanvas->DrawItem( panItem );
*/

    snprintf( infoText
            , 255
            , "Pan[x:%.2f y:%.2f]     Zoom[x:%.2f y:%.2f]"
            , mScene->GetTranslationX()
            , mScene->GetTranslationY()
            , mScene->GetScalingX()
            , mScene->GetScalingY() );

    infoAt.X -= ( font->GetStringSize( *FString(infoText) ) * 0.5f );

    FCanvasTextItem infoItem = FCanvasTextItem( infoAt
                                              , FText::FromString( infoText )
                                              , font
                                              , FLinearColor( 1.0f, 0.5f, 0.0f, 1.0f ) ); // orange

    infoItem.EnableShadow( FLinearColor( 0, 0, 0, 1 ) );

    iParams.mCanvas->DrawItem( infoItem );
}

void
FOdysseyPainterEditorVectorScenePanToolHUD::Draw( BLContext* iBLContext )
{
}
