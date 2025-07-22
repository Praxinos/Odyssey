// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class UTexture2D;
class UPaperSprite;
class UPaperFlipbook;

class FOdysseyPainterEditorFlipbookListener : public TSharedFromThis<FOdysseyPainterEditorFlipbookListener>
{
public:
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSpriteTextureChanged, UPaperSprite*, UTexture2D*)

public:
    /** The destructor */
    ~FOdysseyPainterEditorFlipbookListener();

    /** The constructor */
    FOdysseyPainterEditorFlipbookListener(UPaperFlipbook* iFlipbook);

public:
    FOnSpriteTextureChanged& OnSpriteTextureChanged();

private:
    void OnGlobalObjectPropertyChanged(UObject* iObject, FPropertyChangedEvent& iPropertyChangedEvent);
    void OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain);
    void OnPreSpriteTextureChanged(UPaperSprite* iSprite, const FEditPropertyChain& iEditPropertyChain);
    void OnSpriteTextureChanged(UPaperSprite* iSprite, FPropertyChangedEvent& iPropertyChangedEvent);


private:
    UPaperFlipbook* mFlipbook;
    UTexture2D* mSpritePreviousTexture;
    FOnSpriteTextureChanged mOnSpriteTextureChanged;

    //FOdysseyTextureConfiguration mTextureConfiguration;
};
