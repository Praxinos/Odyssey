// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Templates/SharedPointer.h"
#include "Widgets/SCompoundWidget.h"

class SStoryboardLevelViewport;

class SStoryboardLevelViewportCameraBounds : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SStoryboardLevelViewportCameraBounds)
    {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, TSharedPtr<SStoryboardLevelViewport> InStoryboardLevelViewport);

    //~ Begin SWidget
    virtual int32 OnPaint(const FPaintArgs& InPaintArgs, const FGeometry& InAllottedGeometry, const FSlateRect& InMyCullingRect,
        FSlateWindowElementList& OutDrawElements, int32 InLayerId, const FWidgetStyle& InWidgetStyle, bool bInParentEnabled) const override;
    //~ End SWidget

protected:
    TWeakPtr<SStoryboardLevelViewport> StoryboardLevelViewportWeak;

    void DrawCameraBounds(const FPaintArgs& InPaintArgs, const FGeometry& InAllottedGeometry, const FSlateRect& InMyCullingRect,
        FSlateWindowElementList& OutDrawElements, int32& InOutLayerId, const FLinearColor& InQuadColor) const;
};
