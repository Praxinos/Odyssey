// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineHeader.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationLayerStack"


SOdysseyAnimationLayerStack::~SOdysseyAnimationLayerStack()
{
}

SOdysseyAnimationLayerStack::SOdysseyAnimationLayerStack()
    : mAnimation(nullptr)
    , mPlayer(nullptr)
    , mTreeView()
    , mZoom(1.0f)
    , mOffset(0.0f)
{
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void
SOdysseyAnimationLayerStack::Construct(const FArguments& InArgs)
{
    mAnimation = InArgs._Animation;
    mPlayer = InArgs._Player;
    ChildSlot
    [
        SAssignNew(mTreeView, SOdysseyLayerStackTreeView)
        .LayerStack(mAnimation->GetLayerStack())
        .OnGenerateRow(this, &SOdysseyAnimationLayerStack::OnGenerateRow)
        .HeaderManualWidth(200.f)
        .AdditionalColumns(
            {
                SHeaderRow::Column("Timeline")
                .DefaultLabel(LOCTEXT("", ""))
                .VAlignCell(VAlign_Fill)
                .HAlignCell(HAlign_Fill)
                [
                    SNew(SOdysseyAnimationTimelineHeader)
                    .Animation(mAnimation)
                    .Player(mPlayer)
                    .FrameWidth(50.f)
                    .Zoom(this, &SOdysseyAnimationLayerStack::GetZoom)
                    .Offset(this, &SOdysseyAnimationLayerStack::GetOffset)
                ]
            }
        )
    ];
}

TSharedRef<ITableRow>
SOdysseyAnimationLayerStack::OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable)
{
    check(iLayer);

    UClass* layerClass = iLayer->GetClass();
    if (layerClass == UOdysseyAnimationLayerFolder::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerFolderRow, mTreeView.ToSharedRef(), Cast<UOdysseyAnimationLayerFolder>(iLayer));
    }
    else if (layerClass == UOdysseyAnimationLayerImageRaster::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerImageRasterRow, mTreeView.ToSharedRef(), Cast<UOdysseyAnimationLayerImageRaster>(iLayer));
    }

    return SNew(SOdysseyLayerRow, mTreeView.ToSharedRef(), Cast<UOdysseyLayer>(iLayer)); //Default widget
}

float
SOdysseyAnimationLayerStack::GetZoom() const
{
    return mZoom;
}

float
SOdysseyAnimationLayerStack::GetOffset() const
{
    return mOffset;
}

#undef LOCTEXT_NAMESPACE
