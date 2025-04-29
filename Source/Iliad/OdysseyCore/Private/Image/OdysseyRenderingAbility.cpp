// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyRenderingAbility.h"

#define LOCTEXT_NAMESPACE "Imaging"

FOdysseyRenderingChangedEvent::FOdysseyRenderingChangedEvent(eEventType iType, bool iIsInteractive, const FGuid& iId, const TArray<FIntRect>& iRects)
    : mType(iType)
    , mIsInteractive(iIsInteractive)
    , mId(iId)
    , mRects(iRects)
{
}

FOdysseyRenderingChangedEvent::eEventType
FOdysseyRenderingChangedEvent::GetType() const
{
    return mType;
}

bool
FOdysseyRenderingChangedEvent::IsInteractive() const
{
    return mIsInteractive;
}

const FGuid&
FOdysseyRenderingChangedEvent::GetId() const
{
    return mId;
}

const TArray<FIntRect>&
FOdysseyRenderingChangedEvent::GetRects() const
{
    return mRects;
}

FOdysseyRenderingAbility::FOnChanged&
FOdysseyRenderingAbility::OnRenderingPreChangedDelegate()
{
    static FOnChanged onPreChanged;
    return onPreChanged;
}

FOdysseyRenderingAbility::FOnChanged&
FOdysseyRenderingAbility::OnRenderingChangedDelegate()
{
    static FOnChanged onChanged;
    return onChanged;
}

FOdysseyRenderingAbility::FOdysseyRenderingAbility()
    : mRenderingId(FGuid::NewGuid())
{

}

void
FOdysseyRenderingAbility::RenderingChanged(bool iIsInteractive)
{
    FOdysseyRenderingChangedEvent eventChanged(FOdysseyRenderingChangedEvent::eEventType::kValueChange, iIsInteractive, GetRenderingId(), GetRenderingRects());
    OnRenderingPreChangedDelegate().Broadcast(eventChanged);
    OnRenderingChangedDelegate().Broadcast(eventChanged);
}

void
FOdysseyRenderingAbility::RenderingChanged(const TArray<FIntRect>& iRects, bool iIsInteractive)
{
    FOdysseyRenderingChangedEvent eventChanged(FOdysseyRenderingChangedEvent::eEventType::kValueChange, iIsInteractive, GetRenderingId(), iRects);
    OnRenderingPreChangedDelegate().Broadcast(eventChanged);
    OnRenderingChangedDelegate().Broadcast(eventChanged);
}

void
FOdysseyRenderingAbility::RenderingCompositionChanged(bool iIsInteractive)
{
    FOdysseyRenderingChangedEvent eventChanged(FOdysseyRenderingChangedEvent::eEventType::kCompositionChange, iIsInteractive, GetRenderingId(), {});
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyRenderingAbility::RenderingCompositionChanged::PreChange);
        OnRenderingPreChangedDelegate().Broadcast(eventChanged);
    }

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyRenderingAbility::RenderingCompositionChanged::Change);
        OnRenderingChangedDelegate().Broadcast(eventChanged);
    }
}

TArray<FIntRect>
FOdysseyRenderingAbility::GetRenderingRects() const
{
    check(false); //If you need it, override it in your class
    return {};
}

FGuid
FOdysseyRenderingAbility::GetRenderingId() const
{
    return mRenderingId;
}

TArray<FGuid>
FOdysseyRenderingAbility::GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrame) const
{
    check(false); //If you need it, override it in your class
    return {};
}

#undef LOCTEXT_NAMESPACE
