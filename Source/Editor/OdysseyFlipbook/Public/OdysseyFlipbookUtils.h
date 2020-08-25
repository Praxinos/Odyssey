// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"

class UTexture2D;
class UPaperSprite;
class UPaperFlipbook;
class FOdysseyBlock;

/** Contains High level methods to modify a Flipbook
 * 
 * A Flipbook contains an array of KeyFrames
 * Each Keyframe has a length
 * Therefore, each Keyframe has a Start and an End Position
 * Each Keyframe can have a Sprite (or null)
 * Each Sprite can have a Texture (or null)
*/
class ODYSSEYFLIPBOOK_API FOdysseyFlipbookUtils
{
public:
	FOdysseyFlipbookUtils(UPaperFlipbook* iFlipbook);

public:
    //Modifiers
    bool AddKeyFrame(int32 iIndex, UTexture2D** oTexture, UPaperSprite** oSprite);
    bool DuplicateKeyFrame(int32 iIndex, UTexture2D** oTexture, UPaperSprite** oSprite);
    bool FixKeyFrame(int32 iIndex, UTexture2D** oTexture, UPaperSprite** oSprite);

    void MoveKeyFrames(TArray<int32> iSrcIndexes, int32 iDstIndex);
    void RemoveKeyFrame(int32 iIndex);

    void SetKeyframeTexture(int32 iIndex, UTexture2D* iTexture);
    void SetKeyframeSprite(int32 iIndex, UPaperSprite* iSprite);
 
	void ShowKeyFrameSpriteInContentBrowser(int32 iIndex);
	void OpenKeyFrameSpriteEditor(int32 iIndex);

public:
    //Getters
    UTexture2D* GetKeyframeTexture(int32 iIndex);
    UPaperSprite* GetKeyframeSprite(int32 iIndex);

    int32 GetKeyframeIndexAtPosition(float iPosition);
    float GetKeyframeStartPosition(int32 iIndex);

private:
    void SetSpriteTexture(UPaperSprite* iSprite, UTexture2D* iTexture);

    UTexture2D* CreateTexture(int32 iWidth, int32 iHeight, FOdysseyBlock* iBlock = NULL);
    UPaperSprite* CreateSprite();

private:
	UPaperFlipbook* mFlipbook;
};