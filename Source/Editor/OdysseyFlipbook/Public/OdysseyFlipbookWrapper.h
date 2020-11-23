// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

class UTexture2D;
class UPaperSprite;
class UPaperFlipbook;
class FOdysseyBlock;

// DECLARE_MULTICAST_DELEGATE_OneParam(FOnKeyFrameAdded, int32 /* iIndex */)
// DECLARE_MULTICAST_DELEGATE_TwoParams(FOnKeyFrameChanged, int32 /* iIndex */, FPaperFlipbookKeyFrame& /* iOldValue */)
// DECLARE_MULTICAST_DELEGATE_TwoParams(FOnKeyFrameRemoved, int32 /* iIndex */, FPaperFlipbookKeyFrame& /* iOldValue */)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSpriteTextureChanged, UPaperSprite*, UTexture2D*)

/** High level wrapper class to modify or read a UPaperFlipbook
 * 
 * A Flipbook contains an array of KeyFrames
 * Each Keyframe has a length
 * Therefore, each Keyframe has a Start and an End Position
 * Each Keyframe can have a Sprite (or null)
 * Each Sprite can have a Texture (or null)
*/
class ODYSSEYFLIPBOOK_API FOdysseyFlipbookWrapper : public TSharedFromThis<FOdysseyFlipbookWrapper>
{
public:
    /** The destructor */
	~FOdysseyFlipbookWrapper();

    /** The constructor */
	FOdysseyFlipbookWrapper(UPaperFlipbook* iFlipbook);

public:
    // 
    // Keyframes
    //
    int32 GetKeyframeIndexAtPosition(float iPosition);
    float GetKeyframeStartPosition(int32 iIndex);

    bool CreateKeyFrame(int32 iIndex, UTexture2D** oTexture, UPaperSprite** oSprite);
    bool DuplicateKeyFrame(int32 iIndex, UTexture2D** oTexture, UPaperSprite** oSprite);
    bool FixKeyFrame(int32 iIndex, UTexture2D** oTexture, UPaperSprite** oSprite);
    void MoveKeyFrames(TArray<int32> iSrcIndexes, int32 iDstIndex);
    void RemoveKeyFrame(int32 iIndex);
    void SetKeyFrameLength(int32 iIndex, int32 iLength);

    // 
    // Sprites
    //
    UPaperSprite* GetKeyframeSprite(int32 iIndex);
    void SetKeyframeSprite(int32 iIndex, UPaperSprite* iSprite);
	void ShowKeyFrameSpriteInContentBrowser(int32 iIndex);
	void OpenKeyFrameSpriteEditor(int32 iIndex);

    // 
    // Textures
    //
    UTexture2D* GetKeyframeTexture(int32 iIndex);
    void SetKeyframeTexture(int32 iIndex, UTexture2D* iTexture);

public:
    //
    //  Delegates
    //
    //FOnKeyFrameAdded& OnKeyFrameAdded();
    //FOnKeyFrameChanged& OnKeyFrameChanged();
    //FOnKeyFrameRemoved& OnKeyFrameRemoved();
    //FOnKeyFrameSpriteChanged& OnKeyFrameSpriteChanged();
    FOnSpriteTextureChanged& OnSpriteTextureChanged();

public:
    //tmp
    UPaperFlipbook* Flipbook()
    {
        return mFlipbook;
    }

private:
    void CreateEmptyKeyFrame(int32 iIndex);
    UTexture2D* CreateTexture(int32 iWidth, int32 iHeight, ETextureSourceFormat iFormat, FString iName, FLinearColor iBackgroundColor);
    UTexture2D* CreateTexture(FString iName, FOdysseyBlock* iBlock, ETextureSourceFormat iFormat);
    void CopyTextureContent(UTexture2D* iSrcTexture, UTexture2D* iDstTexture);

    UPaperSprite* CreateSprite(FString iName);
    void OnGlobalObjectPropertyChanged(UObject* iObject, FPropertyChangedEvent& iPropertyChangedEvent);
    void OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain);
    void OnPreSpriteTextureChanged(UPaperSprite* iSprite, const FEditPropertyChain& iEditPropertyChain);
    void OnSpriteTextureChanged(UPaperSprite* iSprite, FPropertyChangedEvent& iPropertyChangedEvent);

    void SetSpriteTexture(UPaperSprite* iSprite, UTexture2D* iTexture);

private:
	UPaperFlipbook* mFlipbook;

    UTexture2D* mSpritePreviousTexture; //

    //FOnKeyFrameAdded mOnKeyFrameAdded;
    //FOnKeyFrameChanged mOnKeyFrameChanged;
    //FOnKeyFrameRemoved mOnKeyFrameRemoved;
    //FOnKeyFrameSpriteChanged mOnKeyFrameSpriteChanged;
    FOnSpriteTextureChanged mOnSpriteTextureChanged;

	FDelegateHandle mOnPrePropertyChangedDelegateHandle;
    FDelegateHandle mOnPropertyChangedDelegateHandle;
};