// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Texture/SOdysseyTextureConfigureWindow.h"

class UPaperFlipbook;
class UPaperSprite;
class UTexture2D;

namespace OdysseyPainterEditorFlipbookUtils
{
    //
    // Keyframes
    //
    int32 GetKeyframeIndexAtPosition(UPaperFlipbook* iFlipbook, float iPosition);
    float GetKeyframeStartPosition(UPaperFlipbook* iFlipbook, int32 iIndex);

    bool CreateKeyFrame(UPaperFlipbook* iFlipbook, int32 iIndex, UTexture2D** oTexture, UPaperSprite** oSprite, FOdysseyTextureConfiguration& ioTextureConfiguration);
    void CreateEmptyKeyFrame(UPaperFlipbook* iFlipbook, int32 iIndex);
    bool DuplicateKeyFrame(UPaperFlipbook* iFlipbook, int32 iIndex, UTexture2D** oTexture, UPaperSprite** oSprite);
    bool FixKeyFrame(UPaperFlipbook* iFlipbook, int32 iIndex, UTexture2D** oTexture, UPaperSprite** oSprite, FOdysseyTextureConfiguration& ioTextureConfiguration);
    void MoveKeyFrames(UPaperFlipbook* iFlipbook, TArray<int32> iSrcIndexes, int32 iDstIndex);
    void RemoveKeyFrame(UPaperFlipbook* iFlipbook, int32 iIndex);
    void SetKeyFrameLength(UPaperFlipbook* iFlipbook, int32 iIndex, int32 iLength);

    //
    // Sprites
    //
    UPaperSprite* CreateSprite(UPaperFlipbook* iFlipbook, FString iName);
    UPaperSprite* GetKeyframeSprite(UPaperFlipbook* iFlipbook, int32 iIndex);
    void SetKeyframeSprite(UPaperFlipbook* iFlipbook, int32 iIndex, UPaperSprite* iSprite);
    void ShowKeyFrameSpriteInContentBrowser(UPaperFlipbook* iFlipbook, int32 iIndex);
    void OpenKeyFrameSpriteEditor(UPaperFlipbook* iFlipbook, int32 iIndex);

    //
    // Textures
    //
    UTexture2D* CreateTexture(UPaperFlipbook* iFlipbook, FOdysseyTextureConfiguration iTextureConfiguration);
    UTexture2D* GetKeyframeTexture(UPaperFlipbook* iFlipbook, int32 iIndex);
    void SetKeyframeTexture(UPaperFlipbook* iFlipbook, int32 iIndex, UTexture2D* iTexture);
    void SetSpriteTexture(UPaperSprite* iSprite, UTexture2D* iTexture);
}
