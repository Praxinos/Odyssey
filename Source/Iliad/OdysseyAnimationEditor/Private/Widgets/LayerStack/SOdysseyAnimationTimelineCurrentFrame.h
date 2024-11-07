// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationEditorExtension;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineCurrentFrame
    : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineCurrentFrame)
        {
            _Visibility = EVisibility::SelfHitTestInvisible;
        }
    SLATE_END_ARGS()

public:
    SOdysseyAnimationTimelineCurrentFrame();

    void Construct(
        const FArguments& iArgs,
        FOdysseyAnimationEditorExtension* iExtension
    );

public:
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
    /**
     * Overwritten from SWidget.
     *
     * LeafWidgets should compute their DesiredSize based solely on their visual representation. There is no need to
     * take child widgets into account as LeafWidgets have none by definition. For example, the TextBlock widget simply
     * measures the area necessary to display its text with the given font and font size.
     */
    virtual FVector2D ComputeDesiredSize(float) const override;

    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    FOdysseyAnimationEditorExtension*            mExtension;
};
