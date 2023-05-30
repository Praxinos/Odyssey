// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineHeader.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerStack"

SOdysseyAnimationLayerStack::~SOdysseyAnimationLayerStack()
{
}

SOdysseyAnimationLayerStack::SOdysseyAnimationLayerStack()
    : mEditor(nullptr)
    , mTreeView()
	, mTimelineScrollBar(nullptr)
{
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void
SOdysseyAnimationLayerStack::Construct(const FArguments& InArgs, FOdysseyAnimationEditor* iEditor )
{
    mEditor = iEditor;
    ChildSlot
    [
        SNew(SVerticalBox)
        +SVerticalBox::Slot()
        .FillHeight(1.0f)
        [
            SAssignNew(mTreeView, SOdysseyLayerStackTreeView)
            .LayerStack(mEditor->Animation()->GetLayerStack())
            .OnGenerateRow(this, &SOdysseyAnimationLayerStack::OnGenerateRow)
            .HeaderManualWidth(200.f)
            .AdditionalColumns(
                {
                    SHeaderRow::Column("Timeline")
                    .DefaultLabel(LOCTEXT("", ""))
                    .VAlignCell(VAlign_Fill)
                    .HAlignCell(HAlign_Fill)
                    [
                        SNew(SOdysseyAnimationTimelineControl, mEditor)
                        [
                            SNew(SOdysseyAnimationTimelineHeader, mEditor)
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
        ]
    ];

    //Set Scrollbar Params
    mTimelineScrollBar->SetState(0.f, 0.5f);
}

TSharedRef<ITableRow>
SOdysseyAnimationLayerStack::OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable)
{
    check(iLayer);

    UClass* layerClass = iLayer->GetClass();
    if (layerClass == UOdysseyAnimationLayerFolder::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerFolderRow, GetTreeView().ToSharedRef(), mEditor, Cast<UOdysseyAnimationLayerFolder>(iLayer));
    }
    else if (layerClass == UOdysseyAnimationLayerImageRaster::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerImageRasterRow, GetTreeView().ToSharedRef(), mEditor, Cast<UOdysseyAnimationLayerImageRaster>(iLayer));
    }

    return SNew(SOdysseyAnimationLayerRow, GetTreeView().ToSharedRef(), mEditor, Cast<UOdysseyAnimationLayer>(iLayer)); //Default widget
}

void
SOdysseyAnimationLayerStack::OnTimelineScrollBarScrolled(float iOffset)
{
    float visiblePercent = 0.5f;
    float scrollbarOffset = FMath::Clamp(iOffset, 0.f, visiblePercent);
    mTimelineScrollBar->SetState(scrollbarOffset, visiblePercent);

    int lastFrameIndex = mEditor->Animation()->GetFrameRange().GetUpperBoundValue();
    float offsetPercent = (scrollbarOffset / (1.f - visiblePercent));
    mEditor->Timeline()->SetOffset(offsetPercent * lastFrameIndex);
}

TSharedPtr<SOdysseyLayerStackTreeView>
SOdysseyAnimationLayerStack::GetTreeView() const
{
    return mTreeView;
}

#undef LOCTEXT_NAMESPACE
