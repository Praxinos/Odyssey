// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Math/Vector2D.h"

struct FStoryboardVisibleArea
{
    FVector2D TopLeft = FVector2D(0.f, 0.f);
    FVector2D TopRight = FVector2D(0.f, 0.f);
    FVector2D BottomLeft = FVector2D(0.f, 0.f);
    FVector2D BottomRight = FVector2D(0.f, 0.f);

    //Return false if the area is not displayable (example: having a null area )
    bool IsValid() const;

    /*
    FVector2D Offset;
    FVector2D VisibleSize;
    FVector2D AbsoluteSize;
    float DPIScale;

    FStoryboardVisibleArea();
    FStoryboardVisibleArea(const FVector2D& InAbsoluteSize);
    FStoryboardVisibleArea(const FVector2D& InAbsoluteSize, float InDPIScale);
    FStoryboardVisibleArea(const FVector2D& InOffset, const FVector2D& InVisibleSize, const FVector2D& InAbsoluteSize, float InDPIScale);

    bool IsValid() const;

    bool IsAbsoluteView() const;
    bool IsModifiedView() const;
    bool IsAbsoluteSize() const;
    bool IsZoomedView() const;
    bool IsCentered() const;
    bool IsOffset() const;

    float GetVisibleAreaFraction() const;
    float GetInvisibleAreaFraction() const;

    FVector2D GetVisiblePosition(const FVector2D& AbsolutePosition) const;
    FVector2D GetDPIScaledVisiblePosition(const FVector2D& AbsolutePosition) const;
    FVector2D GetAbsolutePosition(const FVector2D& VisiblePosition) const;
    FVector2D GetDPIScaledAbsolutePosition(const FVector2D& VisiblePosition) const;
    FVector2D GetVisibleAreaCenter() const;
    FVector2D GetAbsoluteVisibleAreaCenter() const;
    FVector2D GetAbsoluteAreaCenter() const;
    FVector2D GetInvisibleSize() const; */
};
