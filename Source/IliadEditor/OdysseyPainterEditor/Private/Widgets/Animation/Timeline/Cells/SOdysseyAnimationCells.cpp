// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Animation/Timeline/Cells/SOdysseyAnimationCells.h"
#include "Widgets/Animation/Timeline/Cells/SOdysseyAnimationCell.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineSection.h"
#include "Widgets/Layout/SMissingWidget.h"
#include "OdysseyStyle.h"
#include "OdysseyAnimationCell.h"
#include "SOdysseyHandle.h"
#include "Widgets/Input/SComboButton.h"
#include "TimelineTools/OdysseyAnimationTimelineTool.h"
#include "TimelineTools/OdysseyAnimationTimelineTools.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyPainterEditorAnimationProjectSettings.h"
#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellsShortcuts.h"
#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellImageStaggerShortcuts.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SLATE_IMPLEMENT_WIDGET(SOdysseyAnimationCells)
void
SOdysseyAnimationCells::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mCells, EInvalidateWidgetReason::None)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyAnimationCells&>(Widget).OnCellsChanged();
        }
    ));
}

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
SOdysseyAnimationCells::~SOdysseyAnimationCells()
{
    //mAnimationLayer->OnCellsChanged().RemoveAll(this);
}

SOdysseyAnimationCells::SOdysseyAnimationCells()
    : mCells(*this, {})
    , mHoveredCell(nullptr)
    , mTimingHandleBrush(nullptr)
    , mExposureHandleBrush(nullptr)
    , mAddCellsHandleRightBrush(nullptr)
    , mAddCellsHandleLeftBrush(nullptr)
    , mCellBreakIndicatorBrush(nullptr)
    , mCellBreakIndicatorExtendedBrush(nullptr)
{
}

void
SOdysseyAnimationCells::Construct(
    const FArguments& InArgs,
    UOdysseyAnimationLayer* iAnimationLayer
)
{
    ensure(iAnimationLayer);

    mCells.Assign(*this, InArgs._Cells);
    mTimelinePosition = InArgs._TimelinePosition;
    mCurrentFrame = InArgs._CurrentFrame;
    mOnTransactCurrentFrame = InArgs._OnTransactCurrentFrame;

    mTimingHandleBrush = FOdysseyStyle::GetBrush("Animation.CellTimingHandle");
    mExposureHandleBrush = FOdysseyStyle::GetBrush("Animation.CellExposureHandle");
    mAddCellsHandleLeftBrush = FOdysseyStyle::GetBrush("Animation.AddCellsHandleLeft");
    mAddCellsHandleRightBrush = FOdysseyStyle::GetBrush("Animation.AddCellsHandleRight");
    mCellBreakIndicatorBrush = FOdysseyStyle::GetBrush("Animation.CellBreakIndicator");
    mCellBreakIndicatorExtendedBrush = FOdysseyStyle::GetBrush("Animation.CellBreakIndicatorExtended");
    FSlateColor preBehaviourColor( FOdysseyStyle::GetColor( "Animation.Layer.PreBehaviourColor" ) );
    FSlateColor postBehaviourColor( FOdysseyStyle::GetColor( "Animation.Layer.PostBehaviourColor" ) );
    mAnimationLayer = iAnimationLayer;

    mOnCreateCellWidget = InArgs._OnCreateCellWidget;
    mShowHandles = InArgs._ShowHandles;

    //mItemsSource = MakeShared<UE::Slate::Containers::TObservableArray<TSharedPtr<FCellItem>>>();
    //RefreshItemsSource();

    ChildSlot
    .Padding(FMargin(0, 4.f, 0, 4.f))
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SOverlay)
            + SOverlay::Slot()
            [
                //Timeline Section for Layer Offset
                SNew(SOdysseyAnimationTimelineSection)
                .TimelinePosition(mTimelinePosition)
                .WidthInFrames(this, &SOdysseyAnimationCells::GetOffset)
                .Content()
                [
                    SNullWidget::NullWidget
                ]
            ]
            + SOverlay::Slot()
            .Padding(FMargin(-100.f, 0, 4.f, 0)) //100.f is an arbitrary non precise number to allow the prebehaviour button to overlap widgets on the left of it
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Center)
            [
                SNew(SComboButton)
                .Visibility_Lambda([this]() { return mAnimationLayer->GetCellsOffset() > 0 ? EVisibility::Visible : EVisibility::Collapsed; })
                .ButtonStyle(&FAppStyle::Get().GetWidgetStyle< FButtonStyle >( "SimpleButton" ))
                .HasDownArrow(false)
                .OnGetMenuContent(this, &SOdysseyAnimationCells::GetPreBehaviourMenuContent)
                .ButtonContent()
                [
                    SNew(SImage)
                    .Image(this, &SOdysseyAnimationCells::GetPreBehaviourBrush)
                    .ColorAndOpacity(preBehaviourColor)
                ]
            ]
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SOverlay)
            + SOverlay::Slot()
            [
                SNew(SOdysseyAnimationTimelineSection)
                .TimelinePosition(mTimelinePosition)
                .WidthInFrames(this, &SOdysseyAnimationCells::GetCellsWidth)
                .Content()
                [
                    /* SNew(SListView<TSharedPtr<FCellItem>>)
                    .ListItemsSource(mItemsSource)
                    .OnGenerateRow(this, &SOdysseyAnimationCells::OnGenerateRow)
                    .Orientation(Orient_Horizontal)
                    .SelectionMode(ESelectionMode::None)
                    .ScrollbarVisibility(EVisibility::Collapsed)
                    .AllowOverscroll(EAllowOverscroll::No) */
                    SAssignNew(mCellsHBox, SHorizontalBox)
                ]
            ]

            + SOverlay::Slot() //Exposure Handle Top Right
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Fill)
            [
                CreateCellBreakIndicatorWidget()
            ]

            + SOverlay::Slot() //Timing Handle Top Left
            .Padding(0.f, 0.f, -mExposureHandleBrush->ImageSize.X, 0.f)
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Fill)
            [
                CreateTimingHandleWidget()
            ]

            + SOverlay::Slot()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Fill)
            [
                //Add Cells Handle
                CreateAddCellsHandleWidget()
            ]
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(FMargin(4.f, 0.f, 0.f, 0.f))
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Center)
        [
            SNew(SComboButton)
            .ButtonStyle(&FAppStyle::Get().GetWidgetStyle< FButtonStyle >( "SimpleButton" ))
            .HasDownArrow(false)
            .OnGetMenuContent(this, &SOdysseyAnimationCells::GetPostBehaviourMenuContent)
            .ButtonContent()
            [
                SNew(SImage)
                .Image(this, &SOdysseyAnimationCells::GetPostBehaviourBrush)
                .ColorAndOpacity(postBehaviourColor)
            ]
        ]
    ];

    //mAnimationLayer->OnCellsChanged().AddRaw(this, &SOdysseyAnimationCells::OnCellsChanged);
    RefreshCellsHBox();
}

TSharedRef<SWidget>
SOdysseyAnimationCells::CreateCellWidget(UOdysseyLayerCell* iCell)
{
    if (!iCell)
    {
        return SNew(SOdysseyAnimationTimelineSection)
            .TimelinePosition(mTimelinePosition)
            .WidthInFrames(1)
            [
                SNew(SImage)
                .Image(FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameBackground"))
                //.BorderBackgroundColor(FLinearColor(1.f, 1.f, 1.f))
                .ColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.3f))
            ];
    }

    TSharedPtr<SWidget> cellWidget;
    if (!mOnCreateCellWidget.IsBound())
        cellWidget = SMissingWidget::MakeMissingWidget();
    else
        cellWidget = mOnCreateCellWidget.Execute(iCell);

    return SNew(SOdysseyAnimationTimelineSection)
        .TimelinePosition(mTimelinePosition)
        .WidthInFrames(this, &SOdysseyAnimationCells::GetCellExposure, iCell)
        [
            SNew(SOdysseyAnimationCell, mAnimationLayer, iCell)
            .TimelinePosition( mTimelinePosition )
            [
                cellWidget.ToSharedRef()
            ]
        ];
}

void
SOdysseyAnimationCells::OnCellsChanged()
{
    //RefreshItemsSource();
    RefreshCellsHBox();
}

void
SOdysseyAnimationCells::RefreshCellsHBox()
{
    mCellsHBox->ClearChildren();

    TArray<UOdysseyLayerCell*> cells = mCells.Get();
    TMap<UOdysseyLayerCell*, TSharedPtr<SWidget>> cellWidgetsCache;
    for (UOdysseyLayerCell* cell : cells)
    {
        TSharedPtr<SWidget> cellWidget;
        if (mCellWidgetsCache.Contains(cell))
        {
            cellWidget = mCellWidgetsCache[cell];
        }
        else
        {
            cellWidget = CreateCellWidget(cell);
        }


        mCellsHBox->AddSlot()
        .AutoWidth()
        [
            cellWidget.ToSharedRef()
        ];

        cellWidgetsCache.Add(cell, cellWidget);
    }

    mCellWidgetsCache = cellWidgetsCache;
}

/*void
SOdysseyAnimationCells::RefreshItemsSource()
{
    mItemsSource->Reset();

    TArray<UOdysseyLayerCell*> cells = mCells.Get();
    TArray<TSharedPtr<FCellItem>> cellItems;
    cellItems.Reserve(cells.Num());
    for (UOdysseyLayerCell* cell : cells)
    {
        TSharedPtr<FCellItem> cellItem = MakeShared<FCellItem>();
        cellItem->mCell = cell;
        cellItems.Add(cellItem);
    }

    mItemsSource->Append(cellItems);
}*/

float
SOdysseyAnimationCells::GetOffset() const
{
    return mAnimationLayer->GetCellsOffset();
}

float
SOdysseyAnimationCells::GetCellsWidth() const
{
    FInt32Range frameRange = mAnimationLayer->GetFrameRange();
    return frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1;
}

float
SOdysseyAnimationCells::GetCellExposure(UOdysseyLayerCell* iCell) const
{
    return iCell->GetExposure();
}

TSharedRef<SWidget>
SOdysseyAnimationCells::CreateTimingHandleWidget()
{
    return SNew(SHorizontalBox)
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SOdysseyAnimationTimelineSection)
            .TimelinePosition(mTimelinePosition)
            .WidthInFrames_Lambda(
                [this]()
                {
                    if (!IsHovered() && !mTimingHandleDragData.mIsDragging && !mExposureHandleDragData.mIsDragging)
                        return 0;

                    if (!mHoveredCell)
                        return 0;

                    return mHoveredCell->GetFrameRange().GetLowerBoundValue() - mAnimationLayer->GetCellsOffset();
                }
            )
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SOdysseyAnimationTimelineSection)
            .TimelinePosition(mTimelinePosition)
            .WidthInFrames_Lambda(
                [this]()
                {
                    if (!IsHovered() && !mTimingHandleDragData.mIsDragging && !mExposureHandleDragData.mIsDragging)
                        return 0;

                    if (!mHoveredCell)
                        return 0;

                    return mHoveredCell->GetExposure();
                }
            )
            [
                SNew(SOverlay)
                + SOverlay::Slot()
                .Padding(FMargin(1, 1, 0, 0))
                .HAlign(HAlign_Left)
                .VAlign(VAlign_Top)
                [
                    SNew(SOdysseyHandle)
                    .Visibility_Lambda(
                        [this]()
                        {
                            if (mAnimationLayer->IsLockedRecursively())
                                return EVisibility::Hidden;

                            if (FOdysseyAnimationTimelineTools::Get().GetCurrentTool() != EOdysseyTimelineTool::Selection)
                                return EVisibility::Hidden;

                            if (!mHoveredCell)
                                return EVisibility::Hidden;

                            if (mTimingHandleDragData.mIsDragging || mExposureHandleDragData.mIsDragging)
                                return EVisibility::Visible;

                            return IsHovered() ? EVisibility::Visible : EVisibility::Hidden;
                        }
                    )
                    .OnDragStarted(this, &SOdysseyAnimationCells::OnTimingHandleDragStarted)
                    .OnDragged(this, &SOdysseyAnimationCells::OnTimingHandleDragged)
                    .OnDragStopped(this, &SOdysseyAnimationCells::OnTimingHandleDragStopped)
                    [
                        SNew(SImage)
                        .Image(mTimingHandleBrush)
                    ]
                ]
                + SOverlay::Slot()
                .Padding(FMargin(0, 1, -mTimingHandleBrush->ImageSize.X-1, 0))
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Top)
                [
                    SNew(SOdysseyHandle)
                    .Visibility_Lambda(
                        [this]()
                        {
                            if (mAnimationLayer->IsLockedRecursively())
                                return EVisibility::Hidden;

                            if (FOdysseyAnimationTimelineTools::Get().GetCurrentTool() != EOdysseyTimelineTool::Selection)
                                return EVisibility::Hidden;

                            if (!mHoveredCell)
                                return EVisibility::Hidden;

                            if (mHoveredCell->GetIndexInLayer() >= mAnimationLayer->GetCells().Num() - 1)
                                return EVisibility::Hidden;

                            if (mTimingHandleDragData.mIsDragging || mExposureHandleDragData.mIsDragging)
                                return EVisibility::Visible;

                            return IsHovered() ? EVisibility::Visible : EVisibility::Hidden;
                        }
                    )
                    .IsDraggable(false)
                    [
                        SNew(SImage)
                        .Image(mTimingHandleBrush)
                    ]
                ]
                + SOverlay::Slot()
                .Padding(FMargin(-mExposureHandleBrush->ImageSize.X-1, 0, 0, 1))
                .HAlign(HAlign_Left)
                .VAlign(VAlign_Bottom)
                [
                    SNew(SOdysseyHandle)
                    .Visibility_Lambda(
                        [this]()
                        {
                            if (mAnimationLayer->IsLockedRecursively())
                                return EVisibility::Hidden;

                            if (FOdysseyAnimationTimelineTools::Get().GetCurrentTool() != EOdysseyTimelineTool::Selection)
                                return EVisibility::Hidden;

                            if (!mHoveredCell)
                                return EVisibility::Hidden;

                            if (mHoveredCell->GetIndexInLayer() == 0)
                                return EVisibility::Hidden;

                            if (mTimingHandleDragData.mIsDragging || mExposureHandleDragData.mIsDragging)
                                return EVisibility::Visible;

                            return IsHovered() ? EVisibility::Visible : EVisibility::Hidden;
                        }
                    )
                    .IsDraggable(false)
                    [
                        SNew(SImage)
                        .Image(mExposureHandleBrush)
                    ]
                ]
                + SOverlay::Slot()
                .Padding(FMargin(0, 0, 1, 1))
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Bottom)
                [
                    SNew(SOdysseyHandle)
                    .Visibility_Lambda(
                        [this]()
                        {

                            if (mAnimationLayer->IsLockedRecursively())
                                return EVisibility::Hidden;

                            if (FOdysseyAnimationTimelineTools::Get().GetCurrentTool() != EOdysseyTimelineTool::Selection)
                                return EVisibility::Hidden;

                            if (!mHoveredCell)
                                return EVisibility::Hidden;

                            if (mTimingHandleDragData.mIsDragging || mExposureHandleDragData.mIsDragging)
                                return EVisibility::Visible;

                            return IsHovered() ? EVisibility::Visible : EVisibility::Hidden;
                        }
                    )
                    .OnDragStarted(this, &SOdysseyAnimationCells::OnExposureHandleDragStarted)
                    .OnDragged(this, &SOdysseyAnimationCells::OnExposureHandleDragged)
                    .OnDragStopped(this, &SOdysseyAnimationCells::OnExposureHandleDragStopped)
                    [
                        SNew(SImage)
                        .Image(mExposureHandleBrush)
                    ]
                ]
            ]
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationCells::CreateCellBreakIndicatorWidget()
{
    UOdysseyLayerCell* cell = mHoveredCell;
    return SNew(SHorizontalBox)
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SOdysseyAnimationTimelineSection)
            .TimelinePosition(mTimelinePosition)
            .WidthInFrames(this, &SOdysseyAnimationCells::GetCellBreakIndicatorOffset)
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SOdysseyAnimationTimelineSection)
            .TimelinePosition(mTimelinePosition)
            .WidthInFrames(this, &SOdysseyAnimationCells::GetCellBreakIndicatorWidth)
            .Visibility(this, &SOdysseyAnimationCells::GetCellBreakIndicatorVisibility)
            .HAlign(HAlign_Fill)
            [
                SNew(SHorizontalBox)
                +SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SImage)
                    .Image(mCellBreakIndicatorBrush)
                ]
                +SHorizontalBox::Slot()
                [
                    SNew(SImage)
                    .Visibility(this, &SOdysseyAnimationCells::GetCellBreakIndicatorBlankVisibility)
                    .Image(mCellBreakIndicatorExtendedBrush)
                ]
            ]
        ];
}

TSharedRef<SWidget>
SOdysseyAnimationCells::CreateAddCellsHandleWidget()
{
    return SNew(SOdysseyAnimationTimelineSection)
        .TimelinePosition(mTimelinePosition)
        .WidthInFrames_Lambda(
            [this]()
            {
                return mAnimationLayer->GetFrameRange().GetUpperBoundValue() - mAnimationLayer->GetFrameRange().GetLowerBoundValue() + 1;
            }
        )
        [
            SNew(SOverlay)
            + SOverlay::Slot()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Bottom)
            [
                SNew(SOdysseyHandle)
                .Visibility(this, &SOdysseyAnimationCells::GetAddCellsHandleLeftVisibility)
                .OnDragStarted(this, &SOdysseyAnimationCells::OnAddCellsHandleDragStarted, false)
                .OnDragged(this, &SOdysseyAnimationCells::OnAddCellsHandleDragged)
                .OnDragStopped(this, &SOdysseyAnimationCells::OnAddCellsHandleDragStopped)
                [
                    SNew(SImage)
                    .Image(mAddCellsHandleLeftBrush)
                ]
            ]
            + SOverlay::Slot()
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Top)
            [
                SNew(SOdysseyHandle)
                .Visibility(this, &SOdysseyAnimationCells::GetAddCellsHandleRightVisibility)
                .OnDragStarted(this, &SOdysseyAnimationCells::OnAddCellsHandleDragStarted, true)
                .OnDragged(this, &SOdysseyAnimationCells::OnAddCellsHandleDragged)
                .OnDragStopped(this, &SOdysseyAnimationCells::OnAddCellsHandleDragStopped)
                [
                    SNew(SImage)
                    .Image(mAddCellsHandleRightBrush)
                ]
            ]
        ];
}

bool
SOdysseyAnimationCells::SupportsKeyboardFocus() const
{
    return true;
}

FReply
SOdysseyAnimationCells::OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        SharedThis(this),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::CellsTimeline,
        mAnimationLayer
    };

    mTool = FOdysseyAnimationTimelineTools::Get().CreateTool(mTimelinePosition.ToSharedRef(), mAnimationLayer->GetLayerStack()->GetCellSelection());
    if (!mTool)
        return FReply::Unhandled();

    return mTool->OnMouseButtonDown(params);
}

void
SOdysseyAnimationCells::UpdateHoveredCell()
{
    mHoveredCell = nullptr;

    if (mAnimationLayer->GetCells().IsEmpty())
        return;

    //Find frame
    float frame = MousePositionToFrame(mMousePosition.X);

    //Find Cell
    int cellIndex = mAnimationLayer->GetCellIndexAtFrame(frame);
    if (cellIndex == INDEX_NONE)
        return;

    mHoveredCell = mAnimationLayer->GetCells()[cellIndex];
}

FReply
SOdysseyAnimationCells::OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    mMousePosition = iGeometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition());

    UpdateHoveredCell();

    if (!mTool)
        return FReply::Unhandled();

    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        SharedThis(this),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::CellsTimeline,
        mAnimationLayer
    };
    return mTool->OnMouseMove(params);
}

FReply
SOdysseyAnimationCells::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (!mTool)
        return FReply::Unhandled();

    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        SharedThis(this),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::CellsTimeline,
        mAnimationLayer
    };
    return mTool->OnMouseButtonUp(params);
}

FReply
SOdysseyAnimationCells::OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (!mTool)
        return FReply::Unhandled();

    FOdysseyAnimationTimelineTool::FMouseEventParams params =
    {
        iGeometry,
        iEvent,
        SharedThis(this),
        FOdysseyAnimationTimelineTool::EMouseEventOrigin::CellsTimeline,
        mAnimationLayer
    };

    return mTool->OnDragDetected(params);
}

float
SOdysseyAnimationCells::GetCellBreakIndicatorOffset() const
{
    if (FOdysseyAnimationTimelineTools::Get().GetCurrentTool() != EOdysseyTimelineTool::Cut)
        return 0.f;

    if (GetCellBreakIndicatorVisibility() != EVisibility::Visible)
        return 0.f;

    //Find frame
    float frame = MousePositionToFrame(mMousePosition.X) + 0.5f - mAnimationLayer->GetCellsOffset();
    return (int)frame;
}

float
SOdysseyAnimationCells::GetCellBreakIndicatorWidth() const
{
    if (FOdysseyAnimationTimelineTools::Get().GetCurrentTool() != EOdysseyTimelineTool::Cut)
        return 0.f;

    if (GetCellBreakIndicatorVisibility() != EVisibility::Visible)
        return 0.f;

    //Find frame
    float frame = MousePositionToFrame(mMousePosition.X) + 0.5f;
    UOdysseyLayerCell* cell = mAnimationLayer->GetCellAtFrame(frame);
    if (!cell)
        return 0.f;

    return cell->GetFrameRange().GetUpperBoundValue() - (int)frame + 1;
}

EVisibility
SOdysseyAnimationCells::GetCellBreakIndicatorVisibility() const
{
    if (!mAnimationLayer->IsEditable())
        return EVisibility::Hidden;

    if (FOdysseyAnimationTimelineTools::Get().GetCurrentTool() != EOdysseyTimelineTool::Cut)
        return EVisibility::Hidden;

    if (!IsHovered() || !mHoveredCell)
        return EVisibility::Hidden;

    //Find frame
    float frame = MousePositionToFrame(mMousePosition.X) + 0.5f;

    UOdysseyLayerCell* cell = mAnimationLayer->GetCellAtFrame(frame);
    if (!cell)
        return EVisibility::Hidden;

    if ((int)frame == cell->GetFrameRange().GetLowerBoundValue())
        return EVisibility::Hidden;

    return EVisibility::Visible;
}

EVisibility
SOdysseyAnimationCells::GetCellBreakIndicatorBlankVisibility() const
{
    if (FOdysseyAnimationTimelineTools::Get().GetCurrentTool() != EOdysseyTimelineTool::Cut)
        return EVisibility::Collapsed;

    return FSlateApplication::Get().GetModifierKeys().IsControlDown() ? EVisibility::Collapsed : EVisibility::Visible;
}

void
SOdysseyAnimationCells::OnExposureHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (!mHoveredCell)
        return;

    #if WITH_EDITOR
        GEditor->BeginTransaction(LOCTEXT("timeline.cells.transaction.set-cell-exposure", "Change Cell Exposure"));
    #endif

    mExposureHandleDragData.mIsDragging = true;
    mExposureHandleDragData.mCell = mHoveredCell;
    mExposureHandleDragData.mInitialExposure = mHoveredCell->GetExposure();
    mExposureHandleDragData.mMousePosition = iEvent.GetScreenSpacePosition();
}

void
SOdysseyAnimationCells::OnExposureHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (!mExposureHandleDragData.mCell)
        return;

    const int minExposure = 1;
    float mouseOffset = iGeometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition() - mExposureHandleDragData.mMousePosition + iGeometry.GetAbsolutePosition()).X;
    int mouseOffsetInt = 0;
    if ( mouseOffset > 0 )
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() - 0.5f);

    UOdysseyLayerCell* cell = mExposureHandleDragData.mCell;
    int exposure = FMath::Max(1, mExposureHandleDragData.mInitialExposure + mouseOffsetInt);
    cell->SetExposureInteractive(exposure);
}

void
SOdysseyAnimationCells::OnExposureHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (!mExposureHandleDragData.mCell)
        return;

    UOdysseyLayerCell* cell = mExposureHandleDragData.mCell;
    cell->SetExposure(cell->GetExposure());
#if WITH_EDITOR
    GEditor->EndTransaction();
#endif

    mExposureHandleDragData = {};
}

void
SOdysseyAnimationCells::OnTimingHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (!mHoveredCell)
        return;

    #if WITH_EDITOR
        GEditor->BeginTransaction(LOCTEXT("timeline.cells.transaction.set-cell-timing", "Change Cell Timing"));
    #endif

    FInt32Range cellRange = mHoveredCell->GetFrameRange();
    FInt32Range layerRange = mAnimationLayer->GetFrameRange();

    mTimingHandleDragData.mIsDragging = true;
    mTimingHandleDragData.mCell = mHoveredCell;
    mTimingHandleDragData.mMinOffset = -cellRange.GetLowerBoundValue();
    mTimingHandleDragData.mHasMaxOffset = mHoveredCell->GetIndexInLayer() == 0;
    mTimingHandleDragData.mMaxOffset = layerRange.GetUpperBoundValue() - cellRange.GetLowerBoundValue();
    mTimingHandleDragData.mMousePosition = iEvent.GetScreenSpacePosition();
    mTimingHandleDragData.mInitialOffset = mAnimationLayer->GetCellsOffset();
}

void
SOdysseyAnimationCells::OnTimingHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(SOdysseyAnimationCells::OnTimingHandleDragged);

    if (!mTimingHandleDragData.mCell)
        return;

    for (auto element : mTimingHandleDragData.mAffectedCells)
    {
        UOdysseyLayerCell* affectedCell = element.Key;
        int exposure = element.Value;
        affectedCell->SetExposureInteractive(exposure);
    }

    mAnimationLayer->SetCellsOffsetInteractive(mTimingHandleDragData.mInitialOffset);

    //Compute Mouse Offset
    float mouseOffset = iGeometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition() - mTimingHandleDragData.mMousePosition + iGeometry.GetAbsolutePosition()).X;
    int mouseOffsetInt = 0;
    if ( mouseOffset > 0 )
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() - 0.5f);

    mouseOffsetInt = FMath::Max(mouseOffsetInt, mTimingHandleDragData.mMinOffset);

    if ( mTimingHandleDragData.mHasMaxOffset )
        mouseOffsetInt = FMath::Min(mouseOffsetInt, mTimingHandleDragData.mMaxOffset);

    UOdysseyLayerCell* cell = mTimingHandleDragData.mCell;

    //Compute what needs to change
    if ( mouseOffsetInt > 0 )
    {
        // When dragging to the right, the previous cell exposure is always edited if it exists
        // It cannot be removed
        int previousCellIndex = cell->GetIndexInLayer() - 1;
        if ( previousCellIndex >= 0 )
        {
            UOdysseyLayerCell* previousCell = mAnimationLayer->GetCells()[previousCellIndex];

            if (!mTimingHandleDragData.mAffectedCells.Contains(previousCell))
                mTimingHandleDragData.mAffectedCells.Add(previousCell, previousCell->GetExposure());

            int exposure = previousCell->GetExposure() + mouseOffsetInt;
            previousCell->SetExposureInteractive(exposure);
        }
        else //If no previous cell exists, we need to edit the layer's offset value
        {
            mAnimationLayer->SetCellsOffsetInteractive(mAnimationLayer->GetCellsOffset() + mouseOffsetInt);
        }

        //for each cell, adjust cell exposure or hide it
        for ( int i = cell->GetIndexInLayer(); i < mAnimationLayer->GetCells().Num() && mouseOffsetInt > 0; i++ )
        {
            UOdysseyLayerCell* cellToAdjust = mAnimationLayer->GetCells()[i];

            if (!mTimingHandleDragData.mAffectedCells.Contains(cellToAdjust))
                mTimingHandleDragData.mAffectedCells.Add(cellToAdjust, cellToAdjust->GetExposure());

            int exposureToRemove = FMath::Min(cellToAdjust->GetExposure(), mouseOffsetInt);
            int exposure = cellToAdjust->GetExposure() - exposureToRemove;

            cellToAdjust->SetExposureInteractive(exposure);
            mouseOffsetInt -= exposureToRemove;
        }
    }
    else if ( mouseOffsetInt < 0 )
    {
        // When dragging to the left, the current cell exposure is always edited
        // It cannot be removed
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(SOdysseyAnimationCells::OnTimingHandleDragged::SetCurrentCellExposure);

            if (!mTimingHandleDragData.mAffectedCells.Contains(cell))
                mTimingHandleDragData.mAffectedCells.Add(cell, cell->GetExposure());

            int exposure = cell->GetExposure() - mouseOffsetInt;
            cell->SetExposureInteractive(exposure);
        }

        //for each cell adjust its exposure or hide it
        for ( int i = cell->GetIndexInLayer() - 1; i >= 0 && mouseOffsetInt < 0; i-- )
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(SOdysseyAnimationCells::OnTimingHandleDragged::SetCellExposure);
            UOdysseyLayerCell* cellToAdjust = mAnimationLayer->GetCells()[i];

            if (!mTimingHandleDragData.mAffectedCells.Contains(cellToAdjust))
                mTimingHandleDragData.mAffectedCells.Add(cellToAdjust, cellToAdjust->GetExposure());

            int exposureToRemove = FMath::Min(cellToAdjust->GetExposure(), -mouseOffsetInt);
            int exposure = cellToAdjust->GetExposure() - exposureToRemove;

            cellToAdjust->SetExposureInteractive(exposure);

            mouseOffsetInt += exposureToRemove;
        }

        //If the edited cell is the first cell, we also need to adjust the layer offset value
        //if we dragged further than the first cell adjust layer's offset
        if ( mouseOffsetInt < 0 )
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(SOdysseyAnimationCells::OnTimingHandleDragged::SetCellsOffset);
            mAnimationLayer->SetCellsOffsetInteractive(mAnimationLayer->GetCellsOffset() + mouseOffsetInt);
        }
    }
}

void
SOdysseyAnimationCells::OnTimingHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (!mTimingHandleDragData.mCell)
        return;

    TArray<UOdysseyLayerCell*> cellsToRemove;
    for (auto element : mTimingHandleDragData.mAffectedCells)
    {
        UOdysseyLayerCell* affectedCell = element.Key;
        int exposure = element.Value;
        if (affectedCell->GetExposure() <= 0)
        {
            affectedCell->SetExposure(exposure);
            cellsToRemove.Add(affectedCell);
        }
        else
        {
            affectedCell->SetExposure(affectedCell->GetExposure());
        }
    }

    mAnimationLayer->RemoveCells(cellsToRemove);

    mAnimationLayer->SetCellsOffset(mAnimationLayer->GetCellsOffset());

    mTimingHandleDragData.mAffectedCells.Empty();

#if WITH_EDITOR
    GEditor->EndTransaction();
#endif

    mTimingHandleDragData = {};
}

EVisibility
SOdysseyAnimationCells::GetAddCellsHandleRightVisibility() const
{
    if (mAnimationLayer->IsLockedRecursively())
        return EVisibility::Hidden;

    return mShowHandles.Get() ? EVisibility::Visible : EVisibility::Hidden;
}

EVisibility
SOdysseyAnimationCells::GetAddCellsHandleLeftVisibility() const
{
    if (mAnimationLayer->IsLockedRecursively())
        return EVisibility::Hidden;

    if (!mShowHandles.Get())
        return EVisibility::Hidden;

    if (mAddCellsHandleDragData.mIsDragging && !mAddCellsHandleDragData.mIsRightHandle)
        return EVisibility::Visible;

    return mAnimationLayer->GetCellsOffset() > 0 ? EVisibility::Visible : EVisibility::Hidden;
}

void
SOdysseyAnimationCells::OnAddCellsHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, bool iIsRightHandle)
{
    #if WITH_EDITOR
        GEditor->BeginTransaction(LOCTEXT("timeline.cells.transaction.add-cells-at-start", "Add Cells"));
    #endif

    FInt32Range layerRange = mAnimationLayer->GetFrameRange();
    mAddCellsHandleDragData.mIsDragging = true;
    mAddCellsHandleDragData.mIsRightHandle = iIsRightHandle;
    mAddCellsHandleDragData.mMousePosition = iEvent.GetScreenSpacePosition();
    mAddCellsHandleDragData.mInitialOffset = mAnimationLayer->GetCellsOffset();

    if (iIsRightHandle)
    {
        mAddCellsHandleDragData.mMinOffset = layerRange.GetLowerBoundValue() - layerRange.GetUpperBoundValue();
        mAddCellsHandleDragData.mHasMaxOffset = false;
    }
    else
    {
        mAddCellsHandleDragData.mMinOffset = -mAnimationLayer->GetCellsOffset();
        mAddCellsHandleDragData.mMaxOffset = layerRange.GetUpperBoundValue() - layerRange.GetLowerBoundValue();
        mAddCellsHandleDragData.mHasMaxOffset = true;
    }
}

void
SOdysseyAnimationCells::OnAddCellsHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    for (auto element : mAddCellsHandleDragData.mAffectedCells)
    {
        UOdysseyLayerCell* affectedCell = element.Key;
        int exposure = element.Value;
        affectedCell->SetExposureInteractive(exposure);
    }

    mAnimationLayer->SetCellsOffsetInteractive(mAddCellsHandleDragData.mInitialOffset);

    TArray<UOdysseyLayerCell*> cells = mAnimationLayer->GetCells();
    for (int i = cells.Num() - 1; i >= 0; i--)
    {
        if (!cells[i])
            mAnimationLayer->RemoveCellAtIndex(i);
    }

    //---

    int numTempCellsToPrepend = 0;
    int numTempCellsToAppend = 0;

    //Compute Mouse Offset
    float mouseOffset = iGeometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition() - mAddCellsHandleDragData.mMousePosition + iGeometry.GetAbsolutePosition()).X;
    int mouseOffsetInt = 0;
    if ( mouseOffset > 0 )
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() - 0.5f);

    mouseOffsetInt = FMath::Max(mouseOffsetInt, mAddCellsHandleDragData.mMinOffset);

    if (mAddCellsHandleDragData.mHasMaxOffset)
        mouseOffsetInt = FMath::Min(mouseOffsetInt, mAddCellsHandleDragData.mMaxOffset);

    if (mAddCellsHandleDragData.mIsRightHandle)
    {
        if ( mouseOffsetInt > 0 )
        {
            numTempCellsToAppend = mouseOffsetInt;
        }
        else
        {
            //for each cell adjust its exposure or hide it
            for ( int i = mAnimationLayer->GetCells().Num() - 1; i >= 0 && mouseOffsetInt < 0; i-- )
            {
                UOdysseyLayerCell* cellToAdjust = mAnimationLayer->GetCells()[i];

                if (!mAddCellsHandleDragData.mAffectedCells.Contains(cellToAdjust))
                    mAddCellsHandleDragData.mAffectedCells.Add(cellToAdjust, cellToAdjust->GetExposure());

                int exposureToRemove = FMath::Min(cellToAdjust->GetExposure(), -mouseOffsetInt);
                int exposure = cellToAdjust->GetExposure() - exposureToRemove;

                cellToAdjust->SetExposureInteractive(exposure);

                mouseOffsetInt += exposureToRemove;
            }
        }
    }
    else
    {
        mAnimationLayer->SetCellsOffsetInteractive(mAnimationLayer->GetCellsOffset() + mouseOffsetInt);

        if ( mouseOffsetInt < 0 )
        {
            numTempCellsToPrepend = -mouseOffsetInt;
        }
        else
        {
            //for each cell adjust its exposure or hide it
            for ( int i = 0; i < mAnimationLayer->GetCells().Num() && mouseOffsetInt > 0; i++ )
            {
                UOdysseyLayerCell* cellToAdjust = mAnimationLayer->GetCells()[i];

                if (!mAddCellsHandleDragData.mAffectedCells.Contains(cellToAdjust))
                    mAddCellsHandleDragData.mAffectedCells.Add(cellToAdjust, cellToAdjust->GetExposure());

                int exposureToRemove = FMath::Min(cellToAdjust->GetExposure(), mouseOffsetInt);
                int exposure = cellToAdjust->GetExposure() - exposureToRemove;

                cellToAdjust->SetExposureInteractive(exposure);

                mouseOffsetInt -= exposureToRemove;
            }
        }
    }

    if (numTempCellsToPrepend > 0)
        mAnimationLayer->AddCellsInteractive(0, numTempCellsToPrepend);

    if (numTempCellsToAppend > 0)
        mAnimationLayer->AddCellsInteractive(INDEX_NONE, numTempCellsToAppend);
}

void
SOdysseyAnimationCells::OnAddCellsHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    TArray<UOdysseyLayerCell*> cellsToRemove;
    for (auto element : mAddCellsHandleDragData.mAffectedCells)
    {
        UOdysseyLayerCell* affectedCell = element.Key;
        int exposure = element.Value;
        if (affectedCell->GetExposure() <= 0)
        {
            affectedCell->SetExposure(exposure);
            cellsToRemove.Add(affectedCell);
        }
        else
        {
            affectedCell->SetExposure(affectedCell->GetExposure());
        }
    }

    mAnimationLayer->RemoveCells(cellsToRemove);

    mAnimationLayer->SetCellsOffset(mAnimationLayer->GetCellsOffset());

    mAddCellsHandleDragData.mAffectedCells.Empty();

    TArray<UOdysseyLayerCell*> cells = mAnimationLayer->GetCells();
    for (int i = cells.Num() - 1; i >= 0; i--)
    {
        if (!cells[i])
        {
            mAnimationLayer->RemoveCellAtIndex(i);
            mAnimationLayer->AddCell(mAnimationLayer->GetDefaultCellClass(), i);
        }
    }

#if WITH_EDITOR
    GEditor->EndTransaction();
#endif

    mAddCellsHandleDragData.mIsDragging = false;
}

TSharedRef<SWidget>
SOdysseyAnimationCells::GetPreBehaviourMenuContent()
{
    TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();
    FMenuBuilder menuBuilder(true, commandList);

    menuBuilder.AddMenuEntry(
        LOCTEXT("animation.layer.prebehaviour-menu.none", "None"),
        TAttribute<FText>(),
        FSlateIcon("OdysseyStyle", "Animation.Layer.PreBehaviour.None"),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPreBehaviour, EOdysseyLayerImagePostBehaviour::None),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPreBehaviour, EOdysseyLayerImagePostBehaviour::None),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPreBehaviour, EOdysseyLayerImagePostBehaviour::None)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );

    menuBuilder.AddMenuEntry(
        LOCTEXT("animation.layer.prebehaviour-menu.hold", "Hold"),
        TAttribute<FText>(),
        FSlateIcon("OdysseyStyle", "Animation.Layer.PreBehaviour.Hold"),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPreBehaviour, EOdysseyLayerImagePostBehaviour::Hold),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPreBehaviour, EOdysseyLayerImagePostBehaviour::Hold),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPreBehaviour, EOdysseyLayerImagePostBehaviour::Hold)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );

    menuBuilder.AddMenuEntry(
        LOCTEXT("animation.layer.prebehaviour-menu.loop", "Loop"),
        TAttribute<FText>(),
        FSlateIcon("OdysseyStyle", "Animation.Layer.PreBehaviour.Loop"),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPreBehaviour, EOdysseyLayerImagePostBehaviour::Loop),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPreBehaviour, EOdysseyLayerImagePostBehaviour::Loop),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPreBehaviour, EOdysseyLayerImagePostBehaviour::Loop)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );

    menuBuilder.AddMenuEntry(
        LOCTEXT("animation.layer.prebehaviour-menu.pingpong", "PingPong"),
        TAttribute<FText>(),
        FSlateIcon("OdysseyStyle", "Animation.Layer.PreBehaviour.PingPong"),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPreBehaviour, EOdysseyLayerImagePostBehaviour::PingPong),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPreBehaviour, EOdysseyLayerImagePostBehaviour::PingPong),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPreBehaviour, EOdysseyLayerImagePostBehaviour::PingPong)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );

    return menuBuilder.MakeWidget();
}

const FSlateBrush*
SOdysseyAnimationCells::GetPreBehaviourBrush() const
{
    switch(mAnimationLayer->GetPreBehaviour())
    {
        case EOdysseyLayerImagePostBehaviour::None:
            return FOdysseyStyle::GetBrush("Animation.Layer.PreBehaviour.None");
        break;
        case EOdysseyLayerImagePostBehaviour::Hold:
            return FOdysseyStyle::GetBrush("Animation.Layer.PreBehaviour.Hold");
        break;
        case EOdysseyLayerImagePostBehaviour::Loop:
            return FOdysseyStyle::GetBrush("Animation.Layer.PreBehaviour.Loop");
        break;
        case EOdysseyLayerImagePostBehaviour::PingPong:
            return FOdysseyStyle::GetBrush("Animation.Layer.PreBehaviour.PingPong");
        break;
    }
    return nullptr;
}



const FSlateBrush*
SOdysseyAnimationCells::GetPostBehaviourBrush() const
{
    switch(mAnimationLayer->GetPostBehaviour())
    {
        case EOdysseyLayerImagePostBehaviour::None:
            return FOdysseyStyle::GetBrush("Animation.Layer.PostBehaviour.None");
        break;
        case EOdysseyLayerImagePostBehaviour::Hold:
            return FOdysseyStyle::GetBrush("Animation.Layer.PostBehaviour.Hold");
        break;
        case EOdysseyLayerImagePostBehaviour::Loop:
            return FOdysseyStyle::GetBrush("Animation.Layer.PostBehaviour.Loop");
        break;
        case EOdysseyLayerImagePostBehaviour::PingPong:
            return FOdysseyStyle::GetBrush("Animation.Layer.PostBehaviour.PingPong");
        break;
    }
    return nullptr;
}

TSharedRef<SWidget>
SOdysseyAnimationCells::GetPostBehaviourMenuContent()
{
    TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();
    FMenuBuilder menuBuilder(true, commandList);

    menuBuilder.AddMenuEntry(
        LOCTEXT("animation.layer.postbehaviour-menu.none", "None"),
        TAttribute<FText>(),
        FSlateIcon("OdysseyStyle", "Animation.Layer.PostBehaviour.None"),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPostBehaviour, EOdysseyLayerImagePostBehaviour::None),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPostBehaviour, EOdysseyLayerImagePostBehaviour::None),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPostBehaviour, EOdysseyLayerImagePostBehaviour::None)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );

    menuBuilder.AddMenuEntry(
        LOCTEXT("animation.layer.postbehaviour-menu.hold", "Hold"),
        TAttribute<FText>(),
        FSlateIcon("OdysseyStyle", "Animation.Layer.PostBehaviour.Hold"),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPostBehaviour, EOdysseyLayerImagePostBehaviour::Hold),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPostBehaviour, EOdysseyLayerImagePostBehaviour::Hold),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPostBehaviour, EOdysseyLayerImagePostBehaviour::Hold)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );

    menuBuilder.AddMenuEntry(
        LOCTEXT("animation.layer.postbehaviour-menu.loop", "Loop"),
        TAttribute<FText>(),
        FSlateIcon("OdysseyStyle", "Animation.Layer.PostBehaviour.Loop"),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPostBehaviour, EOdysseyLayerImagePostBehaviour::Loop),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPostBehaviour, EOdysseyLayerImagePostBehaviour::Loop),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPostBehaviour, EOdysseyLayerImagePostBehaviour::Loop)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );

    menuBuilder.AddMenuEntry(
        LOCTEXT("animation.layer.postbehaviour-menu.pingpong", "PingPong"),
        TAttribute<FText>(),
        FSlateIcon("OdysseyStyle", "Animation.Layer.PostBehaviour.PingPong"),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPostBehaviour, EOdysseyLayerImagePostBehaviour::PingPong),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPostBehaviour, EOdysseyLayerImagePostBehaviour::PingPong),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPostBehaviour, EOdysseyLayerImagePostBehaviour::PingPong)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );

    return menuBuilder.MakeWidget();
}

void
SOdysseyAnimationCells::SetPreBehaviour(EOdysseyLayerImagePostBehaviour iBehaviour)
{
#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("animation.layer.transaction.set-prebehaviour", "Set Layer Pre Behaviour"));
#endif
    mAnimationLayer->SetPreBehaviour(iBehaviour);
}

bool
SOdysseyAnimationCells::CanSetPreBehaviour(EOdysseyLayerImagePostBehaviour iBehaviour) const
{
    return mAnimationLayer->IsEditable();
}

bool
SOdysseyAnimationCells::IsPreBehaviour(EOdysseyLayerImagePostBehaviour iBehaviour) const
{
    return mAnimationLayer->GetPreBehaviour() == iBehaviour;
}

void
SOdysseyAnimationCells::SetPostBehaviour(EOdysseyLayerImagePostBehaviour iBehaviour)
{
#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("animation.layer.transaction.set-postbehaviour", "Set Layer Post Behaviour"));
#endif
    mAnimationLayer->SetPostBehaviour(iBehaviour);
}

bool
SOdysseyAnimationCells::CanSetPostBehaviour(EOdysseyLayerImagePostBehaviour iBehaviour) const
{
    return mAnimationLayer->IsEditable();
}

bool
SOdysseyAnimationCells::IsPostBehaviour(EOdysseyLayerImagePostBehaviour iBehaviour) const
{
    return mAnimationLayer->GetPostBehaviour() == iBehaviour;
}

float
SOdysseyAnimationCells::MousePositionToFrame(float iX) const
{
    return iX / mTimelinePosition->GetFrameSize();
}

float
SOdysseyAnimationCells::FrameToMousePosition(float iFrame) const
{
    return iFrame * mTimelinePosition->GetFrameSize();
}

#undef LOCTEXT_NAMESPACE
