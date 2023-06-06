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
IOdysseyAnimationImageRenderingAbility::Changed()
{
    OnPreChanged().Broadcast(GetId(), GetRects());
    OnChanged().Broadcast(GetId(), GetRects());
}

void
IOdysseyAnimationImageRenderingAbility::Changed(const TArray<::ULIS::FRectI>& iRects)
{
    OnPreChanged().Broadcast(GetId(), iRects);
    OnChanged().Broadcast(GetId(), iRects);
}

void
IOdysseyAnimationImageRenderingAbility::Commited()
{
    OnPreCommited().Broadcast(GetId(), GetRects());
    OnCommited().Broadcast(GetId(), GetRects());
}

void
IOdysseyAnimationImageRenderingAbility::Commited(const TArray<::ULIS::FRectI>& iRects)
{
    OnPreCommited().Broadcast(GetId(), iRects);
    OnCommited().Broadcast(GetId(), iRects);
}

void
IOdysseyAnimationImageRenderingAbility::CompositionChanged()
{
    OnCompositionPreChanged().Broadcast(GetId());
    OnCompositionChanged().Broadcast(GetId());
}

void
IOdysseyAnimationImageRenderingAbility::CompositionCommited()
{
    OnCompositionPreCommited().Broadcast(GetId());
    OnCompositionCommited().Broadcast(GetId());
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
IOdysseyAnimationImageRenderingAbility::Preload(int iFrame, IOdysseyImageRenderer::eRenderType iRenderType) const
{
    return nullptr;
}

::ULIS::eBlendMode
IOdysseyAnimationImageRenderingAbility::GetBlendMode() const
{
    return ::ULIS::Blend_Normal;
}

float
IOdysseyAnimationImageRenderingAbility::GetOpacity() const
{
    return 1.f;
}