// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Animation/Timeline/Cells/SOdysseyAnimationCells.h"
#include "Widgets/Animation/Timeline/Cells/SOdysseyAnimationCell.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineSection.h"
#include "Widgets/Layout/SMissingWidget.h"
#include "OdysseyStyle.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "SOdysseyHandle.h"
#include "Widgets/Input/SComboButton.h"
#include "TimelineTools/OdysseyAnimationTimelineTool.h"
#include "TimelineTools/OdysseyAnimationTimelineTools.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "UObject/OdysseyObjectEditorUtils.h"

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
    , mTimingHandleCells()
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

    mTimingHandleBrush = FOdysseyStyle::GetBrush("Animation.CellTimingHandle");
    mExposureHandleBrush = FOdysseyStyle::GetBrush("Animation.CellExposureHandle");
    mAddCellsHandleLeftBrush = FOdysseyStyle::GetBrush("Animation.AddCellsHandleLeft");
    mAddCellsHandleRightBrush = FOdysseyStyle::GetBrush("Animation.AddCellsHandleRight");
    mCellBreakIndicatorBrush = FOdysseyStyle::GetBrush("Animation.CellBreakIndicator");
    mCellBreakIndicatorExtendedBrush = FOdysseyStyle::GetBrush("Animation.CellBreakIndicatorExtended");
    FSlateColor preBehaviourColor( FOdysseyStyle::GetColor( "Animation.Layer.PreBehaviourColor" ) );
    FSlateColor postBehaviourColor( FOdysseyStyle::GetColor( "Animation.Layer.PostBehaviourColor" ) );
    float preBehaviourPadding = mTimelinePosition->GetPadding();
    mAnimationLayer = iAnimationLayer;

    mOnCreateCellWidget = InArgs._OnCreateCellWidget;
    mShowHandles = InArgs._ShowHandles;

    mItemsSource = MakeShared<UE::Slate::Containers::TObservableArray<TSharedPtr<FCellItem>>>();
    RefreshItemsSource();

    ChildSlot
    .Padding(FMargin(0, 4.f, 0, 4.f))
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
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
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(FMargin(-preBehaviourPadding, 0, 0.f, 0))
        [
            SNew(SBox)
            .Padding(FMargin(0, 0, 4.f, 0))
            .WidthOverride(preBehaviourPadding)
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Center)
            [
                SNew(SComboButton)
                .Visibility_Lambda([this]() { return mAnimationLayer->CellsOffset > 0 ? EVisibility::Visible : EVisibility::Hidden; })
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
                SNew(SListView<TSharedPtr<FCellItem>>)
                .ListItemsSource(mItemsSource)
                .OnGenerateRow(this, &SOdysseyAnimationCells::OnGenerateRow)
                .Orientation(Orient_Horizontal)
                .SelectionMode(ESelectionMode::None)
                .ScrollbarVisibility(EVisibility::Collapsed)
                .AllowOverscroll(EAllowOverscroll::No)
            ]

            + SOverlay::Slot() //Exposure Handle Top Right
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Fill)
            [
                CreateCellBreakIndicatorWidget()
            ]

            + SOverlay::Slot()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Fill)
            [
                //Add Cells Handle
                CreateAddCellsHandleWidget()
            ]

            + SOverlay::Slot() //Timing Handle Top Left
            .Padding(0.f, 0.f, -mExposureHandleBrush->ImageSize.X, 0.f)
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Fill)
            [
                CreateTimingHandleWidget()
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
    //RefreshCells();
}

TSharedRef<ITableRow>
SOdysseyAnimationCells::OnGenerateRow(TSharedPtr<FCellItem> iCell, const TSharedRef<STableViewBase>& iOwnerTable)
{
    if (!iCell->mCell)
    {
        return SNew(STableRow<TSharedPtr<FCellItem>>, iOwnerTable)
            [
                SNew(SOdysseyAnimationTimelineSection)
                .TimelinePosition(mTimelinePosition)
                .WidthInFrames(1)
                [
                    SNew(SImage)
                    .Image(FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameBackground"))
                    //.BorderBackgroundColor(FLinearColor(1.f, 1.f, 1.f))
                    .ColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.3f))
                ]
            ];
    }

    TSharedPtr<SWidget> cellWidget;
    if (!mOnCreateCellWidget.IsBound())
        cellWidget = SMissingWidget::MakeMissingWidget();
    else
        cellWidget = mOnCreateCellWidget.Execute(iCell->mCell);

    return SNew(STableRow<TSharedPtr<FCellItem>>, iOwnerTable)
        [
            SNew(SOdysseyAnimationTimelineSection)
            .TimelinePosition(mTimelinePosition)
            .WidthInFrames(this, &SOdysseyAnimationCells::GetCellExposure, iCell->mCell)
            [
                SNew(SBorder)
                .BorderImage(FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameBackground"))
                .BorderBackgroundColor(FLinearColor(1.f, 1.f, 1.f))
                .Visibility(this, &SOdysseyAnimationCells::GetCellVisibility, iCell->mCell)
                [
                    SNew(SOdysseyAnimationCell, mAnimationLayer, iCell->mCell)
                    [
                        cellWidget.ToSharedRef()
                    ]
                ]
            ]
        ];
}

void
SOdysseyAnimationCells::OnCellsChanged()
{
    RefreshItemsSource();
}

void
SOdysseyAnimationCells::RefreshItemsSource()
{
    mItemsSource->Reset();

    TArray<UOdysseyAnimationCell*> cells = mCells.Get();
    TArray<TSharedPtr<FCellItem>> cellItems;
    cellItems.Reserve(cells.Num());
    for (UOdysseyAnimationCell* cell : cells)
    {
        TSharedPtr<FCellItem> cellItem = MakeShared<FCellItem>();
        cellItem->mCell = cell;
        cellItems.Add(cellItem);
    }

    mItemsSource->Append(cellItems);
}

float
SOdysseyAnimationCells::GetOffset() const
{
    return mAnimationLayer->CellsOffset;
}

float
SOdysseyAnimationCells::GetCellExposure(UOdysseyAnimationCell* iCell) const
{
    return iCell->Exposure;
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

                    return mHoveredCell->GetFrameRange().GetLowerBoundValue() - mAnimationLayer->CellsOffset;
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

                    return mHoveredCell->Exposure;
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

                            if (mHoveredCell->IndexInLayer >= mAnimationLayer->GetCells().Num() - 1)
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

                            if (mHoveredCell->IndexInLayer == 0)
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
    UOdysseyAnimationCell* cell = mHoveredCell;
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
    float frame = MousePositionToFrame(mMousePosition.X) + 0.5f - mAnimationLayer->CellsOffset;
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
    UOdysseyAnimationCell* cell = mAnimationLayer->GetCellAtFrame(frame);
    if (!cell)
        return 0.f;

    return cell->GetFrameRange().GetUpperBoundValue() - (int)frame + 1;
}

EVisibility
SOdysseyAnimationCells::GetCellBreakIndicatorVisibility() const
{
    if (mAnimationLayer->IsLockedRecursively())
        return EVisibility::Hidden;

    if (FOdysseyAnimationTimelineTools::Get().GetCurrentTool() != EOdysseyTimelineTool::Cut)
        return EVisibility::Hidden;

    if (!IsHovered() || !mHoveredCell)
        return EVisibility::Hidden;

    //Find frame
    float frame = MousePositionToFrame(mMousePosition.X) + 0.5f;

    UOdysseyAnimationCell* cell = mAnimationLayer->GetCellAtFrame(frame);
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

EVisibility
SOdysseyAnimationCells::GetCellVisibility(UOdysseyAnimationCell* iCell) const
{
    if ( iCell->Exposure > 0 )
        return EVisibility::Visible;

    return EVisibility::Collapsed;
}

void
SOdysseyAnimationCells::OnExposureHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (!mHoveredCell)
        return;

    #ifdef WITH_EDITOR
        GEditor->BeginTransaction(LOCTEXT("timeline.cells.transaction.set-cell-exposure", "Change Cell Exposure"));
    #endif

    mExposureHandleDragData.mIsDragging = true;
    mExposureHandleDragData.mCell = mHoveredCell;
    mExposureHandleDragData.mInitialExposure = mHoveredCell->Exposure;
    mExposureHandleDragData.mMousePosition = iEvent.GetScreenSpacePosition().X;
}

void
SOdysseyAnimationCells::OnExposureHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (!mExposureHandleDragData.mCell)
        return;

    const int minExposure = 1;
    float mouseOffset = iEvent.GetScreenSpacePosition().X - mExposureHandleDragData.mMousePosition;
    int mouseOffsetInt = 0;
    if ( mouseOffset > 0 )
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() - 0.5f);

    UOdysseyAnimationCell* cell = mExposureHandleDragData.mCell;
    int exposure = FMath::Max(1, mExposureHandleDragData.mInitialExposure + mouseOffsetInt);
    FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), exposure, EPropertyChangeType::Interactive);
}

void
SOdysseyAnimationCells::OnExposureHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (!mExposureHandleDragData.mCell)
        return;

    UOdysseyAnimationCell* cell = mExposureHandleDragData.mCell;
    FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), cell->Exposure, EPropertyChangeType::ValueSet);
#ifdef WITH_EDITOR
    GEditor->EndTransaction();
#endif

    mExposureHandleDragData = {};
}

void
SOdysseyAnimationCells::OnTimingHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (!mHoveredCell)
        return;

    #ifdef WITH_EDITOR
        GEditor->BeginTransaction(LOCTEXT("timeline.cells.transaction.set-cell-timing", "Change Cell Timing"));
    #endif

    FInt32Range cellRange = mHoveredCell->GetFrameRange();
    FInt32Range layerRange = mAnimationLayer->GetFrameRange();

    mTimingHandleDragData.mIsDragging = true;
    mTimingHandleDragData.mCell = mHoveredCell;
    mTimingHandleDragData.mMinOffset = -cellRange.GetLowerBoundValue();
    mTimingHandleDragData.mHasMaxOffset = mHoveredCell->IndexInLayer == 0;
    mTimingHandleDragData.mMaxOffset = layerRange.GetUpperBoundValue() - cellRange.GetLowerBoundValue();
    mTimingHandleDragData.mMousePosition = iEvent.GetScreenSpacePosition().X;
    mTimingHandleDragData.mInitialOffset = mAnimationLayer->CellsOffset;
}

void
SOdysseyAnimationCells::OnTimingHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(SOdysseyAnimationCells::OnTimingHandleDragged);

    if (!mTimingHandleDragData.mCell)
        return;

    for (auto element : mTimingHandleDragData.mAffectedCells)
    {
        UOdysseyAnimationCell* affectedCell = element.Key;
        int exposure = element.Value;
        affectedCell->Exposure = exposure; //don't use SetPropertyValue to avoid refreshing rendering while dragging (slow)
    }

    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), mTimingHandleDragData.mInitialOffset, EPropertyChangeType::Interactive );

    //Compute Mouse Offset
    float mouseOffset = iEvent.GetScreenSpacePosition().X - mTimingHandleDragData.mMousePosition;
    int mouseOffsetInt = 0;
    if ( mouseOffset > 0 )
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() + 0.5f);
    else
        mouseOffsetInt = (int)(mouseOffset / mTimelinePosition->GetFrameSize() - 0.5f);

    mouseOffsetInt = FMath::Max(mouseOffsetInt, mTimingHandleDragData.mMinOffset);

    if ( mTimingHandleDragData.mHasMaxOffset )
        mouseOffsetInt = FMath::Min(mouseOffsetInt, mTimingHandleDragData.mMaxOffset);

    UOdysseyAnimationCell* cell = mTimingHandleDragData.mCell;

    //Compute what needs to change
    if ( mouseOffsetInt > 0 )
    {
        // When dragging to the right, the previous cell exposure is always edited if it exists
        // It cannot be removed
        int previousCellIndex = cell->IndexInLayer - 1;
        if ( previousCellIndex >= 0 )
        {
            UOdysseyAnimationCell* previousCell = mAnimationLayer->GetCells()[previousCellIndex];

            if (!mTimingHandleDragData.mAffectedCells.Contains(previousCell))
                mTimingHandleDragData.mAffectedCells.Add(previousCell, previousCell->Exposure);

            int exposure = previousCell->Exposure + mouseOffsetInt;
            FOdysseyObjectEditorUtils::SetPropertyValue(previousCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), exposure, EPropertyChangeType::Interactive );
        }
        else //If no previous cell exists, we need to edit the layer's offset value
        {
            FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), mAnimationLayer->CellsOffset + mouseOffsetInt, EPropertyChangeType::Interactive );
        }

        //for each cell, adjust cell exposure or hide it
        for ( int i = cell->IndexInLayer; i < mAnimationLayer->GetCells().Num() && mouseOffsetInt > 0; i++ )
        {
            UOdysseyAnimationCell* cellToAdjust = mAnimationLayer->GetCells()[i];

            if (!mTimingHandleDragData.mAffectedCells.Contains(cellToAdjust))
                mTimingHandleDragData.mAffectedCells.Add(cellToAdjust, cellToAdjust->Exposure);

            int exposureToRemove = FMath::Min(cellToAdjust->Exposure, mouseOffsetInt);
            int exposure = cellToAdjust->Exposure - exposureToRemove;

            FOdysseyObjectEditorUtils::SetPropertyValue(cellToAdjust, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), exposure, EPropertyChangeType::Interactive );
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
                mTimingHandleDragData.mAffectedCells.Add(cell, cell->Exposure);

            int exposure = cell->Exposure - mouseOffsetInt;
            FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), exposure, EPropertyChangeType::Interactive );
        }

        //for each cell adjust its exposure or hide it
        for ( int i = cell->IndexInLayer - 1; i >= 0 && mouseOffsetInt < 0; i-- )
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(SOdysseyAnimationCells::OnTimingHandleDragged::SetCellExposure);
            UOdysseyAnimationCell* cellToAdjust = mAnimationLayer->GetCells()[i];

            if (!mTimingHandleDragData.mAffectedCells.Contains(cellToAdjust))
                mTimingHandleDragData.mAffectedCells.Add(cellToAdjust, cellToAdjust->Exposure);

            int exposureToRemove = FMath::Min(cellToAdjust->Exposure, -mouseOffsetInt);
            int exposure = cellToAdjust->Exposure - exposureToRemove;

            FOdysseyObjectEditorUtils::SetPropertyValue(cellToAdjust, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), exposure, EPropertyChangeType::Interactive );

            mouseOffsetInt += exposureToRemove;
        }

        //If the edited cell is the first cell, we also need to adjust the layer offset value
        //if we dragged further than the first cell adjust layer's offset
        if ( mouseOffsetInt < 0 )
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(SOdysseyAnimationCells::OnTimingHandleDragged::SetCellsOffset);
            FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), mAnimationLayer->CellsOffset + mouseOffsetInt, EPropertyChangeType::Interactive );
        }
    }
}

void
SOdysseyAnimationCells::OnTimingHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    if (!mTimingHandleDragData.mCell)
        return;

    TArray<UOdysseyAnimationCell*> cellsToRemove;
    for (auto element : mTimingHandleDragData.mAffectedCells)
    {
        UOdysseyAnimationCell* affectedCell = element.Key;
        int exposure = element.Value;
        if (affectedCell->Exposure <= 0)
        {
            affectedCell->Exposure = exposure;
            cellsToRemove.Add(affectedCell);
        }
        else
        {
            int newExposure = affectedCell->Exposure;
            affectedCell->Exposure = exposure; //Needed here to have accurate Undos
            FOdysseyObjectEditorUtils::SetPropertyValue(affectedCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), newExposure, EPropertyChangeType::ValueSet );
        }
    }

    mAnimationLayer->RemoveCells(cellsToRemove);

    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), mAnimationLayer->CellsOffset, EPropertyChangeType::ValueSet );

    mTimingHandleDragData.mAffectedCells.Empty();

#ifdef WITH_EDITOR
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

    return mAnimationLayer->CellsOffset > 0 ? EVisibility::Visible : EVisibility::Hidden;
}

void
SOdysseyAnimationCells::OnAddCellsHandleDragStarted(const FGeometry& iGeometry, const FPointerEvent& iEvent, bool iIsRightHandle)
{
    #ifdef WITH_EDITOR
        GEditor->BeginTransaction(LOCTEXT("timeline.cells.transaction.add-cells-at-start", "Add Cells"));
    #endif

    FInt32Range layerRange = mAnimationLayer->GetFrameRange();
    mAddCellsHandleDragData.mIsDragging = true;
    mAddCellsHandleDragData.mIsRightHandle = iIsRightHandle;
    mAddCellsHandleDragData.mMousePosition = iEvent.GetScreenSpacePosition().X;
    mAddCellsHandleDragData.mInitialOffset = mAnimationLayer->CellsOffset;

    if (iIsRightHandle)
    {
        mAddCellsHandleDragData.mMinOffset = layerRange.GetLowerBoundValue() - layerRange.GetUpperBoundValue();
        mAddCellsHandleDragData.mHasMaxOffset = false;
    }
    else
    {
        mAddCellsHandleDragData.mMinOffset = -mAnimationLayer->CellsOffset;
        mAddCellsHandleDragData.mMaxOffset = layerRange.GetUpperBoundValue() - layerRange.GetLowerBoundValue();
        mAddCellsHandleDragData.mHasMaxOffset = true;
    }
}

void
SOdysseyAnimationCells::OnAddCellsHandleDragged(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    for (auto element : mAddCellsHandleDragData.mAffectedCells)
    {
        UOdysseyAnimationCell* affectedCell = element.Key;
        int exposure = element.Value;
        affectedCell->Exposure = exposure; //don't use SetPropertyValue to avoid refreshing rendering while dragging (slow)
    }

    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), mAddCellsHandleDragData.mInitialOffset, EPropertyChangeType::Interactive );

    TArray<UOdysseyAnimationCell*> cells = mAnimationLayer->GetCells();
    for (int i = cells.Num() - 1; i >= 0; i--)
    {
        if (!cells[i])
            mAnimationLayer->RemoveCellAtIndex(i);
    }

    //---

    int numTempCellsToPrepend = 0;
    int numTempCellsToAppend = 0;

    //Compute Mouse Offset
    float mouseOffset = iEvent.GetScreenSpacePosition().X - mAddCellsHandleDragData.mMousePosition;
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
                UOdysseyAnimationCell* cellToAdjust = mAnimationLayer->GetCells()[i];

                if (!mAddCellsHandleDragData.mAffectedCells.Contains(cellToAdjust))
                    mAddCellsHandleDragData.mAffectedCells.Add(cellToAdjust, cellToAdjust->Exposure);

                int exposureToRemove = FMath::Min(cellToAdjust->Exposure, -mouseOffsetInt);
                int exposure = cellToAdjust->Exposure - exposureToRemove;

                FOdysseyObjectEditorUtils::SetPropertyValue(cellToAdjust, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), exposure, EPropertyChangeType::Interactive );

                mouseOffsetInt += exposureToRemove;
            }
        }
    }
    else
    {
        FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), mAnimationLayer->CellsOffset + mouseOffsetInt, EPropertyChangeType::Interactive );

        if ( mouseOffsetInt < 0 )
        {
            numTempCellsToPrepend = -mouseOffsetInt;
        }
        else
        {
            //for each cell adjust its exposure or hide it
            for ( int i = 0; i < mAnimationLayer->GetCells().Num() && mouseOffsetInt > 0; i++ )
            {
                UOdysseyAnimationCell* cellToAdjust = mAnimationLayer->GetCells()[i];

                if (!mAddCellsHandleDragData.mAffectedCells.Contains(cellToAdjust))
                    mAddCellsHandleDragData.mAffectedCells.Add(cellToAdjust, cellToAdjust->Exposure);

                int exposureToRemove = FMath::Min(cellToAdjust->Exposure, mouseOffsetInt);
                int exposure = cellToAdjust->Exposure - exposureToRemove;

                FOdysseyObjectEditorUtils::SetPropertyValue(cellToAdjust, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), exposure, EPropertyChangeType::Interactive );

                mouseOffsetInt -= exposureToRemove;
            }
        }
    }

    if (numTempCellsToPrepend > 0)
        mAnimationLayer->AddNullCells(0, numTempCellsToPrepend);

    if (numTempCellsToAppend > 0)
        mAnimationLayer->AddNullCells(INDEX_NONE, numTempCellsToAppend);
}

void
SOdysseyAnimationCells::OnAddCellsHandleDragStopped(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
    TArray<UOdysseyAnimationCell*> cellsToRemove;
    for (auto element : mAddCellsHandleDragData.mAffectedCells)
    {
        UOdysseyAnimationCell* affectedCell = element.Key;
        int exposure = element.Value;
        if (affectedCell->Exposure <= 0)
        {
            affectedCell->Exposure = exposure;
            cellsToRemove.Add(affectedCell);
        }
        else
        {
            int newExposure = affectedCell->Exposure;
            affectedCell->Exposure = exposure; //Needed here to have accurate Undos
            FOdysseyObjectEditorUtils::SetPropertyValue(affectedCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), newExposure, EPropertyChangeType::ValueSet );
        }
    }

    mAnimationLayer->RemoveCells(cellsToRemove);

    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), mAnimationLayer->CellsOffset, EPropertyChangeType::ValueSet );

    mAddCellsHandleDragData.mAffectedCells.Empty();

    TArray<UOdysseyAnimationCell*> cells = mAnimationLayer->GetCells();
    for (int i = cells.Num() - 1; i >= 0; i--)
    {
        if (!cells[i])
        {
            mAnimationLayer->RemoveCellAtIndex(i);
            mAnimationLayer->AddCell(mAnimationLayer->DefaultCellClass, i);
        }
    }

#ifdef WITH_EDITOR
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
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::None),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::None),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::None)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );

    menuBuilder.AddMenuEntry(
        LOCTEXT("animation.layer.prebehaviour-menu.hold", "Hold"),
        TAttribute<FText>(),
        FSlateIcon("OdysseyStyle", "Animation.Layer.PreBehaviour.Hold"),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Hold),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Hold),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Hold)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );

    menuBuilder.AddMenuEntry(
        LOCTEXT("animation.layer.prebehaviour-menu.loop", "Loop"),
        TAttribute<FText>(),
        FSlateIcon("OdysseyStyle", "Animation.Layer.PreBehaviour.Loop"),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Loop),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Loop),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Loop)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );

    menuBuilder.AddMenuEntry(
        LOCTEXT("animation.layer.prebehaviour-menu.pingpong", "PingPong"),
        TAttribute<FText>(),
        FSlateIcon("OdysseyStyle", "Animation.Layer.PreBehaviour.PingPong"),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::PingPong),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::PingPong),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPreBehaviour, EOdysseyAnimationLayerImagePostBehaviour::PingPong)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );

    return menuBuilder.MakeWidget();
}

const FSlateBrush*
SOdysseyAnimationCells::GetPreBehaviourBrush() const
{
    switch(mAnimationLayer->PreBehaviour)
    {
        case EOdysseyAnimationLayerImagePostBehaviour::None:
            return FOdysseyStyle::GetBrush("Animation.Layer.PreBehaviour.None");
        break;
        case EOdysseyAnimationLayerImagePostBehaviour::Hold:
            return FOdysseyStyle::GetBrush("Animation.Layer.PreBehaviour.Hold");
        break;
        case EOdysseyAnimationLayerImagePostBehaviour::Loop:
            return FOdysseyStyle::GetBrush("Animation.Layer.PreBehaviour.Loop");
        break;
        case EOdysseyAnimationLayerImagePostBehaviour::PingPong:
            return FOdysseyStyle::GetBrush("Animation.Layer.PreBehaviour.PingPong");
        break;
    }
    return nullptr;
}

const FSlateBrush*
SOdysseyAnimationCells::GetPostBehaviourBrush() const
{
    switch(mAnimationLayer->PostBehaviour)
    {
        case EOdysseyAnimationLayerImagePostBehaviour::None:
            return FOdysseyStyle::GetBrush("Animation.Layer.PostBehaviour.None");
        break;
        case EOdysseyAnimationLayerImagePostBehaviour::Hold:
            return FOdysseyStyle::GetBrush("Animation.Layer.PostBehaviour.Hold");
        break;
        case EOdysseyAnimationLayerImagePostBehaviour::Loop:
            return FOdysseyStyle::GetBrush("Animation.Layer.PostBehaviour.Loop");
        break;
        case EOdysseyAnimationLayerImagePostBehaviour::PingPong:
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
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::None),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::None),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::None)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );

    menuBuilder.AddMenuEntry(
        LOCTEXT("animation.layer.postbehaviour-menu.hold", "Hold"),
        TAttribute<FText>(),
        FSlateIcon("OdysseyStyle", "Animation.Layer.PostBehaviour.Hold"),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Hold),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Hold),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Hold)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );

    menuBuilder.AddMenuEntry(
        LOCTEXT("animation.layer.postbehaviour-menu.loop", "Loop"),
        TAttribute<FText>(),
        FSlateIcon("OdysseyStyle", "Animation.Layer.PostBehaviour.Loop"),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Loop),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Loop),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::Loop)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );

    menuBuilder.AddMenuEntry(
        LOCTEXT("animation.layer.postbehaviour-menu.pingpong", "PingPong"),
        TAttribute<FText>(),
        FSlateIcon("OdysseyStyle", "Animation.Layer.PostBehaviour.PingPong"),
        FUIAction(
            FExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::SetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::PingPong),
            FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCells::CanSetPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::PingPong),
            FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCells::IsPostBehaviour, EOdysseyAnimationLayerImagePostBehaviour::PingPong)
        ),
        NAME_None,
        EUserInterfaceActionType::RadioButton
    );

    return menuBuilder.MakeWidget();
}

void
SOdysseyAnimationCells::SetPreBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour)
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("animation.layer.transaction.set-prebehaviour", "Set Layer Pre Behaviour"));
#endif
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, PreBehaviour), iBehaviour);
}

bool
SOdysseyAnimationCells::CanSetPreBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour) const
{
    return !mAnimationLayer->IsLockedRecursively();
}

bool
SOdysseyAnimationCells::IsPreBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour) const
{
    return mAnimationLayer->PreBehaviour == iBehaviour;
}

void
SOdysseyAnimationCells::SetPostBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour)
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("animation.layer.transaction.set-postbehaviour", "Set Layer Post Behaviour"));
#endif
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, PostBehaviour), iBehaviour);
}

bool
SOdysseyAnimationCells::CanSetPostBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour) const
{
    return !mAnimationLayer->IsLockedRecursively();
}

bool
SOdysseyAnimationCells::IsPostBehaviour(EOdysseyAnimationLayerImagePostBehaviour iBehaviour) const
{
    return mAnimationLayer->PostBehaviour == iBehaviour;
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
