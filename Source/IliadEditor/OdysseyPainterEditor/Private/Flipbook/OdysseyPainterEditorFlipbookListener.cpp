// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorFlipbookListener.h"

#include "PaperFlipbook.h"
#include "PaperSprite.h"

FOdysseyPainterEditorFlipbookListener::~FOdysseyPainterEditorFlipbookListener()
{
    FCoreUObjectDelegates::OnPreObjectPropertyChanged.RemoveAll(this);
    FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
}

FOdysseyPainterEditorFlipbookListener::FOdysseyPainterEditorFlipbookListener(UPaperFlipbook* iFlipbook)
    : mFlipbook(iFlipbook)
    , mSpritePreviousTexture(NULL)
{
    FCoreUObjectDelegates::OnPreObjectPropertyChanged.AddRaw(this, &FOdysseyPainterEditorFlipbookListener::OnPreGlobalObjectPropertyChanged);
    FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this, &FOdysseyPainterEditorFlipbookListener::OnGlobalObjectPropertyChanged);
}

FOdysseyPainterEditorFlipbookListener::FOnSpriteTextureChanged&
FOdysseyPainterEditorFlipbookListener::OnSpriteTextureChanged()
{
    return mOnSpriteTextureChanged;
}

void
FOdysseyPainterEditorFlipbookListener::OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain)
{
    if (UPaperSprite* sprite = Cast<UPaperSprite>(iObject))
    {
        OnPreSpriteTextureChanged(sprite, iEditPropertyChain);
    }
}

void
FOdysseyPainterEditorFlipbookListener::OnGlobalObjectPropertyChanged(UObject* iObject, FPropertyChangedEvent& iPropertyChangedEvent)
{
    if (UPaperSprite* sprite = Cast<UPaperSprite>(iObject))
    {
        OnSpriteTextureChanged(sprite, iPropertyChangedEvent);
    }
}

void
FOdysseyPainterEditorFlipbookListener::OnPreSpriteTextureChanged(UPaperSprite* iSprite, const FEditPropertyChain& iEditPropertyChain)
{
    if (!mFlipbook || !mFlipbook->ContainsSprite(iSprite))
        return;

    FProperty* property = iEditPropertyChain.GetActiveNode()->GetValue();
    if (property->GetFName() == "SourceTexture")
    {
        FSoftObjectProperty* textureProperty = CastField<FSoftObjectProperty>(property);
        mSpritePreviousTexture = Cast<UTexture2D>(textureProperty->GetObjectPropertyValue(textureProperty->ContainerPtrToValuePtr<UPaperSprite>(iSprite)));
    }
}

void
FOdysseyPainterEditorFlipbookListener::OnSpriteTextureChanged(UPaperSprite* iSprite, FPropertyChangedEvent& iPropertyChangedEvent)
{
    if (!mFlipbook || !mFlipbook->ContainsSprite(iSprite))
        return;

    FName propertyName = iPropertyChangedEvent.GetPropertyName();
    if (propertyName == "SourceTexture")
    {
        mOnSpriteTextureChanged.Broadcast(iSprite, mSpritePreviousTexture);
    }
}
