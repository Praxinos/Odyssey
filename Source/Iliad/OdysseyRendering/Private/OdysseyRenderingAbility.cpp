// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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

IOdysseyRenderingAbility::FOnChanged&
IOdysseyRenderingAbility::OnRenderingPreChangedDelegate()
{
    static FOnChanged onPreChanged;
    return onPreChanged;
}

IOdysseyRenderingAbility::FOnChanged&
IOdysseyRenderingAbility::OnRenderingChangedDelegate()
{
    static FOnChanged onChanged;
    return onChanged;
}

IOdysseyRenderingAbility::IOdysseyRenderingAbility()
    : mRenderingId(FGuid::NewGuid())
{

}

void
IOdysseyRenderingAbility::RenderingChanged(bool iIsInteractive)
{
    FOdysseyRenderingChangedEvent eventChanged(FOdysseyRenderingChangedEvent::eEventType::kValueChange, iIsInteractive, GetRenderingId(), { GetDefaultRenderRect() });
    OnRenderingPreChangedDelegate().Broadcast(eventChanged);
    OnRenderingChangedDelegate().Broadcast(eventChanged);
}

void
IOdysseyRenderingAbility::RenderingChanged(const TArray<FIntRect>& iRects, bool iIsInteractive)
{
    FOdysseyRenderingChangedEvent eventChanged(FOdysseyRenderingChangedEvent::eEventType::kValueChange, iIsInteractive, GetRenderingId(), iRects);
    OnRenderingPreChangedDelegate().Broadcast(eventChanged);
    OnRenderingChangedDelegate().Broadcast(eventChanged);
}

void
IOdysseyRenderingAbility::RenderingCompositionChanged(bool iIsInteractive)
{
    FOdysseyRenderingChangedEvent eventChanged(FOdysseyRenderingChangedEvent::eEventType::kCompositionChange, iIsInteractive, GetRenderingId(), {});
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(IOdysseyRenderingAbility::RenderingCompositionChanged::PreChange);
        OnRenderingPreChangedDelegate().Broadcast(eventChanged);
    }

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(IOdysseyRenderingAbility::RenderingCompositionChanged::Change);
        OnRenderingChangedDelegate().Broadcast(eventChanged);
    }
}

FIntRect
IOdysseyRenderingAbility::GetDefaultRenderRect() const
{
    check(false); //If you need it, override it in your class
    return FIntRect(0, 0, 0, 0);
}

FGuid
IOdysseyRenderingAbility::GetRenderingId() const
{
    return mRenderingId;
}

TArray<FGuid>
IOdysseyRenderingAbility::GetRenderingComposition(uint64 iRenderType, int iFrame) const
{
    check(false); //If you need it, override it in your class
    return {};
}

#undef LOCTEXT_NAMESPACE
