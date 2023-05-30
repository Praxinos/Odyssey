// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Abilities/IOdysseyAnimationImageRenderingAbility.h"


IOdysseyAnimationImageRenderingAbility::FOnChanged&
IOdysseyAnimationImageRenderingAbility::OnPreChanged()
{
    static FOnChanged onPreChanged;
    return onPreChanged;
}

IOdysseyAnimationImageRenderingAbility::FOnChanged&
IOdysseyAnimationImageRenderingAbility::OnChanged()
{
    static FOnChanged onChanged;
    return onChanged;
}

IOdysseyAnimationImageRenderingAbility::FOnChanged&
IOdysseyAnimationImageRenderingAbility::OnPreCommited()
{
    static FOnChanged onPreCommited;
    return onPreCommited;
}

IOdysseyAnimationImageRenderingAbility::FOnChanged&
IOdysseyAnimationImageRenderingAbility::OnCommited()
{
    static FOnChanged onCommited;
    return onCommited;
}

IOdysseyAnimationImageRenderingAbility::FOnCompositionChanged&
IOdysseyAnimationImageRenderingAbility::OnCompositionPreChanged()
{
    static FOnCompositionChanged onCompositionPreChanged;
    return onCompositionPreChanged;
}

IOdysseyAnimationImageRenderingAbility::FOnCompositionChanged&
IOdysseyAnimationImageRenderingAbility::OnCompositionChanged()
{
    static FOnCompositionChanged onCompositionChanged;
    return onCompositionChanged;
}

IOdysseyAnimationImageRenderingAbility::FOnCompositionChanged&
IOdysseyAnimationImageRenderingAbility::OnCompositionPreCommited()
{
    static FOnCompositionChanged onCompositionPreCommited;
    return onCompositionPreCommited;
}

IOdysseyAnimationImageRenderingAbility::FOnCompositionChanged&
IOdysseyAnimationImageRenderingAbility::OnCompositionCommited()
{
    static FOnCompositionChanged onCompositionCommited;
    return onCompositionCommited;
}

void
IOdysseyAnimationImageRenderingAbility::Changed(const FGuid& iId, const TArray<::ULIS::FRectI>& iRects)
{
    IOdysseyAnimationImageRenderingAbility::OnPreChanged().Broadcast(iId, iRects);
    IOdysseyAnimationImageRenderingAbility::OnChanged().Broadcast(iId, iRects);
}

void
IOdysseyAnimationImageRenderingAbility::Commited(const FGuid& iId, const TArray<::ULIS::FRectI>& iRects)
{
    IOdysseyAnimationImageRenderingAbility::OnPreCommited().Broadcast(iId, iRects);
    IOdysseyAnimationImageRenderingAbility::OnCommited().Broadcast(iId, iRects);
}

void
IOdysseyAnimationImageRenderingAbility::CompositionChanged(const FGuid& iId)
{
    IOdysseyAnimationImageRenderingAbility::OnCompositionPreChanged().Broadcast(iId);
    IOdysseyAnimationImageRenderingAbility::OnCompositionChanged().Broadcast(iId);
}

void
IOdysseyAnimationImageRenderingAbility::CompositionCommited(const FGuid& iId)
{
    IOdysseyAnimationImageRenderingAbility::OnCompositionPreCommited().Broadcast(iId);
    IOdysseyAnimationImageRenderingAbility::OnCompositionCommited().Broadcast(iId);
}

IOdysseyAnimationImageRenderingAbility::IOdysseyAnimationImageRenderingAbility()
    : mId(FGuid::NewGuid())
{
}
    
const FGuid&
IOdysseyAnimationImageRenderingAbility::GetId() const
{
    return mId;
}

TSharedPtr<IOdysseyHandle>
IOdysseyAnimationImageRenderingAbility::Preload(int iFrame) const
{
    return nullptr;
}