// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyImageRenderingAbility.h"

FOdysseyImageRenderingChangedEvent::FOdysseyImageRenderingChangedEvent(eEventType iType, bool iIsCommit, const FGuid& iId, const TArray<::ULIS::FRectI>& iRects)
    : mType(iType)
    , mIsCommit(iIsCommit)
    , mId(iId)
    , mRects(iRects)
{
}

FOdysseyImageRenderingChangedEvent::eEventType
FOdysseyImageRenderingChangedEvent::GetType() const
{
    return mType;
}

bool
FOdysseyImageRenderingChangedEvent::IsCommit() const
{
    return mIsCommit;
}

const FGuid&
FOdysseyImageRenderingChangedEvent::GetId() const
{
    return mId;
}

const TArray<::ULIS::FRectI>&
FOdysseyImageRenderingChangedEvent::GetRects() const
{
    return mRects;
}

FOdysseyImageRenderingAbility::FOnChanged&
FOdysseyImageRenderingAbility::OnImageRenderingPreChangedDelegate()
{
    static FOnChanged onPreChanged;
    return onPreChanged;
}

FOdysseyImageRenderingAbility::FOnChanged&
FOdysseyImageRenderingAbility::OnImageRenderingChangedDelegate()
{
    static FOnChanged onChanged;
    return onChanged;
}

FOdysseyImageRenderingAbility::FOdysseyImageRenderingAbility()
    : mImageRenderingId(FGuid::NewGuid())
{

}

void
FOdysseyImageRenderingAbility::ImageRenderingChanged()
{
    FOdysseyImageRenderingChangedEvent eventChanged(FOdysseyImageRenderingChangedEvent::eEventType::kValueChange, false, GetImageRenderingId(), GetImageRenderingRects());
    OnImageRenderingPreChangedDelegate().Broadcast(eventChanged);
    OnImageRenderingChangedDelegate().Broadcast(eventChanged);
}

void
FOdysseyImageRenderingAbility::ImageRenderingChanged(const TArray<::ULIS::FRectI>& iRects)
{
    FOdysseyImageRenderingChangedEvent eventChanged(FOdysseyImageRenderingChangedEvent::eEventType::kValueChange, false, GetImageRenderingId(), iRects);
    OnImageRenderingPreChangedDelegate().Broadcast(eventChanged);
    OnImageRenderingChangedDelegate().Broadcast(eventChanged);
}

void
FOdysseyImageRenderingAbility::ImageRenderingCommited()
{
    FOdysseyImageRenderingChangedEvent eventChanged(FOdysseyImageRenderingChangedEvent::eEventType::kValueChange, true, GetImageRenderingId(), GetImageRenderingRects());
    OnImageRenderingPreChangedDelegate().Broadcast(eventChanged);
    OnImageRenderingChangedDelegate().Broadcast(eventChanged);
}

void
FOdysseyImageRenderingAbility::ImageRenderingCommited(const TArray<::ULIS::FRectI>& iRects)
{
    FOdysseyImageRenderingChangedEvent eventChanged(FOdysseyImageRenderingChangedEvent::eEventType::kValueChange, true, GetImageRenderingId(), iRects);
    OnImageRenderingPreChangedDelegate().Broadcast(eventChanged);
    OnImageRenderingChangedDelegate().Broadcast(eventChanged);
}

void
FOdysseyImageRenderingAbility::ImageRenderingCompositionChanged()
{
    FOdysseyImageRenderingChangedEvent eventChanged(FOdysseyImageRenderingChangedEvent::eEventType::kCompositionChange, false, GetImageRenderingId(), {});
    OnImageRenderingPreChangedDelegate().Broadcast(eventChanged);
    OnImageRenderingChangedDelegate().Broadcast(eventChanged);
}

void
FOdysseyImageRenderingAbility::ImageRenderingCompositionCommited()
{
    FOdysseyImageRenderingChangedEvent eventChanged(FOdysseyImageRenderingChangedEvent::eEventType::kCompositionChange, true, GetImageRenderingId(), {});
    OnImageRenderingPreChangedDelegate().Broadcast(eventChanged);
    OnImageRenderingChangedDelegate().Broadcast(eventChanged);
}

TArray<::ULIS::FRectI>
FOdysseyImageRenderingAbility::GetImageRenderingRects() const
{
    check(false); //If you need it, override it in your class
    return {};
}

FGuid
FOdysseyImageRenderingAbility::GetImageRenderingId() const
{
    return mImageRenderingId;
}
