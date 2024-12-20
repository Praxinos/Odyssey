// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "StoryboardViewport/StoryboardVisibleArea.h"

/* FStoryboardVisibleArea::FStoryboardVisibleArea()
    : FStoryboardVisibleArea(FVector2D::ZeroVector, FVector2D::ZeroVector, FVector2D::ZeroVector, 1.f)
{
}

FStoryboardVisibleArea::FStoryboardVisibleArea(const FVector2D& InAbsoluteSize)
    : FStoryboardVisibleArea(FVector2D::ZeroVector, InAbsoluteSize, InAbsoluteSize, 1.f)
{
}

FStoryboardVisibleArea::FStoryboardVisibleArea(const FVector2D& InAbsoluteSize, float InDPIScale)
    : FStoryboardVisibleArea(FVector2D::ZeroVector, InAbsoluteSize, InAbsoluteSize, InDPIScale)
{
}

FStoryboardVisibleArea::FStoryboardVisibleArea(const FVector2D& InOffset, const FVector2D& InVisibleSize, const FVector2D& InAbsoluteSize, float InDPIScale)
    : Offset(InOffset)
    , VisibleSize(InVisibleSize)
    , AbsoluteSize(InAbsoluteSize)
    , DPIScale(InDPIScale)
{
}

bool IsValidViewportSize(const FVector2D& InViewportSize)
{
    return InViewportSize.X > 0 && InViewportSize.Y > 0
        && !FMath::IsNearlyZero(InViewportSize.X) && !FMath::IsNearlyZero(InViewportSize.Y)
#if ENABLE_NAN_DIAGNOSTIC
        && !FMath::IsNaN(InViewportSize.X) && !FMath::IsNaN(InViewportSize.Y)
#endif
    ;
} */

bool FStoryboardVisibleArea::IsValid() const
{
    return true;
}
/*
bool FStoryboardVisibleArea::IsAbsoluteView() const
{
    return IsAbsoluteSize() && IsCentered();
}

bool FStoryboardVisibleArea::IsModifiedView() const
{
    return !IsAbsoluteView();
}

bool FStoryboardVisibleArea::IsAbsoluteSize() const
{
    return FMath::IsNearlyEqual(VisibleSize.X, AbsoluteSize.X)
        && FMath::IsNearlyEqual(VisibleSize.Y, AbsoluteSize.Y);
}

bool FStoryboardVisibleArea::IsZoomedView() const
{
    return !IsAbsoluteSize();
}

bool FStoryboardVisibleArea::IsCentered() const
{
    return FMath::IsNearlyZero(Offset.X)
        && FMath::IsNearlyZero(Offset.Y);
}

bool FStoryboardVisibleArea::IsOffset() const
{
    return !IsCentered();
}

float FStoryboardVisibleArea::GetVisibleAreaFraction() const
{
    if (FMath::IsNearlyZero(AbsoluteSize.X))
    {
        return 1.f;
    }

    return VisibleSize.X / AbsoluteSize.X;
}

float FStoryboardVisibleArea::GetInvisibleAreaFraction() const
{
    if (FMath::IsNearlyZero(AbsoluteSize.X))
    {
        return 0.f;
    }

    return 1.f - (VisibleSize.X / AbsoluteSize.X);
}

FVector2D FStoryboardVisibleArea::GetVisiblePosition(const FVector2D& AbsolutePosition) const
{
    if (IsAbsoluteView())
    {
        return AbsolutePosition;
    }

    FVector2D VisiblePosition = AbsolutePosition;
    VisiblePosition -= Offset;
    VisiblePosition /= GetVisibleAreaFraction();

    return VisiblePosition;
}

FVector2D FStoryboardVisibleArea::GetDPIScaledVisiblePosition(const FVector2D& AbsolutePosition) const
{
    if (IsAbsoluteView())
    {
        return AbsolutePosition * DPIScale;
    }

    FVector2D VisiblePosition = AbsolutePosition;
    VisiblePosition -= Offset / DPIScale;
    VisiblePosition /= GetVisibleAreaFraction();

    return VisiblePosition * DPIScale;
}

FVector2D FStoryboardVisibleArea::GetAbsolutePosition(const FVector2D& VisiblePosition) const
{
    if (IsAbsoluteView())
    {
        return VisiblePosition;
    }

    FVector2D AbsolutePosition = VisiblePosition;
    AbsolutePosition *= GetVisibleAreaFraction();
    AbsolutePosition += Offset;

    return AbsolutePosition;
}

FVector2D FStoryboardVisibleArea::GetDPIScaledAbsolutePosition(const FVector2D& VisiblePosition) const
{
    if (IsAbsoluteView())
    {
        return VisiblePosition / DPIScale;
    }

    FVector2D AbsolutePosition = VisiblePosition;
    AbsolutePosition *= GetVisibleAreaFraction();
    AbsolutePosition += Offset * DPIScale;

    return AbsolutePosition / DPIScale;
}

FVector2D FStoryboardVisibleArea::GetVisibleAreaCenter() const
{
    return VisibleSize * 0.5f;
}

FVector2D FStoryboardVisibleArea::GetAbsoluteVisibleAreaCenter() const
{
    return Offset + VisibleSize * 0.5f;
}

FVector2D FStoryboardVisibleArea::GetAbsoluteAreaCenter() const
{
    return AbsoluteSize * 0.5f;
}

FVector2D FStoryboardVisibleArea::GetInvisibleSize() const
{
    return AbsoluteSize - VisibleSize;
} */
