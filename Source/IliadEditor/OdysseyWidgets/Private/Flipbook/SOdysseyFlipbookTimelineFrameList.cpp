// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyFlipbookTimelineFrameList.h"

#include "DragAndDrop/AssetDragDropOp.h"
#include "Fonts/FontMeasure.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SOverlay.h"

#include "SOdysseyFlipbookTimelineFrame.h"
#include "SOdysseyDropWidget.h"
#include "FOdysseyFlipbookTimelineFrameDragDropOperation.h"
#include "OdysseyStyle.h"
#include "SOdysseyFlipbookTimelineFrameHandle.h"

//---
//---
//---

FOdysseyFlipbookTimelineFrameListFrameMetaData::FOdysseyFlipbookTimelineFrameListFrameMetaData(int32 iIndex) :
    mIndex(iIndex)
{

}

int32
FOdysseyFlipbookTimelineFrameListFrameMetaData::Index() const
{
    return mIndex;
}

void
FOdysseyFlipbookTimelineFrameListFrameMetaData::Index(int32 iIndex)
{
    mIndex = iIndex;
}

//---
//---
//---

void SOdysseyFlipbookTimelineFrameList::Construct( const SOdysseyFlipbookTimelineFrameList::FArguments& InArgs )
{
    //Set Attributes
    mFrameSize = InArgs._FrameSize;

    //Set Events
    mOnFramesMoved = InArgs._OnFramesMoved;
    mOnFramesLengthChanged = InArgs._OnFramesLengthChanged;
    mOnFramesEditStart = InArgs._OnFramesEditStart;
    mOnFramesEditStop = InArgs._OnFramesEditStop;
    mOnFramesEditCancel = InArgs._OnFramesEditCancel;
    mOnGenerateFrameContextMenu = InArgs._OnGenerateFrameContextMenu;

    //Set Internals
    mTimingHandleBrush = FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameTimingHandle");
    mLengthHandleBrush = FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameLengthHandle");

    mDropPreviewAlignment = kDropPreviewAlignment_None;
    mFrameDragOverIndex = -1;

    //Construct Children
    ChildSlot
    .Padding(0.f, 0.f, 0.f, 0.f)
    [
        SAssignNew(mFramesContainer, SHorizontalBox)
    ];
}

FVector2D
SOdysseyFlipbookTimelineFrameList::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    //Add a half of the size of a handle to be able to see the last one it entirely
    FVector2D size = mFramesContainer->GetDesiredSize();
    size.X += FMath::Max(mLengthHandleBrush->ImageSize.X, mTimingHandleBrush->ImageSize.X) / 2;
    // size.Y += mTimingHandleBrush->ImageSize.Y / 2;
    return size;
}

FReply
SOdysseyFlipbookTimelineFrameList::OnDragOver(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent)
{
    /*
    mFrameDragOverIndex = mFrames.Num() - 1;
    mDropPreviewAlignment = kDropPreviewAlignment_Right;
    return FReply::Unhandled(); */

    TSharedPtr<FDragDropOperation> Operation = iDragDropEvent.GetOperation();
    if (!Operation.IsValid())
    {
        mFrameDragOverIndex = -1;
        mDropPreviewAlignment = kDropPreviewAlignment_None;
        return FReply::Unhandled();
    }

    if (Operation->IsOfType<FOdysseyFlipbookTimelineFrameDragDropOperation>())
    {
        //If there's no frame or only one frame (which is the one we are dragging), we can still drop but there's no preview
        if (mFrames.Num() <= 1)
        {
            mFrameDragOverIndex = -1;
            mDropPreviewAlignment = kDropPreviewAlignment_None;
            return FReply::Handled();
        }

        const auto& frameDragDropOp = StaticCastSharedPtr<FOdysseyFlipbookTimelineFrameDragDropOperation>(Operation);

        TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> metadata = frameDragDropOp->Frame()->GetMetaData<FOdysseyFlipbookTimelineFrameListFrameMetaData>();
        if (!metadata)
            return FReply::Handled();

        mFrameDragOverIndex = (metadata->Index() == mFrames.Num() - 1) ? mFrames.Num() - 2 : mFrames.Num() - 1;
        mDropPreviewAlignment = kDropPreviewAlignment_Right;
        return FReply::Handled();
    }
    return FReply::Unhandled();
}

void
SOdysseyFlipbookTimelineFrameList::OnDragLeave(const FDragDropEvent& iDragDropEvent)
{
    mFrameDragOverIndex = -1;
    mDropPreviewAlignment = kDropPreviewAlignment_None;
}

FReply
SOdysseyFlipbookTimelineFrameList::OnDrop(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent)
{
    if (mFrameDragOverIndex <= 0)
        return FReply::Unhandled();

    TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> metadata = mFrames[mFrameDragOverIndex]->GetMetaData<FOdysseyFlipbookTimelineFrameListFrameMetaData>();
    return OnFrameDrop(mFrames[mFrameDragOverIndex]->GetCachedGeometry(), iDragDropEvent, metadata);
}

//---
//---
//---


float
SOdysseyFlipbookTimelineFrameList::GetFrameSize() const
{
    return mFrameSize.Get();
}

int32
SOdysseyFlipbookTimelineFrameList::GetFrameCount() const
{
    return mFrames.Num();
}

int32
SOdysseyFlipbookTimelineFrameList::GetFrameLength(int32 iIndex) const
{
    return mFrames[iIndex]->Length();
}

TSharedPtr<SWidget>
SOdysseyFlipbookTimelineFrameList::GetFrameContent(int32 iIndex) const
{

    return mFrames[iIndex]->Content();
}

EVisibility
SOdysseyFlipbookTimelineFrameList::GetFrameVisibility(int32 iIndex) const
{
    return mFramesContainer->GetChildren()->GetChildAt(iIndex)->GetVisibility();
    //return mFrames[iIndex]->GetVisibility();
}

//---
//---
//---

void
SOdysseyFlipbookTimelineFrameList::SetFrameLength(int32 iIndex, int32 iLength)
{
    return mFrames[iIndex]->Length(iLength);
}

void
SOdysseyFlipbookTimelineFrameList::SetFrameContent(int32 iIndex, TSharedPtr<SWidget> iContent)
{
    return mFrames[iIndex]->Content(iContent);
}

void
SOdysseyFlipbookTimelineFrameList::SetFrameVisibility(int32 iIndex, EVisibility iVisibility)
{
    mFramesContainer->GetChildren()->GetChildAt(iIndex)->SetVisibility(iVisibility);
    //return mFrames[iIndex]->SetVisibility(iVisibility);
}

//---
//---
//---

int32
SOdysseyFlipbookTimelineFrameList::AddFrame()
{
    TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> metadata = MakeShareable(new FOdysseyFlipbookTimelineFrameListFrameMetaData(mFrames.Num()));

    TSharedPtr<SOdysseyFlipbookTimelineFrame> frame = SNew(SOdysseyFlipbookTimelineFrame)
        .FrameSize(mFrameSize)
        .MinLength(1)
        .Length(1)
        .OnDragDetected(this, &SOdysseyFlipbookTimelineFrameList::OnFrameDragDetected, metadata)
        .OnGenerateContextMenu(this, &SOdysseyFlipbookTimelineFrameList::OnGenerateFrameContextMenu, metadata);

    frame->AddMetadata(metadata.ToSharedRef());

    mFrames.Add(frame);
    mFramesContainer->AddSlot()
    .Padding(0.f, 0.f, 0.f, 0.f)
    .AutoWidth()
    [
        CreateFrameControlWidget(frame).ToSharedRef()
    ];

    return mFrames.Num() - 1;
}

//---
//---
//---

void
SOdysseyFlipbookTimelineFrameList::InsertFrame(int32 iIndex)
{
    TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> metadata = MakeShareable(new FOdysseyFlipbookTimelineFrameListFrameMetaData(iIndex));

    TSharedPtr<SOdysseyFlipbookTimelineFrame> frame = SNew(SOdysseyFlipbookTimelineFrame)
        .FrameSize(mFrameSize)
        .MinLength(1)
        .Length(1)
        .OnDragDetected(this, &SOdysseyFlipbookTimelineFrameList::OnFrameDragDetected, metadata)
        .OnGenerateContextMenu(this, &SOdysseyFlipbookTimelineFrameList::OnGenerateFrameContextMenu, metadata);

    frame->AddMetadata(metadata.ToSharedRef());

    mFrames.Insert(frame, iIndex);
    mFramesContainer->InsertSlot(iIndex)
    .Padding(0.f, 0.f, 0.f, 0.f)
    .AutoWidth()
    [
        CreateFrameControlWidget(frame).ToSharedRef()
    ];

    for (int i = iIndex; i < mFrames.Num(); i++)
    {
        metadata = mFrames[i]->GetMetaData<FOdysseyFlipbookTimelineFrameListFrameMetaData>();
        metadata->Index(i);
    }
}


void
SOdysseyFlipbookTimelineFrameList::MoveFrame(int32 iSrcIndex, int32 iDestIndex)
{
    int32 fixedDestIndex = (iSrcIndex < iDestIndex) ? iDestIndex - 1 : iDestIndex;

    TSharedPtr<SOdysseyFlipbookTimelineFrame> frame = mFrames[iSrcIndex];

    //Remove frame
    mFramesContainer->RemoveSlot(mFramesContainer->GetChildren()->GetChildAt(iSrcIndex)); //Remove he last slot to avoid regenerating framedata indexes
    mFrames.RemoveAt(iSrcIndex);

    //Insert Frame
    mFrames.Insert(frame, fixedDestIndex);
    mFramesContainer->InsertSlot(fixedDestIndex)
    .Padding(0.f, 0.f, 0.f, 0.f)
    .AutoWidth()
    [
        CreateFrameControlWidget(frame).ToSharedRef()
    ];

    for (int i = FMath::Min(iSrcIndex, fixedDestIndex); i < mFrames.Num(); i++)
    {
        TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> metadata = mFrames[i]->GetMetaData<FOdysseyFlipbookTimelineFrameListFrameMetaData>();
        metadata->Index(i);
    }
}

void
SOdysseyFlipbookTimelineFrameList::RemoveFrameAt(int32 iIndex)
{
    mFramesContainer->RemoveSlot(mFramesContainer->GetChildren()->GetChildAt(iIndex)); //Remove he last slot to avoid regenerating framedata indexes
    mFrames.RemoveAt(iIndex);

    for(int i = iIndex; i < mFrames.Num(); i++)
    {
        TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> metadata = mFrames[i]->GetMetaData<FOdysseyFlipbookTimelineFrameListFrameMetaData>();
        metadata->Index(i);
    }
}

void
SOdysseyFlipbookTimelineFrameList::RemoveAllFrames()
{
    mFramesContainer->ClearChildren();
    mFrames.Empty();
}

//---
//---
//---

void
SOdysseyFlipbookTimelineFrameList::OnLengthHandleDragStarted(TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> iMetaData)
{
    mLengthOnDragStart = mFrames[iMetaData->Index()]->Length();
    mOnFramesEditStart.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineFrameList::OnLengthHandleDragged(int32 iOffset, TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> iMetaData)
{
    mFrames[iMetaData->Index()]->Length(mLengthOnDragStart + iOffset);
    TArray<int32> indexes;
    indexes.Add(iMetaData->Index());
    mOnFramesLengthChanged.ExecuteIfBound(indexes);
}

void
SOdysseyFlipbookTimelineFrameList::OnLengthHandleDragStopped(TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> iMetaData)
{
    mOnFramesEditStop.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineFrameList::OnTimingHandleDragStarted(TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> iMetaData)
{
    mLengthOnDragStart = mFrames[iMetaData->Index()]->Length();

    mOnFramesEditStart.ExecuteIfBound();

    if (iMetaData->Index() + 1 >= mFrames.Num())
    {
        return;
    }
    TSharedPtr<SOdysseyFlipbookTimelineFrame>& nextFrame = mFrames[iMetaData->Index() + 1];
    mNextLengthOnDragStart = nextFrame->Length();
}

void
SOdysseyFlipbookTimelineFrameList::OnTimingHandleDragged(int32 iOffset, TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> iMetaData)
{
    mFrames[iMetaData->Index()]->Length(mLengthOnDragStart + iOffset);

    TArray<int32> indexes;
    indexes.Add(iMetaData->Index());

    if (iMetaData->Index() + 1 < mFrames.Num())
    {
        TSharedPtr<SOdysseyFlipbookTimelineFrame>& nextFrame = mFrames[iMetaData->Index() + 1];

        int32 offset = mFrames[iMetaData->Index()]->Length() - mLengthOnDragStart;
        nextFrame->Length(mNextLengthOnDragStart - offset);

        indexes.Add(iMetaData->Index() + 1);
    }

    mOnFramesLengthChanged.ExecuteIfBound(indexes);
}

void
SOdysseyFlipbookTimelineFrameList::OnTimingHandleDragStopped(TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> iMetaData)
{
    mOnFramesEditStop.ExecuteIfBound();
}



FReply
SOdysseyFlipbookTimelineFrameList::OnFrameDrop( const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent, TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> iMetaData)
{
    mFrameDragOverIndex = -1;
    mDropPreviewAlignment = kDropPreviewAlignment_None;

    TSharedPtr<FDragDropOperation> Operation = iDragDropEvent.GetOperation();
    if (!Operation.IsValid())
        return FReply::Unhandled();

    FVector2D screenPosition = iDragDropEvent.GetScreenSpacePosition();
    FVector2D widgetPosition = iGeometry.AbsoluteToLocal(screenPosition);
    float normalizedX = widgetPosition.X / iGeometry.GetLocalSize().X;

    int32 indexOffset = normalizedX > 0.5f ? 1 : 0;

    if (Operation->IsOfType<FAssetDragDropOp>())
    {
        const auto& AssetDragDropOp = StaticCastSharedPtr<FAssetDragDropOp>(Operation);
        //TODO: Here is the place were to manage drop of external assets
    }
    else if (Operation->IsOfType<FOdysseyFlipbookTimelineFrameDragDropOperation>())
    {
        const auto& frameDragDropOp = StaticCastSharedPtr<FOdysseyFlipbookTimelineFrameDragDropOperation>(Operation);

        TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> srcMetadata = frameDragDropOp->Frame()->GetMetaData<FOdysseyFlipbookTimelineFrameListFrameMetaData>();
        if (!srcMetadata)
            return FReply::Handled();

        TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> destMetadata = iMetaData;

        int32 srcIndex = srcMetadata->Index();
        int32 destIndex = destMetadata->Index() + indexOffset;

        //Move in GUI
        SetFrameVisibility(srcIndex, EVisibility::Visible);
        MoveFrame(srcIndex, destIndex);

        TArray<int32> indexes;
        indexes.Add(srcIndex);
        mOnFramesMoved.ExecuteIfBound(indexes, destIndex);
        return FReply::Handled();
    }
    return FReply::Unhandled();
}

void
SOdysseyFlipbookTimelineFrameList::OnFrameDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent, TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> iMetaData)
{
}

FReply
SOdysseyFlipbookTimelineFrameList::OnFrameDragOver( const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent, TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> iMetaData)
{
    TSharedPtr<FDragDropOperation> Operation = iDragDropEvent.GetOperation();
    if (!Operation.IsValid())
    {
        mFrameDragOverIndex = -1;
        mDropPreviewAlignment = kDropPreviewAlignment_None;
        return FReply::Unhandled();
    }

    if (Operation->IsOfType<FOdysseyFlipbookTimelineFrameDragDropOperation>())
    {
        mFrameDragOverIndex = iMetaData->Index();

        FVector2D screenPosition = iDragDropEvent.GetScreenSpacePosition();
        FVector2D widgetPosition = iGeometry.AbsoluteToLocal(screenPosition);
        float normalizedX = widgetPosition.X / iGeometry.GetLocalSize().X;

        if (normalizedX > 0.5f)
        {
            mDropPreviewAlignment = kDropPreviewAlignment_Right;
        }
        else
        {
            mDropPreviewAlignment = kDropPreviewAlignment_Left;
        }
        return FReply::Handled();
    }
    return FReply::Unhandled();
}

void
SOdysseyFlipbookTimelineFrameList::OnFrameDragLeave(const FDragDropEvent& DragDropEvent, TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> iMetaData)
{
    mDropPreviewAlignment = kDropPreviewAlignment_None;
    mFrameDragOverIndex = -1;
}

FReply
SOdysseyFlipbookTimelineFrameList::OnFrameDragDetected( const FGeometry& iGeometry, const FPointerEvent& iMouseEvent, TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> iMetaData)
{
    if (iMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        TSharedRef<FOdysseyFlipbookTimelineFrameDragDropOperation> operation = MakeShareable(new FOdysseyFlipbookTimelineFrameDragDropOperation(mFrames[iMetaData->Index()], SharedThis(this)));
        return FReply::Handled().BeginDragDrop(operation);
    }

    return FReply::Unhandled();
}

FReply
SOdysseyFlipbookTimelineFrameList::OnGenerateFrameContextMenu( const FGeometry& iGeometry, const FPointerEvent& iMouseEvent, TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> iMetadata )
{
    if (!mOnGenerateFrameContextMenu.IsBound())
        return FReply::Unhandled();

    return mOnGenerateFrameContextMenu.Execute(iGeometry, iMouseEvent, iMetadata->Index());
}


//---
//---
//---

TSharedPtr<SWidget>
SOdysseyFlipbookTimelineFrameList::CreateFrameControlWidget(TSharedPtr<SOdysseyFlipbookTimelineFrame>& iFrame)
{
    static const float dropPreviewWidth = 20.0f;
    static const FLinearColor dropPreviewColor(0.2f, 0.2f, 1.0f, 0.3f);

    TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> metaData = iFrame->GetMetaData<FOdysseyFlipbookTimelineFrameListFrameMetaData>();

    return SNew(SOdysseyDropWidget)
    .OnDrop(this, &SOdysseyFlipbookTimelineFrameList::OnFrameDrop, metaData)
    .OnDragEnter(this, &SOdysseyFlipbookTimelineFrameList::OnFrameDragEnter, metaData)
    .OnDragOver(this, &SOdysseyFlipbookTimelineFrameList::OnFrameDragOver, metaData)
    .OnDragLeave(this, &SOdysseyFlipbookTimelineFrameList::OnFrameDragLeave, metaData)
    [
        SNew(SOverlay)
        + SOverlay::Slot()
        .Padding(0.f, 0.f, 0.f, 0.f)
        [
            SNew(SOverlay)
            +SOverlay::Slot()
            .Padding(0.f, mTimingHandleBrush->ImageSize.Y/2, 0.f, 0.f)
            [
                SNew(SOverlay)
                +SOverlay::Slot()
                [
                    iFrame.ToSharedRef()
                ]
                + SOverlay::Slot()
                //.Padding(0.0f, 0.0f, -mLengthHandleBrush->ImageSize.X/2, 0.f)
                .Padding(TAttribute<FMargin>(this, &SOdysseyFlipbookTimelineFrameList::GetTimingHandlePadding))
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Center)
                [
                    SNew(SBox)
                    .Visibility(this, &SOdysseyFlipbookTimelineFrameList::GetLengthHandleVisibility)
                    // .Padding(FFlipbookUIConstants::FramePadding)
                    .WidthOverride(mLengthHandleBrush->ImageSize.X)
                    .HeightOverride(mLengthHandleBrush->ImageSize.Y)
                    [
                        SNew(SOdysseyFlipbookTimelineFrameHandle)
                        .FrameSize(mFrameSize)
                        .OnDragStarted(this, &SOdysseyFlipbookTimelineFrameList::OnLengthHandleDragStarted, metaData)
                        .OnDragged(this, &SOdysseyFlipbookTimelineFrameList::OnLengthHandleDragged, metaData)
                        .OnDragStopped(this, &SOdysseyFlipbookTimelineFrameList::OnLengthHandleDragStopped, metaData)
                        [
                            SNew(SImage)
                            .Image(mLengthHandleBrush)
                        ]
                    ]
                ]
            ]

            +SOverlay::Slot()
            //.Padding(0.0f, 0.0f, -mTimingHandleBrush->ImageSize.X/2, 0.f)
            .Padding(TAttribute<FMargin>(this, &SOdysseyFlipbookTimelineFrameList::GetTimingHandlePadding))
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Top)
            [
                SNew(SBox)
                .Visibility(this, &SOdysseyFlipbookTimelineFrameList::GetLengthHandleVisibility)
                .WidthOverride(mTimingHandleBrush->ImageSize.X)
                .HeightOverride(mTimingHandleBrush->ImageSize.Y)
                [
                    SNew(SOdysseyFlipbookTimelineFrameHandle)
                    .FrameSize(mFrameSize)
                    .OnDragStarted(this, &SOdysseyFlipbookTimelineFrameList::OnTimingHandleDragStarted, metaData)
                    .OnDragged(this, &SOdysseyFlipbookTimelineFrameList::OnTimingHandleDragged, metaData)
                    .OnDragStopped(this, &SOdysseyFlipbookTimelineFrameList::OnTimingHandleDragStopped, metaData)
                    [
                        SNew(SImage)
                        .Image(mTimingHandleBrush)
                    ]
                ]
            ]
        ]

        + SOverlay::Slot()
        .Padding(-dropPreviewWidth/2, 0.0f, 0.f, 0.f)
        .HAlign(HAlign_Left)
        [
            SNew(SBox)
            .WidthOverride(dropPreviewWidth)
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            .Visibility(this, &SOdysseyFlipbookTimelineFrameList::DropPreviewLeftVisibility, metaData)
            [
                SNew(SColorBlock)
                .Color(dropPreviewColor)
            ]
        ]
        + SOverlay::Slot()
        .Padding(0.0f, 0.f, -dropPreviewWidth/2, 0.f)
        .HAlign(HAlign_Right)
        [
            SNew(SBox)
            .WidthOverride(dropPreviewWidth)
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            .Visibility(this, &SOdysseyFlipbookTimelineFrameList::DropPreviewRightVisibility, metaData)
            [
                SNew(SColorBlock)
                .Color(dropPreviewColor)
            ]
        ]
    ];
}

EVisibility
SOdysseyFlipbookTimelineFrameList::DropPreviewLeftVisibility(TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> iMetaData) const
{
    if (mFrameDragOverIndex != iMetaData->Index() || mDropPreviewAlignment != kDropPreviewAlignment_Left)
        return EVisibility::Collapsed;

    return EVisibility::HitTestInvisible;
}

EVisibility
SOdysseyFlipbookTimelineFrameList::DropPreviewRightVisibility(TSharedPtr<FOdysseyFlipbookTimelineFrameListFrameMetaData> iMetaData) const
{
    if (mFrameDragOverIndex != iMetaData->Index() || mDropPreviewAlignment != kDropPreviewAlignment_Right)
        return EVisibility::Collapsed;

    return EVisibility::HitTestInvisible;
}

EVisibility
SOdysseyFlipbookTimelineFrameList::GetTimingHandleVisibility() const
{
    return (GetFrameSize() < mTimingHandleBrush->ImageSize.X / 2) ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility
SOdysseyFlipbookTimelineFrameList::GetLengthHandleVisibility() const
{
    return (GetFrameSize() < mLengthHandleBrush->ImageSize.X / 2) ? EVisibility::Collapsed : EVisibility::Visible;
}


FMargin
SOdysseyFlipbookTimelineFrameList::GetTimingHandlePadding() const
{
    return (GetFrameSize() < mTimingHandleBrush->ImageSize.X / 2) ?
        FMargin(0.0f, 0.0f, 0.f, 0.f) :
        FMargin(0.0f, 0.0f, -mTimingHandleBrush->ImageSize.X / 2, 0.f);
}

FMargin
SOdysseyFlipbookTimelineFrameList::GetLengthHandlePadding() const
{
    return (GetFrameSize() < mTimingHandleBrush->ImageSize.X / 2) ?
        FMargin(0.0f, 0.0f, 0.f, 0.f) :
        FMargin(0.0f, 0.0f, -mTimingHandleBrush->ImageSize.X / 2, 0.f);
}
