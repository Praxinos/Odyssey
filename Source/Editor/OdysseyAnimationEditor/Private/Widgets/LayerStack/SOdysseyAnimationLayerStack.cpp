// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineHeader.h"
#include "Widgets/SOdysseyLayerStackAddLayerButton.h"
#include "Widgets/SOdysseyAnimationPlaybackControls.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "Widgets/LayerStack/SOdysseyAnimationLayerStackTreeView.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SLATE_IMPLEMENT_WIDGET(SOdysseyAnimationLayerStack)
void
SOdysseyAnimationLayerStack::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mLayerStack, EInvalidateWidgetReason::Layout)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyAnimationLayerStack&>(Widget).RebuildWidgets();
        }
    ));
}

SOdysseyAnimationLayerStack::~SOdysseyAnimationLayerStack()
{
}

SOdysseyAnimationLayerStack::SOdysseyAnimationLayerStack()
    : mExtension(nullptr)
    , mLayerStack(*this, nullptr)
    , mTreeView()
	, mTimelineScrollBar(nullptr)
{
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void
SOdysseyAnimationLayerStack::Construct(const FArguments& InArgs, FOdysseyAnimationEditorExtension* iExtension )
{
    mExtension = iExtension;
    mLayerStack.Assign(*this, InArgs._LayerStack);
    RebuildWidgets();
}

void
SOdysseyAnimationLayerStack::RebuildWidgets()
{
    this->ChildSlot.DetachWidget();
    UOdysseyLayerStack* layerstack = mLayerStack.Get();
    
    TSharedPtr<SWidget> widget =
    SNew(SVerticalBox)
    + SVerticalBox::Slot()
    .AutoHeight()
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SOdysseyLayerStackAddLayerButton)
            .LayerStack(mExtension->LayerStack())
            .OnAdded( this, &SOdysseyAnimationLayerStack::OnLayerAdded)
        ]
        + SHorizontalBox::Slot()
        .FillWidth(1.f)
        .HAlign( HAlign_Center )
        .VAlign( VAlign_Center )
        [
            SNew(SOdysseyAnimationPlaybackControls, mExtension)
            .PlaybackFramesPerSecond(this, &SOdysseyAnimationLayerStack::PlaybackFramesPerSecond)
        ]
    ]
    +SVerticalBox::Slot()
    .FillHeight(1.0f)
    [
        SAssignNew(mTreeView, SOdysseyAnimationLayerStackTreeView, mExtension)
        .LayerStack(mExtension->LayerStack())
        .OnGenerateRow(this, &SOdysseyAnimationLayerStack::OnGenerateRow)
        .HeaderManualWidth(200.f)
        .AdditionalColumns(
            {
                SHeaderRow::Column("Timeline")
                .DefaultLabel(FText())
                .VAlignCell(VAlign_Fill)
                .HAlignCell(HAlign_Fill)
                [
                    SAssignNew(mTimelineControl, SOdysseyAnimationTimelineControl, mExtension)
                    [
                        SNew(SOdysseyAnimationTimelineHeader, mExtension)
                    ]
                ]
            }
        )
    ]
    +SVerticalBox::Slot()
    .AutoHeight()
    [
        SAssignNew(mTimelineScrollBar, SScrollBar)
        .Orientation( Orient_Horizontal )
        .OnUserScrolled_Raw(this, &SOdysseyAnimationLayerStack::OnTimelineScrollBarScrolled)
    ];

    this->ChildSlot.AttachWidget(widget.ToSharedRef());
}

TSharedRef<ITableRow>
SOdysseyAnimationLayerStack::OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable)
{
    check(iLayer);

    UClass* layerClass = iLayer->GetClass();
    if (layerClass == UOdysseyAnimationLayerFolder::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerFolderRow, GetTreeView().ToSharedRef(), mExtension, Cast<UOdysseyAnimationLayerFolder>(iLayer));
    }
    else if (layerClass == UOdysseyAnimationLayerImageRaster::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerImageRasterRow, GetTreeView().ToSharedRef(), mExtension, Cast<UOdysseyAnimationLayerImageRaster>(iLayer));
    }
    else if (layerClass == UOdysseyAnimationLayerImageVector::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerImageVectorRow, GetTreeView().ToSharedRef(), mExtension, Cast<UOdysseyAnimationLayerImageVector>(iLayer));
    }

    return SNew(SOdysseyAnimationLayerRow, GetTreeView().ToSharedRef(), mExtension, Cast<UOdysseyAnimationLayer>(iLayer)); //Default widget
}

void
SOdysseyAnimationLayerStack::OnTimelineScrollBarScrolled(float iOffset)
{
    int lastFrameIndex = mExtension->Animation()->GetFrameRange().GetUpperBoundValue();
    float frameWidth = mExtension->Timeline()->GetFrameWidth();
    float columnWidth = mTimelineControl->GetPaintSpaceGeometry().GetLocalSize().X;
    float contentWidth = (lastFrameIndex + 1) * frameWidth;
    float adjustedContentWidth = FMath::Max(contentWidth, columnWidth) + columnWidth - frameWidth;
    float visiblePercent = columnWidth / adjustedContentWidth;
    float scrollbarOffset = FMath::Clamp(iOffset, 0.f, 1.f - visiblePercent);
    float offsetPercent = (scrollbarOffset / (1.f - visiblePercent));
    float offsetAmount = FMath::Max(lastFrameIndex, columnWidth / frameWidth - 1.f);
    mExtension->Timeline()->SetOffset(offsetPercent * offsetAmount);
}

TSharedPtr<SOdysseyLayerStackTreeView>
SOdysseyAnimationLayerStack::GetTreeView() const
{
    return mTreeView;
}

void
SOdysseyAnimationLayerStack::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
    if (!mTreeView)
        return;
        
    if (!mExtension->Animation())
        return;

    TSharedPtr<SHeaderRow> headerRow = mTreeView->GetHeaderRow();
    if (!headerRow)
        return;
        
    const TIndirectArray<SHeaderRow::FColumn>& columns = headerRow->GetColumns();

    for ( const SHeaderRow::FColumn& column : columns )
    {
        if ( column.ColumnId != "Timeline" )
            continue;

        int lastFrameIndex = mExtension->Animation()->GetFrameRange().GetUpperBoundValue();
        float frameWidth = mExtension->Timeline()->GetFrameWidth();
        float offset = mExtension->Timeline()->GetOffset() * frameWidth;

        float columnWidth = mTimelineControl->GetPaintSpaceGeometry().GetLocalSize().X;
        float contentWidth = (lastFrameIndex + 1) * frameWidth;
        float adjustedContentWidth = FMath::Max(contentWidth, columnWidth) + columnWidth - frameWidth;

        float visiblePercent = columnWidth / adjustedContentWidth;


        float offsetPercent = offset / adjustedContentWidth;
        float scrollbarOffset = FMath::Clamp(offsetPercent, 0.f, 1.f - visiblePercent);
        mTimelineScrollBar->SetState(scrollbarOffset, visiblePercent);

        break;
    }
}

void
SOdysseyAnimationLayerStack::OnLayerAdded(UOdysseyLayer* iLayer)
{
    if (iLayer->GetClass() == UOdysseyAnimationLayerImageRaster::StaticClass())
    {
        UOdysseyAnimationLayerImageRaster* layer = Cast<UOdysseyAnimationLayerImageRaster>(iLayer);
        TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(layer, 1, mExtension->Animation()->Width(), mExtension->Animation()->Height(), mExtension->Animation()->Format());
        FOdysseyAnimationCellsMutator mutator(layer, layer->GetCellsContainer());
        mutator.Add({ cell });
        mutator.Commit();
    }
    else if (iLayer->GetClass() == UOdysseyAnimationLayerImageVector::StaticClass())
    {
        UOdysseyAnimationLayerImageVector* layer = Cast<UOdysseyAnimationLayerImageVector>(iLayer);
        TSharedPtr<FOdysseyAnimationCellImageVector> cell = FOdysseyAnimationCellImageVector::Create(layer, 1, mExtension->Animation()->Width(), mExtension->Animation()->Height());
        FOdysseyAnimationCellsMutator mutator(layer, layer->GetCellsContainer());
        mutator.Add({ cell });
        mutator.Commit();
    }
}

float
SOdysseyAnimationLayerStack::PlaybackFramesPerSecond() const
{
    return mExtension->PlaybackFramesPerSecond();
}

#undef LOCTEXT_NAMESPACE
