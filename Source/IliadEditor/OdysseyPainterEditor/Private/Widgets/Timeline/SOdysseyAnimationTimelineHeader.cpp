// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationTimelineHeader.h"

#include "Editor.h"
#include "Fonts/FontMeasure.h"
#include "ScopedTransaction.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayerStack.h"
#include "SOdysseyAnimationTimelineScrollBox.h"
#include "OdysseyStyle.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyPainterEditorAnimationUserSettings.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "SOdysseyAnimationTimelineSection.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "Shortcuts/Global/OdysseyAnimationGlobalTimelineHeaderShortcuts.h"
#include "SOdysseyHandle.h"
#include "Widgets/Input/SNumericEntryBox.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FText GSetLeftBoundTransactionName = LOCTEXT("timeline.transaction.set-left-bound", "Set Animation Left Bound");
FText GSetRightBoundTransactionName = LOCTEXT("timeline.transaction.set-right-bound", "Set Animation Right Bound");

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationTimelineHeader

void
SOdysseyAnimationTimelineHeader::Construct(const FArguments& InArgs)
{
    mAnimation = InArgs._Animation;
    mTimelinePosition = InArgs._TimelinePosition;
    mOnScrubStart = InArgs._OnScrubStart;
    mOnScrubEnd = InArgs._OnScrubEnd;
    mOnCurrentFrameChanged = InArgs._OnCurrentFrameChanged;
    mOnCurrentFrameCommited = InArgs._OnCurrentFrameCommited;

    ChildSlot
    [
        //use this scrollbox to display other widgets in the timeline header
        //for now there is no wodgets to display, but I can clearly imagine some
        SNew(SOdysseyAnimationTimelineScrollBox)
        .TimelinePosition(mTimelinePosition)
        + SOdysseyAnimationTimelineScrollBox::Slot()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SOdysseyAnimationTimelineSection)
                .TimelinePosition(InArgs._TimelinePosition)
                .WidthInFrames_Lambda(
                    [this]()
                    {
                        return mAnimation->GetLeftBoundValue();
                    }
                )
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SOdysseyAnimationTimelineSection)
                .TimelinePosition(InArgs._TimelinePosition)
                .WidthInFrames_Lambda(
                    [this]()
                    {
                        return mAnimation->GetRightBoundValue() - mAnimation->GetLeftBoundValue() + 1;
                    }
                )
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .HAlign(HAlign_Left)
                    .VAlign(VAlign_Bottom)
                    [
                        //left handle
                        SNew(SOdysseyHandle)
                        .Visibility_Lambda(
                            [this]()
                            {
                                return mAnimation->GetLeftBoundMode() == EOdysseyAnimationBoundMode::Automatic ? EVisibility::Collapsed : EVisibility::Visible;
                            }
                        )
                        .OnDragStarted(this, &SOdysseyAnimationTimelineHeader::OnLeftHandleDragStarted)
                        .OnDragged(this, &SOdysseyAnimationTimelineHeader::OnLeftHandleDragged)
                        .OnDragStopped(this, &SOdysseyAnimationTimelineHeader::OnLeftHandleStopped)
                        [
                            SNew(SImage)
                            .Image(FOdysseyStyle::Get().GetBrush("Animation.Timeline.LeftBoundHandle"))
                            .ColorAndOpacity(FOdysseyStyle::Get().GetSlateColor("Animation.Timeline.LeftBoundColor"))
                        ]
                    ]

                    + SHorizontalBox::Slot()
                    .HAlign(HAlign_Right)
                    .VAlign(VAlign_Bottom)
                    [
                        //right handle
                        SNew(SOdysseyHandle)
                        .Visibility_Lambda(
                            [this]()
                            {
                                return mAnimation->GetRightBoundMode() == EOdysseyAnimationBoundMode::Automatic ? EVisibility::Collapsed : EVisibility::Visible;
                            }
                        )
                        .OnDragStarted(this, &SOdysseyAnimationTimelineHeader::OnRightHandleDragStarted)
                        .OnDragged(this, &SOdysseyAnimationTimelineHeader::OnRightHandleDragged)
                        .OnDragStopped(this, &SOdysseyAnimationTimelineHeader::OnRightHandleStopped)
                        [
                            SNew(SImage)
                            .Image(FOdysseyStyle::Get().GetBrush("Animation.Timeline.RightBoundHandle"))
                            .ColorAndOpacity(FOdysseyStyle::Get().GetSlateColor("Animation.Timeline.RightBoundColor"))
                        ]
                    ]
                ]
            ]
        ]
    ];
}

int32 SOdysseyAnimationTimelineHeader::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );
    const FLinearColor& backgroundColorEven = FOdysseyStyle::GetColor("TimelineHeader.backgroundColorEven");
    const FLinearColor& backgroundColorOdd = FOdysseyStyle::GetColor("TimelineHeader.backgroundColorOdd");

    FLinearColor leftBoundColor = FOdysseyStyle::Get().GetSlateColor("Animation.Timeline.LeftBoundColor").GetSpecifiedColor();
    FLinearColor rightBoundColor = FOdysseyStyle::Get().GetSlateColor("Animation.Timeline.RightBoundColor").GetSpecifiedColor();

    const UOdysseyPainterEditorAnimationUserSettings* settings = UOdysseyPainterEditorAnimationUserSettings::Get();
    int startFrame = settings->StartFrame;

    const int32 backgroundLayer = LayerId;
    const int32 textLayer = backgroundLayer + 1;
    const int32 proxyLayer = textLayer + 1;

    const FSlateFontInfo textFontInfo = FCoreStyle::GetDefaultFontStyle("Regular", 10);
    const float height = AllottedGeometry.GetLocalSize().Y;
    const float width = AllottedGeometry.GetLocalSize().X;
    const float frameSize = mTimelinePosition->GetFrameSize();
    float padding = mTimelinePosition->GetPadding() / frameSize; //fixed padding in pixels to display layer pre behaviour
    float offset = mTimelinePosition->GetOffset();
    const float frameNumberMinSize = 30.f;
    const int32 frameNumberFrequency = FMath::Max(1, FGenericPlatformMath::CeilToInt(frameNumberMinSize / frameSize));
    int32 startKey = FMath::Max(0, FGenericPlatformMath::FloorToInt(offset - padding ));
    int32 endKey = FMath::Max(0, FGenericPlatformMath::CeilToInt(offset - padding + (width / frameSize)));

    UOdysseyAnimation* animation = mAnimation;
    UOdysseyAnimationLayerStack* layerstack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    FInt32Range animationRange = animation->GetFrameRange();

    for(int32 keyNum = startKey; keyNum <= endKey; keyNum++)
    {
        float x = (keyNum - offset ) * frameSize + mTimelinePosition->GetPadding();

        //Draw background
        const FColor backgroundColor = (keyNum & 1) ? backgroundColorOdd.ToFColor(true) : backgroundColorEven.ToFColor(true);
        const FVector2D pos(x, 0.f);
        const FVector2D size(frameSize, height);

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            backgroundLayer,
            AllottedGeometry.ToPaintGeometry( size, FSlateLayoutTransform( 1.0, TransformPoint( 1.0, pos ) ) ),
            GenericBrush,
            ESlateDrawEffect::None,
            InWidgetStyle.GetColorAndOpacityTint() * backgroundColor
        );

        //Draw key num
        if (!(keyNum % frameNumberFrequency))
        {
            const FString frameString = FString::Printf(TEXT("%d"), startFrame + keyNum);
            const FVector2D textPos(x + 2.f, 0.f);

            const TSharedRef< FSlateFontMeasure > fontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
            const FVector2D textSize = fontMeasureService->Measure(frameString, textFontInfo);

            FSlateDrawElement::MakeText(
                OutDrawElements,
                textLayer,
                AllottedGeometry.ToPaintGeometry( textSize, FSlateLayoutTransform( 1.0, TransformPoint( 1.0, textPos ) ) ),
                frameString,
                textFontInfo,
                ESlateDrawEffect::None);
        }
    }

    int leftBoundFrame = mAnimation->GetLeftBoundValue();
    int rightBoundFrame = mAnimation->GetRightBoundValue();

    float leftBoundX = FrameToMousePosition(leftBoundFrame);
    float rightBoundX = FrameToMousePosition(rightBoundFrame + 1);

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(FVector2D(2.f, height), FSlateLayoutTransform(1.0, TransformPoint(1.0, FVector2D(leftBoundX, 0.f)))),
        GenericBrush,
        ESlateDrawEffect::None,
        leftBoundColor
    );

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(FVector2D(2.f, height), FSlateLayoutTransform(1.0, TransformPoint(1.0, FVector2D(rightBoundX - 2.f, 0.f)))),
        GenericBrush,
        ESlateDrawEffect::None,
        rightBoundColor
    );

    LayerId = SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

    return LayerId;
}

FReply
SOdysseyAnimationTimelineHeader::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (!mIsScrubbing && MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        mIsScrubbing = true;
        mOnScrubStart.ExecuteIfBound();

        const float minScrub = 0.0f;
        float posX = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X;
        float frame = FMath::Max(0.f, MousePositionToFrame(posX));

        mOnCurrentFrameChanged.ExecuteIfBound((int)frame);

        // This has prevent throttling on so that viewports continue to run whilst dragging the slider
        return FReply::Handled().CaptureMouse( SharedThis(this) ).PreventThrottling();
    }

    return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineHeader::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if(mIsScrubbing)
    {
        const float minScrub = 0.0f;
        float posX = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X;
        float frame = FMath::Max(0.f, MousePositionToFrame(posX));
        mOnCurrentFrameChanged.ExecuteIfBound((int)frame);
        return FReply::Handled();
    }

    return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineHeader::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (mIsScrubbing)
    {
        const float minScrub = 0.0f;
        float posX = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X;
        float frame = MousePositionToFrame(posX);

        mIsScrubbing = false;

        mOnCurrentFrameCommited.ExecuteIfBound(FMath::Max(0, (int)frame));
        mOnScrubEnd.ExecuteIfBound();
        return FReply::Handled().ReleaseMouseCapture();
    }

    if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();
        MapContextMenuActions(commandList);
        FMenuBuilder menuBuilder(true, commandList);
        BuildContextMenu(menuBuilder);

        TSharedRef<SWidget> menuContents = menuBuilder.MakeWidget();
        FWidgetPath widgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();
        FSlateApplication::Get().PushMenu(AsShared(), widgetPath, menuContents, MouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
        return FReply::Handled();
    }

    return FReply::Unhandled();
}

void
SOdysseyAnimationTimelineHeader::BuildContextMenu(FMenuBuilder& iMenuBuilder)
{
    iMenuBuilder.BeginSection("Bounds", LOCTEXT( "animation.timeline.header.context-menu.bounds-section", "Bounds" ));

        iMenuBuilder.AddSubMenu(
            LOCTEXT( "animation.,timeline.header.context-menu.left-bound-submenu.name", "Left Bound" ),
            LOCTEXT( "animation.timeline.header.context-menu.left-bound-submenu.tooltip", "Set Left Bound Mode and Value" ),
            FNewMenuDelegate::CreateLambda(
                [this]( FMenuBuilder& iSubMenuBuilder )
                {
                    iSubMenuBuilder.AddMenuEntry(
                        FOdysseyPainterEditorAnimationCommands::Get().SetAnimationLeftBoundAutomatic,
                        NAME_None,
                        LOCTEXT( "animation.timeline.header.context-menu.set-left-bound-mode-automatic.name", "Automatic" )
                    );

                    iSubMenuBuilder.AddMenuEntry(
                        FOdysseyPainterEditorAnimationCommands::Get().SetAnimationLeftBoundManual,
                        NAME_None,
                        LOCTEXT( "animation.timeline.header.context-menu.set-left-bound-mode-manual.name", "Manual" )
                    );

                    iSubMenuBuilder.AddSeparator();

                    iSubMenuBuilder.AddWidget(
                        SNew(SNumericEntryBox<int>)
                        .IsEnabled_Lambda([this](){ return mAnimation->GetLeftBoundMode() == EOdysseyAnimationBoundMode::Manual;})
                        .Value_Lambda([this]() { return mAnimation->GetLeftBoundValue();})
                        .AllowSpin(true)
                        .ShiftMultiplier(10)
                        .Delta(1)
                        .MinValue(0)
                        .MinSliderValue(0)
                        .MaxValue_Lambda(
                            [this]() -> TOptional< int >
                            {
                                return mAnimation->GetRightBoundValue();
                            }
                        )
                        .MaxSliderValue_Lambda(
                            [this]() -> TOptional< int >
                            {
                                return mAnimation->GetRightBoundValue();
                            }
                        )
                        .OnValueChanged(this, &SOdysseyAnimationTimelineHeader::OnAnimationLeftBoundValueChanged)
                        .OnValueCommitted(this, &SOdysseyAnimationTimelineHeader::OnAnimationLeftBoundValueCommitted)
                        .OnBeginSliderMovement(this, &SOdysseyAnimationTimelineHeader::OnAnimationLeftBoundBeginSliderMovement)
                        .OnEndSliderMovement(this, &SOdysseyAnimationTimelineHeader::OnAnimationLeftBoundEndSliderMovement),
                        LOCTEXT( "animation.timeline.header.context-menu.set-left-bound-value.name", "Frame" )
                    );
                }
            )
        );

        iMenuBuilder.AddSubMenu(
            LOCTEXT( "animation.timeline.header.context-menu.right-bound-submenu.name", "Right Bound" ),
            LOCTEXT( "animation.timeline.header.context-menu.right-bound-submenu.tooltip", "Set Right Bound Mode and Value" ),
            FNewMenuDelegate::CreateLambda(
                [this]( FMenuBuilder& iSubMenuBuilder )
                {
                    iSubMenuBuilder.AddMenuEntry(
                        FOdysseyPainterEditorAnimationCommands::Get().SetAnimationRightBoundAutomatic,
                        NAME_None,
                        LOCTEXT( "animation.timeline.header.context-menu.set-right-bound-mode-automatic.name", "Automatic" )
                    );

                    iSubMenuBuilder.AddMenuEntry(
                        FOdysseyPainterEditorAnimationCommands::Get().SetAnimationRightBoundManual,
                        NAME_None,
                        LOCTEXT( "animation.timeline.header.context-menu.set-right-bound-mode-manual.name", "Manual" )
                    );

                    iSubMenuBuilder.AddSeparator();

                    iSubMenuBuilder.AddWidget(
                        SNew(SNumericEntryBox<int>)
                        .IsEnabled_Lambda([this](){ return mAnimation->GetRightBoundMode() == EOdysseyAnimationBoundMode::Manual;})
                        .Value_Lambda([this]() { return mAnimation->GetRightBoundValue();})
                        .AllowSpin(true)
                        .ShiftMultiplier(10)
                        .Delta(1)
                        .MinValue_Lambda(
                            [this]() -> TOptional< int >
                            {
                                return mAnimation->GetLeftBoundValue();
                            }
                        )
                        .MinSliderValue_Lambda(
                            [this]() -> TOptional< int >
                            {
                                return mAnimation->GetLeftBoundValue();
                            }
                        )
                        .MaxValue(TOptional< int >())
                        .MaxSliderValue(TOptional< int >())
                        .OnValueChanged(this, &SOdysseyAnimationTimelineHeader::OnAnimationRightBoundValueChanged)
                        .OnValueCommitted(this, &SOdysseyAnimationTimelineHeader::OnAnimationRightBoundValueCommitted)
                        .OnBeginSliderMovement(this, &SOdysseyAnimationTimelineHeader::OnAnimationRightBoundBeginSliderMovement)
                        .OnEndSliderMovement(this, &SOdysseyAnimationTimelineHeader::OnAnimationRightBoundEndSliderMovement),
                        LOCTEXT( "animation.timeline.header.context-menu.set-right-bound-value.name", "Frame" )
                    );
                }
            )
        );
    iMenuBuilder.EndSection();
}


void
SOdysseyAnimationTimelineHeader::MapContextMenuActions(TSharedPtr<FUICommandList> iCommandList)
{
    OdysseyAnimationGlobalTimelineHeaderShortcuts::MapActions_SetAnimationBounds(iCommandList.ToSharedRef(), mAnimation);
}

float
SOdysseyAnimationTimelineHeader::MousePositionToFrame(float iX) const
{
    return (iX - mTimelinePosition->GetPadding() + mTimelinePosition->GetOffset() * mTimelinePosition->GetFrameSize()) / mTimelinePosition->GetFrameSize();
}

float
SOdysseyAnimationTimelineHeader::FrameToMousePosition(float iFrame) const
{
    return iFrame * mTimelinePosition->GetFrameSize() + mTimelinePosition->GetPadding() - mTimelinePosition->GetOffset() * mTimelinePosition->GetFrameSize();
}

void
SOdysseyAnimationTimelineHeader::OnLeftHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
#if WITH_EDITOR
    GEditor->BeginTransaction(GSetLeftBoundTransactionName);
#endif
    mHandleMousePosition = iEvent.GetScreenSpacePosition().X;
    mInitialLeftBound = mAnimation->GetLeftBoundValue();
}

void
SOdysseyAnimationTimelineHeader::OnLeftHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    float mouseOffset = iEvent.GetScreenSpacePosition().X - mHandleMousePosition;

    int mouseOffsetInt = 0;
    if ( mouseOffset > 0 )
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() - 0.5f);

    mAnimation->SetLeftBoundValue(mInitialLeftBound + mouseOffsetInt);
}

void
SOdysseyAnimationTimelineHeader::OnLeftHandleStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    GEditor->EndTransaction();
}

void
SOdysseyAnimationTimelineHeader::OnRightHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
#if WITH_EDITOR
    GEditor->BeginTransaction(GSetRightBoundTransactionName);
#endif
    mHandleMousePosition = iEvent.GetScreenSpacePosition().X;
    mInitialRightBound = mAnimation->GetRightBoundValue();
}

void
SOdysseyAnimationTimelineHeader::OnRightHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    float mouseOffset = iEvent.GetScreenSpacePosition().X - mHandleMousePosition;

    int mouseOffsetInt = 0;
    if ( mouseOffset > 0 )
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() - 0.5f);

    mAnimation->SetRightBoundValue(mInitialRightBound + mouseOffsetInt);
}

void
SOdysseyAnimationTimelineHeader::OnRightHandleStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    GEditor->EndTransaction();
}

void
SOdysseyAnimationTimelineHeader::OnAnimationLeftBoundValueCommitted(int iValue, ETextCommit::Type iType)
{
    FScopedTransaction ScopedTransaction(GSetLeftBoundTransactionName);
    mAnimation->SetLeftBoundValue(iValue);
}

void
SOdysseyAnimationTimelineHeader::OnAnimationLeftBoundValueChanged(int iValue)
{
    mAnimation->SetLeftBoundValue(iValue);
}

void
SOdysseyAnimationTimelineHeader::OnAnimationLeftBoundBeginSliderMovement()
{
    //Creating a transaction here manages entering a value using slider
    GEditor->BeginTransaction(GSetLeftBoundTransactionName);
}

void
SOdysseyAnimationTimelineHeader::OnAnimationLeftBoundEndSliderMovement(int iValue)
{
    GEditor->EndTransaction();
}

void
SOdysseyAnimationTimelineHeader::OnAnimationRightBoundValueCommitted(int iValue, ETextCommit::Type iType)
{
    FScopedTransaction ScopedTransaction(GSetRightBoundTransactionName);
    mAnimation->SetRightBoundValue(iValue);
}

void
SOdysseyAnimationTimelineHeader::OnAnimationRightBoundValueChanged(int iValue)
{
    mAnimation->SetRightBoundValue(iValue);
}

void
SOdysseyAnimationTimelineHeader::OnAnimationRightBoundBeginSliderMovement()
{
    //Creating a transaction here manages entering a value using slider
    GEditor->BeginTransaction(GSetRightBoundTransactionName);
}

void
SOdysseyAnimationTimelineHeader::OnAnimationRightBoundEndSliderMovement(int iValue)
{
    GEditor->EndTransaction();
}

#undef LOCTEXT_NAMESPACE
